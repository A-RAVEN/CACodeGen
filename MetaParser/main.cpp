#include <Parser/parser.h>
#include <argparse/argparse.hpp>
#include <Windows.h>
#include <filesystem>

int main(int argc, char *argv[])
{
    std::filesystem::path current_path = std::filesystem::current_path();
    std::cout << "current path: " << current_path.string() << std::endl;

    //LoadLibrary("CACInterfaceCodeGen.dll");
    std::cout << "PARSER ARGS:" << std::endl;
    for(int i = 0; i < argc; ++i)
    {
        std::cout << argv[i] << std::endl;
    }

    argparse::ArgumentParser program("cacodegen", "0.0.1");

    program.add_argument("-P", "--project")
    .required()
    .help("project name");

    program.add_argument("-S","--cppstd")
    .help("cpp standard: 11, 14, 17, 20")
    .default_value(20)
    .scan<'i', int>();

    program.add_argument("-I", "--includes")
    .help("include directories")
    .default_value(std::vector<std::string>{})
    .append();

    program.add_argument("-M", "--modules")
    .help("codegen module directories")
    .default_value(std::vector<std::string>{})
    .append();

    program.add_argument("-W", "--workspace")
    .required()
    .help("workspace path");

    program.add_argument("-O", "--outdir")
    .required()
    .help("library output dir");

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    if(program.get<bool>("--help"))
    {
        std::cout << program <<std::endl;
        return 0;
    }

    auto module_paths = program.get<std::vector<std::string>>("--modules");
    for(auto& module_path : module_paths)
    {
        std::cout << "load codegen modules at: " << module_path << std::endl;
        std::filesystem::directory_iterator module_dir(module_path);
        for(auto& entry : module_dir)
        {
            if(entry.is_regular_file())
            {
                if(entry.path().extension() == ".dll")
                {
                    std::cout << "loading: " << entry.path().string() << std::endl;
                    LoadLibrary(entry.path().string().c_str());
                }
            }
        }
    }


    LanguageInfo languageInfo;
    languageInfo.m_cpp_standard = CPPStandard::e2020;
    int cppstd = program.get<int>("--cppstd");
    std::cout << "using cpp standard: " << std::to_string(cppstd) << std::endl;
    switch(cppstd)
    {
        case 11:
            languageInfo.m_cpp_standard = CPPStandard::e2011;
            break;
        case 14:
            languageInfo.m_cpp_standard = CPPStandard::e2014;
            break;
        case 17:
            languageInfo.m_cpp_standard = CPPStandard::e2017;
            break;
        case 20:
            languageInfo.m_cpp_standard = CPPStandard::e2020;
            break;
    }

    WorkSpaceInfo workSpaceInfo;
    std::string project_name = program.get<std::string>("--project");
    workSpaceInfo.SetProjectName(project_name);
    std::cout << "project name: " << project_name << std::endl;
    std::string workspace_path = program.get<std::string>("--workspace");
    std::string output_path = program.get<std::string>("--outdir");
    workSpaceInfo.SetWorkSpacePath(workspace_path);
    workSpaceInfo.SetOutputPath(output_path);
    std::cout << "work space: " << workspace_path << std::endl;
    auto include_paths = program.get<std::vector<std::string>>("--includes");
    std::cout << "include paths: " << std::endl;
    for(auto& include_path : include_paths)
    {
        std::cout << "  " << include_path << std::endl;
        workSpaceInfo.AddIncludePath(include_path);
    }

    CodeParser parser(languageInfo, workSpaceInfo);
    parser.Parse();
    return 0;
}
