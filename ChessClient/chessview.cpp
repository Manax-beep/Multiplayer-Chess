#include "chessview.h"
#include <QPainter>

QString makeFileName(int code, QChar bw) {
    return "fig"+QString::number(code)+"_"+bw+".png" ;
}

QPointF calcPos(int i, int j) {
    return QPointF(i*CELLSIZE+SPACE, j*CELLSIZE+SPACE) ;
}

void ChessView::clearChess() {
    for (int i=0; i<8; i++)
        for (int j=0; j<8; j++) {
            map[i][j]->setWay(false);
            map[i][j]->setRed(false) ;
            map[i][j]->setFigure(false);
        }
}

void ChessView::addFigure(char color, int i, int j, int code) {
    QPixmap ** figs = (color==COLOR_BLACK)?figs_black:figs_white ;
    map[i][j]->setFigure(true,figs[code]);    
    if ((game->isCheck())&&(code==CODE_KING)&&(color==game->getPlayerColor())) map[i][j]->setRed(true) ;
}

ChessView::ChessView(QWidget *parent, Game *game, char playercolor): QGraphicsView(parent)
{
    selpos.setX(-1); selpos.setY(-1);
    this->game = game ;
    this->playercolor = playercolor ;

    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    this->setMouseTracking(true);

    selpix = new QPixmap("sel.png") ;

    scene = new QGraphicsScene();

    figs_black = new QPixmap*[6] ;
    figs_black[CODE_PESH]=new QPixmap(makeFileName(CODE_PESH,'b')) ;
    figs_black[CODE_SLON]=new QPixmap(makeFileName(CODE_SLON,'b')) ;
    figs_black[CODE_HORSE]=new QPixmap(makeFileName(CODE_HORSE,'b')) ;
    figs_black[CODE_TOWER]=new QPixmap(makeFileName(CODE_TOWER,'b')) ;
    figs_black[CODE_QUEEN]=new QPixmap(makeFileName(CODE_QUEEN,'b')) ;
    figs_black[CODE_KING]=new QPixmap(makeFileName(CODE_KING,'b')) ;
    figs_white = new QPixmap*[6] ;
    figs_white[CODE_PESH]=new QPixmap(makeFileName(CODE_PESH,'w')) ;
    figs_white[CODE_SLON]=new QPixmap(makeFileName(CODE_SLON,'w')) ;
    figs_white[CODE_HORSE]=new QPixmap(makeFileName(CODE_HORSE,'w')) ;
    figs_white[CODE_TOWER]=new QPixmap(makeFileName(CODE_TOWER,'w')) ;
    figs_white[CODE_QUEEN]=new QPixmap(makeFileName(CODE_QUEEN,'w')) ;
    figs_white[CODE_KING]=new QPixmap(makeFileName(CODE_KING,'w')) ;

    connect(game,SIGNAL(clearChess()),this,SLOT(clearChess())) ;
    connect(game,SIGNAL(addToChess(char,int,int,int)),
            this,SLOT(addFigure(char,int,int,int))) ;

    this->setScene(scene) ;

    QGraphicsPixmapItem * item = scene->addPixmap(
                QPixmap(playercolor==COLOR_WHITE?"board_white.png":"board_black.png")) ;
    item->setPos(0,0) ;

    map = new QChessEl**[8] ;
    for (int i=0; i<8; i++)
        map[i] = new QChessEl*[8] ;

    for (int i=0; i<8; i++)
        for (int j=0; j<8; j++) {
            map[i][j]=new QChessEl(scene,selpix,i,j) ;
            map[i][j]->setPos(calcPos(i,j)) ;
            scene->addItem(map[i][j]);
        }

    game->fillChess();
}

ChessView::~ChessView()
{
    delete scene ;
}

void ChessView::clearSel()
{
    selpos.setX(-1); selpos.setY(-1);
}

bool ChessView::isSelected() const
{
    return selpos.x()>=0 ;
}

void ChessView::updateSelection(QChessEl *item)
{
    int i = item->i ;
    int j = item->j ;

    for (QGraphicsItem * item: scene->items()) {
        ((QChessEl*)item)->clearIsSel();
        ((QChessEl*)item)->setWay(false);
    }
    item->setIsSel();

    selpos.setX(i); selpos.setY(j);

    for (QPoint sel: game->getSteps(i,j))
        map[sel.x()][sel.y()]->setWay(true);
}

void ChessView::mouseMoveEvent(QMouseEvent *event)
{
    if (!game->mystep) return ;    

    QPointF scenePos = this->mapToScene(event->pos()) ;

    for (QGraphicsItem * item: scene->items())
        ((QChessEl*)item)->clearIsOver();
    QGraphicsItem * item = scene->itemAt(scenePos,QTransform()) ;
    if (item!=NULL)
        ((QChessEl*)item)->setIsOver();

}

void ChessView::mousePressEvent(QMouseEvent *event)
{
    // Если конец игры
    if (game->isGameOver()) return ;
    // Пропуск, если не свой ход
    if (!game->mystep) return ;
    // Пропуск, если не левая кнопка мыши
    if(event->button() != Qt::LeftButton) return ;

    // Получить позицию клика
    QPointF scenePos = this->mapToScene(event->pos()) ;
    // Получить элемент
    QGraphicsItem * tek = scene->itemAt(scenePos,QTransform()) ;

    // Если не элемент, то пропускаем
    if (tek==NULL) return ;

    // Если не фигура, то пропускаем
    if (tek->pos().x()<1) return ;

    QChessEl * fig = (QChessEl*)tek ;

    // Защита от клика по самой себе, когда фигура выделена
    if ((fig->x()==selpos.x())&&(fig->y()==selpos.y())) return ;

    enum GameAction { gaSelFigure, gaDoStep, gaRockStep, gaNone } ;

    GameAction act = gaNone ;

    if (fig->isFigure()) {
         if (game->isPlayerFigure(fig->i,fig->j)) {
             act=gaSelFigure ;
             if (isSelected())
                if (game->isRock(selpos.x(),selpos.y(),fig->i,fig->j))
                    act=gaRockStep ;
         }
         else {
             if (isSelected()) act=gaDoStep ;
         }
    }
    else {
        if (isSelected()) act=gaDoStep ;
    }

    switch (act) {
    case gaSelFigure:
        //for (QGraphicsItem * item: scene->items())
        //    ((QChessEl*)item)->clearIsSel();
        //fig->setIsSel();
        updateSelection(fig) ;
        scene->update(0,0,1000,1000);
        break ;
    case gaDoStep:
        if (game->doStep(selpos.x(),selpos.y(),fig->i,fig->j,true)) {
            game->fillChess();
            scene->update(0,0,1000,1000);
            clearSel() ;
        }
        break ;
    case gaRockStep:
        if (game->doRock(selpos.x(),selpos.y(),fig->i,fig->j)) {
            game->fillChess();
            scene->update(0,0,1000,1000);
            clearSel() ;
        }
        break ;
    case gaNone: ;
    }

}
