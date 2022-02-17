#ifndef EVENTACTIONEXECUTOR_PARSEMANUAL_H
#define EVENTACTIONEXECUTOR_PARSEMANUAL_H

#include <QObject>
#include "eventactionexecutor.h"

class EventActionExecutor_ParseManual : public EventActionExecutor
{
    Q_OBJECT
public:
    EventActionExecutor_ParseManual();
    virtual ~EventActionExecutor_ParseManual();
    
    virtual bool executeEvent(EventExecutionParameters *action_parameters) override;
private:

    struct FileToFilter
    {
        QString file_path;
        QString parameters;
    };


    bool unpack(QString pack_path, QString unpack_path);
    bool copyFilesToFilter(QString filter_path, QList<FileToFilter *> files_to_filter);
};

#endif // EVENTACTIONEXECUTOR_PARSEMANUAL_H
