#ifndef SENDACTION_H
#define SENDACTION_H

#include <QObject>

#include "simpleactionparser.h"

class SendActionParser : public SimpleActionParser
{
    Q_OBJECT
public:
    SendActionParser();
    virtual ~SendActionParser();
    bool parseActionParameters(QDomElement parameters_element, DataPack **data_pack) override;

private:
    DataPack* formDataPack(QString source, QString target, QString notify);
};

#endif // SENDACTION_H
