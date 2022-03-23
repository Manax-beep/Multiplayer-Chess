#ifndef GAME_H
#define GAME_H

#include <QObject>
#include <QList>
#include <QPoint>

// Запись фигуры
struct Figure {
   int code ; // Код
   char color ; // Цвет
   Figure * createCopy() const {
       return new Figure { this->code, this->color } ;
   }
} ;

// Класс игры
class Game : public QObject
{
    Q_OBJECT
private:
    // Добавление фигуры
    void addFigure(char color, int code, int i, int j) ;
    // Текущий цвет игрока
    char playercolor ;
    char enemycolor ;
    bool gameover ;
    // Методы для построения ходов
    bool processAttack(QList<QPoint> &list, int i, int j) const;
    void processAsSlon(QList<QPoint> &list, int i, int j) const;
    void processAsTower(QList<QPoint> &list, int i, int j) const;
    bool processEnemyAttack(QList<QPoint> &list, int i, int j) const;
    void processEnemyAsSlon(QList<QPoint> &list, int i, int j) const;
    void processEnemyAsTower(QList<QPoint> &list, int i, int j) const;

    Game * createCopy() const ;

    Game() ;
    void initMap() ;
public:
    // Массив клеток доски
    Figure *** map ;
    // Метка хода (свой или чужой)
    bool mystep ;
    explicit Game(char playercolor);

    // Заполнение представления
    void fillChess() ;
    // Получение доступных ходов
    QList<QPoint> getSteps(int i, int j) const ;
    bool isRock(int i1, int j1, int i2, int j2) const ;
    // Выполнить ход, указав, ходит ли человек или сокет
    bool doStep(int i1, int j1, int i2, int j2, bool playerstep) ;
    bool doRock(int i1, int j1, int i2, int j2) ;
    void applyRock(int i1, int i2, int xk, int xt, bool isplayer) ;
    char getPlayerColor() const ;
    // Получение параметров доски по точкам
    bool isPlayerFigure(int i, int j) const ;
    bool isEnemyFigure(int i, int j) const ;
    bool isEnemyFigureAndNoKing(int i, int j) const ;
    bool isEmpty(int i, int j) const ;
    bool isPlayerFigure(QPoint p) const ;
    bool isEnemyFigure(QPoint p) const ;
    bool isEnemyFigureAndNoKing(QPoint p) const ;
    bool isEnemyPesh(int i, int j) const ;
    bool isPlayerPesh(int i, int j) const ;
    bool isEmpty(QPoint p) const ;
    bool isCheck() const ;
    bool isMate() const ;
    bool isGameOver() const ;
    void setGameOver() ;
    // Построение клеток под атакой врага
    QList<QPoint> buildEnemyAttacked() const;
    bool isStepToCheck(int i1, int j1, int i2, int j2) const ;
    void transformFigure(int i, int j, int code) ;

signals:
    // Сигналы на заполнение представления
    void clearChess() ;
    void addToChess(char color, int i, int j, int code) ;
    // И на отправу хода через сокет
    void sendStepOverSocket(const QString & cmd) ;

public slots:
};

const int CODE_PESH = 0 ;
const int CODE_HORSE = 1 ;
const int CODE_SLON = 2 ;
const int CODE_TOWER = 3 ;
const int CODE_KING = 4 ;
const int CODE_QUEEN = 5 ;

const char COLOR_BLACK = 'b' ;
const char COLOR_WHITE = 'w' ;

const int SZ = 8 ;

#endif // GAME_H
