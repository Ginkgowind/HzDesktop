#pragma once

#include <QtWidgets/QWidget>
#include "ui_HzDesktopMgr.h"

QT_BEGIN_NAMESPACE
namespace Ui { class HzDesktopMgrClass; };
QT_END_NAMESPACE

class HzDesktopMgr : public QWidget
{
    Q_OBJECT

public:
    HzDesktopMgr(QWidget *parent = nullptr);
    ~HzDesktopMgr();

private:
    Ui::HzDesktopMgrClass *ui;
};
