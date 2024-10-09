#pragma once
#include "BaseType.h"

class NameSpaceInfo;
class CodeInfoContainerState;
class BaseClassInfo
{
public:
    BaseClassInfo(const Cursor& baseClassCursor);
private:
    Cursor m_BaseClassCursor;
};

class ClassOrStructInfo : public BaseTypeInfo
{
public:
    ClassOrStructInfo(const Cursor& cursor, CodeInfoContainerState* codeInfoState, ClassOrStructInfo const* ownerClass = nullptr);
    virtual ~ClassOrStructInfo(void) {}
private:
    std::vector<BaseClassInfo> m_BaseClasses;
    ClassOrStructInfo const* m_OwnerClass;
    std::vector<ClassOrStructInfo*> m_NestedClasses;
};