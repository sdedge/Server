#ifndef EVENTACTIONEXECUTOR_COPYFILE_H
#define EVENTACTIONEXECUTOR_COPYFILE_H

#include <QObject>
#include "eventactionexecutor.h"

class EventActionExecutor_CopyFile : public EventActionExecutor
{
    Q_OBJECT
public:
    EventActionExecutor_CopyFile();
    virtual ~EventActionExecutor_CopyFile();
    
    virtual bool executeEvent(EventExecutionParameters *action_parameters) override;
};

#endif // EVENTACTIONEXECUTOR_COPYFILE_H
