#include "parser_options.h"

void WorkSpaceInfo::SetWorkSpacePath(std::string path)
{
    std::replace(path.begin(), path.end(), '\\', '/');
    m_workspace_path = path;
}

fs::path WorkSpaceInfo::GetWorkSpacePath() const
{
    return m_workspace_path;
}

fs::path WorkSpaceInfo::GetCodeGenOutputPath() const
{
    return GetWorkSpacePath() / "CodeGen";
}

std::string WorkSpaceInfo::GetWorkSpaceFolderName() const
{
    return m_workspace_path.has_filename() ? m_workspace_path.filename().string() : "unknown_workspace";
}

void WorkSpaceInfo::AddIncludePath(std::string path)
{
    std::replace(path.begin(), path.end(), '\\', '/');
    m_include_paths.push_back(path);
}

std::vector<fs::path> const& WorkSpaceInfo::GetExtraIncludePaths() const
{
    return m_include_paths;
}

std::vector<fs::path> WorkSpaceInfo::FindAllHeaderFilesInProject() const
{
    std::vector<fs::path> result;
    fs::path path = GetWorkSpacePath();
    if(fs::exists(path) && fs::is_directory(path))
    {
        for (const auto& file : fs::directory_iterator(path))
        {
            if(file.path().extension() == ".h")
            {
                auto relativePath = fs::relative(file.path(), path);
                auto file_path = relativePath.string();
                std::replace(file_path.begin(), file_path.end(), '\\', '/');
                result.push_back(file_path);
            }
        }
    }
    return result;
}
