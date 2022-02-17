#include "eventactionexecutor_getvarfromfilename.h"

#include <QDebug>


#include "settings.h"


EventActionExecutor_GetVarFromFileName::EventActionExecutor_GetVarFromFileName()
{

}

EventActionExecutor_GetVarFromFileName::~EventActionExecutor_GetVarFromFileName()
{

}

bool EventActionExecutor_GetVarFromFileName::executeEvent(EventExecutionParameters *action_parameters)
{

    QString variables_buffer = action_parameters->task_exec_parameters;

    Settings::replaceVariablesIn(variables_buffer, m_variables);

    QStringList arguments = variables_buffer.split(arguments_spacer);

    QString var_name = arguments.at(0);

    QString start_marker = arguments.at(1);

    QString finish_marker = arguments.at(2);

    QString file_name = Settings::searchVar(Settings::settings()->getVarNameForMessageFileName(), m_variables)->value;

    int start_var_position = file_name.lastIndexOf(start_marker);
    int finish_var_position = file_name.lastIndexOf(finish_marker);

    return Settings::settings()->setValueToVar(var_name, file_name.mid(start_var_position, finish_var_position - start_var_position));
}
