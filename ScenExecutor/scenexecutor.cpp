#include <QFile>


#include <QDebug>
#include "scenexecutor.h"

#include "settings.h"
#include "ActionExecutors/actionexecutor.h"

ScenExecutor::ScenExecutor(QList<Variable*> variables, EventExecutor *initializer, QString wait_list_id)
{
    m_action = new Action();
    m_variables = variables;
    m_initializer = initializer;
    m_wait_list_id = wait_list_id;
    break_scen = false;
}


ScenExecutor::~ScenExecutor()
{

}

void ScenExecutor::setClientSocket(QTcpSocket* client_socket)
{
    m_client_socket = client_socket;
}

QTcpSocket *ScenExecutor::clientSocket()
{
    return m_client_socket;
}


void ScenExecutor::startClientWorkThread(QTcpSocket *socket)
{
    DataPack data;
    data.type = 10;
    emit commandPackFormed(socket, data);
}

void ScenExecutor::stopClientWorkThread(QTcpSocket *socket)
{
    DataPack data;
    data.type = 8;
    emit commandPackFormed(socket, data);
}




bool ScenExecutor::executeScen(QFile &scen_file)
{
    scen_file_name = scen_file.fileName();


    startClientWorkThread(m_client_socket);

    QString error_message;
    int error_line;
    int error_column;
    int wait_delay = 300000;


    wait_for_answer_timer.setInterval(wait_delay);
    connect (&wait_for_answer_timer, &QTimer::timeout, &wait_for_answer_event_loop, &QEventLoop::quit);
    connect(this, &ScenExecutor::receivedAnswer, &wait_for_answer_event_loop, &QEventLoop::quit);
    wait_for_answer_timer.start();
    wait_for_answer_event_loop.exec();



    if(scen_document.setContent(&scen_file, &error_message, &error_line, &error_column))
    {
        QDomElement root = scen_document.documentElement();
        if (root.tagName().toUpper() == "SCEN")
        {
            QDomNode node = root.firstChild();
            while (!node.isNull())
            {
                QDomElement action_element = node.toElement();
                if (action_element.tagName().toUpper() == "ACTION")
                {
                    if(m_action->execAction(node, m_variables) == true)
                    {

                        DataPack command_pack = m_action->getDataPack();

                        QString message_buffer = m_action->getMessage();

                        Settings::replaceVariablesIn(message_buffer, m_variables);

                        qDebug() << message_buffer;

                        //wait_delay = command_pack->working_time.toInt();

                        //отправка пакета с командой
                        emit commandPackFormed(m_client_socket, command_pack);

                        //ожидание ответа
                        wait_for_answer_timer.setInterval(wait_delay);
                        connect (&wait_for_answer_timer, &QTimer::timeout, &wait_for_answer_event_loop, &QEventLoop::quit);
                        connect(this, &ScenExecutor::receivedAnswer, &wait_for_answer_event_loop, &QEventLoop::quit);
                        wait_for_answer_timer.start();
                        wait_for_answer_event_loop.exec();

                        //обработка ответа
                        if(answerHandling(node) != true)
                        {
                            m_action->finish();
                            return false;
                        }
                        else if(break_scen == true)
                            return true;


                        m_action->finish();
                    }
                    else
                    {
                        return false;
                    }

                }
                else if(action_element.tagName().toUpper() != "LABEL")
                {
                    qDebug() << "Неверное имя тега команды псевдоплана " << scen_file.fileName() << node.toElement().tagName();
                    return false;
                }
                //флаг подмены псевдоплана, пропускает переход к следующей ноде 1 раз
                node = node.nextSibling();
            }
        }
        else
        {
            qDebug() << "Ошибка при чтении файла псевдоплана " + scen_file.fileName() + " Неверное имя главного тега псевдоплана.";
            return false;
        }
    }
    else
    {
        qDebug() << "Ошибка при чтении файла псевдоплана " + scen_file.fileName() + ": \n Строка "
                    + QString::number(error_line) + ", символ " + QString::number(error_column) + "\n" +
                    error_message;

    }

    stopClientWorkThread(m_client_socket);
    return true;
}

void ScenExecutor::setScenName(QString scen_name)
{
    scen_file_name = scen_name;
}

QString ScenExecutor::scenFileName() const
{
    return scen_file_name;
}

bool ScenExecutor::answerHandling(QDomNode &current_node)
{
    QString message = m_action->getMessage();
    Settings::replaceVariablesIn(message, m_variables);
    if(answer_data_pack.type == 404)
    {
        qDebug() << "Ошибка при приёме пакета с ответом";
        return false;
    }
    int return_code;

    if(isActionComplete(answer_data_pack, &return_code))
    {
        qDebug() << message << "Выполнено успешно!";
        answer_data_pack.type = 404;
        return true;
    }
    else
    {
        Settings::setValueToVar(Settings::settings()->getVarNameForReturnCode(), QString::number(return_code), m_variables);

        if(m_action->isSimpleAction())
        {
            if(m_action->canBeSkiped())
            {
                qDebug() << message << "Не выполнено. Пропуск команды";
                answer_data_pack.type = 404;
                return true;
            }
            else
            {
                qDebug() << message << "Не выполнено. Остановка выполнения псевдоплана.";
                return false;
            }
        }
        else //branching action
        {
            BranchingCommand *decision = m_action->serchForBranch(current_node, return_code);

            if(decision == nullptr)
            {
                if(m_action->canBeSkiped())
                {
                    qDebug() << message << "Не выполнено. Пропуск команды";
                    answer_data_pack.type = 404;
                    return true;
                }
                else
                {
                    qDebug() << message << "Не выполнено. Остановка выполнения псевдоплана.";
                    return false;
                }
            }
            else // decision != nullptr
            {
                if(decision->type == 0)
                {
                    qDebug() << decision->message_for_user;
                    answer_data_pack.type = 404;
                    return m_action->canBeSkiped();
                }
                else if(decision->type == 1)
                {
                    qDebug() << decision->message_for_user;
                    QStringList scen_file_names = decision->command.split("\\s*|\\s*");
                    bool exec_result = true;
                    for(QString scen_file_name: scen_file_names)
                    {
                        if(exec_result == true)
                        {
                            QFile branching_scen_file(scen_file_name);
                            ScenExecutor branching_scen_executor(m_variables, m_initializer, m_wait_list_id);
                            exec_result = branching_scen_executor.executeScen(branching_scen_file);
                        }
                        else
                        {
                            return false;
                        }
                    }

                    answer_data_pack.type = 404;
                    return exec_result;
                }
                else if(decision->type == 2)
                {
                     qDebug() << decision->message_for_user;
                     QStringList scen_file_names = decision->command.split("\\s*|\\s*");
                     EventExecutionParameters *new_parameters = new EventExecutionParameters();

                     for(QString scen_file_name : scen_file_names)
                     {
                         int action_type = 1;
                         new_parameters->task_type = action_type;
                         qDebug() << "action_type" << new_parameters->task_type;

                         new_parameters->message_for_user = "message";

                         qDebug() << "message" << new_parameters->message_for_user;

                         new_parameters->skip = false;

                         qDebug() << "skip" << new_parameters->skip;

                         QString parameters_string = scen_file_name + "|" + m_wait_list_id;

                         qDebug() << "scen_path" << "wait_list_id" << parameters_string;

                         new_parameters->task_exec_parameters = parameters_string;

                         m_initializer->insertNewTaskAfterCurrentTask(new_parameters);
                     }
                     answer_data_pack.type = 404;
                     return true;

                }
                else if(decision->type == 3)
                {
                    qDebug() << decision->message_for_user;
                    QDomElement root = scen_document.documentElement();
                    QDomNode node = root.firstChild();
                    while (!node.isNull())
                    {
                        if((node.toElement().tagName().toUpper() == "LABEL") &&
                                node.toElement().attribute("name") == decision->command)
                        {
                            current_node = node;
                            answer_data_pack.type = 404;
                            return true;
                        }
                    }

                }
                else if(decision->type == 4)
                {
                    qDebug() << message << "Выполнено успешно!";
                    answer_data_pack.type = 404;
                    return true;
                }
                else
                {
                    qDebug() << "Ошибка неверный тип команды ветвления";
                    return false;
                }
            }
        }
    }
    qDebug() << "Пропущен обработчик ответа от клиента";
    return false;
}

bool ScenExecutor::isActionComplete(DataPack data_pack, int *return_code)
{
    bool converted_to_int;
    *return_code = data_pack.attributes.first().toInt(&converted_to_int);
    if(converted_to_int)
    {
        if (*return_code == 0)
            return true;
        else
            return false;
    }
    else
    {
        qDebug() << "Невозможно обработать ответ от клиента.";
    }
    return false;
}

void ScenExecutor::commandPackReceived(QTcpSocket *client_socket, DataPack data_pack)
{
    if(client_socket == m_client_socket)
    {
        answer_data_pack = data_pack;
        emit receivedAnswer();
    }
}
