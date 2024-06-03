#include <QUrl>
#include <QSettings>
#include <QDesktopServices>

#include "HzItemMenu_p.h"
#include "windows/UiOperation.h"

// TODO 处理private部分的资源释放
HzDesktopBlankMenuPrivate::HzDesktopBlankMenuPrivate()
{

}

HzDesktopBlankMenuPrivate::~HzDesktopBlankMenuPrivate()
{}

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
            connect(action, &QAction::triggered, [action, url]() {
                QDesktopServices::openUrl(url);
            });

            actions.push_back(action);
        }

        menuReg.endGroup();
    }

	return actions;
}
