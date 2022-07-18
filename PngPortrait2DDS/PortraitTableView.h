#pragma once

#include <QTableView>
#include <QFileInfo>

class QStandardItemModel;

class PortraitTableView  : public QTableView
{
	Q_OBJECT

public:
	PortraitTableView(QWidget *parent);
	~PortraitTableView();

	QStandardItemModel* getModel() { return itemModel; }

	void setPortraitsInfo(QFileInfoList* portraits);


protected:
	virtual void resizeEvent(QResizeEvent* evt) override;

private:
	void setHeaders();
	void resizeHeaders();
private:
	QStandardItemModel* itemModel;
};
