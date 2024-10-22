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
    std::vector<MethodInfo> const& GetConstructors() const { return m_Constructors; }
    MetaPropertyInfo const* GetPropertyInfoInParents(std::string const& propertyName) const;
    std::string GetFullName(std::string const& separator = "::") const;
    
    bool DefaultConstructable() const { return m_DefaultConstructable; }
    bool CopyConstructable() const { return m_CopyConstructable; }
private:
    bool m_IsStruct;
    std::vector<BaseClassInfo> m_BaseClasses;
    ClassOrStructInfo const* m_OwnerClass;
    std::vector<ClassOrStructInfo*> m_NestedClasses;
    std::vector<FieldInfo> m_Fields;
    std::vector<MethodInfo> m_Methods;
    std::vector<MethodInfo> m_Constructors;
    bool m_DefaultConstructable;
    bool m_CopyConstructable;
};