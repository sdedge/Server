#ifndef RECEIVINGFILEEXECUTOR_H
#define RECEIVINGFILEEXECUTOR_H

#include <QObject>
#include <QFile>
#include <QMap>
#include <QList>
#include <QTcpSocket>


#include "EventActionExecutor/eventactionexecutor_copyfile.h"
#include "EventActionExecutor/eventactionexecutor_deletefile.h"
#include "EventActionExecutor/eventactionexecutor_getvarfromfilename.h"
#include "EventActionExecutor/eventactionexecutor_makecontainer.h"
#include "EventActionExecutor/eventactionexecutor_parsefile.h"
#include "EventActionExecutor/eventactionexecutor_parsemanual.h"
#include "EventActionExecutor/eventactionexecutor_startscen.h"
#include "EventActionExecutor/eventactionexecutor_replacefile.h"
#include "EventActionExecutor/eventactionexecutor_unpackcontainer.h"
#include "EventActionExecutor/eventactionexecutor_varsarifmethicoperations.h"

struct Variable;


class EventExecutor : public QObject
{
    Q_OBJECT
public:
    void run();

    explicit EventExecutor();

    virtual ~EventExecutor();

    void setExecutionSettings(QList<ReceivingFileActions*> settings);

    bool executeFileReceiving(QString receiving_file_name);

    bool executeActions(QList<EventExecutionParameters*> execution_parameters);

    void setJunkDir(QString junk_path);

    void getCurrentVariables();

    QTcpSocket* getExecutorSocket() const;

    void insertNewTaskAfterCurrentTask(EventExecutionParameters* new_task);

    void setExecutorSocket(QTcpSocket *executor_socket);

    void setClientFileName(QString client_file_name);


private:

    QList<EventExecutionParameters *> searchParametersForFileName(QString file_name);

    bool selectActionExecutor(int action_type);

 /*\\имя файла\\соответствующие имени настройки обработки\\*/
    QList<ReceivingFileActions*> m_settings;

    QList<EventExecutionParameters*> current_execution_parameters;

    EventActionExecutor *m_current_action_executor;

    QString m_junk_path;

    QList<Variable*> variables_copy;

    EventExecutionParameters* current_task;

    QTcpSocket *m_executor_socket;

    QString    m_client_file_name;

signals:
    void finished();
};

#endif // RECEIVINGFILEEXECUTOR_H
