#ifndef EVENTACTIONEXECUTOR_MAKECONTAINER_H
#define EVENTACTIONEXECUTOR_MAKECONTAINER_H

#include <QObject>
#include <QDir>
#include <QXmlStreamWriter>

#include "eventactionexecutor.h"

struct PackSettings;

struct SettingsForPackFile;

class EventActionExecutor_MakeContainer : public EventActionExecutor
{
    Q_OBJECT
public:
    EventActionExecutor_MakeContainer();
    virtual ~EventActionExecutor_MakeContainer();
    
    virtual bool executeEvent(EventExecutionParameters *action_parameters) override;

private:
    bool bruteSearch(QDir dir);
    bool prioritySearch(QDir dir, SettingsForPackFile *settings);
    void writeVarsForFile(QString relative_file_path, QString canonical_file_path,QStringList vars);


    PackSettings *pack_settings;

    QXmlStreamWriter *manual_file_writer;


};

#endif // EVENTACTIONEXECUTOR_MAKECONTAINER_H
