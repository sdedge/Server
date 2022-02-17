#include "TCPServer.h"

// Типы:
// 8  - сообщение клиенту о завершении сценария
// 9  - сообщение - часть файла доставлена
// 10 - запрос/прием подтверждения выполнения команды
// 11 - отправка файла на клиент
// 12 - запуск приложения на клиенте
// 13 - получение файла от клиента
// 14 - отправление команды копирования
// 16 - отправление команды удаления
// 17 - сообщение о получении файла
// 19 - получение имени ПК и роли




TCPServer::TCPServer()
{
    m_tcp_server = new QTcpServer(this);

    connect(m_tcp_server, &QTcpServer::newConnection, this, &TCPServer::slotNewConnection);
    connect(this, SIGNAL(readyForNextPart(QTcpSocket*, DataPack)), this, SLOT(sendPartOfFile(QTcpSocket*, DataPack)));
}

TCPServer::~TCPServer()
{
    if(m_tcp_server != nullptr)
    {
        delete m_tcp_server;
        m_tcp_server = nullptr;
    }
}

bool TCPServer::listen(int port)
{
    if(m_tcp_server != nullptr)
    {
        return m_tcp_server->listen(QHostAddress::Any, port);
    }
    return false;
}




//==============================================================================================================================================================
//слот подключения нового клиента к серверу
//==============================================================================================================================================================
void TCPServer::slotNewConnection()
{
    QTcpSocket* client_tcp_socket = new QTcpSocket;

    client_tcp_socket = m_tcp_server->nextPendingConnection();

    connect(client_tcp_socket, &QTcpSocket::readyRead,       this, &TCPServer::slotReadClient);
    connect(client_tcp_socket, &QTcpSocket::disconnected,    this, &TCPServer::slotClientDisconnected);

    m_nNextBlockSize[client_tcp_socket] = 0;

    emit newClient(client_tcp_socket);
}



//==============================================================================================================================================================
//слот отключения клиента от сервера
//==============================================================================================================================================================
void TCPServer::slotClientDisconnected()
{
    QTcpSocket *tcpSocket = dynamic_cast<QTcpSocket*>(sender());
    m_nNextBlockSize.remove(tcpSocket);
    emit lostClient(tcpSocket);
    tcpSocket->close();
}



//==============================================================================================================================================================
//слот получения пакетов от клиента
//==============================================================================================================================================================
void TCPServer::slotReadClient()
{
    QTcpSocket *tcpSocket = dynamic_cast<QTcpSocket*>(sender());
    quint32 ip = tcpSocket->peerAddress().toIPv4Address();
    QByteArray block = 0;
    QDataStream in(tcpSocket);
    DataPack data;
    for(;;)                                                                                                     //цикл ожидания пакета
    {
        if(!m_nNextBlockSize[tcpSocket])
        {
            if(tcpSocket->bytesAvailable()<sizeof(quint32))
            {
                break;
            }
            in>>m_nNextBlockSize[tcpSocket];
        }
        if(tcpSocket->bytesAvailable()<m_nNextBlockSize[tcpSocket])
        {
            break;
        }

        in >> data;                                                                                             //запись полного пакета



        if(data.type == 9)                                                                                      //часть файла получена клиентом
        {                                                                                                       //(клиент готов получить следующую часть файла)
            emit readyForNextPart(tcpSocket, data);
        }




        else if(data.type == 13)                                                                                //клиент передает файл на сервер
        {
            if(receiveFile[ip] == nullptr)                                                                      //если клиент с таким ip
            {                                                                                                   //начинает передачу файла
                                                                                                                //инициализация приема нового файла
                QString path = data.target.left(data.target.lastIndexOf("/"));
                QDir check(path);
                if(check.mkpath(path))                                                                          //проверка существования указанного пути и
                {                                                                                               //попытка его создания при необходимости
                    receiveFile[ip] = new QFile(data.target);
                    if(!receiveFile[ip]->open(QIODevice::WriteOnly))                                            //если файл не удалось открыть
                    {
                        receiveFile[ip] = nullptr;                                                              //отменить прием файла
                        qDebug() << "Не удалось открыть файл " + receiveFile[ip]->fileName() + ". Отмена передачи файла";
                    }
                    else                                                                                        //если файл удалось открыть
                    {
                        emit newFile(data.target);                                                              //"блокировка" файла до полной передачи
                        sizeReceivedData[ip] = 0;                                                               //обнуление количества полученных байт файла
                        sizeSendedData[ip] = data.file_size;                                                    //запись полного размера получаемого файла
                    }
                }
            }
            if(receiveFile[ip] != nullptr)                                                                      //если файл находится в процессе получения
            {
                block.clear();                                                                                  //очистка буфера
                in >> block;                                                                                    //потоковое чтение получаемой части файла
                block.remove(m_nNextBlockSize[tcpSocket] - sizeof(data), block.size() - (m_nNextBlockSize[tcpSocket] - sizeof(data))); //удаление из буфера "мусора", считанного из потока
                sizeReceivedData[ip] += block.size();                                                           //уточнение полученных байт файла
                receiveFile[ip]->write(block);                                                                  //запись содержимого буфера в файл

                if(sizeReceivedData[ip] == sizeSendedData[ip])                                                  //если файл получен полностью
                {
                    receiveFile[ip]->close();                                                                   //закрыть файл
                    emit fileClosed(data. target);                                                              //разблокировать файл
                    receiveFile[ip] = 0;
                }
                else                                                                                            //если файл получен НЕ полностью
                {
                    DataPack answer_pack;                                                                       //создание пакета-ответа, оповещающего
                    answer_pack.type = 9;                                                                       //клиента о том, что часть файла получена,                                                         //и сервер готов к приему следующей части
                    answer_pack. target = data.target;
                    slotWriteClient(tcpSocket, answer_pack);                                                    //отправка пакета-ответа
                }
            }
        }
        else                                                                                                    //отправка остальных сообщений в главный поток
        {                                                                                                       //для дальнейшей обработки
            emit getDataPackFromClient(tcpSocket, data);
        }



        m_nNextBlockSize[tcpSocket] = quint32(0);                                                               //блок считан, обнуление количества байт в получаемом блоке
    }
}




//==============================================================================================================================================================
//слот отправки следующей части файла клиенту
//==============================================================================================================================================================
void TCPServer::sendPartOfFile(QTcpSocket *socket, DataPack data)
{
    if (socket->isOpen())
    {
        quint32 ip = socket->peerAddress().toIPv4Address();
        data.type = 11;
        data.file_size = sendFile[ip]->size();

        char block[2000000] = {0};
        QByteArray byte_array = 0;
        QByteArray buf;
        QDataStream out(&byte_array, QIODevice::WriteOnly);
        if(!sendFile[ip]->atEnd())                                                                              //если не конец отправляемого файла
        {
            quint64 in = sendFile[ip]->read(block, sizeof(block));                                              //отправить следующую часть
            buf = buf.fromRawData(block, sizeof(block));
            buf.truncate(in);

            out << quint32(0);
            out << data;
            out << buf;

            out.device()->seek(0);
            out << quint32(byte_array.size() - sizeof(quint32));
            if (socket->isOpen())
            {
                socket->write(byte_array);
                if(sendFile[ip]->atEnd())                                                                       //если была отправлена последняя часть
                {
                    sendFile[ip]->close();                                                                      //закрыть файл
                    sendFile[ip] = 0;
                }
            }
            else                                                                                                //никогда не должен сюда попасть,
            {                                                                                                   //пусть останется, на всякий случай
                emit lostClient(socket);
            }
        }
        else                                                                                                    //никогда не должен сюда попасть,
        {                                                                                                       //пусть останется, на всякий случай
            qDebug() << "Ошибка при передаче файла.";
        }
    }
    else                                                                                                        //никогда не должен сюда попасть,
    {                                                                                                           //пусть останется, на всякий случай
        emit lostClient(socket);
    }
}




//==============================================================================================================================================================
//слот отправки пакетов клиенту
//==============================================================================================================================================================
void TCPServer::slotWriteClient(QTcpSocket *socket, DataPack data)
{
    if (socket->isOpen())
    {
        quint32 ip = socket->peerAddress().toIPv4Address();
        QByteArray arrBlock;
        QDataStream out(&arrBlock,QIODevice::WriteOnly);



        if(data.type == 11)                                                                                     //отправка файла на клиент
        {
            sendFile[ip] = nullptr;
            if(sendFile[ip] == nullptr)                                                                         //если файл НЕ в процессе отправки
            {                                                                                                   //инициализация отправки файла с проверками
                QString path = data.source;
                sendFile[ip] = new QFile(path);
                if(sendFile[ip]->exists())                                                                      //проверка на существование файла
                {
                    if(!sendFile[ip]->open(QIODevice::ReadOnly))                                                //проверка возможности открытия файла
                    {
                        sendFile[ip] = 0;
                        qDebug() << "Файл для отправки " + path + " не может быть открыт.";
                    }
                }
                else
                {
                    sendFile[ip] = 0;
                    qDebug() << "Файл для отправки " + path + " не существует.";
                }
            }
            if(sendFile[ip] != nullptr)                                                                         //если файл в процессе отправки
            {                                                                                                   //или инициализация прошла успешно
                if (socket->isOpen())
                {
                    socket->waitForBytesWritten();
                    sendPartOfFile(socket, data);                                                               //отправить часть файла
                }
                else                                                                                            //никогда не должен сюда попасть,
                {                                                                                               //пусть останется, на всякий случай
                    emit lostClient(socket);
                }
            }
            else
            {
                qDebug() << "Файл для передачи " + data.source + " не удалось найти или открыть. Отмена передачи файла.";
                data.type = 8;                                                                                  //отмена передачи файла
                out << quint32(0) << data;
                out.device()->seek(0);
                out << quint32(arrBlock.size()-sizeof(quint32));
                socket->write(arrBlock);
                socket->flush();
            }
        }


        else                                                                                                    //отправка пакетов остальных типов клиенту
        {                                                                                                       //без изменений для их обработки
            out << quint32(0) << data;
            out.device()->seek(0);
            out << quint32(arrBlock.size()-sizeof(quint32));
            socket->write(arrBlock);
            socket->flush();
        }
        socket->waitForBytesWritten();
    }
    else                                                                                                        //никогда не должен сюда попасть,
    {                                                                                                           //пусть останется, на всякий случай
        emit lostClient(socket);
    }
}
