#include "MethodInfo.h"
#include "CodeInfoContainer.h"
#include "ClassOrStruct.h"
#include <iostream>

ArgumentInfo::ArgumentInfo(Cursor const& cursor)
    : m_Cursor(cursor)
{
    assert(cursor.getKind() == CXCursor_ParmDecl);
    std::cout << "argument: " << m_Cursor.getDisplayName()
    << " type: " << m_Cursor.getType().GetDisplayName()
    << " isReference: " << (m_Cursor.getType().IsReference())
    << " isPointer: " << (m_Cursor.getType().IsPointer())
    << " canonical type: " << m_Cursor.getType().GetCanonicalType().GetDisplayName();
    std::cout << " kind spelling: " << m_Cursor.getType().GetKindSpelling();
    if(m_Cursor.getType().IsReferenceOrPointer())
    {
        std::cout << " pointee type: " << m_Cursor.getType().GetPointeeType().GetDisplayName();
    }
    std::cout << std::endl;
}


MethodInfo::MethodInfo(const Cursor& cursor
, CodeInfoContainerState* codeInfoState
, ClassOrStructInfo const* ownerClass)
    : BaseTypeInfo(cursor, codeInfoState->m_CurrentNameSpace.get())
    , m_OwnerClass(ownerClass)
    , m_Spelling(cursor.getSpelling())
{
    assert(cursor.getKind() == CXCursor_CXXMethod);
    std::cout << "Method: " << cursor.getDisplayName() 
    << "\n\tspelling: " << cursor.getSpelling() 
    << "\n\tcanonical: " << cursor.getType().GetCanonicalType().GetDisplayName() 
    << "\n\targument count: " << cursor.getType().GetArgumentCount() 
    << std::endl;
    m_IsConst = cursor.getType().IsConst();
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

