#include <QtWidgets>

if (bAlreadyRunning)
{
	HWND hwnd = ::FindWindow(NULL, windowTitle);
	if (hwnd) {
		// 此处置顶逻辑，必须先HWND_TOPMOST再HWND_NOTOPMOST，否则在某些环境下会出现不能置顶的问题，使用HWND_TOP也会不能置顶
		::SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		::SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		// 对于SetForegroundWindow，在某些环境下只会任务栏图标亮一下，但不会置顶
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