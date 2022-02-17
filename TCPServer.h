#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTime>
#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <QEventLoop>
#include <QTimer>
#include <QDir>


#include "GlobalData/datapack.h"

Q_DECLARE_METATYPE(DataPack)



class TCPServer : public QObject
{
    Q_OBJECT
public:
    explicit    TCPServer();
    ~TCPServer();
    int         counter = 0;
    QTcpServer  *m_tcp_server;
    QTcpSocket  *mTcpSocket = 0;

    bool listen(int port);

public slots:
    void        slotNewConnection       ();
    void        slotClientDisconnected  ();
    void        slotReadClient          ();
    void        slotWriteClient         (QTcpSocket *socket,  DataPack data);
    void        sendPartOfFile          (QTcpSocket *socket, DataPack data);

signals:
    void        getDataPackFromClient   (QTcpSocket*, DataPack);
    void        newClient               (QTcpSocket*);
    void        lostClient              (QTcpSocket*);
    void        offClient               (quint32);
    void        onClient                (quint32);
    void        updateCurPower          (quint32, double);
    void        readyForNextPart        (QTcpSocket*, DataPack data);
    void        newFile                 (QString fileName);
    void        fileClosed              (QString fileName);
private:
    QMap<quint32, QFile*>       receiveFile;
    QMap<quint32, QFile*>       sendFile;
    QMap<quint32, int>          sizeReceivedData;
    QMap<quint32, int>          sizeSendedData;
    QMap<QTcpSocket*, quint32>  m_nNextBlockSize;
};

#endif // TCPSERVER_H
