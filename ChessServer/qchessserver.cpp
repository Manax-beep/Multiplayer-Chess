#include "qchessserver.h"
#include <QDebug>

QChessServer::QChessServer(QObject *parent) : QObject(parent)
{    
    tcpServer = new QTcpServer(this);
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(newUser()));
    if (!tcpServer->listen(QHostAddress::Any, CHESS_PORT) && server_status==0) {
        qDebug() <<  QObject::tr("Unable to start the server: %1.").arg(tcpServer->errorString());
    } else {
        qDebug() << "Server started at port"<<CHESS_PORT ;
        server_status=1;
    }
}

void QChessServer::newUser()
{
    if(server_status==1){
        QTcpSocket* clientSocket=tcpServer->nextPendingConnection();
        int idusersocs=clientSocket->socketDescriptor();
        SClients[idusersocs]=clientSocket;
        connect(SClients[idusersocs],SIGNAL(readyRead()),this, SLOT(readFromClient()));
        qDebug() << "Client connected" ;
    }
}

void QChessServer::readFromClient()
{
    QTcpSocket* clientSocket = (QTcpSocket*)sender();

    QByteArray data = clientSocket->readAll();
    QString cmd = QString::fromLatin1(data) ;
    if (cmd!=lastcmd) qDebug() << "Get command: "<<cmd ;
    lastcmd = cmd ;
    if (cmd.startsWith("START")) {
        if (SClients.count()==1) clientSocket->write("WHITE") ; else
        if (SClients.count()==2) clientSocket->write("BLACK") ; else
        clientSocket->write("CLOSED") ;
    }
    if (cmd.startsWith("STEP")||cmd.startsWith("ROCK")||cmd.startsWith("UPGR")||cmd.startsWith("MATE")) {
        clientSocket->write("OK") ;
        savedcmd = cmd ;
        saved_descriptor = clientSocket->socketDescriptor() ;
    }            
    if (cmd.startsWith("WAIT")) {
        if ((savedcmd.length()>0)&&(saved_descriptor!=clientSocket->socketDescriptor())) {
            QByteArray res(savedcmd.toLocal8Bit()) ;
            clientSocket->write(res) ;
            savedcmd="" ;
        }
        else {
            clientSocket->write("NOP") ;
        }
    }
    // Здесь мы получаем сообщения от клиента и пишем его в чат
    if (cmd.startsWith("SENDMSG")) {
        ChatMsg msg ;
        msg.setPacked(data.mid(QString("SENDMSG").length())) ;
        messages.append(msg) ;
        clientSocket->write("OK") ;        
    }
    // Здесь мы отправляем клиенту сообщения, либо целиком, либо старше заданного id
    if (cmd.startsWith("GETNEWMSG")) {
        int lastid = cmd.mid(QString("GETNEWMSG").length()).toInt() ;
        if (lastid==-1)
            clientSocket->write(ChatMsg::toArray(messages)) ;
        else {
            QList<ChatMsg> res ;
            bool isnew=false ;
            foreach(auto msg, messages)
                if (isnew)
                    res.append(msg) ;
                else
                    if (msg.id==lastid) isnew=true ;
            clientSocket->write(ChatMsg::toArray(res)) ;
        }
    }
}

