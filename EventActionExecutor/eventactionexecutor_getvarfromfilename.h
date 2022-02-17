#ifndef EVENTACTIONEXECUTOR_GETVARFROMFILENAME_H
#define EVENTACTIONEXECUTOR_GETVARFROMFILENAME_H

#include <QObject>
#include "eventactionexecutor.h"

class EventActionExecutor_GetVarFromFileName : public EventActionExecutor
{
    Q_OBJECT
public:
    EventActionExecutor_GetVarFromFileName();
    virtual ~EventActionExecutor_GetVarFromFileName();
    
    virtual bool executeEvent(EventExecutionParameters *action_parameters) override;
};

#endif // EVENTACTIONEXECUTOR_GETVARFROMFILENAME_H
