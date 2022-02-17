#ifndef ACTIONEXECUTOR_H
#define ACTIONEXECUTOR_H

#include <QObject>


#include "../ActionParsers/actionparser.h"
#include "GlobalData/globalenums.h"
#include "GlobalData/branchingcommand.h"

struct Variable;

class ActionExecutor: public QObject
{
    Q_OBJECT
public:
    ActionExecutor();
    virtual ~ActionExecutor();

    virtual bool parseAction(QDomNode action_node, QList<Variable*> variables) = 0;

    void setActionType(ActionType action_type);

    int getCurrentActionType();

    QString getMessage();

    bool canBeSkiped();

    DataPack getDataPack();

    virtual bool isSimpleAction() = 0;

    virtual void finish();

    virtual BranchingCommand* serchForBranch(QDomNode action_node, int return_code) = 0;

protected:

    virtual void addSystemInfoToDataPack();

    virtual bool parseActionElement(QDomElement action_element, QList<Variable*> variables) = 0;

    virtual bool setCurrentActionParser() = 0;


    ActionParser *m_current_action_parser;
    ActionType m_action_type;
    QString m_message;
    bool m_can_be_skiped;
    DataPack *m_data_pack;
};

#endif // ACTIONEXECUTOR_H
