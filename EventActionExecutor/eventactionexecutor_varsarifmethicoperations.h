#ifndef EVENTACTIONEXECUTOR_VARSARIFMETHICOPERATIONS_H
#define EVENTACTIONEXECUTOR_VARSARIFMETHICOPERATIONS_H

#include <QObject>
#include "eventactionexecutor.h"

class EventActionExecutor_VarsArifmethicOperations : public EventActionExecutor
{
    Q_OBJECT
public:
    EventActionExecutor_VarsArifmethicOperations();
    virtual ~EventActionExecutor_VarsArifmethicOperations();
    
    virtual bool executeEvent(EventExecutionParameters *action_parameters) override;
};

#endif // EVENTACTIONEXECUTOR_VARSARIFMETHICOPERATIONS_H
