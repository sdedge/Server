#ifndef DELETEACTION_H
#define DELETEACTION_H

#include <QObject>

#include "simpleactionparser.h"

class DeleteActionParser : public SimpleActionParser
{
    Q_OBJECT
public:
    DeleteActionParser();
    virtual ~DeleteActionParser();
    bool parseActionParameters(QDomElement parameters_element, DataPack **data_pack) override;

private:
    DataPack* formDataPack(QString exec_attributes);
};

#endif // DELETEACTION_H
