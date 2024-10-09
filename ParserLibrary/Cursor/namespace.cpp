#include "namespace.h"

void NameSpace::Subspace(std::string const& newname)
{
    m_names.push_back(newname);
}
void NameSpace::ParentSpace()
{
    m_names.pop_back();
}
std::string NameSpace::QualifiedName(std::string const& displayName, std::string const& split) const
{
    if(m_names.empty())
        return displayName;
    std::string result;
    for(auto& name : m_names)
    {
        result += split + name;
    }
    result += split + displayName;
    return result;
}

std::string NameSpace::QualifiedNameSpace(std::string const& split) const
{
    if(m_names.empty())
        return "";
    std::string result;
    for(auto& name : m_names)
    {
        result += split + name;
    }
    return result;
}