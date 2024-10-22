#pragma once
#include <vector>
#include <memory>
#include "ClassOrStruct.h"
#include "NameSpaceInfo.h"

class CodeInfoContainer
{
public:
    std::vector<std::shared_ptr<ClassOrStructInfo>> const& GetAllClasses() const { return m_AllClasses; }
    std::shared_ptr<ClassOrStructInfo> FindClassOrStruct(Cursor const& classDefinitionCursor) const;
private:
    std::vector<std::shared_ptr<ClassOrStructInfo>> m_AllClasses;
    std::map<uint32_t, int> m_CursorToClassMap;
    
    std::vector<std::shared_ptr<ClassOrStructInfo>> m_RootClasses;
    std::vector<std::shared_ptr<NameSpaceInfo>> m_RootNameSpaces;
    friend class CodeInfoContainerState;
};

class CodeInfoContainerState
{
public:
    CodeInfoContainerState(CodeInfoContainer* container);
    std::shared_ptr<ClassOrStructInfo> AddClassOrStruct(Cursor const& classDefinitionCursor, ClassOrStructInfo const* ownerClass = nullptr);
    void SubNameSpace(std::string nameSpace);
    void ParentNameSpace();
    std::shared_ptr<NameSpaceInfo> m_CurrentNameSpace = nullptr;
private:
    CodeInfoContainer* m_Container;
};