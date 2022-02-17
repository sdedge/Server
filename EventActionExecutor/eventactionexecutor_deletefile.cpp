#include "eventactionexecutor_deletefile.h"
#include <QDebug>
#include <QFile>

#include "settings.h"


EventActionExecutor_DeleteFile::EventActionExecutor_DeleteFile()
{

}

EventActionExecutor_DeleteFile::~EventActionExecutor_DeleteFile()
{

}

bool EventActionExecutor_DeleteFile::executeEvent(EventExecutionParameters *action_parameters)
{

    QString variables_buffer = action_parameters->task_exec_parameters;

    Settings::replaceVariablesIn(variables_buffer, m_variables);

    QString delete_path = variables_buffer;


    bool success = false;

    if(QFile::remove(delete_path) || (!QFile::exists(delete_path)))
    {
        success = true;
    }

    return success | action_parameters->skip;
}
