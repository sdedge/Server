#ifndef WAITLIST_H
#define WAITLIST_H

#include <QObject>

class WaitList : public QObject
{
    Q_OBJECT
public:
    WaitList(QString id);
    ~WaitList();

    void addScen(QString scen_name);
    void removeScen(QString scen_name);
    void setDelay(int msec);
    QString listId() const;

signals:
    void listIsEmpty(QString list_id);
private slots:
    void lastCheck();

private:
    QString list_id;
    QStringList scen_list;
    int delay;
};

#endif // WAITLIST_H
