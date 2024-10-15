#pragma once
#include "BaseType.h"

class NameSpaceInfo;
class ClassOrStructInfo;
class CodeInfoContainerState;
class FieldInfo : public BaseTypeInfo
{
public:
    FieldInfo(const Cursor& cursor, CodeInfoContainerState* codeInfoState, ClassOrStructInfo const* ownerClass = nullptr);
    virtual ~FieldInfo(void) {}
    bool IsBuiltInType(void) const;
private:
    ClassOrStructInfo const* m_OwnerClass;
    std::string m_Name;
};