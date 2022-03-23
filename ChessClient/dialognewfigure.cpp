#include "dialognewfigure.h"
#include "ui_dialognewfigure.h"
#include "game.h"

DialogNewFigure::DialogNewFigure(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNewFigure)
{
    ui->setupUi(this);
}

DialogNewFigure::~DialogNewFigure()
{
    delete ui;
}

int DialogNewFigure::selectNewFigure()
{
    DialogNewFigure d(NULL) ;
    d.exec() ;
    return d.getFigureCode() ;
}

int DialogNewFigure::getFigureCode()
{
    if (ui->rbSlon->isChecked()) return CODE_SLON ;
    if (ui->rbHorse->isChecked()) return CODE_HORSE ;
    if (ui->rbTower->isChecked()) return CODE_TOWER ;
    if (ui->rbQueen->isChecked()) return CODE_QUEEN ;
    return CODE_PESH ;
}
