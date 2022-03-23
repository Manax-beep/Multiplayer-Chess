#ifndef QCHESSSERVER_H
#define QCHESSSERVER_H

#include <QObject>
#include <QtNetwork>
#include <QTcpSocket>
#include <QObject>
#include <QByteArray>
#include <QMap>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include "ChatMsg.h"

class QChessServer : public QObject
{
    Q_OBJECT
private:
    // Сервер
    QTcpServer *tcpServer;
    int server_status = 0;
    // Клиенты сервера (нужны для очистки)
    QMap<int,QTcpSocket *> SClients;
    // Сообщения чата
    QList<ChatMsg> messages ;
    QString savedcmd = "" ;
    // Дескриптор последнего клиента
    int saved_descriptor=-1 ;
    QString lastcmd ;
public:    
    explicit QChessServer(QObject *parent = nullptr);

signals:

public slots:
    void newUser() ;
    void readFromClient() ;
};

const int CHESS_PORT = 777 ;

#endif // QCHESSSERVER_H
