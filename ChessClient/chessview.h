#ifndef CHESSVIEW_H
#define CHESSVIEW_H

#include <QObject>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QKeyEvent>
#include "game.h"
#include "qchessel.h"

// Класс просмотра доски
class ChessView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit ChessView(QWidget *parent, Game * game, char playercolor);
    ~ChessView();
    QGraphicsScene * scene ;

private:
    // Сцена
    QChessEl *** map ;
    // Игра отображаемая
    Game * game ;
    // Текстуры фигур
    QPixmap ** figs_black ;
    QPixmap ** figs_white ;
    // Текстура выделения хода
    QPixmap * selpix ;
    // Текущая выделенная фигура
    //QChessEl * selitem ;
    QPoint selpos ;
    void clearSel() ;
    bool isSelected() const ;
    // Цвет игрока
    char playercolor ;
    // Обновление доступных ходов фигуры
    void updateSelection(QChessEl * item) ;    
protected:
    // Обработка движений и нажатия мыши
    virtual void mouseMoveEvent ( QMouseEvent * event );
    virtual void mousePressEvent ( QMouseEvent * event );    
public slots:
    // Слоты для получения данных от игры
    void clearChess() ;
    void addFigure(char color, int i, int j, int code);
};

// Размер пустого поля по краям
const int SPACE=28 ;

#endif // CHESSVIEW_H
