#include "qchessel.h"

QChessEl::QChessEl(QObject *parent, QPixmap * Apmsel, int i, int j)
{
    this->pm_sel = Apmsel ;
    this->pm_figure = NULL ;
    this->i=i ;
    this->j=j ;
    isover=false ;
    issel=false ;
    hasfigure = false ;
    hasway = false ;
    isred = false ;

    polygon.append(QPoint(0,0));
    polygon.append(QPoint(CELLSIZE,0));
    polygon.append(QPoint(CELLSIZE,CELLSIZE));
    polygon.append(QPoint(0,CELLSIZE));
    polygon.append(QPoint(0,0));
}

bool QChessEl::isFigure()
{
    return hasfigure ;
}

void QChessEl::setIsSel()
{
    issel = true ;
    update() ;
}

void QChessEl::clearIsSel()
{
    if (!issel) return ;
    issel = false ;
    update() ;
}

void QChessEl::setIsOver()
{
    isover = true ;
    update() ;
}

void QChessEl::clearIsOver()
{
    if (!isover) return ;
    isover = false ;
    update() ;
}

void QChessEl::setWay(bool way)
{
    hasway = way ;
}

void QChessEl::setFigure(bool figure, QPixmap * pixmap)
{
    hasfigure = figure ;
    pm_figure = pixmap ;
}

void QChessEl::setRed(bool isred)
{
    this->isred=isred ;
}

QRectF QChessEl::boundingRect() const
{
    return QRectF(0,0,CELLSIZE,CELLSIZE);
}

void QChessEl::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // figure
    if (hasfigure) {
        painter->drawPixmap(0,0,*pm_figure);
        if (isred) {
            //painter->setPen(QPen(Qt::red, 3, Qt::SolidLine, Qt::RoundCap));
            painter->fillRect(boundingRect(),QBrush(QColor(255,0,0,128))) ;
        }
        if (issel) {
            painter->setPen(QPen(Qt::green, 3, Qt::SolidLine, Qt::RoundCap));
            painter->drawPolyline(polygon) ;
        }
        else
            if (isover) {
                painter->setPen(QPen(Qt::blue, 3, Qt::SolidLine, Qt::RoundCap));
                painter->drawPolyline(polygon) ;
            }
    }

    // way
    if (hasway) {
        painter->drawPixmap(0,0,*pm_sel);
        if (isover) {
            painter->setPen(QPen(Qt::green, 3, Qt::SolidLine, Qt::RoundCap));
            painter->drawPolyline(polygon) ;
        }
    }
}
