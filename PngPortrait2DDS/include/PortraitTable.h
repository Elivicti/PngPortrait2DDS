#pragma once

#include <QTableWidget>
#include "PresetManager.h"

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

	void loadPortraitsInfo(const PresetData& preset);

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
	PresetData::SharedDataPointer portraits;
	QList<QList<QCheckBox*>> cbUsingTypes;
};
