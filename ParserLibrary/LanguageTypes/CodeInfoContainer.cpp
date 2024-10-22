#include "CodeInfoContainer.h"

CodeInfoContainerState::CodeInfoContainerState(CodeInfoContainer* container)
: m_Container(container)
{
}

std::shared_ptr<ClassOrStructInfo> CodeInfoContainer::FindClassOrStruct(Cursor const& classDefinitionCursor) const
{
    auto found =  m_CursorToClassMap.find(classDefinitionCursor.getHash());
    if(found != m_CursorToClassMap.end())
    {
        return m_AllClasses[found->second];
    }
    return nullptr;
}

std::shared_ptr<ClassOrStructInfo> CodeInfoContainerState::AddClassOrStruct(Cursor const& classDefinitionCursor, ClassOrStructInfo const* ownerClass)
{
    auto found =  m_Container->m_CursorToClassMap.find(classDefinitionCursor.getHash());
    if(found != m_Container->m_CursorToClassMap.end())
    {
        return m_Container->m_AllClasses[found->second];
    }
    auto classInfo = std::make_shared<ClassOrStructInfo>(classDefinitionCursor, this, ownerClass);
    m_Container->m_AllClasses.push_back(classInfo);
    m_Container->m_CursorToClassMap.insert({classDefinitionCursor.getHash(), m_Container->m_AllClasses.size() - 1});
    if(m_CurrentNameSpace == nullptr)
    {
        m_Container->m_RootClasses.push_back(classInfo);
    }
    else
    {
        m_CurrentNameSpace->m_ClassesInNameSpace.push_back(classInfo);
    }
    return classInfo;
}

void CodeInfoContainerState::SubNameSpace(std::string nameSpace)
{
    if(m_CurrentNameSpace == nullptr)
    {
        m_Container->m_RootNameSpaces.push_back(std::make_shared<NameSpaceInfo>(nameSpace, nullptr, false));
    }
    else
    {
        for(auto& subNameSpace : m_CurrentNameSpace->m_SubNameSpaces)
        {
            if(subNameSpace->m_NameSpace == nameSpace)
            {
                m_CurrentNameSpace = subNameSpace;
                return;
            }
        }
        auto parentNameSpace = m_CurrentNameSpace;
        m_CurrentNameSpace = std::make_shared<NameSpaceInfo>(nameSpace, parentNameSpace, false);
        parentNameSpace->m_SubNameSpaces.push_back(m_CurrentNameSpace);
    }
}

void CodeInfoContainerState::ParentNameSpace()
{
    if(m_CurrentNameSpace == nullptr)
    {
        return;
    }
    m_CurrentNameSpace = m_CurrentNameSpace->m_ParentNameSpace;
}