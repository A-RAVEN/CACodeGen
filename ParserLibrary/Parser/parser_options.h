#pragma once
#include <pch.h>
#include <vector>
#include <string>
#include <filesystem>

enum class CPPStandard
{
    e2011,
    e2014,
    e2017,
    e2020,
};

class LanguageInfo
{
public:
    CPPStandard m_cpp_standard;
};

class WorkSpaceInfo
{
public:
    std::vector<fs::path> m_include_paths;
    fs::path m_workspace_path;
    fs::path m_output_path;

    std::vector<std::filesystem::path> FindAllHeaderFilesInProject() const;
};