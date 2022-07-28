#pragma once

#include <QTableWidget>
#include <QFileInfo>

class QCheckBox;

enum class PortraitUsingType
{
	Species,
	Leader,
	Ruler
};

class PortraitTable  : public QTableWidget
{
	Q_OBJECT

public:
	PortraitTable(QWidget *parent);
	~PortraitTable();


	void setPortraitsInfo(const QStringList& portraits);
	void appendPortraitInfo(const QString& pic, bool species, bool leader, bool ruler);

	bool isUsingPortraitType(int row, PortraitUsingType t);

	void clearContents();

signals:
	void tableLoadingCompleted();

protected:
	virtual void resizeEvent(QResizeEvent* evt) override;

private:
	void setHeaders();
	void resizeHeaders();
private:
	
	QList<QList<QCheckBox*>> cbUsingTypes;
};
