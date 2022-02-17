#ifndef EVENTACTIONEXECUTOR_REPLACEFILE_H
#define EVENTACTIONEXECUTOR_REPLACEFILE_H

#include <QObject>
#include "eventactionexecutor.h"

class EventActionExecutor_ReplaceFile : public EventActionExecutor
{
    Q_OBJECT
public:
    EventActionExecutor_ReplaceFile();
    virtual ~EventActionExecutor_ReplaceFile();
    
    virtual bool executeEvent(EventExecutionParameters *action_parameters) override;
};

#endif // EVENTACTIONEXECUTOR_REPLACEFILE_H
