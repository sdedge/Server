#include "eventactionexecutor_unpackcontainer.h"

#include <QDebug>
#include "settings.h"
#include "QProcess"


EventActionExecutor_UnpackContainer::EventActionExecutor_UnpackContainer()
{

}

EventActionExecutor_UnpackContainer::~EventActionExecutor_UnpackContainer()
{

}

bool EventActionExecutor_UnpackContainer::executeEvent(EventExecutionParameters *action_parameters)
{

    QString variables_buffer = action_parameters->task_exec_parameters;

    Settings::replaceVariablesIn(variables_buffer, m_variables);

    QStringList arguments = variables_buffer.split(arguments_spacer);

    QString pack_path = arguments.at(0);

    QString unpack_path = arguments.at(1);

    SettingsParsers::createDirIfDoesNotExist(unpack_path);

    QString packer_prekeys = "";
    QString packer_afterkeys = pack_path + " -ep " + unpack_path;

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
        return true;
    else
        return false;
}
