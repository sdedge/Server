#ifndef EVENTACTIONEXECUTOR_PARSEFILE_H
#define EVENTACTIONEXECUTOR_PARSEFILE_H

#include <QObject>
#include "eventactionexecutor.h"

class EventActionExecutor_ParseFile : public EventActionExecutor
{
    Q_OBJECT
public:
    EventActionExecutor_ParseFile();
    virtual ~EventActionExecutor_ParseFile();
    
    virtual bool executeEvent(EventExecutionParameters *action_parameters) override;
};

#endif // EVENTACTIONEXECUTOR_PARSEFILE_H
