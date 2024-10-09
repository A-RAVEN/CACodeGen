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
    std::cout << "class Info:" << cursor.getDisplayName() << " namespace:" << std::endl;
    auto children = getCurosr().getChildren();
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
                //m_fields.emplace_back(new Field(child, current_namespace, this));
                break;
            // method
            case CXCursor_CXXMethod:
                //m_methods.emplace_back(new Method(child, current_namespace, this));
            default:
                break;
        }
    }
}
