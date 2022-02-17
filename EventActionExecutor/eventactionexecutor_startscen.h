#ifndef EVENTACTIONEXECUTOR_STARTSCEN_H
#define EVENTACTIONEXECUTOR_STARTSCEN_H

#include <QObject>
#include "eventactionexecutor.h"

class EventActionExecutor_StartScen : public EventActionExecutor
{
    Q_OBJECT
public:
    EventActionExecutor_StartScen();
    virtual ~EventActionExecutor_StartScen();
    
    virtual bool executeEvent(EventExecutionParameters *action_parameters) override;
};

#endif // EVENTACTIONEXECUTOR_STARTSCEN_H
