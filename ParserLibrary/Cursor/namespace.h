#pragma once

#include <pch.h>

class NameSpace
{
public:
    void Subspace(std::string const& newname);
    void ParentSpace();
    std::string QualifiedNameSpace(std::string const& split = "::") const;
    std::string QualifiedName(std::string const& displayName, std::string const& split = "::") const;
private:
    std::vector<std::string> m_names;
};