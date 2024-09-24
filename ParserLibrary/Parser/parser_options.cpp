#include "parser_options.h"

std::vector<std::filesystem::path> WorkSpaceInfo::FindAllHeaderFilesInProject() const
{
    std::vector<std::filesystem::path> result;
    fs::path path = m_workspace_path;
    if(fs::exists(path) && fs::is_directory(path))
    {
        for (const auto& file : fs::directory_iterator(path))
        {
            if(file.path().extension() == ".h")
            {
                auto file_path = file.path().string();
                std::replace(file_path.begin(), file_path.end(), '\\', '/');
                result.push_back(file_path);
            }
        }
    }
    return result;
}
