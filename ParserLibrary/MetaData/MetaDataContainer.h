#pragma once
#include <vector>
#include "MetaPropertyInfo.h"
#include <Cursor/cursor.h>

class MetaDataContainer
{
public:
    MetaDataContainer(Cursor const& cursor);
    MetaPropertyInfo const* GetPropertyInfo(std::string const& propertyName) const;
private:
    std::map<std::string, int> m_PropertyNameToIndex;
    std::vector<MetaPropertyInfo> m_PropertyInfos;
};