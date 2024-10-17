#include "parser.h"
#include "Parser/parser_options.h"
#include <Cursor/cursor.h>
#include <MetaData/MetaDataContainer.h>
#include <LanguageTypes/BaseType.h>
#include <LanguageTypes/ClassOrStruct.h>
#include <LanguageTypes/CodeInfoContainer.h>
#include <Generator/Generator.h>

class LibClangArguments
{
public:
    const int STD_POS = 2;
    const char* REFLECTION_MACRO = "-D__CASCADE_REFLECTION_PARSER__";

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
            for(auto ppath = m_IncludePaths.begin(); ppath != m_IncludePaths.end(); ++ppath)
            {
                arguments.push_back("-I");
                arguments.push_back(ppath->c_str());
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
        if (!fs::exists(path))
        {
            std::cerr << "includepath: (" << path << ") is not exist" << std::endl;
        }
        m_IncludePaths.push_back(path);
        if(m_CombinedIncludePaths == "")
        {
            m_CombinedIncludePaths += "-I";
        }
        m_CombinedIncludePaths += " " + path;
    }

    CPPStandard m_CPPStandard = CPPStandard::e2011;
    std::vector<std::string> m_IncludePaths;
    std::string m_CombinedIncludePaths = "";
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
    for(auto& additionalIncludePaths : m_WorkSpaceInfo.GetExtraIncludePaths())
    {
        clangArgs.AddIncludePath(additionalIncludePaths.string());
    }

    auto parsingSourceFiles = m_WorkSpaceInfo.FindAllHeaderFilesInProject();

    std::fstream temp_parsing_file;
    fs::remove_all(m_WorkSpaceInfo.GetCodeGenOutputPath());
    std::filesystem::create_directories(m_WorkSpaceInfo.GetCodeGenOutputPath());
    fs::path codegenFilePath = m_WorkSpaceInfo.GetCodeGenOutputPath() / "CodeGen.cpp";
    temp_parsing_file.open(codegenFilePath, std::ios::out);
    if (!temp_parsing_file.is_open())
    {
        std::cout << "Could not open the Source Include file: " << codegenFilePath << std::endl;
    }
    for (auto include_item : parsingSourceFiles)
    {
        temp_parsing_file << "#include  <" << include_item.string() << ">" << std::endl;
    }
    temp_parsing_file.close();

    auto argStr = clangArgs.GetArguments();
    std::cout << "arguments: " << std::endl;
    for(auto arg : argStr)
    {
        std::cout << arg << std::endl;
    }
    m_Index = clang_createIndex(0, 0);
    m_TranslationUnit = clang_createTranslationUnitFromSourceFile(
    m_Index, codegenFilePath.string().c_str(), static_cast<int>(argStr.size()), argStr.data(), 0, nullptr);
    Cursor cursor = clang_getTranslationUnitCursor(m_TranslationUnit);
    CodeInfoContainer codeInfoContainer;
    CodeInfoContainerState containerState(&codeInfoContainer);
    TraverseCodeSpace(cursor, containerState);

    ICodeGenerator codeGenerator{};
    codeGenerator.GenerateCode(m_WorkSpaceInfo, codeInfoContainer);

    clang_disposeTranslationUnit(m_TranslationUnit);
    clang_disposeIndex(m_Index);
}





void CodeParser::TraverseCodeSpace(const Cursor& parentCursor, CodeInfoContainerState& currentState)
{
    std::cout << "Traverse Code Space" << std::endl;
    for (auto& cursor : parentCursor.getChildren())
    {
        auto kind = cursor.getKind();

        // actual definition and a class or struct
        if (cursor.isDefinition())
        {
            switch(kind)
            {
                case CXCursor_ClassDecl:
                case CXCursor_StructDecl:
                {
                    currentState.AddClassOrStruct(cursor, nullptr);
                    break;
                }
                case CXCursor_EnumDecl:
                    break;
                case CXCursor_FunctionDecl:
                    break;
            }
        }
        {
            switch(kind)
            {
                case CXCursor_Namespace:
                {
                    std::cout << "enter subspace: " << cursor.getDisplayName() << std::endl;
                    auto display_name = cursor.getDisplayName();
                    if (!display_name.empty())
                    {
                        currentState.SubNameSpace(cursor.getDisplayName());
                        TraverseCodeSpace(cursor, currentState);
                        currentState.ParentNameSpace();
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }
}
