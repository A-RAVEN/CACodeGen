// #include <CAReflection/reflection.h>
// #include <Parser/parser.h>
#include <argparse/argparse.hpp>

int main(int argc, char *argv[])
{
    argparse::ArgumentParser program("cacodegen", "0.0.1");

    // program.add_argument("--cppstd")
    // .help("cpp standard: 11, 14, 17, 20")
    // .default_value(20)
    // .scan<'i', int>();

    // program.add_argument("--includes")
    // .help("include directories")
    // .default_value(std::vector<std::string>{})
    // .nargs(argparse::nargs_pattern::any);

    // program.add_argument("--workspace")
    // .required()
    // .help("workspace path");

    // try {
    //     program.parse_args(argc, argv);
    // }
    // catch (const std::exception& err) {
    //     std::cerr << err.what() << std::endl;
    //     std::cerr << program;
    //     return 1;
    // }

    // if(program.get<bool>("--help"))
    // {
    //     std::cout << program <<std::endl;
    //     return 0;
    // }

    // LanguageInfo languageInfo;
    // languageInfo.m_cpp_standard = CPPStandard::e2020;
    // int cppstd = program.get<int>("--cppstd");
    // std::cout << "using cpp standard: " << std::to_string(cppstd) << std::endl;
    // switch(cppstd)
    // {
    //     case 11:
    //         languageInfo.m_cpp_standard = CPPStandard::e2011;
    //         break;
    //     case 14:
    //         languageInfo.m_cpp_standard = CPPStandard::e2014;
    //         break;
    //     case 17:
    //         languageInfo.m_cpp_standard = CPPStandard::e2017;
    //         break;
    //     case 20:
    //         languageInfo.m_cpp_standard = CPPStandard::e2020;
    //         break;
    // }

    // WorkSpaceInfo workSpaceInfo;
    // std::string workspace_path = program.get<std::string>("--workspace");
    // workSpaceInfo.SetWorkSpacePath(workspace_path);
    // std::cout << "work space: " << workspace_path << std::endl;
    // auto include_paths = program.get<std::vector<std::string>>("--includes");
    // std::cout << "include paths: " << std::endl;
    // for(auto& include_path : include_paths)
    // {
    //     std::cout << "  " << include_path << std::endl;
    //     workSpaceInfo.AddIncludePath(include_path);
    // }

    // CodeParser parser(languageInfo, workSpaceInfo);
    // parser.Parse();
    return 0;
}
