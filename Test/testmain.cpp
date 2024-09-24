#include <CACodeGen/CAMetaProps.h>
#include <CAReflection/reflection.h>
#include <Parser/parser.h>

int main()
{
    std::string path = "D:/Projects/CACodeGen/Test";
    LanguageInfo languageInfo;
    languageInfo.m_cpp_standard = CPPStandard::e2020;
    WorkSpaceInfo workSpaceInfo;
    workSpaceInfo.m_workspace_path = path;

    CodeParser parser(languageInfo, workSpaceInfo);
    parser.Parse();
    return 0;
}
