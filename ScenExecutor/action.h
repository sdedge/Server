#ifndef ACTION_H
#define ACTION_H

#include <QObject>
#include <QDomDocument>

#include "ActionExecutors/simpleactionexecutor.h"
#include "ActionExecutors/branchingactionexecutor.h"

#include "GlobalData/datapack.h"



struct Variable;

class Action: public QObject
{
    Q_OBJECT
public:
    Action();
    ~Action();

    bool execAction(QDomNode action_node, QList<Variable*> variables);

    int getCurrentActionType();

    QString getMessage();

    bool canBeSkiped();

    DataPack getDataPack();

    bool isSimpleAction();

    BranchingCommand* serchForBranch(QDomNode action_node, int return_code);

    void finish();


private:

    bool selectExecutor(ActionType action_type);

    ActionExecutor *m_current_action_executor;

    SimpleActionExecutor *m_simple_action_executor;

    BranchingActionExecutor *m_branching_action_executor;
};

#endif // ACTION_H
