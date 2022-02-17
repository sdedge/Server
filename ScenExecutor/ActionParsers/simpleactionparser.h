#ifndef SIMPLEACTIONPARSER_H
#define SIMPLEACTIONPARSER_H

#include <QObject>

#include "actionparser.h"

class SimpleActionParser : public ActionParser
{
    Q_OBJECT
public:
    SimpleActionParser();
    virtual ~SimpleActionParser();

    virtual bool parseActionParameters(QDomElement parameters_element, DataPack **data_pack) = 0;
    virtual BranchingCommand* serchForBranch(QDomNode branching_node, int return_code) override;
};

#endif // SIMPLEACTIONPARSER_H
