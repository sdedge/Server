#ifndef BRANCHINGACTIONPARSER_H
#define BRANCHINGACTIONPARSER_H

#include <QObject>

#include "actionparser.h"


class BranchingActionParser : public ActionParser
{
    Q_OBJECT
public:
    BranchingActionParser();
    virtual ~BranchingActionParser();

    virtual bool parseActionParameters(QDomElement parameters_element, DataPack **data_pack) = 0;
    virtual BranchingCommand* serchForBranch(QDomNode branching_node, int return_code) = 0;

};

#endif // BRANCHINGACTIONPARSER_H
