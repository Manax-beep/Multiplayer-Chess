#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QPair>

void buildData() {
    QPixmap board("board.png") ;
    QPainter p(&board) ;
    p.setPen(Qt::black) ;
    p.setFont(QFont("Arial",14)) ;
    for (int i=0; i<8; i++) {
        p.drawText(QPoint(28+56*i+20,17),QChar(65+i));
        p.drawText(QPoint(28+56*i+20,28+56*8+25),QChar(65+i));
    }
    for (int i=0; i<8; i++) {
        p.drawText(QPoint(10,28+56*i+20),QChar(49+7-i));
        p.drawText(QPoint(10+56*8+23,28+56*i+20),QChar(49+7-i));
    }
    p.end() ;
    board.save("board_white.png") ;

    QPixmap board2("board.png") ;
    QPainter p2(&board2) ;
    p2.setPen(Qt::black) ;
    p2.setFont(QFont("Arial",14)) ;
    for (int i=0; i<8; i++) {
        p2.drawText(QPoint(28+56*i+20,17),QChar(65+7-i));
        p2.drawText(QPoint(28+56*i+20,28+56*8+25),QChar(65+7-i));
    }
    for (int i=0; i<8; i++) {
        p2.drawText(QPoint(10,28+56*i+20),QChar(49+i));
        p2.drawText(QPoint(10+56*8+23,28+56*i+20),QChar(49+i));
    }
    p2.end() ;
    board2.save("board_black.png") ;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);  
    ui->widgetChat->hide() ;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(ontimer()));    
}

void MainWindow::ontimer()
{
    if (!game->mystep) {
        QByteArray waits("WAIT") ;
        client->write(waits) ;
        client->waitForReadyRead(1000) ;

        QByteArray data = client->readAll();

        QString cmd = QString::fromLatin1(data) ;
        if ((cmd.startsWith("STEP")||cmd.startsWith("ROCK")||cmd.startsWith("UPGR"))) {
            int i1 = cmd[4].digitValue() ;
            int j1 = cmd[5].digitValue() ;
            int i2 = cmd[6].digitValue() ;
            int j2 = cmd[7].digitValue() ;
            if (cmd.startsWith("STEP"))
                game->doStep(7-i1,7-j1,7-i2,7-j2,false);
            if (cmd.startsWith("ROCK"))
                game->applyRock(7-i1,7-i2,7-i2,7-j2,false);
            if (cmd.startsWith("UPGR")) {
                game->doStep(7-i1,7-j1,7-i2,7-j2,false);
                game->transformFigure(7-i2,7-j2,cmd[8].digitValue()) ;
            }

            game->fillChess();
            view->scene->invalidate(0,0,1000,1000);            
            game->mystep=true ;
            if (game->isMate()) {
                setMsg("Вам мат, игра окончена") ;
                game->setGameOver() ;
                QTimer::singleShot(1000,this,SLOT(sendMateMessage())) ;
            }
            else {
                setMsg(MSG_MYSTEP+(game->isCheck()?" (вам шах)":"")) ;
                // Передача управления боту
                if (isBotMode()) QTimer::singleShot(1000,this,SLOT(doBotStep())) ;
            }
        }
        if (cmd.startsWith("MATE")) {
            setMsg("Вы победили") ;
            game->setGameOver() ;
        }
    }

    // Проверка сообщений на сервере
    int last_id=messages.count()==0?-1:messages.last().id ;    
    QString datalast = "GETNEWMSG"+QString::number(last_id) ;
    client->write(datalast.toLatin1()) ;
    client->waitForReadyRead(1000) ;

    QByteArray data = client->readAll();
    QList<ChatMsg> newlist = ChatMsg::getArray(data) ;
    foreach(auto msg, newlist)
        if (!messages.contains(msg)) messages.append(msg) ;
    messages2ui() ;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_ButClientRun_clicked()
{
    if (ui->edHost->text().trimmed().length()==0) {
        QMessageBox::information(this,"Сообщение","Нужно указать IP или имя хоста") ;
        return ;
    }

    ui->ButClientRun->hide();
    ui->label->hide();
    ui->edHost->hide();
    ui->cbUseBot->hide();
    ui->widgetChat->show() ;

    client = new QTcpSocket(this) ;

    client->connectToHost(ui->edHost->text(), CHESS_PORT);
    if (client->waitForConnected()) {
        if(client->state() == QAbstractSocket::ConnectedState)
            {
                if (isBotMode()) this->setWindowTitle(this->windowTitle()+" (бот)") ;
                client->write("START") ;
                client->waitForReadyRead(1000) ;
                QByteArray arr = client->readAll() ;
                QString res = QString::fromLocal8Bit(arr) ;
                if (res=="WHITE") {
                    newGame(COLOR_WHITE) ;
                    setMsg(MSG_MYSTEP) ;
                    timer->start(1000) ;
                    // Передача управления боту, если включен режим
                    if (isBotMode()) QTimer::singleShot(1000,this,SLOT(doBotStep())) ;
                }
                else
                if (res=="BLACK") {
                    newGame(COLOR_BLACK) ;
                    setMsg(MSG_WAITSTEP) ;                    
                    timer->start(1000) ;
                }
                else {
                    setMsg("Игра уже заполнена") ;
                    client->close();
                    return ;
                }                
            }
        else
            QMessageBox::information(this,"Ошибка","Нет соединения с сервером") ;
    }
    else
        QMessageBox::information(this,"Ошибка","Не удается подключиться к серверу") ;
}

void MainWindow::setMsg(QString msg)
{
    ui->labStepInfo->setText(msg);
}

void MainWindow::newGame(const char color)
{    
    game = new Game(color) ;    
    connect(game,SIGNAL(sendStepOverSocket(const QString &)),
        this,SLOT(sendStep(const QString &))) ;
    view = new ChessView(ui->centralWidget,game,game->getPlayerColor()) ;
    view->setGeometry(30,60,2*SPACE+CELLSIZE*8+4,2*SPACE+CELLSIZE*8+4);
    view->show();
}

void MainWindow::messages2ui()
{    
    foreach(auto msg, messages)
        if (!view_ids.contains(msg.id)) {
            ui->textChat->append(msg.text) ;
            view_ids.append(msg.id) ;
        }
}

bool MainWindow::isBotMode() const
{
    return ui->cbUseBot->isChecked() ;
}

void MainWindow::sendStep(const QString & msg)
{
    game->mystep=false ;    
    client->write(msg.toLocal8Bit()) ;
    client->waitForReadyRead(1000) ;
    QByteArray arr = client->readAll() ;
    setMsg(MSG_WAITSTEP) ;
}

void MainWindow::doBotStep()
{    
    if (ai.doStepAI(game)) {
        game->fillChess();
        view->scene->update(0,0,1000,1000);
    }
}

void MainWindow::sendMateMessage()
{
    client->write("MATE") ;
    client->waitForReadyRead(1000) ;
    QByteArray arr = client->readAll() ;
}

void MainWindow::on_ButSend_clicked()
{    
    // Отправка сообщения чата на сервер
    if (ui->lineMsg->text().trimmed().length()==0) {
        QMessageBox::information(this,"Сообщение","Текст сообщения пуст") ;
        return ;
    }

    QString text = QDateTime::currentDateTime().toString("hh:MM:ss")+" "+
            (game->getPlayerColor()==COLOR_WHITE?"Белые: ":"Черные: ")+
            ui->lineMsg->text() ;
    ChatMsg msg { QRandomGenerator::system()->bounded(10000), text } ;
    client->write("SENDMSG"+msg.getPacked()) ;
    client->waitForReadyRead(1000) ;
    // Просто читаем ответ
    QByteArray arr = client->readAll() ;
    messages.append(msg) ;
    messages2ui() ;
    ui->lineMsg->clear() ;
}
