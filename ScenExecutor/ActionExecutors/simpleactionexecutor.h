#ifndef SIMPLEACTIONEXECUTOR_H
#define SIMPLEACTIONEXECUTOR_H

#include <QObject>

#include "actionexecutor.h"

#include "../ActionParsers/copyactionparser.h"
#include "../ActionParsers/deleteactionparser.h"
#include "../ActionParsers/getactionparser.h"
#include "../ActionParsers/sendactionparser.h"


class SimpleActionExecutor : public ActionExecutor
{
    Q_OBJECT
public:
    SimpleActionExecutor();
    virtual ~SimpleActionExecutor();

    bool parseAction(QDomNode action_node, QList<Variable*> variables) override;

    void finish() override;

    bool isSimpleAction() override;

    virtual BranchingCommand* serchForBranch(QDomNode action_node, int return_code) override;

protected:

    bool parseActionElement(QDomElement action_element, QList<Variable*> variables) override;

    bool setCurrentActionParser() override;

    void addSystemInfoToDataPack() override;


    CopyActionParser *m_copy_action_parser;
    DeleteActionParser *m_delete_action_parser;
    GetActionParser *m_get_action_parser;
    SendActionParser *m_send_action_parser;
};


#endif // SIMPLEACTIONEXECUTOR_H
