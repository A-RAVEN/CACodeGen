#include "NameSpaceInfo.h"

NameSpaceInfo::NameSpaceInfo(std::string nameSpace, std::shared_ptr<NameSpaceInfo> parentNameSpace, bool anonymous)
 : m_NameSpace(nameSpace)
 , m_ParentNameSpace(parentNameSpace)
 , m_Anonymous(anonymous)
{
}
