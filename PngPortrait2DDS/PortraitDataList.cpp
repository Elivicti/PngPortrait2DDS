#include "PortraitDataList.h"
#include <QDebug>


PortraitDataList::PortraitDataList(int x, int y, double scale)
{
	data.append(new PortraitData(x, y, scale));
}

PortraitDataList::~PortraitDataList()
{
	for (auto i : data)
		delete i;
}

void PortraitDataList::clear()
{
	auto default_ = data.takeAt(0);
	for (auto i : data)
		delete i;
	data.clear();
	data.append(default_);
}
