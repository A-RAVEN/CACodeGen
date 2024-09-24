#pragma once

#include <pch.h>
#include <cursor.h>

class NameSpace
{
public:
    void Subspace(std::string const& newname);
    void ParentSpace();

    std::string QualifiedName(std::string const& displayName, std::string const& split = "::");
    std::string QualifiedName(Cursor const& displayName, std::string const& split = "::");
private:
    std::vector<std::string> m_names;
};