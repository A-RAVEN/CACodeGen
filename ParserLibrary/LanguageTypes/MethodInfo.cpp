#include "MethodInfo.h"
#include "CodeInfoContainer.h"
#include "ClassOrStruct.h"
#include <iostream>

ArgumentInfo::ArgumentInfo(Cursor const& cursor)
    : m_Cursor(cursor)
{
    assert(cursor.getKind() == CXCursor_ParmDecl);
    std::cout << "argument: " << m_Cursor.getDisplayName() << " type: " << m_Cursor.getType().GetDisplayName() << std::endl;
}


MethodInfo::MethodInfo(const Cursor& cursor
, CodeInfoContainerState* codeInfoState
, ClassOrStructInfo const* ownerClass)
    : BaseTypeInfo(cursor, codeInfoState->m_CurrentNameSpace.get())
    , m_OwnerClass(ownerClass)
    , m_Name(cursor.getDisplayName())
{
    assert(cursor.getKind() == CXCursor_CXXMethod);
    int count = cursor.getType().GetArgumentCount();
    std::cout << "Method: " << m_Name << " argument count: " << count << std::endl;

    auto children = cursor.getChildren();
    for (auto& child : children)
    {
        switch (child.getKind())
        {
        case CXCursor_ParmDecl:
            m_Arguments.emplace_back(child);
            break;
        default:
            break;
        }
    }


    // m_Arguments.clear();
    // m_Arguments.reserve(count);
    // for (int i = 0; i < count; ++i)
    // {
    //     m_Arguments.emplace_back(cursor.getType().GetArgument(i));
    // }
}

int MethodInfo::GetArgumentCount(void) const
{
    return getCurosr().getType().GetArgumentCount();
}

