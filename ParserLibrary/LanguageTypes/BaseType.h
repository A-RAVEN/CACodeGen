#pragma once
#include <Cursor/cursor.h>
#include <MetaData/MetaDataContainer.h>

class NameSpaceInfo;
class BaseTypeInfo
{
public:
    BaseTypeInfo(const Cursor& cursor, NameSpaceInfo const* nameSpace);
    virtual ~BaseTypeInfo(void) {}

    const MetaDataContainer& getMetaData(void) const;

    std::string getSourceFile(void) const;

    NameSpaceInfo const* getCurrentNamespace() const;

    Cursor& getCurosr();

protected:
    MetaDataContainer m_MetaData;

    NameSpaceInfo const* m_NameSpace;

private:
    // cursor that represents the root of this language type
    Cursor m_RootCursor;
};