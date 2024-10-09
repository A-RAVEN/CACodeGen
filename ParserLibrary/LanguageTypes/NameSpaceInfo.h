#pragma once
#include <vector>
#include <memory>
#include <string>

class ClassOrStructInfo;
class NameSpaceInfo
{
public:
    NameSpaceInfo(std::string nameSpace, std::shared_ptr<NameSpaceInfo> parentNameSpace, bool anonymous = false);
private:
    bool m_Anonymous;
    std::string m_NameSpace;
    std::shared_ptr<NameSpaceInfo> m_ParentNameSpace;
    std::vector<std::shared_ptr<NameSpaceInfo>> m_SubNameSpaces;
    std::vector<std::shared_ptr<ClassOrStructInfo>> m_ClassesInNameSpace;
    friend class CodeInfoContainerState;
};