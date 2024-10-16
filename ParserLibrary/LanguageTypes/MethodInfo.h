#pragma once
#include "BaseType.h"

class NameSpaceInfo;
class ClassOrStructInfo;
class CodeInfoContainerState;

class ArgumentInfo
{
public:
    ArgumentInfo(Cursor const& cursor);
    std::string GetName() const { return m_Cursor.getDisplayName(); }
    std::string GetTypeName() const { return m_Cursor.getType().GetDisplayName(); }
    Cursor const& GetCursor() const { return m_Cursor; }
private:
    Cursor m_Cursor;
};

class MethodInfo : public BaseTypeInfo
{
public:
    MethodInfo(const Cursor& cursor, CodeInfoContainerState* codeInfoState, ClassOrStructInfo const* ownerClass = nullptr);
    virtual ~MethodInfo(void) {}
    int GetArgumentCount(void) const;
    bool IsConst(void) const { return m_IsConst; }
    std::string const& GetSpelling() const { return m_Spelling; }
    std::vector<ArgumentInfo> const& GetArguments(void) const { return m_Arguments; }
    ClassOrStructInfo const* GetOwnerClass(void) const { return m_OwnerClass; }
private:
    ClassOrStructInfo const* m_OwnerClass;
    std::string m_Spelling;
    bool m_IsConst;
    std::vector<ArgumentInfo> m_Arguments;
};