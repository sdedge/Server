#ifndef SCENEXECUTOR_H
#define SCENEXECUTOR_H

#include <QObject>
#include <QFile>
#include <QTimer>
#include <QEventLoop>
#include <QTcpSocket>

#include "action.h"

class EventExecutor;

struct Variable;

class ScenExecutor : public QObject
{
    Q_OBJECT
public:
    explicit ScenExecutor(QList<Variable*> variables, EventExecutor *initializer, QString wait_list_id);
    virtual ~ScenExecutor();

    void setClientSocket(QTcpSocket* client_socket);
    QTcpSocket* clientSocket();
    bool executeScen(QFile &scen_file);
    void setScenName(QString scen_name);
    QString scenFileName() const;

private:
    bool answerHandling(QDomNode &current_node);
    bool isActionComplete(DataPack data_pack, int *return_code);

    void startClientWorkThread(QTcpSocket *socket);
    void stopClientWorkThread(QTcpSocket *socket);

    Action *m_action;
    QList<Variable*> m_variables;
    QString scen_file_name;

    QEventLoop wait_for_answer_event_loop;
    QTimer     wait_for_answer_timer;
    DataPack   answer_data_pack;
    QDomDocument scen_document;
    bool break_scen;

    EventExecutor *m_initializer;

    QString m_wait_list_id;

    QTcpSocket *m_client_socket;
public slots:
    void commandPackReceived(QTcpSocket* client_socket, DataPack data_pack);

signals:
    void commandPackFormed(QTcpSocket* client_socket, DataPack data_pack);
    void receivedAnswer();
};

#endif // SCENEXECUTOR_H
