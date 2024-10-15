#pragma once
#include <vector>
#include <memory>
#include "ClassOrStruct.h"
#include "NameSpaceInfo.h"

class CodeInfoContainer
{
public:
    std::vector<std::shared_ptr<ClassOrStructInfo>> const& GetAllClasses() const { return m_AllClasses; }
private:
    std::vector<std::shared_ptr<ClassOrStructInfo>> m_AllClasses;
    
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