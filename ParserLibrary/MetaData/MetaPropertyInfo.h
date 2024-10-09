#pragma once

#include <string>

class MetaPropertyInfo
{
public:
MetaPropertyInfo(std::string const& name);
std::string GetName() const;
private:
std::string m_Name;

};