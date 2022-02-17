#include "eventactionexecutor_parsefile.h"

#include <QDebug>

#include "settings.h"



EventActionExecutor_ParseFile::EventActionExecutor_ParseFile()
{

}

EventActionExecutor_ParseFile::~EventActionExecutor_ParseFile()
{

}

bool EventActionExecutor_ParseFile::executeEvent(EventExecutionParameters *action_parameters)
{



    QString variables_buffer = action_parameters->task_exec_parameters;

    Settings::replaceVariablesIn(variables_buffer, m_variables);

    QString file_path = variables_buffer;


    if(Settings::settings()->planParseRules() == nullptr)
    {
        qDebug() << "Отсутствуют настройки для разбора файла " + file_path;
        return false;
    }

    QString errorStr;
    int errorLine;
    int errorColumn;

    QFile plan_file(file_path);
    if(plan_file.exists())
    {
        QDomDocument plan_document;
        if(!plan_document.setContent(&plan_file, true, &errorStr, &errorLine, &errorColumn))
        {
            QString error_text = "Ошибка при разборе файла " + file_path + " : \n Строка "
                                 + QString::number(errorLine) + ", символ " + QString::number(errorColumn) + "\n" +
                                 errorStr;
            qDebug() << error_text;
            return false;
        }

        if(plan_document.documentElement().tagName().toUpper() == Settings::settings()->planParseRules()->plan_tag.toUpper())
        {
            QDomElement root = plan_document.documentElement();
            QDomNode work_program_node = root.firstChild();

            WorkProgram *new_work_program =  new WorkProgram();

            while(!work_program_node.isNull())
            {
                QString date_time_format = "dd.MM.yyyy hh:mm:ss";
                if(work_program_node.toElement().tagName().toUpper() == Settings::settings()->planParseRules()->work_program_tag.toUpper())
                {
                    new_work_program->index = work_program_node.toElement().attribute(Settings::settings()->planParseRules()->work_program_parameter_index).toInt();
                    new_work_program->begin_time = QDateTime::fromString(work_program_node.toElement().attribute(Settings::settings()->planParseRules()->work_program_parameter_begin_time), date_time_format);
                    new_work_program->end_time = QDateTime::fromString(work_program_node.toElement().attribute(Settings::settings()->planParseRules()->work_program_parameter_end_time), date_time_format);

                    Settings::setValueToVar(Settings::settings()->getVarNameForWorkProgramCurrentIndex(), QString::number(new_work_program->index), m_variables);

                    QDomNode seance_node = work_program_node.firstChild();

                    while(!seance_node.isNull())
                    {
                        if(seance_node.toElement().tagName().toUpper() == Settings::settings()->planParseRules()->drop_tag.toUpper())
                        {
                            Drop *new_drop = new Drop();

                            new_drop->drop_index = seance_node.toElement().attribute(Settings::settings()->planParseRules()->drop_parameter_drop_index).toInt();
                            new_drop->circle_index = seance_node.toElement().attribute(Settings::settings()->planParseRules()->drop_parameter_circle_index).toInt();
                            new_drop->begin_time = QDateTime::fromString(seance_node.toElement().attribute(Settings::settings()->planParseRules()->drop_parameter_begin_time), date_time_format);
                            new_drop->end_time = QDateTime::fromString(seance_node.toElement().attribute(Settings::settings()->planParseRules()->drop_parameter_end_time), date_time_format);

                            new_work_program->drops.append(new_drop);

                        }
                        else
                        {
                            QString error_text = "Ошибка при чтении файла плана: неверное имя тега секции сеанса. \n Строка "
                                                 + QString::number(seance_node.lineNumber()) + ", символ " + QString::number(seance_node.columnNumber()) + ".";
                            qDebug() << error_text;
                            return false;
                        }
                        seance_node = seance_node.nextSibling();
                    }
                }
                else
                {
                    QString error_text = "Ошибка при чтении файла плана: неверное имя тега секции рабочей программы. \n Строка "
                                         + QString::number(work_program_node.lineNumber()) + ", символ " + QString::number(work_program_node.columnNumber()) + ".";
                    qDebug() << error_text;
                    return false;
                }

                Settings::settings()->timer()->insertNewWorkProgram(new_work_program);

                work_program_node = work_program_node.nextSibling();
            }
            return true;

        }
        else
        {
            QString error_text = "Ошибка при чтении файла плана: неверное имя главного тега.";
            qDebug() << error_text;
            return false;
        }
    }
    else
    {
        QString error_text = "Ошибка при чтении файла плана: файл плана отсутствует.";
        qDebug() << error_text  << file_path;
        return false;
    }
    return false;
}
