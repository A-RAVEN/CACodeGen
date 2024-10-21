#include "ClassOrStruct.h"
#include <iostream>
#include "CodeInfoContainer.h"

BaseClassInfo::BaseClassInfo(const Cursor& baseClassCursor) : m_BaseClassCursor(baseClassCursor)
{
    auto baseClassDeclare = baseClassCursor.getType().GetDeclaration();
    std::cout << "base class type:" << baseClassDeclare.getDisplayName() << std::endl;
}

ClassOrStructInfo::ClassOrStructInfo(const Cursor& cursor, CodeInfoContainerState* codeInfoState, ClassOrStructInfo const* ownerClass)
 : BaseTypeInfo(cursor, codeInfoState->m_CurrentNameSpace.get())
, m_OwnerClass(ownerClass)
{
    assert(cursor.getKind() == CXCursor_ClassDecl || cursor.getKind() == CXCursor_StructDecl);
    m_IsStruct = cursor.getKind() == CXCursor_StructDecl;
    std::cout << "class Info:" << cursor.getDisplayName() << " namespace:" << std::endl;
    auto children = getCurosr().getChildren();
    bool anyDefaultConstructor = false;
    for(auto& child : children)
    {
        switch (child.getKind())
        {
            case CXCursor_CXXBaseSpecifier: {
                BaseClassInfo baseClass(child);
                m_BaseClasses.emplace_back(baseClass);
                break;
            }
            case CXCursor_ClassDecl:
            case CXCursor_StructDecl:
            {
                if(child.isDefinition())
                {
                    m_NestedClasses.emplace_back(codeInfoState->AddClassOrStruct(child, this).get());
                }
                break;
            }
            // field
            case CXCursor_FieldDecl:
                m_Fields.emplace_back(child, codeInfoState, this);
                break;
            // method
            case CXCursor_CXXMethod:
                m_Methods.emplace_back(child, codeInfoState, this);
                break;
            case CXCursor_Constructor:
                std::cout << "constructor found: " << cursor.getDisplayName() << std::endl;
                if(clang_CXXConstructor_isDefaultConstructor(child.get()))
                {
                    anyDefaultConstructor = true;
                    std::cout << "default constructor found: " << cursor.getDisplayName() << std::endl;
                }
                m_Constructors.emplace_back(child, codeInfoState, this);
                break;
            default:
                break;
        }
        m_DefaultConstructable = anyDefaultConstructor || m_Constructors.empty();
    }
}
