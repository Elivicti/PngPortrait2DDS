#pragma once

#include <QTableView>
#include <QFileInfo>

class QStandardItemModel;
class QCheckBox;

enum class PortraitUsingType
{
	Species,
	Leader,
	Ruler
};

class PortraitTableView  : public QTableView
{
	Q_OBJECT

public:
	PortraitTableView(QWidget *parent);
	~PortraitTableView();

	QStandardItemModel* getModel() { return itemModel; }

	void setPortraitsInfo(const QStringList& portraits);

	bool isUsingPortraitType(int row, PortraitUsingType t);

protected:
	virtual void resizeEvent(QResizeEvent* evt) override;

private:
	void setHeaders();
	void resizeHeaders();
private:
	QStandardItemModel* itemModel;
	QList<QList<QCheckBox*>> cbUsingTypes;
};
