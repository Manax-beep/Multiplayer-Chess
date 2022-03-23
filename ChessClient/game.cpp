#include "game.h"
#include <QDebug>
#include "dialognewfigure.h"

void Game::addFigure(char color, int code, int i, int j)
{
    //Figure * fig = new Figure { code, color } ;
    //fig->code=code ;
    //fig->color=color ;
    //map[i][j]=fig ;
    map[i][j]=new Figure { code, color } ;
}

Game::Game(char playercolor) : QObject(NULL)
{
    this->playercolor=playercolor ;
    this->mystep=(playercolor==COLOR_WHITE) ;
    this->gameover = false ;

    enemycolor = (playercolor==COLOR_WHITE)?COLOR_BLACK:COLOR_WHITE ;

    initMap() ;

    for (int i=0; i<8; i++) {
        addFigure(enemycolor,CODE_PESH,i,1);
        addFigure(playercolor,CODE_PESH,i,6);
    }
    addFigure(playercolor,CODE_TOWER,0,7);
    addFigure(playercolor,CODE_HORSE,1,7);
    addFigure(playercolor,CODE_SLON,2,7);
    if (playercolor==COLOR_WHITE) {
        addFigure(playercolor,CODE_QUEEN,3,7);
        addFigure(playercolor,CODE_KING,4,7);
    }
    else {
        addFigure(playercolor,CODE_QUEEN,4,7);
        addFigure(playercolor,CODE_KING,3,7);
    }
    addFigure(playercolor,CODE_SLON,5,7);
    addFigure(playercolor,CODE_HORSE,6,7);
    addFigure(playercolor,CODE_TOWER,7,7);

    addFigure(enemycolor,CODE_TOWER,0,0);
    addFigure(enemycolor,CODE_HORSE,1,0);
    addFigure(enemycolor,CODE_SLON,2,0);
    if (playercolor==COLOR_WHITE) {
        addFigure(enemycolor,CODE_QUEEN,3,0);
        addFigure(enemycolor,CODE_KING,4,0);
    }
    else {
        addFigure(enemycolor,CODE_QUEEN,4,0);
        addFigure(enemycolor,CODE_KING,3,0);
    }
    addFigure(enemycolor,CODE_SLON,5,0);
    addFigure(enemycolor,CODE_HORSE,6,0);
    addFigure(enemycolor,CODE_TOWER,7,0);

    // test code
   /* if (playercolor==COLOR_WHITE) {
        addFigure(enemycolor,CODE_KING,4,5);
        addFigure(playercolor,CODE_KING,3,2);
    }
    else {
        addFigure(enemycolor,CODE_KING,4,5);
        addFigure(playercolor,CODE_KING,3,2);
    }
    */

}

bool Game::isEmpty(int i, int j) const
{
    return map[i][j]==NULL ;
}

bool Game::isEmpty(QPoint p) const
{
    return map[p.x()][p.y()]==NULL ;
}

bool Game::isCheck() const
{
    auto attacked = buildEnemyAttacked() ;
    foreach(auto p, attacked)
        if (isPlayerFigure(p)) {
            if (map[p.x()][p.y()]->code==CODE_KING)
            //qDebug()<<p.x()<<p.y()<<map[p.x()][p.y()]->code ;
                return true ;
        }
    return false ;
}

bool Game::isMate() const
{
    if (!isCheck()) return false ;

    for (int i=0; i<SZ; i++)
        for (int j=0; j<SZ; j++)
            if (isPlayerFigure(i,j))
                foreach(auto p, getSteps(i,j))
                    if (!isStepToCheck(i,j,p.x(),p.y())) return false ;

    return true ;
}

bool Game::isGameOver() const
{
    return gameover ;
}

void Game::setGameOver()
{
    gameover=true ;
}

void Game::fillChess()
{
    emit clearChess();
    for (int i=0; i<SZ; i++)
        for (int j=0; j<SZ; j++)
            if (!isEmpty(i,j))
                emit addToChess(map[i][j]->color,i,j,map[i][j]->code);
}

bool isIn(QPoint p) {
    return (p.x()>=0)&&(p.y()>=0)&&(p.x()<SZ)&&(p.y()<SZ) ;
}

bool isIn(int i, int j) {
    return isIn(QPoint(i,j)) ;
}

bool Game::processAttack(QList<QPoint> & list, int i, int j) const {
    QPoint p(i,j) ;
    if (isEmpty(p)) {
        list.append(p);
        return false ;
    }
    if (isEnemyFigureAndNoKing(p)) {
        list.append(p);
        return true ;
    }
    else
        return true ;
}

bool Game::processEnemyAttack(QList<QPoint> & list, int i, int j) const {
    QPoint p(i,j) ;
    if (!isIn(p)) return true ;

    list.append(p);
    return !isEmpty(p) ;
}

void Game::processAsSlon(QList<QPoint> & list, int i, int j) const {
    for (int ki=i+1, kj=j+1; isIn(ki,kj); ki++, kj++)
        if (processAttack(list,ki,kj)) break ;
    for (int ki=i+1, kj=j-1; isIn(ki,kj); ki++, kj--)
        if (processAttack(list,ki,kj)) break ;
    for (int ki=i-1, kj=j+1; isIn(ki,kj); ki--, kj++)
        if (processAttack(list,ki,kj)) break ;
    for (int ki=i-1, kj=j-1; isIn(ki,kj); ki--, kj--)
        if (processAttack(list,ki,kj)) break ;
}

void Game::processAsTower(QList<QPoint> & list, int i, int j) const {
for (int ki=i+1; isIn(ki,j); ki++)
    if (processAttack(list,ki,j)) break ;
for (int ki=i-1; isIn(ki,j); ki--)
    if (processAttack(list,ki,j)) break ;
for (int kj=j+1; isIn(i,kj); kj++)
    if (processAttack(list,i,kj)) break ;
for (int kj=j-1; isIn(i,kj); kj--)
    if (processAttack(list,i,kj)) break ;
}

void Game::processEnemyAsSlon(QList<QPoint> & list, int i, int j) const {
    for (int ki=i+1, kj=j+1; isIn(ki,kj); ki++, kj++)
        if (processEnemyAttack(list,ki,kj)) break ;
    for (int ki=i+1, kj=j-1; isIn(ki,kj); ki++, kj--)
        if (processEnemyAttack(list,ki,kj)) break ;
    for (int ki=i-1, kj=j+1; isIn(ki,kj); ki--, kj++)
        if (processEnemyAttack(list,ki,kj)) break ;
    for (int ki=i-1, kj=j-1; isIn(ki,kj); ki--, kj--)
        if (processEnemyAttack(list,ki,kj)) break ;
}

void Game::processEnemyAsTower(QList<QPoint> & list, int i, int j) const {
for (int ki=i+1; isIn(ki,j); ki++)
    if (processEnemyAttack(list,ki,j)) break ;
for (int ki=i-1; isIn(ki,j); ki--)
    if (processEnemyAttack(list,ki,j)) break ;
for (int kj=j+1; isIn(i,kj); kj++)
    if (processEnemyAttack(list,i,kj)) break ;
for (int kj=j-1; isIn(i,kj); kj--)
    if (processEnemyAttack(list,i,kj)) break ;
}

Game *Game::createCopy() const
{
    Game * game = new Game() ;
    game->mystep = mystep ;
    game->playercolor = playercolor ;
    game->enemycolor = enemycolor ;
    for (int i=0; i<SZ; i++)
        for (int j=0; j<SZ; j++)
            if (map[i][j]==NULL)
                game->map[i][j]=NULL ;
            else
                game->map[i][j]=map[i][j]->createCopy() ;

    return game ;
}

Game::Game(): QObject(NULL)
{
    this->gameover = false ;
    initMap() ;
}

void Game::initMap()
{
    map = new Figure**[SZ] ;
    for (int i=0; i<SZ; i++) {
        map[i]=new Figure*[SZ] ;
        for (int j=0; j<SZ; j++)
            map[i][j]=NULL ;
    }
}

QList<QPoint> Game::buildEnemyAttacked() const
{
    QList<QPoint> r ;
    for (int i=0; i<SZ; i++)
        for (int j=0; j<SZ; j++)
            if (isEnemyFigure(i,j)) {
                int code = map[i][j]->code ;
                switch(code) {
                case CODE_PESH: {
                    QPoint p1(i-1,j+1) ;  r.append(p1);
                    QPoint p2(i+1,j+1) ;  r.append(p2);
                    break ;
                }
                case CODE_HORSE: {
                    r.append(QPoint(i-1,j-2)) ; r.append(QPoint(i-2,j-1)) ;
                    r.append(QPoint(i+1,j-2)) ; r.append(QPoint(i+2,j-1)) ;
                    r.append(QPoint(i-1,j+2)) ; r.append(QPoint(i-2,j+1)) ;
                    r.append(QPoint(i+1,j+2)) ; r.append(QPoint(i+2,j+1)) ;
                    break ;
                }
                case CODE_KING: {
                    r.append(QPoint(i-1,j)) ; r.append(QPoint(i+1,j)) ;
                    r.append(QPoint(i-1,j-1)) ; r.append(QPoint(i,j-1)) ; r.append(QPoint(i+1,j-1)) ;
                    r.append(QPoint(i-1,j+1)) ; r.append(QPoint(i,j+1)) ; r.append(QPoint(i+1,j+1)) ;
                    break ;
                }
                case CODE_TOWER: {
                    processEnemyAsTower(r,i,j) ;
                    break ;
                }
                case CODE_SLON: {
                    processEnemyAsSlon(r,i,j) ;
                    break ;
                }
                case CODE_QUEEN: {
                    processEnemyAsTower(r,i,j) ;
                    processEnemyAsSlon(r,i,j) ;
                    break ;
                }
                default:
                    break ;
                }
            }

    return r;
}

bool Game::isStepToCheck(int i1, int j1, int i2, int j2) const
{
    Game * test = this->createCopy() ;
    test->map[i2][j2]=test->map[i1][j1] ;
    test->map[i1][j1]=NULL ;
    bool ischeck = test->isCheck() ;
    delete test ;
    return ischeck ;
}

void Game::transformFigure(int i, int j, int code)
{
    map[i][j]->code = code ;
}

QList<QPoint> Game::getSteps(int i, int j) const
{
    int code = map[i][j]->code ;

    QList<QPoint> r ;

    switch(code) {
    case CODE_PESH: {
        QPoint p1(i-1,j-1) ;
        if (isEnemyFigureAndNoKing(p1))
            r.append(p1);
        else {
        // Добавляем взятие на проходе
        if (j==3) {
            if (isEmpty(p1)&&(isEnemyPesh(i-1,j))) r.append(p1);
        }
        }

        QPoint p2(i+1,j-1) ;
        if (isEnemyFigureAndNoKing(p2))
            r.append(p2);
        else {
        // Добавляем взятие на проходе
        if (j==3) {
            if (isEmpty(p2)&&(isEnemyPesh(i+1,j))) r.append(p2);
        }
        }

        QPoint p3(i,j-1) ;
        if (isEmpty(p3)) r.append(p3);
        if (j==6) {
            QPoint p4(i,j-2) ;
            if (isEmpty(p4)) r.append(p4);
        }
        break ;
    }
    case CODE_TOWER: {
        processAsTower(r,i,j) ;
        break ;
    }
    case CODE_SLON: {
        processAsSlon(r,i,j) ;
        break ;
    }
    case CODE_QUEEN: {
        processAsTower(r,i,j) ;
        processAsSlon(r,i,j) ;
        break ;
    }
    case CODE_KING: {
        QList<QPoint> attacked = buildEnemyAttacked() ;
        QList<QPoint> steps ;        
        steps.append(QPoint(i-1,j)) ; steps.append(QPoint(i+1,j)) ;
        steps.append(QPoint(i-1,j-1)) ; steps.append(QPoint(i,j-1)) ; steps.append(QPoint(i+1,j-1)) ;
        steps.append(QPoint(i-1,j+1)) ; steps.append(QPoint(i,j+1)) ; steps.append(QPoint(i+1,j+1)) ;
        for (QPoint p: steps)
            if (isIn(p))
                if (!attacked.contains(p))
                    if (isEmpty(p)||isEnemyFigureAndNoKing(p)) r.append(p);
        break ;
    }
    case CODE_HORSE: {
        QList<QPoint> steps ;
        steps.append(QPoint(i-1,j-2)) ; steps.append(QPoint(i-2,j-1)) ;
        steps.append(QPoint(i+1,j-2)) ; steps.append(QPoint(i+2,j-1)) ;
        steps.append(QPoint(i-1,j+2)) ; steps.append(QPoint(i-2,j+1)) ;
        steps.append(QPoint(i+1,j+2)) ; steps.append(QPoint(i+2,j+1)) ;
        for (QPoint p: steps)
            if (isIn(p))
                if (isEmpty(p)||isEnemyFigureAndNoKing(p)) r.append(p);
        break ;
    }
    default:
        break ;
    }

    return r ;
}

bool Game::isRock(int i1, int j1, int i2, int j2) const
{
    // Здесь проверка корректности рокировки
    if (map[i1][j1]==NULL) return false ;
    if (map[i2][j2]==NULL) return false ;
    if (map[i1][j1]->color!=playercolor) return false ;
    if (map[i2][j2]->color!=playercolor) return false ;

    if ((j1!=7)||(j2!=7)) return false ;

    int kingi=(playercolor==COLOR_WHITE)?4:3 ;

    if (i1>i2) {
        int tmp=i1 ; i1=i2 ; i2=tmp ;
    }

    // Ладья слева
    if ((i1==0)&&(map[i1][7]->code==CODE_TOWER)) {
        if (i2==kingi)
            if (map[i2][7]->code==CODE_KING) {
                int xk=kingi-2 ;
                int xt=kingi-1 ;
                return (map[xk][7]==NULL)&&(map[xt][7]==NULL) ;
            }
    }
    else
    // Ладья справа
    if ((i2==7)&&(map[i2][7]->code==CODE_TOWER)) {
        if (i1==kingi)
            if (map[i1][7]->code==CODE_KING) {
                int xk=kingi+2 ;
                int xt=kingi+1 ;
                return (map[xk][7]==NULL)&&(map[xt][7]==NULL) ;
            }
    }

    return false ;
}

bool Game::doStep(int i1, int j1, int i2, int j2, bool playerstep)
{
    if (playerstep) {
        auto steps = getSteps(i1,j1) ;
        if (!steps.contains(QPoint(i2,j2))) return false ;
    }

    if (playerstep) { // Запрет на ход под шах или оставление шаха, но только для игрока
        if (isStepToCheck(i1,j1,i2,j2)) return false ;
    }

    // Добавляем взятие на проходе
    if (map[i1][j1]->code==CODE_PESH)
        if ( (playerstep&&((j1==3)&&(j2==2))) ||
           ((!playerstep)&&((j1==4)&&(j2==5))) )
            if (i1!=i2)
                if ( (playerstep && isEnemyPesh(i2,j1)) ||
                   ((!playerstep) && isPlayerPesh(i2,j1)) )
                    map[i2][j1]=NULL ;

    map[i2][j2]=map[i1][j1] ;
    map[i1][j1]=NULL ;

    QString stepcmd = QString("STEP%1%2%3%4").arg(i1).arg(j1).arg(i2).arg(j2) ;

    if (playerstep)
        if ((map[i2][j2]->code==CODE_PESH)&&(j2==0)) {
            map[i2][j2]->code=DialogNewFigure::selectNewFigure() ;
            stepcmd = QString("UPGR%1%2%3%4%5").arg(i1).arg(j1).arg(i2).arg(j2).arg(map[i2][j2]->code) ;
        }

    if (playerstep) emit sendStepOverSocket(stepcmd);

    return true ;
}

bool Game::doRock(int i1, int j1, int i2, int j2)
{
    // Здесь рокировка
    int xk = 0 ;
    int xt = 0 ;

    int kingi=(playercolor==COLOR_WHITE)?4:3 ;

    if (i1>i2) {
        int tmp=i1 ; i1=i2 ; i2=tmp ;
    }

    // Ладья слева
    if ((i1==0)&&(map[i1][7]->code==CODE_TOWER)) {
        if (i2==kingi)
            if (map[i2][7]->code==CODE_KING) {
                xk=kingi-2 ;
                xt=kingi-1 ;
            }
    }
    else
    // Ладья справа
    if ((i2==7)&&(map[i2][7]->code==CODE_TOWER)) {
        if (i1==kingi)
            if (map[i1][7]->code==CODE_KING) {
                xk=kingi+2 ;
                xt=kingi+1 ;
            }
    }

    applyRock(i1,i2,xk,xt,true) ;

    QString stepcmd =QString("ROCK%1%2%3%4").arg(i1).arg(i2).arg(xk).arg(xt) ;
    emit sendStepOverSocket(stepcmd);
    return true ;
}

void Game::applyRock(int i1, int i2, int xk, int xt, bool isplayer)
{
    int j=isplayer?7:0 ;
    int color=isplayer?playercolor:enemycolor ;
    map[i1][j]=NULL ;
    map[i2][j]=NULL ;
    addFigure(color,CODE_KING,xk,j);
    addFigure(color,CODE_TOWER,xt,j);
}

char Game::getPlayerColor() const
{
    return playercolor ;
}

bool Game::isPlayerFigure(int i, int j) const
{
    if (!isIn(i,j)) return false ;
    if (map[i][j]==NULL) return false ;
    return map[i][j]->color==playercolor ;
}

bool Game::isEnemyFigure(int i, int j) const
{
    if (!isIn(i,j)) return false ;
    if (map[i][j]==NULL) return false ;
    return map[i][j]->color!=playercolor ;
}

bool Game::isEnemyFigureAndNoKing(int i, int j) const
{
    if (!isIn(i,j)) return false ;
    if (map[i][j]==NULL) return false ;
    return (map[i][j]->color!=playercolor)&&(map[i][j]->code!=CODE_KING) ;
}

bool Game::isEnemyPesh(int i, int j) const
{
    if (!isIn(i,j)) return false ;
    if (map[i][j]==NULL) return false ;
    return (map[i][j]->color!=playercolor)&&(map[i][j]->code==CODE_PESH) ;
}

bool Game::isPlayerPesh(int i, int j) const
{
    if (!isIn(i,j)) return false ;
    if (map[i][j]==NULL) return false ;
    return (map[i][j]->color==playercolor)&&(map[i][j]->code==CODE_PESH) ;
}

bool Game::isPlayerFigure(QPoint p) const
{
    return isPlayerFigure(p.x(),p.y()) ;
}

bool Game::isEnemyFigure(QPoint p) const
{
    return isEnemyFigure(p.x(),p.y()) ;
}

bool Game::isEnemyFigureAndNoKing(QPoint p) const
{
    return isEnemyFigureAndNoKing(p.x(),p.y()) ;
}
