#ifndef EVENTACTIONEXECUTOR_UNPACKCONTAINER_H
#define EVENTACTIONEXECUTOR_UNPACKCONTAINER_H

#include <QObject>
#include "eventactionexecutor.h"

class EventActionExecutor_UnpackContainer : public EventActionExecutor
{
    Q_OBJECT
public:
    EventActionExecutor_UnpackContainer();
    virtual ~EventActionExecutor_UnpackContainer();
    
    virtual bool executeEvent(EventExecutionParameters *action_parameters) override;
};

#endif // EVENTACTIONEXECUTOR_UNPACKCONTAINER_H
