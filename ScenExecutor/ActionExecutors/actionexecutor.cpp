#include "actionexecutor.h"
#include <QDebug>

#include "settings.h"

ActionExecutor::ActionExecutor()
{

}

ActionExecutor::~ActionExecutor()
{

}

void ActionExecutor::setActionType(ActionType action_type)
{
    m_action_type = action_type;
}

int ActionExecutor::getCurrentActionType()
{
    if(m_action_type != UNDEFINED)
        return m_action_type;
    else
    {
        qDebug() << "Не удалось получить тип действия.";

    }
    return -1;
}

DataPack ActionExecutor::getDataPack()
{
    if(m_action_type != UNDEFINED)
        return *m_data_pack;
    else
    {
        qDebug() << "Не удалось получить упаковку с командой.";
    }
    return DataPack();
}

QString ActionExecutor::getMessage()
{
    if(m_action_type != UNDEFINED)
        return m_message;
    else
    {
       qDebug() << "Не удалось получить сообщение для пользователя.";
    }
    return "";
}

bool ActionExecutor::canBeSkiped()
{
    if(m_action_type != UNDEFINED)
        return m_can_be_skiped;
    else
    {
        qDebug() << "Не удалось получить информацию о возможности пропуска команды.";
    }
    return false;
}


void ActionExecutor::addSystemInfoToDataPack()
{
    m_data_pack->time = QTime::currentTime();
    m_data_pack->message = m_message;
}

void ActionExecutor::finish()
{
    m_action_type = UNDEFINED;
    m_message = "";
    m_can_be_skiped = false;
    if(m_data_pack != nullptr)
    {
        delete m_data_pack;
        m_data_pack = nullptr;
    }
}

