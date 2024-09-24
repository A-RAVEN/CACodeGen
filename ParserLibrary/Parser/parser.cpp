#include "parser.h"
#include "Parser/parser_options.h"

constexpr char* REFLECTION_MACRO = "-D__REFLECTION_PARSER__";

class LibClangArguments
{
public:
    const int STD_POS = 2;

    std::vector<const char*> const& GetArguments()
    {
        return arguments;
    }

    void SetCPPStandard(CPPStandard standard)
    {
        switch (standard) {
            case CPPStandard::e2011:
                arguments[STD_POS] = "-std=c++11";
                break;
            case CPPStandard::e2014:
                arguments[STD_POS] = "-std=c++14";
                break;
            case CPPStandard::e2017:
                arguments[STD_POS] = "-std=c++17";
                break;
            case CPPStandard::e2020:
                arguments[STD_POS] = "-std=c++20";
                break;
            default:
                break;
        }
    }

    std::vector<const char*>  arguments =
                            {{
                                "-x",
                                "c++",
                                "-std=c++11",
                                REFLECTION_MACRO,
                                "-DNDEBUG",
                                "-D__clang__",
                                "-w",
                                "-MG",
                                "-M",
                                "-ferror-limit=0",
                                "-o clangLog.txt"
                            }};
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
    m_TranslationUnit = clang_createTranslationUnitFromSourceFile(
    m_Index, temp_parsing_file_path.c_str(), static_cast<int>(argStr.size()), argStr.data(), 0, nullptr);
    auto cursor = clang_getTranslationUnitCursor(m_TranslationUnit);
}
