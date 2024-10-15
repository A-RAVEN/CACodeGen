#pragma once
#include "BaseType.h"

class NameSpaceInfo;
class ClassOrStructInfo;
class CodeInfoContainerState;

class ArgumentInfo
{
public:
    ArgumentInfo(Cursor const& cursor);
private:
    Cursor m_Cursor;
};

class MethodInfo : public BaseTypeInfo
{
public:
    MethodInfo(const Cursor& cursor, CodeInfoContainerState* codeInfoState, ClassOrStructInfo const* ownerClass = nullptr);
    virtual ~MethodInfo(void) {}
    int GetArgumentCount(void) const;
private:
    ClassOrStructInfo const* m_OwnerClass;
    std::string m_Name;
    std::vector<ArgumentInfo> m_Arguments;
};