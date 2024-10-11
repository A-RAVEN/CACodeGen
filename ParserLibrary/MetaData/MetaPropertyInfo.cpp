#include "MetaPropertyInfo.h"

MetaPropertyInfo::MetaPropertyInfo(std::string const& name, std::string const& constructArgs)
    : m_Name(name)
    , m_ConstructArgs(constructArgs)
{
}

std::string MetaPropertyInfo::GetName() const
{
    return m_Name;
}

std::string MetaPropertyInfo::GetConstructArgs() const
{
    return m_ConstructArgs;
}