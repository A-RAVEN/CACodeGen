#include "FieldInfo.h"
#include "CodeInfoContainer.h"
#include "ClassOrStruct.h"
#include <iostream>

FieldInfo::FieldInfo(const Cursor& cursor
, CodeInfoContainerState* codeInfoState
, ClassOrStructInfo const* ownerClass)
 : BaseTypeInfo(cursor, codeInfoState->m_CurrentNameSpace.get())
 , m_OwnerClass(ownerClass)
 , m_Name(cursor.getDisplayName())
{
    assert(cursor.getKind() == CXCursor_FieldDecl);
    std::cout << "Field: " << m_Name << " Type: " << getCurosr().getType().GetDisplayName() << std::endl;
}

bool FieldInfo::IsBuiltInType(void) const
{
    auto cursorKind = getCurosr().getType().GetKind();
    return cursorKind >= CXType_FirstBuiltin && cursorKind <= CXType_LastBuiltin;
}


