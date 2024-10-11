#pragma once

#include <string>

class MetaPropertyInfo
{
public:
    MetaPropertyInfo(std::string const& name, std::string const& constructArgs);
    std::string GetName() const;
    std::string GetConstructArgs() const;
private:
    std::string m_Name;
    std::string m_ConstructArgs;
};