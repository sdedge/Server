#include "eventactionexecutor_varsarifmethicoperations.h"

#include <QDebug>
#include <QString>

#include "settings.h"


EventActionExecutor_VarsArifmethicOperations::EventActionExecutor_VarsArifmethicOperations()
{

}

EventActionExecutor_VarsArifmethicOperations::~EventActionExecutor_VarsArifmethicOperations()
{

}

bool EventActionExecutor_VarsArifmethicOperations::executeEvent(EventExecutionParameters *action_parameters)
{

    QString variables_buffer = action_parameters->task_exec_parameters;

    Settings::replaceVariablesIn(variables_buffer, m_variables);

    QStringList arguments = variables_buffer.split(arguments_spacer);

    QString var_name = arguments.at(0);

    QString arifmethic_operator = arguments.at(1);

    QString number = arguments.at(2);

    bool success = false;

    Variable *var = Settings::settings()->searchVar(var_name);

    if(var->type != VariableType::number)
    {
        qDebug() << "Выполнение арифметических операци с переменными, хранящими не числа не возможно.";
        return false;
    }

    int var_value = var->value.toInt();

    if(arifmethic_operator == "+")
    {
        var_value += number.toInt();
        Settings::settings()->setValueToVar(var_name, QString::number(var_value));
        success = true;
    }
    else if(arifmethic_operator == "-")
    {
        var_value -= number.toInt();
        Settings::settings()->setValueToVar(var_name, QString::number(var_value));
        success = true;
    }
    else
    {
        qDebug() << "Некорректный оператор " + arifmethic_operator;
        success = false;
    }

    return success | action_parameters->skip;
}
