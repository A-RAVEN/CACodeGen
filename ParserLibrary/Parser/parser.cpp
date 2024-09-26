#include "parser.h"
#include "Parser/parser_options.h"
#include <Cursor/cursor.h>


class LibClangArguments
{
public:
    const int STD_POS = 2;
    const char* REFLECTION_MACRO = "-D__REFLECTION_PARSER__";

    std::vector<const char*> GetArguments()
    {
        std::vector<const char*>  arguments =
        {{
            "-x",
            "c++",
            GetCPPStandardStr(),
            REFLECTION_MACRO,
            "-DNDEBUG",
            "-D__clang__",
            "-w",
            "-MG",
            "-M",
            "-ferror-limit=0",
            "-o clangLog.txt"
        }};

        if(m_IncludePaths.size() > 0)
        {
            arguments.push_back("-I");
            for(std::string const& path : m_IncludePaths)
            {
                arguments.push_back(path.c_str());
            }
        }
        return arguments;
    }

    const char* GetCPPStandardStr()
    {
        switch (m_CPPStandard) {
            case CPPStandard::e2014:
                return "-std=c++14";
            case CPPStandard::e2017:
                return "-std=c++17";
            case CPPStandard::e2020:
                return "-std=c++20";
            case CPPStandard::e2011:
            default:
                return "-std=c++11";
        }
    }

    void SetCPPStandard(CPPStandard standard)
    {
        m_CPPStandard = standard;
    }

    void ClearIncludePaths()
    {
        m_IncludePaths.clear();
    }

    void AddIncludePath(std::string const& path)
    {
        m_IncludePaths.push_back(path);
    }

    CPPStandard m_CPPStandard = CPPStandard::e2011;
    std::vector<std::string> m_IncludePaths;
};

CodeParser::CodeParser(LanguageInfo const& languageInfo, WorkSpaceInfo const& workSpaceInfo)
 : m_LanguageInfo(languageInfo)
 , m_WorkSpaceInfo(workSpaceInfo)
{
    
}


void CodeParser::Parse()
{
    LibClangArguments clangArgs;
    clangArgs.SetCPPStandard(m_LanguageInfo.m_cpp_standard);
    clangArgs.AddIncludePath(m_WorkSpaceInfo.GetWorkSpacePath().string());

    auto parsingSourceFiles = m_WorkSpaceInfo.FindAllHeaderFilesInProject();

    std::fstream temp_parsing_file;
    std::string temp_parsing_file_path = "Test.cpp";
    temp_parsing_file.open(temp_parsing_file_path, std::ios::out);
    if (!temp_parsing_file.is_open())
    {
        std::cout << "Could not open the Source Include file: " << temp_parsing_file_path << std::endl;
    }
    for (auto include_item : parsingSourceFiles)
    {
        temp_parsing_file << "#include  \"" << include_item.string() << "\"" << std::endl;
    }
    temp_parsing_file.close();

    auto argStr = clangArgs.GetArguments();
    m_Index = clang_createIndex(0, 0);
    m_TranslationUnit = clang_createTranslationUnitFromSourceFile(
    m_Index, temp_parsing_file_path.c_str(), static_cast<int>(argStr.size()), argStr.data(), 0, nullptr);
    Cursor cursor = clang_getTranslationUnitCursor(m_TranslationUnit);

    clang_disposeTranslationUnit(m_TranslationUnit);
    clang_disposeIndex(m_Index);
}

void CodeParser::TraverseCodeSpace(const Cursor& cursor, NameSpace& current_namespace)
{
    for (auto& child : cursor.getChildren())
    {
        auto kind = child.getKind();

        // actual definition and a class or struct
        if (child.isDefinition() && (kind == CXCursor_ClassDecl || kind == CXCursor_StructDecl))
        {
            //auto class_ptr = std::make_shared<Class>(child, current_namespace);

            //TRY_ADD_LANGUAGE_TYPE(class_ptr, classes);
        }
        else
        {
            if (kind == CXCursor_Namespace)
            {
                auto display_name = cursor.getDisplayName();
                if (!display_name.empty())
                {
                    current_namespace.Subspace(display_name);
                    TraverseCodeSpace(cursor, current_namespace);
                    current_namespace.ParentSpace();
                }
            }
        }
    }
}
