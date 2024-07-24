#include <QApplication>

#include "HzDesktopMgr.h"
#include "common/CommonTools.h"

int main(int argc, char *argv[])
{
    SPDLOG_DEBUG("start");

	wil::unique_mutex hMutex;
	hMutex.reset(::CreateMutexW(NULL, FALSE, HZDESKTOP_MUTEX_NAME));

	if (::GetLastError() == ERROR_ALREADY_EXISTS ||
		::GetLastError() == ERROR_ACCESS_DENIED) {
		SPDLOG_INFO("hzdesktop is already running, exit");
		return 0;
	}

    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication a(argc, argv);
	a.setWindowIcon(QIcon(":/main/view/res/common/HzDesktop.ico"));

    // ��ȡ���ݿ⣬��ȡ����������λ�á�����ʲô��

    // ��ȡ��ݼ�����������

    HzDesktopMgr w;

    return a.exec();
}