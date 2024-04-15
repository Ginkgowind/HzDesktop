#include "HzDesktopMgr.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HzDesktopMgr w;
    w.show();
    return a.exec();
}
