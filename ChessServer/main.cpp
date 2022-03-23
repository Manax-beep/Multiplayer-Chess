#include <QCoreApplication>
#include "qchessserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QChessServer server ;
    return a.exec();
}
