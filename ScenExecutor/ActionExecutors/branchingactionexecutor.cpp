#include "branchingactionexecutor.h"

#include "../ActionParsers/branchingactionparser.h"
#include <QDebug>


extern bool boolFromString(QString str);

BranchingActionExecutor::BranchingActionExecutor()
{
    m_action_type = UNDEFINED;

    m_start_action_parser = new StartActionParser();
}

BranchingActionExecutor::~BranchingActionExecutor()
{
    if(m_start_action_parser != nullptr)
    {
        delete m_start_action_parser;
        m_start_action_parser = nullptr;
    }

    if(m_data_pack != nullptr)
    {
        delete m_data_pack;
        m_data_pack = nullptr;
    }
}


bool BranchingActionExecutor::parseActionElement(QDomElement action_element, QList<Variable*> variables)
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



bool BranchingActionExecutor::setCurrentActionParser()
{
    switch (m_action_type)
    {
        case StartAction:
        {
            m_current_action_parser = m_start_action_parser;
            return true;
            break;
        }
    default:
        return false;
    }
}


void BranchingActionExecutor::finish()
{
    m_current_action_parser = nullptr;

    ActionExecutor::finish();
}

bool BranchingActionExecutor::isSimpleAction()
{
    return false;
}



bool BranchingActionExecutor::parseAction(QDomNode action_node, QList<Variable *> variables)
{
    bool no_error = parseActionElement(action_node.toElement(), variables);
    if(no_error)
        addSystemInfoToDataPack();
    return no_error;
}


BranchingCommand *BranchingActionExecutor::serchForBranch(QDomNode action_node, int return_code)
{
    QDomNode branching_node = action_node.firstChild().nextSibling();
    return m_current_action_parser->serchForBranch(branching_node, return_code);
}

void BranchingActionExecutor::addSystemInfoToDataPack()
{
    ActionExecutor::addSystemInfoToDataPack();
}
