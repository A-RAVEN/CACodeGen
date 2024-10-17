#include "BaseType.h"
#include <string_utils.h>

BaseTypeInfo::BaseTypeInfo(const Cursor& cursor, const NameSpaceInfo* nameSpace) :
    m_MetaData(cursor), m_RootCursor(cursor),
    m_NameSpace(nameSpace)
{}

const MetaDataContainer& BaseTypeInfo::getMetaData(void) const { return m_MetaData; }

std::string BaseTypeInfo::getSourceFile(void) const { return Utils::replace(m_RootCursor.getSourceFile(), "\\", "/"); }

NameSpaceInfo const* BaseTypeInfo::getCurrentNamespace() const { return m_NameSpace; }

Cursor& BaseTypeInfo::getCurosr() { return m_RootCursor; }
