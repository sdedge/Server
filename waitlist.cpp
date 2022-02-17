#include "waitlist.h"
#include <QTimer>

WaitList::WaitList(QString id)
{
    list_id = id;
    delay = 10000;
}

WaitList::~WaitList()
{

}

void WaitList::addScen(QString scen_name)
{
    scen_list.append(scen_name);
}

void WaitList::removeScen(QString scen_name)
{
    scen_list.removeOne(scen_name);
    if(scen_list.isEmpty())
    {
        QTimer empty_timer;
        empty_timer.singleShot(delay, this, &WaitList::lastCheck);
    }
}

void WaitList::setDelay(int msec)
{
    delay = msec;
}

QString WaitList::listId() const
{
    return list_id;
}

void WaitList::lastCheck()
{
    if(scen_list.isEmpty())
        emit listIsEmpty(list_id);
}
