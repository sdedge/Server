#include "action.h"
#include <QDebug>

#include "settings.h"

Action::Action()
{
    m_simple_action_executor = new SimpleActionExecutor();

    m_branching_action_executor = new BranchingActionExecutor();
}

Action::~Action()
{
    if(m_simple_action_executor != nullptr)
    {
        delete m_simple_action_executor;
        m_simple_action_executor = nullptr;
    }

    if(m_branching_action_executor)
    {
        delete m_branching_action_executor;
        m_branching_action_executor = nullptr;
    }
}

bool Action::execAction(QDomNode action_node, QList<Variable*> variables)
{
    QDomElement action_element = action_node.toElement();
    int type = action_element.attribute("type", "-1").toInt();
    if(selectExecutor(fromIntToActionType(type)) == false)
    {
        return false;
    }
    else
    {

        if(m_current_action_executor->parseAction(action_node, variables) == true)
        {
            return true;
        }
        return false;
    }
}

int Action::getCurrentActionType()
{
    return m_current_action_executor->getCurrentActionType();
}

DataPack Action::getDataPack()
{
    return m_current_action_executor->getDataPack();
}

bool Action::isSimpleAction()
{
    if(m_current_action_executor != nullptr)
    {
        return m_current_action_executor->isSimpleAction();
    }
    return true;
}

BranchingCommand* Action::serchForBranch(QDomNode action_node, int return_code)
{
    return m_current_action_executor->serchForBranch(action_node, return_code);
}

QString Action::getMessage()
{
    return m_current_action_executor->getMessage();
}

bool Action::canBeSkiped()
{
    return m_current_action_executor->canBeSkiped();
}


void Action::finish()
{
    m_current_action_executor->finish();
    m_current_action_executor = nullptr;
}

bool Action::selectExecutor(ActionType action_type)
{
    switch (action_type)
    {
        case CopyAction:
        {
            m_current_action_executor = m_simple_action_executor;
            m_current_action_executor->setActionType(action_type);
            return true;
        }
        case DeleteAction:
        {
            m_current_action_executor = m_simple_action_executor;
            m_current_action_executor->setActionType(action_type);
            return true;
        }
        case GetAction:
        {
            m_current_action_executor = m_simple_action_executor;
            m_current_action_executor->setActionType(action_type);
            return true;
        }
        case SendAction:
        {
            m_current_action_executor = m_simple_action_executor;
            m_current_action_executor->setActionType(action_type);
            return true;
        }
        case StartAction:
        {
            m_current_action_executor = m_branching_action_executor;
            m_current_action_executor->setActionType(action_type);
            return true;
        }
    default:
        return false;
    }
}

