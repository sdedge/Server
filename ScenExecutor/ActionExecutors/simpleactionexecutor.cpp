#include "simpleactionexecutor.h"
#include <QDebug>


extern bool boolFromString(QString str);


SimpleActionExecutor::SimpleActionExecutor()
{
    m_action_type = UNDEFINED;

    m_copy_action_parser = new CopyActionParser();
    m_delete_action_parser = new DeleteActionParser();
    m_get_action_parser = new GetActionParser();
    m_send_action_parser = new SendActionParser();
}

SimpleActionExecutor::~SimpleActionExecutor()
{
    if(m_copy_action_parser != nullptr)
    {
        delete m_copy_action_parser;
        m_copy_action_parser = nullptr;
    }

    if(m_delete_action_parser != nullptr)
    {
        delete m_delete_action_parser;
        m_delete_action_parser = nullptr;
    }

    if(m_get_action_parser != nullptr)
    {
        delete m_get_action_parser;
        m_get_action_parser = nullptr;
    }

    if(m_send_action_parser != nullptr)
    {
        delete m_send_action_parser;
        m_send_action_parser = nullptr;
    }

    if(m_data_pack != nullptr)
    {
        delete m_data_pack;
        m_data_pack = nullptr;
    }

}


bool SimpleActionExecutor::parseActionElement(QDomElement action_element, QList<Variable *> variables)
{
    m_message = action_element.attribute("message");
    m_can_be_skiped = boolFromString(action_element.attribute("skip", "0"));

    bool no_error;

    if(setCurrentActionParser())
    {
        m_current_action_parser->setVariables(variables);
        no_error = m_current_action_parser->parseActionParameters(action_element.firstChild().toElement(), &m_data_pack);
    }
    else
        no_error = false;

    return no_error;
}



bool SimpleActionExecutor::setCurrentActionParser()
{
    switch (m_action_type)
    {
        case CopyAction:
        {
            m_current_action_parser = m_copy_action_parser;
            return true;
            break;
        }
        case DeleteAction:
        {
            m_current_action_parser = m_delete_action_parser;
            return true;
            break;
        }
        case GetAction:
        {
            m_current_action_parser = m_get_action_parser;
            return true;
            break;
        }
        case SendAction:
        {
            m_current_action_parser = m_send_action_parser;
            return true;
            break;
        }
    default:
        return false;
    }
}


void SimpleActionExecutor::finish()
{
    m_current_action_parser = nullptr;

    ActionExecutor::finish();
}

bool SimpleActionExecutor::isSimpleAction()
{
    return true;
}

BranchingCommand *SimpleActionExecutor::serchForBranch(QDomNode action_node, int return_code)
{
    Q_UNUSED(action_node)
    Q_UNUSED(return_code)

    qDebug() << "Простые действия не позволяют обрабатывать ошибки.";
    return nullptr;
}


bool SimpleActionExecutor::parseAction(QDomNode action_node, QList<Variable *> variables)
{
    bool no_error = parseActionElement(action_node.toElement(), variables);
    if(no_error)
        addSystemInfoToDataPack();
    return no_error;
}


void SimpleActionExecutor::addSystemInfoToDataPack()
{
    ActionExecutor::addSystemInfoToDataPack();
}
