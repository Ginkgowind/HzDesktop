#include "HzDesktopMgr.h"

HzDesktopMgr::HzDesktopMgr(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HzDesktopMgrClass())
{
    ui->setupUi(this);
}

HzDesktopMgr::~HzDesktopMgr()
{
    delete ui;
}
