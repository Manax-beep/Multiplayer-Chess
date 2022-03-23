#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "chessview.h"
#include "game.h"
#include <QtNetwork>
#include <QTcpSocket>
#include <QObject>
#include <QByteArray>
#include <QTimer>
#include <QTime>
#include <QJsonObject>
#include <QJsonDocument>
#include "ChatMsg.h"
#include "chessai.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_ButClientRun_clicked();
    void ontimer() ;
    void on_ButSend_clicked();

private:
    Ui::MainWindow *ui;
    // Представление
    ChessView * view ;
    // Игра
    Game * game ;
    // Клиент-сокет
    QTcpSocket * client  ;

    // Сообщения чата
    QList<ChatMsg> messages ;
    // Коды уже показанных сообщений
    QList<int> view_ids ;

    // Установка сообщения
    void setMsg(QString msg) ;
    void newGame(const char color) ;
    // Перевод сообщений в интерфейс
    void messages2ui() ;

    // Бот (может быть не задействован)
    ChessAI ai ;
    // Проверка режима бота
    bool isBotMode() const ;

    // Таймер для запроса хода сервера
    QTimer * timer ;
public slots:
    // Отправка хода на сервер
    void sendStep(const QString & cmd) ;
    void doBotStep() ;
    void sendMateMessage() ;
};

// Порт
const int CHESS_PORT = 777 ;
const QString MSG_MYSTEP = "Ваш ход" ;
const QString MSG_WAITSTEP = "Ожидаем ход противника" ;

#endif // MAINWINDOW_H
