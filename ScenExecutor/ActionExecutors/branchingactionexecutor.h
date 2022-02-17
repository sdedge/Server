#ifndef BRANCHINGACTIONEXECUTOR_H
#define BRANCHINGACTIONEXECUTOR_H

#include <QObject>


#include "actionexecutor.h"

#include "../ActionParsers/startactionparser.h"


class BranchingActionExecutor : public ActionExecutor
{
    Q_OBJECT
public:
    BranchingActionExecutor();
    virtual ~BranchingActionExecutor();

    bool parseAction(QDomNode action_node, QList<Variable*> variables) override;

    void finish() override;

    bool isSimpleAction() override;

    virtual BranchingCommand* serchForBranch(QDomNode action_node, int return_code) override;

protected:

    bool parseActionElement(QDomElement action_element, QList<Variable*> variables) override;

    bool setCurrentActionParser() override;

    void addSystemInfoToDataPack() override;


    StartActionParser *m_start_action_parser;
};

#endif // BRANCHINGACTIONEXECUTOR_H
