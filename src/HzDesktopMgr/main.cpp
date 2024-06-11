#include "HzDesktopMgr.h"
#include "spdlog/spdlog.h"
#include <QApplication>

bool checkIsRunning();

int main(int argc, char *argv[])
{
    SPDLOG_DEBUG("start");

    if (checkIsRunning()) {
        SPDLOG_INFO("hzdesktop is already running, exit");
        return 0;
    }

    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication a(argc, argv);

    // 读取数据库，获取格子数量、位置、内容什么的

    // 获取快捷键，各种配置

    HzDesktopMgr w;

    return a.exec();
}

bool checkIsRunning()
{
    return false;
}