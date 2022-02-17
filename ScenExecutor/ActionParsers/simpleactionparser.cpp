#include "simpleactionparser.h"

#include <QDebug>

SimpleActionParser::SimpleActionParser()
{

}

SimpleActionParser::~SimpleActionParser()
{

}

BranchingCommand *SimpleActionParser::serchForBranch(QDomNode branching_node, int return_code)
{
    Q_UNUSED(branching_node)
    Q_UNUSED(return_code)

    qDebug() << "Ошибка. Невозможно осуществить ветвление процесса на простом действии.";
    return nullptr;
}
