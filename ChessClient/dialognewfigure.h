#ifndef DIALOGNEWFIGURE_H
#define DIALOGNEWFIGURE_H

#include <QDialog>

namespace Ui {
class DialogNewFigure;
}

class DialogNewFigure : public QDialog
{
    Q_OBJECT

public:
    explicit DialogNewFigure(QWidget *parent = nullptr);
    ~DialogNewFigure();

    static int selectNewFigure() ;

private:
    Ui::DialogNewFigure *ui;
    int getFigureCode() ;
};

#endif // DIALOGNEWFIGURE_H
