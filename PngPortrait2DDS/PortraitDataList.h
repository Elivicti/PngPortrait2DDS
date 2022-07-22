#pragma once

#include <QList>
#include <QFileInfo>
#include <QPoint>

struct PortraitData
{
	QFileInfo portrait;

	bool useSeperateSetting;
	QPoint offset;
	double scale;

	PortraitData(const QFileInfo& file) : portrait(file), useSeperateSetting(false), offset(0, 0), scale(0.20) {}
	PortraitData(int x, int y, double scale) : portrait(), useSeperateSetting(false), offset(x, y), scale(scale) {}
};

class PortraitDataList
{

public:
	PortraitDataList(int x, int y, double scale);
	~PortraitDataList();

	inline void append(PortraitData* portrait) { data.append(portrait); }
	inline PortraitData* at(qsizetype i) { return data.at(i + 1); }

	void clear();
public:
#define GetIndex(i) ((i + 1) * data.at(i + 1)->useSeperateSetting)
	inline QFileInfo& portrait(qsizetype i) { return data.at(i + 1)->portrait; }
	inline QPoint& offset(qsizetype i) { return data.at(GetIndex(i))->offset; }
	inline double& scale(qsizetype i) { return data.at(GetIndex(i))->scale; }

	inline void setOffsetX(qsizetype i, int x) { data.at(GetIndex(i))->offset.setX(x); }
	inline void setOffsetY(qsizetype i, int y) { data.at(GetIndex(i))->offset.setY(y); }
	inline void setOffset(qsizetype i, const QPoint& offset) { data.at(GetIndex(i))->offset = offset; }
	inline void setOffset(qsizetype i, int x, int y) { data.at(GetIndex(i))->offset = { x, y }; }
	inline void setScale(qsizetype i, double scale) { data.at(GetIndex(i))->scale = scale; }
#undef GetIndex(i)

	inline QPoint& defaultOffset() { return data.at(0)->offset; }
	inline double& defaultScale() { return data.at(0)->scale; }

	inline void setDefaultOffset(const QPoint& offset) { data.at(0)->offset = offset; }
	inline void setDefaultOffset(int x, int y) { data.at(0)->offset = { x, y }; }
	inline void setDefaultScale(double scale) { data.at(0)->scale = scale; }

public:
	QList<PortraitData*> data;
};

