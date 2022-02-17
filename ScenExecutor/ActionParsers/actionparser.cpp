#include "actionparser.h"


ActionParser::ActionParser()
{
    //qDebug() << "constr ActionParser";
}

ActionParser::~ActionParser()
{

}


void ActionParser::setActionType(int type)
{
    m_type = type;
}

void ActionParser::setVariables(QList<Variable*> variables)
{
    m_variables = variables;
}
