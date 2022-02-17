#include "eventactionexecutor_startscen.h"

#include <QDebug>
#include "settings.h"
#include "ScenExecutor/scenexecutor.h"

#include "mainwindow.h"

EventActionExecutor_StartScen::EventActionExecutor_StartScen()
{

}

EventActionExecutor_StartScen::~EventActionExecutor_StartScen()
{

}

bool EventActionExecutor_StartScen::executeEvent(EventExecutionParameters *action_parameters)
{

    QString variables_buffer = action_parameters->task_exec_parameters;

    Settings::replaceVariablesIn(variables_buffer, m_variables);

    QStringList arguments = variables_buffer.split(arguments_spacer);

    QString scen_path = arguments.at(0);

    QString wait_list_id = arguments.at(1);

    QString scen_name = scen_path.right(scen_path.size() - scen_path.lastIndexOf("/") - 1);

    QFile scen_file(scen_path);
    bool result = false;

    ScenExecutor scen_executor(m_variables, m_initializer, wait_list_id);
    scen_executor.setScenName(scen_name);

    scen_executor.setClientSocket(m_initializer->getExecutorSocket());

    if(MainWindow::getPtr()->setScenClient(&scen_executor) == true)
    {
        if(wait_list_id != "0")
        {
            Settings::settings()->insertScenInWaitList(scen_path, wait_list_id);
            result = scen_executor.executeScen(scen_file);
            Settings::settings()->removeScenFromWaitList(scen_path, wait_list_id);
        }
        else
        {
            result = scen_executor.executeScen(scen_file);
        }
        MainWindow::getPtr()->releaseScenClient(&scen_executor);
    }


    return result;
}
