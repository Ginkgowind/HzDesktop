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

    QApplication a(argc, argv);

    // ��ȡ���ݿ⣬��ȡ����������λ�á�����ʲô��

    // ��ȡ��ݼ�����������

    HzDesktopMgr w;

    return a.exec();
}

bool checkIsRunning()
{
    return false;
}