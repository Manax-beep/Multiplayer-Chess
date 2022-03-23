#ifndef QCHESSEL_H
#define QCHESSEL_H

#include <QObject>
#include <QGraphicsItem>
#include <QPainter>

// Базовый элемент для вывода на доске
class QChessEl : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    explicit QChessEl(QObject *parent, QPixmap * Apmsel, int i, int j) ;
    // Управление выделением
    void setIsOver() ;
    void clearIsOver() ;
    void setIsSel() ;
    void clearIsSel() ;
    // Возврат, фигура ли это
    bool isFigure() ;
    //
    void setWay(bool way) ;
    void setFigure(bool figure, QPixmap * pixmap = NULL) ;
    void setRed(bool isred) ;
    int i ;
    int j ;
protected:
    // Текстуры
    QPixmap * pm_figure ;
    QPixmap * pm_sel ;
    // Флаги
    bool hasfigure ;
    bool hasway ;

    bool isover ;
    bool issel ;
    bool isred ;
    // Размер
    QPolygon polygon ;
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) ;
};

// Размер ячейки доски
const int CELLSIZE = 56 ;

#endif // QCHESSEL_H
