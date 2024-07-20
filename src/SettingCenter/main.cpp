#include <QtWidgets>

if (bAlreadyRunning)
{
	HWND hwnd = ::FindWindow(NULL, windowTitle);
	if (hwnd) {
		// �˴��ö��߼���������HWND_TOPMOST��HWND_NOTOPMOST��������ĳЩ�����»���ֲ����ö������⣬ʹ��HWND_TOPҲ�᲻���ö�
		::SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		::SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		// ����SetForegroundWindow����ĳЩ������ֻ��������ͼ����һ�£��������ö�
		::SetForegroundWindow(hwnd);
		LOG_INFO("data clear app is already running, set foreground window");
	}
	else
	{
		LOG_ERROR("It is already running but the window was not found!");
	}
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    DesktopWidget desktop;
    desktop.setWindowTitle("Desktop Grid Example");
    desktop.resize(600, 400);
    desktop.show();

    return app.exec();
}