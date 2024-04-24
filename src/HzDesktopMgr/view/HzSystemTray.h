#pragma once

#include <QSystemTrayIcon>

class HzSystemTray : public QSystemTrayIcon
{
	Q_OBJECT

public:
	HzSystemTray(QWidget* parent);
	~HzSystemTray();

private:
};

