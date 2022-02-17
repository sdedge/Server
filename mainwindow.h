#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>

#include "TCPServer.h"
#include "clientwidget.h"
#include "GlobalData/datapack.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class ScenExecutor;

struct Client              //клиент
{
    QString       name = "";
    QString       group= "";
    QString       role = "";
    QTcpSocket    *socket;
    ClientWidget  *clientWidget = 0;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    static MainWindow* getPtr();
    void initSystem();
signals:
    void sendAnswerPack(QTcpSocket*, DataPack);

public slots:
    void clientOn(QTcpSocket *socket);
    void clientOff(QTcpSocket *socket);
    void handleDataPackFromClient(QTcpSocket *socket, DataPack data);
    bool setScenClient(ScenExecutor *scen_executor);
    bool releaseScenClient(ScenExecutor *scen_executor);
    void printOnPlainTextEdit(QString text);

private slots:
    void on_actionStartScen_triggered();

private:
    Ui::MainWindow *ui;
    static MainWindow *m_ptr;
    TCPServer *server;
    QGridLayout *layout;
    QMap<quint32, Client*>  clients;
    int clients_number;

    void initInterface();
};
#endif // MAINWINDOW_H
