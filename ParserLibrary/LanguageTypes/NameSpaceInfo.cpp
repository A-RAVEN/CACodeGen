#include "NameSpaceInfo.h"

NameSpaceInfo::NameSpaceInfo(std::string nameSpace, std::shared_ptr<NameSpaceInfo> parentNameSpace, bool anonymous)
 : m_NameSpace(nameSpace)
 , m_ParentNameSpace(parentNameSpace)
 , m_Anonymous(anonymous)
{
}

std::string NameSpaceInfo::GetFullNameSpace(std::string separator) const
{
    std::string fullNameSpace = m_NameSpace;
    if(m_ParentNameSpace)
    {
        fullNameSpace = m_ParentNameSpace->GetFullNameSpace(separator) + separator + fullNameSpace;
    }
    return fullNameSpace;
}
