#include "startactionparser.h"

#include "settings.h"

StartActionParser::StartActionParser()
{
    //qDebug() << "constr StartActionParser";
}

StartActionParser::~StartActionParser()
{
    //qDebug() << "destr StartActionParser";
}

bool StartActionParser::parseActionParameters(QDomElement parameters_element, DataPack **data_pack)
{
    QString application_path = parameters_element.attribute("application_path");
    QString working_time = parameters_element.attribute("working_time");
    QStringList attributes = parameters_element.attribute("attributes").split("\\s*|\\s*");
    normal_exit_codes = parameters_element.attribute("normal_exit_codes").split("\\s*|\\s*");

    Settings::replaceVariablesIn(application_path, m_variables);
    Settings::replaceVariablesIn(working_time, m_variables);
    for(QString &attribute: attributes)
        Settings::replaceVariablesIn(attribute, m_variables);



    *data_pack = formDataPack(application_path, working_time, attributes);

    return true;
}


DataPack* StartActionParser::formDataPack(QString application_path, QString working_time, QStringList attributes)
{
    DataPack *command_data_pack = new DataPack();
    command_data_pack->type = 12;
    command_data_pack->application_path = application_path;
    command_data_pack->working_time = working_time;
    command_data_pack->attributes = attributes;
    return command_data_pack;
}


BranchingCommand *StartActionParser::serchForBranch(QDomNode branching_node, int return_code)
{

    BranchingCommand *new_branching_comand = new BranchingCommand();

    QString return_code_string = QString::number(return_code);
    for(QString normal_code: normal_exit_codes)
    {
        if(normal_code == return_code_string)
        {
            new_branching_comand->type = 4;
            return new_branching_comand;
        }
    }
    QDomNode return_code_node = branching_node.firstChild();

    while(!return_code_node.isNull())
    {
        QDomElement return_code_element = return_code_node.toElement();
        QString message;
        if(return_code_element.tagName().toUpper() == "EXIT_CODE")
        {
            if(return_code_element.attribute("code") == return_code_string)
            {
                message = return_code_element.attribute("message");
                Settings::replaceVariablesIn(message, m_variables);
                new_branching_comand->message_for_user = message;

                QString command = return_code_element.attribute("continue_scen", "-");
                if(command != "-")
                {
                    Settings::replaceVariablesIn(command, m_variables);
                    new_branching_comand->type = 1;
                    new_branching_comand->command = command;
                    return new_branching_comand;
                }

                command = return_code_element.attribute("break_scen", "-");
                if(command != "-")
                {
                    Settings::replaceVariablesIn(command, m_variables);
                    new_branching_comand->type = 2;
                    new_branching_comand->command = command;
                    return new_branching_comand;
                }

                command = return_code_element.attribute("label_name", "-");
                if(command != "-")
                {
                    Settings::replaceVariablesIn(command, m_variables);
                    new_branching_comand->type = 3;
                    new_branching_comand->command = command;
                    return new_branching_comand;
                }


                new_branching_comand->type = 0;


                return new_branching_comand;
            }
        }
        else if(return_code_element.tagName().toUpper() == "DEFAULT_ERROR")
        {
            message = return_code_element.attribute("message");
            Settings::replaceVariablesIn(message, m_variables);
            new_branching_comand->message_for_user = message;
            new_branching_comand->type = 0;
            return new_branching_comand;
        }

        return_code_node.nextSibling();
    }
    qDebug() << "Ошибка в команде ветвления псевдоплана. Не задано сообщение об ошибке по умолчанию.";
    return nullptr;
}
