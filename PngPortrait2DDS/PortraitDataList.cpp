#include "PortraitDataList.h"

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
	for (auto i : data)
		delete i;
	data.clear();
}
