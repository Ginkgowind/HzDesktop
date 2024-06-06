#include <Windows.h>
#include <shellapi.h>

#include <QUrl>
#include <QProcess>
#include <QSettings>
#include <QDesktopServices>

#include "HzItemMenu_p.h"
#include "windows/UiOperation.h"
#include "windows/tools.h"

// TODO 处理private部分的资源释放
HzDesktopBlankMenuPrivate::HzDesktopBlankMenuPrivate()
{

}

HzDesktopBlankMenuPrivate::~HzDesktopBlankMenuPrivate()
{}

QList<QAction*> HzDesktopBlankMenuPrivate::getBackgroundShellActions()
{
	HZQ_Q(HzDesktopBlankMenu);

	QList<QAction*> actions;
	QSettings menuReg("HKEY_CLASSES_ROOT\\Directory\\Background\\shell", QSettings::NativeFormat);
	QStringList groupList = menuReg.childGroups();

	foreach(QString group, groupList)
	{
		menuReg.beginGroup(group);
		QIcon icon = HZ::getIconFromLocation(menuReg.value("Icon").toString());
		QString text = HZ::getDirectString(menuReg.value(".").toString());
		// ��ȡִ������
		menuReg.beginGroup("command");
		QString command = menuReg.value(".").toString();

		if (!icon.isNull() && !text.isEmpty() && !command.isEmpty()) {
			QAction* action = new QAction(icon, text);
			connect(action, &QAction::triggered, [command, q]() {
				QString commandFormatted = command;
				commandFormatted.replace("%V", q->m_param->dirPath, Qt::CaseInsensitive);

				int argCnt = 0;
				LPWSTR* wszArglist = CommandLineToArgvW(commandFormatted.toStdWString().c_str(), &argCnt);

				QStringList args;
				for (int i = 1; i < argCnt; ++i) {
					args << QString::fromStdWString(wszArglist[i]);
				}

				QProcess process;
				process.setWorkingDirectory(q->m_param->dirPath);
				process.setProgram(QString::fromStdWString(wszArglist[0]));
				process.setArguments(args);
				process.startDetached();
				});

			actions.push_back(action);
		}

		menuReg.endGroup();
		menuReg.endGroup();
	}

	return actions;
}

QList<QAction*> HzDesktopBlankMenuPrivate::getBackgroundShellExActions()
{
	HZQ_Q(HzDesktopBlankMenu);

	QList<QAction*> actions;
	QSettings menuReg("HKEY_CLASSES_ROOT\\Directory\\Background\\shellex\\ContextMenuHandlers", QSettings::NativeFormat);
	QStringList groupList = menuReg.childGroups();

	foreach(QString group, groupList)
	{
		menuReg.beginGroup(group);
		QIcon icon = HZ::getIconFromLocation(menuReg.value("Icon").toString());
		QString text = HZ::getTextFromGUID(menuReg.value(".").toString());

		//!icon.isNull() &&
		if (!text.isEmpty()) {
			QAction* action = new QAction(icon, text);

			actions.push_back(action);
		}

		menuReg.endGroup();
	}

	return actions;
}

QList<QAction*> HzDesktopBlankMenuPrivate::getDesktopBackgroundActions()
{
    QList<QAction*> actions;
	QSettings menuReg("HKEY_CLASSES_ROOT\\DesktopBackground\\Shell", QSettings::NativeFormat);
	QStringList groupList = menuReg.childGroups();

    foreach(QString group, groupList)
    {
        menuReg.beginGroup(group);
        QIcon icon = HZ::getIconFromLocation(menuReg.value("Icon").toString());
        QString text = HZ::getDirectString(menuReg.value(".").toString());
        QString url = menuReg.value("SettingsUri").toString();

        if (!icon.isNull() && !text.isEmpty() && !url.isEmpty()) {
            QAction* action = new QAction(icon, text);
            connect(action, &QAction::triggered, [url]() {
                QDesktopServices::openUrl(url);
            });

            actions.push_back(action);
        }

        menuReg.endGroup();
    }

	return actions;
}
