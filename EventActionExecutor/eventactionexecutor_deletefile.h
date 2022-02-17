#ifndef EVENTACTIONEXECUTOR_DELETEFILE_H
#define EVENTACTIONEXECUTOR_DELETEFILE_H

#include <QObject>
#include "eventactionexecutor.h"

class EventActionExecutor_DeleteFile : public EventActionExecutor
{
    Q_OBJECT
public:
    EventActionExecutor_DeleteFile();
    virtual ~EventActionExecutor_DeleteFile();
    
    virtual bool executeEvent(EventExecutionParameters *action_parameters) override;
};

#endif // EVENTACTIONEXECUTOR_DELETEFILE_H
