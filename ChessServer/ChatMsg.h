#ifndef CHATMSG_H
#define CHATMSG_H

// Это структура сообщения чата
struct ChatMsg {
    // Код
    int id ;
    // Текст
    QString text ;
    // Упаковка
    QByteArray getPacked() const {
        QJsonObject obj ;
        obj["id"]=id ;
        obj["text"]=text ;
        return QJsonDocument(obj).toJson() ;
    }
    // Распаковка
    void setPacked(const QByteArray & data) {
        auto doc = QJsonDocument::fromJson(data);
        id = doc.object()["id"].toInt() ;
        text = doc.object()["text"].toString() ;
    }
    // Сравнение
    bool operator==(const ChatMsg & msg) {
        return (msg.id==id)&&(msg.text==text) ;
    }
    // Создать массив сообщений из массива байт
    static QList<ChatMsg> getArray(const QByteArray & data) {
        QList<ChatMsg> res ;
        auto doc = QJsonDocument::fromJson(data);
        for (int i=0; i<doc.array().count(); i++) {
            ChatMsg msg ;
            msg.id = doc.array()[i].toObject()["id"].toInt() ;
            msg.text = doc.array()[i].toObject()["text"].toString() ;
            res.append(msg) ;
        }
        return res ;
    }
    // Записать массив сообщений в массив байт
    static QByteArray toArray(const QList<ChatMsg> & list) {
        QJsonArray arr ;
        for (int i=0; i<list.length(); i++) {
            QJsonObject obj ;
            obj["id"]=list[i].id ;
            obj["text"]=list[i].text ;
            arr.append(obj) ;
        }
        return QJsonDocument(arr).toJson() ;
    }
};

#endif // CHATMSG_H
