#pragma once
#include "BaseType.h"
#include "FieldInfo.h"
#include "MethodInfo.h"

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
    std::vector<MethodInfo> const& GetMethods() const { return m_Methods; }
    bool DefaultConstructable() const { return m_DefaultConstructable; }
private:
    bool m_IsStruct;
    std::vector<BaseClassInfo> m_BaseClasses;
    ClassOrStructInfo const* m_OwnerClass;
    std::vector<ClassOrStructInfo*> m_NestedClasses;
    std::vector<FieldInfo> m_Fields;
    std::vector<MethodInfo> m_Methods;
    std::vector<MethodInfo> m_Constructors;
    bool m_DefaultConstructable;
};