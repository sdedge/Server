#ifndef ACTIONPARSER_H
#define ACTIONPARSER_H

#include <QObject>
#include <QDomDocument>

#include <QDebug>

#include "GlobalData/datapack.h"
#include "GlobalData/branchingcommand.h"

struct Variable;

class ActionParser: public QObject
{
    Q_OBJECT
public:
    ActionParser();
    virtual ~ActionParser();

    void setActionType(int type);
    void setVariables(QList<Variable*> variables);
    virtual bool parseActionParameters(QDomElement parameters_element, DataPack **data_pack) = 0;
    virtual BranchingCommand* serchForBranch(QDomNode branching_node, int return_code) = 0;


protected:
    int m_type;
    QList<Variable*> m_variables;
};

#endif // ACTIONPARSER_H
