#ifndef GETACTION_H
#define GETACTION_H

#include <QObject>

#include "simpleactionparser.h"

class GetActionParser : public SimpleActionParser
{
    Q_OBJECT
public:
    GetActionParser();
    virtual ~GetActionParser();
    bool parseActionParameters(QDomElement parameters_element, DataPack **data_pack) override;

private:
    DataPack* formDataPack(QString source, QString target);
};

#endif // GETACTION_H
