#ifndef COPYACTION_H
#define COPYACTION_H

#include <QObject>

#include "simpleactionparser.h"

class CopyActionParser : public SimpleActionParser
{
    Q_OBJECT
public:
    CopyActionParser();
    virtual ~CopyActionParser();
    bool parseActionParameters(QDomElement parameters_element, DataPack **data_pack) override;

private:
    DataPack* formDataPack(QString source, QString target);
};

#endif // COPYACTION_H
