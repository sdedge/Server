#include "eventactionexecutor_parsemanual.h"


#include <QDebug>
#include <QList>
#include <QProcess>
#include <QEventLoop>

#include "settings.h"




EventActionExecutor_ParseManual::EventActionExecutor_ParseManual()
{

}

EventActionExecutor_ParseManual::~EventActionExecutor_ParseManual()
{

}

bool EventActionExecutor_ParseManual::executeEvent(EventExecutionParameters *action_parameters)
{
    QString variables_buffer = action_parameters->task_exec_parameters;

    Settings::replaceVariablesIn(variables_buffer, m_variables);

    QString manual_path = variables_buffer;


    QString errorStr;
    int errorLine;
    int errorColumn;

    QFile manual_file(manual_path);
    if(manual_file.exists())
    {
        QDomDocument manual_document;
        if(!manual_document.setContent(&manual_file, true, &errorStr, &errorLine, &errorColumn))
        {
            QString error_text = "Ошибка при обработке справки упаковки " + manual_path + " : \n Строка "
                                 + QString::number(errorLine) + ", символ " + QString::number(errorColumn) + "\n" +
                                 errorStr;
            qDebug() << error_text;
            return false;
        }

        if(manual_document.documentElement().tagName().toUpper() == "UNPACK_MANUAL")
        {
            QDomElement root = manual_document.documentElement();
            QDomNode manual_node = root.firstChild();

            QString pack_path = "";
            QString unpack_path = "";
            QString filter_path;

            QList<FileToFilter*> files_to_filter;

            while(!manual_node.isNull())
            {
                if(manual_node.toElement().tagName().toUpper() == "PACK_DATA_FILE")
                {
                    QString current_path = manual_path.left(manual_path.lastIndexOf("/"));
                    pack_path = current_path + "/" + manual_node.toElement().attribute("name");
                }
                else if(manual_node.toElement().tagName().toUpper() == "UNPACK")
                {
                    unpack_path = manual_node.toElement().attribute("path");
                }
                else if(manual_node.toElement().tagName().toUpper() == "FILTER")
                {
                    filter_path = manual_node.toElement().attribute("path");
                }
                else if(manual_node.toElement().tagName().toUpper() == "COPY_TO_FILTER")
                {
                    QDomNode file_node = manual_node.firstChild();
                    while (!file_node.isNull())
                    {
                        if(file_node.toElement().tagName().toUpper() == "FILE")
                        {
                            FileToFilter *file_to_filter = new FileToFilter();
                            QString file_relative_path = file_node.toElement().attribute("path");
                            file_relative_path = file_relative_path.remove(0,1);
                            file_to_filter->file_path = unpack_path + file_relative_path;
                            file_to_filter->parameters = file_node.toElement().attribute("vars");
                            files_to_filter.append(file_to_filter);
                        }
                        else
                        {
                            QString error_text = "Ошибка при обработке справки упаковки: неверное имя тега. \n Строка "
                                                 + QString::number(file_node.lineNumber()) + ", символ " + QString::number(file_node.columnNumber()) + ".";
                            qDebug() << error_text;
                            return false;
                        }
                        file_node = file_node.nextSibling();
                    }
                }
                else
                {
                    QString error_text = "Ошибка при обработке справки упаковки: неверное имя тега. \n Строка "
                                         + QString::number(manual_node.lineNumber()) + ", символ " + QString::number(manual_node.columnNumber()) + ".";
                    qDebug() << error_text;
                    return false;
                }


                manual_node = manual_node.nextSibling();
            }

            if((pack_path != "") && (unpack_path != ""))
            {
                if(unpack(pack_path, unpack_path) == false)
                    return false;
            }
            else
                return false;

            if((filter_path != "") && (files_to_filter.size() > 0))
            {
                if(copyFilesToFilter(filter_path, files_to_filter) == false)
                    return false;
            }
            else
                return false;


            return true;
        }
        else
        {
            QString error_text = "Ошибка при обработке справки упаковки: неверное имя главного тега.";
            qDebug() << error_text;
            return false;
        }
    }
    else
    {
        QString error_text = "Ошибка при обработке справки упаковки: файл справки упаковки отсутствует.";
        qDebug() << error_text  << manual_path;
        return false;
    }
    return false;
}

bool EventActionExecutor_ParseManual::unpack(QString pack_path, QString unpack_path)
{
    SettingsParsers::createDirIfDoesNotExist(unpack_path);

    QString packer_prekeys = "";
    QString packer_afterkeys = pack_path + " " + unpack_path;

    Settings::settings()->setValueToVar("PPK", packer_prekeys, m_variables);

    Settings::settings()->setValueToVar("PAK", packer_afterkeys, m_variables);

    QString packer_command_buffer = Settings::settings()->unpackCommand();

    if(Settings::replaceVariablesIn(packer_command_buffer, m_variables) == false)
    {
        return false;
    }

    QStringList packer_command = packer_command_buffer.split(QRegExp("\\s+"));

    QString packer_program_path = packer_command.at(0);
    packer_command.removeAt(0);


    QProcess archiver_process;

    archiver_process.start(packer_program_path, packer_command);




    while(!archiver_process.waitForFinished())
    {
        QEventLoop loop;                                                                //
        QTimer timer;                                                                   //
        timer.setInterval(1000);                                                        //   небольшая задержка, чтобы не грузить ЦП
        connect (&timer, &QTimer::timeout, &loop, &QEventLoop::quit);                   //
        timer.start();                                                                  //
        loop.exec();                                                                    //
    }


    if(archiver_process.exitCode() == 0)
    {
        return QFile::remove(pack_path + ".rar");
    }
    else
        return false;
}

bool EventActionExecutor_ParseManual::copyFilesToFilter(QString filter_path, QList<EventActionExecutor_ParseManual::FileToFilter*> files_to_filter)
{

    for(FileToFilter* file_to_filter: files_to_filter)
    {
        QStringList variables_parameters = file_to_filter->parameters.split(QRegExp("\\s*!\\s*"), QString::SkipEmptyParts);

        for(QString variable_parameter : variables_parameters)
        {
            QStringList parameters = variable_parameter.split(QRegExp("\\s*,\\s*"));

            QString variable_type  = parameters.at(0);
            QString variable_name  = parameters.at(1);
            QString variable_value = parameters.at(2);

            Settings::settings()->setValueToVar(variable_name, variable_value);
        }

        QString file_name =  file_to_filter->file_path.right(file_to_filter->file_path.size() - file_to_filter->file_path.lastIndexOf("/"));
        if(QFile::copy(file_to_filter->file_path, filter_path + file_name) == false)
        {
            return false;
        }
        QEventLoop loop;                                                                //
        QTimer timer;                                                                   //
        timer.setInterval(1000);                                                        //   небольшая задержка, на обработку
        connect (&timer, &QTimer::timeout, &loop, &QEventLoop::quit);                   //
        timer.start();                                                                  //
        loop.exec();
    }
    return true;
}
