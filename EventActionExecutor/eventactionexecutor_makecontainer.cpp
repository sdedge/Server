#include "eventactionexecutor_makecontainer.h"


#include <QDebug>

#include "settings.h"
#include <QProcess>


EventActionExecutor_MakeContainer::EventActionExecutor_MakeContainer()
{

}

EventActionExecutor_MakeContainer::~EventActionExecutor_MakeContainer()
{

}

bool EventActionExecutor_MakeContainer::executeEvent(EventExecutionParameters *action_parameters)
{

    QString variables_buffer = action_parameters->task_exec_parameters;

    Settings::replaceVariablesIn(variables_buffer, m_variables);

    QString settings_7_file_path = variables_buffer;

    pack_settings = SettingsParsers::parseSettings7(settings_7_file_path);


    if(pack_settings == nullptr)
    {
        return false;
    }

    QDir search_dir(pack_settings->search_dir_path);


    QFile manual_file(pack_settings->pack_creation_dir_path + pack_settings->manual_file_name);

    if(manual_file.open(QIODevice::WriteOnly) != true)
    {
        qDebug() << "Не открылся файл справки " + manual_file.fileName();
        return false;
    }

    manual_file_writer = new QXmlStreamWriter(&manual_file);

    manual_file_writer->setAutoFormatting(true);
    manual_file_writer->writeStartElement("unpack_manual");

        manual_file_writer->writeStartElement("pack_data_file");
        manual_file_writer->writeAttribute("name", pack_settings->pack_data_file_name);
        manual_file_writer->writeEndElement();

        manual_file_writer->writeStartElement("unpack");
        manual_file_writer->writeAttribute("path", pack_settings->search_dir_path);
        manual_file_writer->writeEndElement();

        manual_file_writer->writeStartElement("filter");
        manual_file_writer->writeAttribute("path", pack_settings->filter_path);
        manual_file_writer->writeEndElement();

        manual_file_writer->writeStartElement("copy_to_filter");


    //копирование файлов для упаковки в отдельную директорию и добавление информации в справку
    if(pack_settings->search_algoritm_type == "0")
    {
        for(SettingsForPackFile *settings : pack_settings->settings_for_pack_files)
        {
            prioritySearch(search_dir, settings);
        }
    }
    else if(pack_settings->search_algoritm_type == "1")
    {
        bruteSearch(search_dir);
    }
    else
    {
        qDebug() << "Неверный тип алгоритма поиска";
        return false;
    }


        manual_file_writer->writeEndElement();

    manual_file_writer->writeEndElement();

    manual_file.close();

    delete manual_file_writer;

    QString packer_prekeys = "";

    QString packer_afterkeys =  pack_settings->pack_creation_dir_path + pack_settings->pack_data_file_name + " -r -ep -x"
                                + pack_settings->manual_file_name +
                                + " " + pack_settings->pack_creation_dir_path;

    Settings::settings()->setValueToVar("PPK", packer_prekeys, m_variables);

    Settings::settings()->setValueToVar("PAK", packer_afterkeys, m_variables);


    QString packer_command_buffer = Settings::settings()->packCommand();

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

    if(archiver_process.exitCode() != 0)
    {
        return false;
    }


    QString out_path = Settings::settings()->searchVar("OUT_dir_path")->value;

    packer_prekeys = "";
    packer_afterkeys = out_path + pack_settings->pack_data_manual_file_name + " -r -ep " +
                                pack_settings->pack_creation_dir_path;

    Settings::settings()->setValueToVar("PPK", packer_prekeys, m_variables);

    Settings::settings()->setValueToVar("PAK", packer_afterkeys, m_variables);

    packer_command_buffer = Settings::settings()->packCommand();

    if(Settings::replaceVariablesIn(packer_command_buffer, m_variables) == false)
    {
        return false;
    }

    packer_command = packer_command_buffer.split(QRegExp("\\s+"));

    packer_program_path = packer_command.at(0);
    packer_command.removeAt(0);

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

    delete pack_settings;
    if(archiver_process.exitCode() == 0)
        return true;
    else
        return false;
}



bool EventActionExecutor_MakeContainer::bruteSearch(QDir dir)
{
    for(QFileInfo file_info : dir.entryInfoList(QDir::Filter::NoDotAndDotDot | QDir::Filter::Dirs | QDir::Filter::Files, QDir::SortFlag::NoSort))
    {
        if(file_info.isFile())
        {
            for(SettingsForPackFile *settings : pack_settings->settings_for_pack_files)
            {
                if(settings->file_name_reg_exp.exactMatch(file_info.fileName()))
                {
                    if(QFile::copy(file_info.canonicalFilePath(), pack_settings->pack_creation_dir_path + file_info.fileName()) != true)
                    {
                        qDebug() << "Ошибка при сборка упаковки. Не удалось скопировать файл " + file_info.canonicalFilePath() + " в директорию " + pack_settings->pack_creation_dir_path;
                    }
                    else
                    {
                        QString canonical_file_path = file_info.canonicalFilePath();
                        QString relative_file_path = canonical_file_path.remove(dir.absolutePath());
                        settings->file_vars.remove('\r');
                        settings->file_vars.remove('\n');
                        settings->file_vars.remove('\t');
                        QStringList vars = settings->file_vars.split(QRegExp("\\s*!\\s*"));
                        writeVarsForFile(relative_file_path, canonical_file_path, vars);
                    }
                }
            }
        }
        else if(file_info.isDir())
        {
            QDir child_dir(file_info.canonicalFilePath());
            bruteSearch(child_dir);
        }
    }
    return true;
}

bool EventActionExecutor_MakeContainer::prioritySearch(QDir dir, SettingsForPackFile *settings)
{
    for(QFileInfo file_info : dir.entryInfoList(QDir::Filter::NoDotAndDotDot | QDir::Filter::Dirs | QDir::Filter::Files, QDir::SortFlag::NoSort))
    {
        if(file_info.isFile())
        {
            if(settings->file_name_reg_exp.exactMatch(file_info.fileName()))
            {
                if(QFile::copy(file_info.canonicalFilePath(), pack_settings->pack_creation_dir_path + file_info.fileName()) != true)
                {
                    qDebug() << "Ошибка при сборка упаковки. Не удалось скопировать файл " + file_info.canonicalFilePath() + " в директорию " + pack_settings->pack_creation_dir_path;
                }
                else
                {
                    QString canonical_file_path = file_info.canonicalFilePath();
                    QString relative_file_path = file_info.canonicalFilePath().remove(dir.absolutePath());
                    settings->file_vars.remove('\r');
                    settings->file_vars.remove('\n');
                    settings->file_vars.remove('\t');
                    QStringList vars = settings->file_vars.split(QRegExp("\\s*!\\s*"));
                    writeVarsForFile(relative_file_path, canonical_file_path, vars);
                }
            }
        }
        else if(file_info.isDir())
        {
            QDir child_dir(file_info.canonicalFilePath());
            prioritySearch(child_dir, settings);
        }
    }
    return true;
}

void EventActionExecutor_MakeContainer::writeVarsForFile(QString relative_file_path, QString canonical_file_path, QStringList vars)
{
    manual_file_writer->writeStartElement("file");

    manual_file_writer->writeAttribute("path", relative_file_path);

    QString var_string;

    for(QString var : vars)
    {
        Settings::replaceVariablesIn(var, m_variables);
        QStringList var_parameters = var.split(QRegExp("\\s*,\\s*"));


        QString var_type = var_parameters.at(0);
        QString var_name = var_parameters.at(1);
        QString var_begin_marker = var_parameters.at(2);

        int index_of_begin_marker = canonical_file_path.lastIndexOf(var_begin_marker);
        int index_of_end_marker = canonical_file_path.indexOf("/", index_of_begin_marker);

        QString var_value = canonical_file_path.mid(index_of_begin_marker + var_begin_marker.size(), index_of_end_marker - index_of_begin_marker - var_begin_marker.size());

        var_string += "!" + var_type + "," + var_name + "," + var_value;

    }


    manual_file_writer->writeAttribute("vars", var_string);
    manual_file_writer->writeEndElement();
}
