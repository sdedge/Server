#ifndef STARTACTION_H
#define STARTACTION_H

#include <QObject>

#include "branchingactionparser.h"

class StartActionParser : public BranchingActionParser
{
    Q_OBJECT
public:
    StartActionParser();
    virtual ~StartActionParser();
    bool parseActionParameters(QDomElement parameters_element, DataPack **data_pack) override;
    BranchingCommand* serchForBranch(QDomNode branching_node, int return_code) override;

private:
    DataPack* formDataPack(QString application_path, QString working_time, QStringList attributes);
    QStringList normal_exit_codes;
};

#endif // STARTACTION_H
