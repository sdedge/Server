#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QDebug>
#include <QPlainTextEdit>
#include <QLabel>
#include <QThread>
#include <QFileDialog>
#include <QNetworkInterface>

#include "TCPServer.h"
#include "settingsparsers.h"
#include "settings.h"


MainWindow* MainWindow::m_ptr = nullptr;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_ptr = this;
    clients_number = 1;

    ui->plainTextEdit->setReadOnly(true);

    initInterface();

}

MainWindow::~MainWindow()
{
    delete ui;
}

MainWindow *MainWindow::getPtr()
{
    return m_ptr;
}

void MainWindow::initSystem()
{
    Settings::settings();
    connect(Settings::settings(), &Settings::newText, this, &MainWindow::printOnPlainTextEdit);
    qRegisterMetaType <DataPack>("DataPack");

    QString main_settings_path = QApplication::applicationDirPath() + "/settings/settings_6.xml";

    SettingsParsers::parseSettings6(main_settings_path);

    QString other_settings_path = "";

    if(Settings::settings()->searchVar("settings_dir_path") == nullptr)
        return;
    else
        other_settings_path = Settings::settings()->searchVar("settings_dir_path")->value;


    SettingsParsers::parseSettings3(other_settings_path + "settings_3.xml");
    SettingsParsers::parseSettings1(other_settings_path + "settings_1.xml");
    SettingsParsers::parseSettings10(other_settings_path + "settings_10.xml");
    SettingsParsers::parseSettings5(other_settings_path + "settings_5.xml");
    SettingsParsers::parseSettings4(other_settings_path + "settings_4.xml");


    server = new TCPServer();

    connect(server, &TCPServer::getDataPackFromClient,  this,    &MainWindow::handleDataPackFromClient);
    connect(server, &TCPServer::newClient,              this,    &MainWindow::clientOn);
    connect(server, &TCPServer::lostClient,             this,    &MainWindow::clientOff);

    int port = Settings::settings()->portForClients();
    if(port != -1)
    {
        if(server->listen(port))
        {
            qDebug() << "Сервер запущен: порт " + QString::number(port);
        }
        else
        {
            qDebug() << "Сервер не запущен: порт " + QString::number(port);
        }
    }
    else
    {
        qDebug() << "Сервер не запущен. Некорректный порт.";
    }

    QThread *thread = new QThread();

    connect(thread,         &QThread::started,              Settings::settings()->timer(),    &Timer::run);
    connect(Settings::settings()->timer(),    &Timer::finished,    thread,         &QThread::terminate);

    Settings::settings()->timer()->moveToThread(thread);
    thread->start();


}

void MainWindow::initInterface()
{
    layout = new QGridLayout(ui->scrollAreaWidgetContents);

    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 5; ++j)
        {
            QLabel *label = new QLabel();
            label->setObjectName("label");
            label->setGeometry(0,0,128,128);
            label->setMaximumSize(128,128);
            label->setMinimumSize(128,128);
            label->setStyleSheet("QLabel#label"
                                 "{"
                                    "border: solid;"
                                    "border-style: outset;"
                                    "border-width: 2px;"
                                    "border-radius: 10px;"
                                    "border-color: grey;"
                                  "}");

            layout->addWidget(label,i,j);
        }
    }
}


//==============================================================================================================================================================
//подключение исполняющего клиента
//==============================================================================================================================================================
void MainWindow::clientOn(QTcpSocket *socket)
{
    quint32 ip = socket->peerAddress().toIPv4Address();
    if (clients.count(ip) == 0)                                                                 //если у клиента с таким ip нет виджета
    {
        Client *new_client = new Client;
        QString str = "widget_" + QString::number(clients_number);
        ClientWidget *new_client_widget = new ClientWidget(str, "Нет роли", clients_number);    //создание виджета нового клиента
        new_client->socket = socket;
        layout->removeWidget(layout->widget());                                                 //добавление его на рабочую область
        layout->addWidget(new_client_widget,(clients_number-1)/5,(clients_number-1)%5);
        new_client_widget->setColor(Yellow);
        new_client_widget->setStatus(false);
        new_client->clientWidget = new_client_widget;                                           //добавление указателя в структуру
        clients.insert(ip, new_client);                                                         //добавление структуры клиента в "карту" клиентов
        clients[ip]->clientWidget->setStatusClient(true);

        ++clients_number;
        if ((clients_number-1)/5 > 1)
        {
            for (int j = ((clients_number-1)%5); j < 5; ++j)
            {
                QLabel *label = new QLabel();
                label->setObjectName("label");
                label->setGeometry(0,0,128,128);
                label->setMaximumSize(128,128);
                label->setMinimumSize(128,128);
                label->setStyleSheet("QLabel#label"
                                     "{"
                                        "border: solid;"
                                        "border-style: outset;"
                                        "border-width: 2px;"
                                        "border-radius: 10px;"
                                        "border-color: grey;"
                                     "}");
                layout->addWidget(label,(clients_number-1)/5,j);
            }
        }
    }
    else                                                                                        //виджет есть
    {

        if (clients[ip]->socket == 0)                                                           //если нет информации о сокете (виджет создан, но не отображен)
        {
            ClientWidget *old_widget = clients[ip]->clientWidget;                               //получение указателя виджета нового клиента
            layout->removeWidget(layout->widget());                                             //добавление его на рабочую область
            layout->addWidget(old_widget,(clients_number-1)/5,(clients_number-1)%5);
            old_widget->setColor(Yellow);
            old_widget->setStatus(false);
            clients[ip]->clientWidget = old_widget;                                             //возвращение указателя в структуру

            ++clients_number;
            if ((clients_number-1)/5 > 1)
                for (int j = ((clients_number-1)%5); j < 5; ++j)
                {
                    QLabel *label = new QLabel();
                    label->setObjectName("label");
                    label->setGeometry(0,0,128,128);
                    label->setMaximumSize(128,128);
                    label->setMinimumSize(128,128);
                    label->setStyleSheet("QLabel#label"
                                         "{"
                                            "border: solid;"
                                            "border-style: outset;"
                                            "border-width: 2px;"
                                            "border-radius: 10px;"
                                            "border-color: grey;"
                                         "}");
                    layout->addWidget(label,(clients_number-1)/5,j);
                }
        }
        clients[ip]->socket = socket;
        clients[ip]->clientWidget->setStatusClient(true);
    }
    qDebug() << "Клиент подключился." << socket->peerAddress().toString();
}


//==============================================================================================================================================================
//отключение исполняющего клиента
//==============================================================================================================================================================
void MainWindow::clientOff(QTcpSocket *socket)
{
    quint32 ip = socket->peerAddress().toIPv4Address();
    if (clients.count(ip) != 0)
    {
        clients[ip]->clientWidget->setStatusClient(false);
        clients[ip]->clientWidget->setColor(Red);
    }
    qDebug() << "Клиент отключился." << ip;
}



//==============================================================================================================================================================
//обработка сообщений от клиентов
//==============================================================================================================================================================
void MainWindow::handleDataPackFromClient(QTcpSocket *socket, DataPack data)
{
    int ip = socket->peerAddress().toIPv4Address();
    if (data.type == 17)                                                                        //Сообщение от клиента о поступлении файла
    {
        QString file_name = data.source;
        QString file_path = data.attributes.first();

        Settings::settings()->setValueToVar(Settings::settings()->getVarNameForMessageFileName(), file_name);
        Settings::settings()->setValueToVar(Settings::settings()->getVarNameForMessageFilePath(), file_path);


        Settings::settings()->clientsFileExecutor()->getCurrentVariables();
        Settings::settings()->clientsFileExecutor()->setClientFileName(file_name);
        Settings::settings()->clientsFileExecutor()->setExecutorSocket(socket);
        QThread *thread = new QThread();

        connect(thread, &QThread::started, Settings::settings()->clientsFileExecutor(), &EventExecutor::run);
        connect(Settings::settings()->clientsFileExecutor(),    &EventExecutor::finished,    thread,         &QThread::terminate);

        Settings::settings()->clientsFileExecutor()->moveToThread(thread);
        thread->start();

    }
    else if(data.type == 19)                                                                    //Идентификация подключившегося клиента
    {
        clients[ip]->clientWidget->setName(data.name_PC);
        clients[ip]->name = data.name_PC;
        int slashPos  = data.message.lastIndexOf("/");
        QString group = data.message.mid(0, slashPos);
        QString role  = data.message.mid(slashPos + 1);
        if(Settings::settings()->groupsRolesMap().contains(group))
        {
            clients[ip]->group = group;
            if(Settings::settings()->groupsRolesMap()[group].contains(role))
            {
                clients[ip]->clientWidget->setRole(role);
                clients[ip]->role = role;
            }
            else
            {
                clients[ip]->clientWidget->setRole(Settings::settings()->defaultRolesMap()[group]);
                clients[ip]->role = Settings::settings()->defaultRolesMap()[group];
            }

            clients[ip]->clientWidget->setColor(Green);
            clients[ip]->clientWidget->setStatus(true);
        }
        else
        {
            clients[ip]->group = "???";
            clients[ip]->clientWidget->setRole("Ошибка");
            clients[ip]->role = "Ошибка";
            qDebug() << "Клиент " << ip << " подключился с неизвестной заданной рабочей группой";
        }
    }
    else if(data.type == 10)                                                                    //Работа с потоком клиента
    {
        emit sendAnswerPack(clients[ip]->socket, data);
    }
    else
    {
        qDebug() << "Необрабатываемый тип " << data.type;
    }
}

bool MainWindow::setScenClient(ScenExecutor *scen_executor)
{
    QString scen_name = scen_executor->scenFileName();
    qDebug() << "Поиск исполнителя сценария";
    QString executor_role = "";

    if(scen_executor->clientSocket() != nullptr)
    {
        connect(scen_executor, &ScenExecutor::commandPackFormed, server, &TCPServer::slotWriteClient);
        connect(this, &MainWindow::sendAnswerPack, scen_executor, &ScenExecutor::commandPackReceived);
        return true;
    }
    else
    {


        for (QMap<QString,QStringList> roles : Settings::settings()->groupsRolesMap())              //поиск роли для выполнения сценария
        {
            for (QStringList scens: roles)
            {
                if(scens.contains(scen_name))
                {
                    executor_role = roles.key(scens);
                    break;
                }
            }
        }
        if(executor_role == "")                                                                     //если нет подходящей ролей назначить роль обработкчика по-умолчанию
        {
            executor_role = Settings::settings()->defaultExecutorRole();
        }

        for (Client *client : clients)                                                              //поиск подходящего клиента
        {
            if((client->clientWidget->getStatus()) && (client->role == executor_role))
            {
                scen_executor->setClientSocket(client->socket);
                connect(scen_executor, &ScenExecutor::commandPackFormed, server, &TCPServer::slotWriteClient);
                connect(this, &MainWindow::sendAnswerPack, scen_executor, &ScenExecutor::commandPackReceived);
                return true;
            }
        }

    }
    qDebug() << "Не удалось найти исполнителя сценария.";
    return false;
}



bool MainWindow::releaseScenClient(ScenExecutor *scen_executor)
{
    QTcpSocket *executor_socket = scen_executor->clientSocket();
    qDebug() << "Отключение исполнителя по завершению сценария " << scen_executor->scenFileName();

    for (Client *client : clients)                                                              //поиск подходящего клиента
    {
        if(client->socket == executor_socket)
        {
            disconnect(scen_executor, &ScenExecutor::commandPackFormed, server, &TCPServer::slotWriteClient);
            disconnect(this, &MainWindow::sendAnswerPack, scen_executor, &ScenExecutor::commandPackReceived);
            return true;
        }
    }
    return false;
}

void MainWindow::printOnPlainTextEdit(QString text)
{
    ui->plainTextEdit->appendPlainText(text);
}

void MainWindow::on_actionStartScen_triggered()
{
    QString path = QFileDialog::getOpenFileName(this,tr("Открыть файл"),"",tr("XML Файл (*.xml)"));

    if(path != "")
    {
        EventExecutor *scen_executor = new EventExecutor();

        scen_executor->getCurrentVariables();

        QList<EventExecutionParameters *> execution_parameters;

        EventExecutionParameters *scen_parameters = new EventExecutionParameters();

        scen_parameters->skip = 0;
        scen_parameters->task_type = 1;
        scen_parameters->message_for_user = "Запуск пользовательского сценария вручную";
        scen_parameters->task_exec_parameters = path + "!0";

        execution_parameters.append(scen_parameters);

        scen_executor->executeActions(execution_parameters);

        delete scen_parameters;
        delete scen_executor;
    }
}
