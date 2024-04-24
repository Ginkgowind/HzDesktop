#include <QSortFilterProxyModel>
#include <QStandardItemModel>

#include "HzIconView.h"
#include "showItem/HzFileItem.h"

HzIconView::HzIconView(QWidget *parent)
	: QListView(parent)
{
	m_itemProxyModel = new QSortFilterProxyModel(this);
	m_itemModel = new QStandardItemModel(m_itemProxyModel);
	m_itemProxyModel->setSourceModel(m_itemModel);

	setModel(m_itemProxyModel);

	// Í¼±êÄ£Ê½
	setViewMode(QListView::IconMode);

	for (int i = 0; i < 6; i++) {
		HzFileItem* newItem = new HzFileItem();
		m_itemModel->appendRow(newItem);
	}
}

HzIconView::~HzIconView()
{

}
