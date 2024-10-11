#pragma once
#include <vector>
#include "MetaPropertyInfo.h"
#include <Cursor/cursor.h>

class MetaDataContainer
{
public:
    MetaDataContainer(Cursor const& cursor);
private:
    std::vector<MetaPropertyInfo> m_PropertyInfos;
};