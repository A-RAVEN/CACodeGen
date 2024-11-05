#include <Generator/GeneratorManager.h>
#include <Generator/Generator.h>
#include <LanguageTypes/CodeInfoContainer.h>
#include <string_utils.h>
#include <set>
#include <CInterfaceGeneratorCommon.h>

class CSharpBindingsGenerator : public ICodeGenerator
{
public:
    virtual void GenerateCode(WorkSpaceInfo const& workSpaceInfo, CodeInfoContainer const& codeInfoContainer, GeneratedCodeResults& codeResults) override;
};

CA_STATIC_GENERATOR(CSharpBindingsGenerator)

struct CSharpCodeGenState
{
    CodeInfoContainer const& codeInfoContainer;
    std::string libraryName;
};

std::string GetFieldTypeForCSharp(CursorType cursorType)
{
    bool isOpaqueType = CInterfaceCodeGenCommon::IsOpaqueType(cursorType);
    return isOpaqueType ? "IntPtr" : cursorType.GetCanonicalType().GetDisplayName();
}

std::string GetArgumentTypeForCSharp(CursorType cursorType, bool isOut)
{
    bool hasConst = cursorType.IsConst();
    bool needPointer = cursorType.IsReferenceOrPointer();
    if(needPointer)
    {
        cursorType = cursorType.GetPointeeType();
    }
    cursorType = cursorType.GetCanonicalType();
    bool isOpaqueType = CInterfaceCodeGenCommon::IsOpaqueType(cursorType);
    if(isOpaqueType)
    {
        needPointer = false;
    }

    Mustache::mustache argumentTemplate = Utils::CreateMustacheNoEscape("{{pointer}}{{typename}}");
    Mustache::data data;
    data["typename"] = isOpaqueType ? "IntPtr" : cursorType.GetCanonicalType().GetDisplayName();
    data["pointer"] = needPointer ? (hasConst ? "in " : (isOut ? "out " : "ref ")) : "";
    return argumentTemplate.render(data);
}

std::string GetReturnTypeForCSharp(CursorType cursorType)
{
    bool isVoid = cursorType.IsVoid();
    if(isVoid)
    {
        return "void";
    }

    bool hasConst = cursorType.IsConst();
    bool needPointer = cursorType.IsReferenceOrPointer();
    if(needPointer)
    {
        cursorType = cursorType.GetPointeeType();
    }
    cursorType = cursorType.GetCanonicalType();
    bool isOpaqueType = CInterfaceCodeGenCommon::IsOpaqueType(cursorType);
    needPointer = needPointer || isOpaqueType;

    Mustache::mustache returnTypeTemplate = Utils::CreateMustacheNoEscape("{{typename}}");
    Mustache::data data;
    data["typename"] = needPointer ? "IntPtr" : cursorType.GetDisplayName();
    return returnTypeTemplate.render(data);
}


void GenerateMethod(CSharpCodeGenState const& codeGenState, MethodInfo const& methodInfo
, Mustache::data& outMethods)
{
    auto methodTemplte = Utils::CreateMustacheNoEscape(R"(
    [DllImport("{{library_name}}")]
    extern static {{return_type}} {{method_name}}({{arguments}});)");


    std::vector<std::string> arguments;
    arguments.push_back("IntPtr handle");

    for(auto& argInfo : methodInfo.GetArguments())
    {
        //TODO: Handle out arguments
        arguments.push_back(GetArgumentTypeForCSharp(argInfo.GetCursor().getType(), false) + " " + argInfo.GetName());
    }

    Mustache::data methodData;
    methodData["library_name"] = codeGenState.libraryName;
    methodData["method_name"] = CInterfaceCodeGenCommon::GetFunctionNameForC(methodInfo);
    methodData["arguments"] = Utils::join(arguments, ", ");
    methodData["return_type"] = GetReturnTypeForCSharp(methodInfo.getCurosr().GetReturnType());

    outMethods.push_back(Mustache::data{"native_method", methodTemplte.render(methodData)});
}

void GenerateMethods(CSharpCodeGenState const& codeGenState
, ClassOrStructInfo const& classInfo
, Mustache::data& outMethods)
{
    auto& methodInfos = classInfo.GetMethods();
    for(auto const& methodInfo : methodInfos)
    {
        GenerateMethod(codeGenState, methodInfo, outMethods);
    }
}

void GenerateConstructors(CSharpCodeGenState const& codeGenState
, ClassOrStructInfo const& classInfo
, Mustache::data& outMethods)
{

    //Default Constructor
    if(classInfo.DefaultConstructable())
    {
        std::string constructorName = CInterfaceCodeGenCommon::GetCustructorNameForC(classInfo, "");
        Mustache::mustache defaultConstructTemplate = Utils::CreateMustacheNoEscape(R"(
    [DllImport("{{library_name}}")]
    extern static IntPtr {{method_name}}();)");
        Mustache::data defaultConstructorData;
        defaultConstructorData["library_name"] = codeGenState.libraryName;
        defaultConstructorData["method_name"] = constructorName;
        outMethods.push_back(Mustache::data{"native_method", defaultConstructTemplate.render(defaultConstructorData)});
    }

    auto& methodInfos = classInfo.GetConstructors();
    for(auto const& methodInfo : methodInfos)
    {
        if(!methodInfo.IsDefaultConstructor())
        {
            auto& constructorArgs = methodInfo.GetArguments();
            std::vector<std::string> argDecls;
            std::vector<std::string> argTypes;
            for(auto& argInfo : constructorArgs)
            {
                argDecls.push_back(GetArgumentTypeForCSharp(argInfo.GetCursor().getType(), false) + " " + argInfo.GetName());
                argTypes.push_back(CInterfaceCodeGenCommon::GetArgumentTypeNameForC(argInfo.GetCursor().getType()));
            }
            std::string constructorName = CInterfaceCodeGenCommon::GetCustructorNameForC(classInfo, "_" + Utils::join(argTypes, "_"));
            Mustache::mustache constructorDeclTemplate = Utils::CreateMustacheNoEscape(R"(
    [DllImport("{{library_name}}")]
    extern static IntPtr {{method_name}}({{argDecls}});)");

            Mustache::data constructorData;
            constructorData["library_name"] = codeGenState.libraryName;
            constructorData["method_name"] = constructorName;
            constructorData["class_name"] = classInfo.getCurosr().getType().GetCanonicalType().GetDisplayName();
            constructorData["argDecls"] = Utils::join(argDecls, ", ");

            outMethods.push_back(Mustache::data{"native_method", constructorDeclTemplate.render(constructorData)});
        }
    }
}

void GenerateDestructors(CSharpCodeGenState const& codeGenState
, ClassOrStructInfo const& classInfo
, Mustache::data& outMethods)
{
    std::string destructorName = CInterfaceCodeGenCommon::GetDestructorNameForC(classInfo);
    Mustache::mustache defaultDestructorTemplate = Utils::CreateMustacheNoEscape(R"(
    [DllImport("{{library_name}}")]
    extern static void {{method_name}}(IntPtr releasePtr);)");
    
    Mustache::data defaultDestructorData;
    defaultDestructorData["library_name"] = codeGenState.libraryName;
    defaultDestructorData["method_name"] = destructorName;
    outMethods.push_back(Mustache::data{"native_method", defaultDestructorTemplate.render(defaultDestructorData)});
}

void GenerateFieldGetterAndSetters(CSharpCodeGenState const& codeGenState, ClassOrStructInfo const& classInfo
, Mustache::data& outMethods)
{
    auto& fields = classInfo.GetFields();

    Mustache::mustache simpleSetterDecl
     = Utils::CreateMustacheNoEscape(R"(
    [DllImport("{{library_name}}")]
    extern static void {{setter_method_name}}(IntPtr thisHandle, {{field_type}} value);)");

    Mustache::mustache simpleGetterDecl
     = Utils::CreateMustacheNoEscape(R"(
    [DllImport("{{library_name}}")]
    extern static {{return_type}} {{getter_method_name}}(IntPtr thisHandle);)");

    Mustache::mustache getterSetterDecl
     = Utils::CreateMustacheNoEscape(R"(
    public {{}}
    extern static {{return_type}} {{getter_method_name}}(IntPtr thisHandle);)");

    for(auto& field : fields)
    {
        std::string methodNameBase = classInfo.GetFullName("_") + "_" + field.getCurosr().getDisplayName();
        std::string field_type = GetArgumentTypeForCSharp(field.getCurosr().getType(), false);
        Mustache::data getterSetterData;
        getterSetterData["library_name"] = codeGenState.libraryName;
        getterSetterData["setter_method_name"] = "Set_" + methodNameBase;
        getterSetterData["getter_method_name"] = "Get_" + methodNameBase;
        getterSetterData["field_type"] = field_type;
        getterSetterData["field_name"] = field.getCurosr().getDisplayName();
        getterSetterData["return_type"] = GetReturnTypeForCSharp(field.getCurosr().getType());
        outMethods.push_back(Mustache::data{"native_method", simpleSetterDecl.render(getterSetterData)});
        outMethods.push_back(Mustache::data{"native_method", simpleGetterDecl.render(getterSetterData)});
    }
}

void CSharpBindingsGenerator::GenerateCode(WorkSpaceInfo const& workSpaceInfo, CodeInfoContainer const& codeInfoContainer, GeneratedCodeResults& codeResults)
{
    std::cout << "Generated CSharp Bindings: " << std::endl;

    Mustache::mustache sourceTemplate = Utils::CreateMustacheNoEscape
(R"(
// This file is generated by the code generator. Do not modify this file manually.
{{#usings}}
using {{using}};
{{/usings}}

{{#has_namespace}}
namespace {{namespace}}
{
{{/has_namespace}}

public class {{class_name}}
{
    //Handle to the native object
    internal IntPtr handle;{{#class_constructors}}
{{class_constructor}}
{{/class_constructors}}{{#getter_setters}}
{{getter_setter}}
{{/getter_setters}}{{#class_functions}}
{{class_function}}
{{/class_functions}}{{#native_methods}}
{{native_method}}
{{/native_methods}}
}
{{#has_namespace}}
}
{{/has_namespace}}
)");


    std::string projectName = workSpaceInfo.GetProjectName();
    projectName = Utils::replace(projectName, " ", "_");
    std::string libraryName = projectName;

    CSharpCodeGenState state{codeInfoContainer, libraryName};

    Mustache::data usings = Mustache::data::type::list;
    //Common usings
    usings.push_back(Mustache::data{"using", "System"});
    usings.push_back(Mustache::data{"using", "System.Runtime.InteropServices"});

    fs::path csDir = workSpaceInfo.GetCodeGenOutputPath() / "CSharp";
    fs::create_directories(csDir);
    std::vector<std::string> sourceFiles;
    for(auto const& pclassInfo : codeInfoContainer.GetAllClasses())
    {
        auto pCodeGenAttribute = pclassInfo->GetPropertyInfoInParents("CodeGen");
        if(pCodeGenAttribute)
        {
            auto namespaceInfo = pclassInfo->getCurrentNamespace();

            std::string classSourceFileName = pclassInfo->GetFullName("_") + ".cs";

            std::string className = pclassInfo->getCurosr().getDisplayName();
            std::cout << "SOURCE FILE: " << classSourceFileName << std::endl;

            Mustache::data class_constructors = Mustache::data::type::list;
            Mustache::data getter_setters = Mustache::data::type::list;
            Mustache::data class_functions = Mustache::data::type::list;
            Mustache::data native_methods = Mustache::data::type::list;

            //Native Methods
            GenerateConstructors(state, *pclassInfo, native_methods);
            GenerateDestructors(state, *pclassInfo, native_methods);
            GenerateFieldGetterAndSetters(state, *pclassInfo, native_methods);
            GenerateMethods(state, *pclassInfo, native_methods);

            Mustache::data sourceFileData;
            sourceFileData["class_name"] = className;
            sourceFileData.set("usings", usings);
            sourceFileData.set("class_constructors", class_constructors);
            sourceFileData.set("getter_setters", getter_setters);
            sourceFileData.set("class_functions", class_functions);
            sourceFileData.set("native_methods", native_methods);
            sourceFileData.set("has_namespace", namespaceInfo != nullptr);
            if(namespaceInfo)
            {
                std::cout << "CSNamespace: " << namespaceInfo->GetFullNameSpace(".") << std::endl;
                sourceFileData.set("namespace", namespaceInfo->GetFullNameSpace("."));
            }
            
            fs::path sourceFilePass = csDir / classSourceFileName;
            std::fstream codegenfile;
            codegenfile.open(sourceFilePass, std::ios::out);
            sourceFiles.push_back(sourceFilePass.string());
            if (!codegenfile.is_open())
            {
                std::cout << "Could not create file: " << sourceFilePass << std::endl;
            }
            else
            {
                codegenfile << sourceTemplate.render(sourceFileData);
                codegenfile.close();
            }


        }
    }

    if(sourceFiles.size() > 0)
    {
        Mustache::mustache csProjTemplate = Utils::CreateMustacheNoEscape(R"(
<Project Sdk="Microsoft.NET.Sdk">
    <PropertyGroup>
        <TargetFramework>{{dotnet_version}}</TargetFramework>
        <ImplicitUsings>enable</ImplicitUsings>
        <Nullable>enable</Nullable>
    </PropertyGroup>
</Project>)");

        Mustache::mustache directoryBuildPropsTemplate = Utils::CreateMustacheNoEscape(R"(
<Project>
   <PropertyGroup>
      <OutDir>{{buildPath}}</OutDir>
   </PropertyGroup>
</Project>)");

        Mustache::data csProjData;
        csProjData["buildPath"] = "$(MSBuildThisFileDirectory)/Binaries";
        csProjData["dotnet_version"] = "net8.0";

        fs::path csProjDir = csDir / (libraryName + "_CS.csproj");
        {
            std::fstream csProjFile;
            csProjFile.open(csProjDir, std::ios::out);

            csProjFile << csProjTemplate.render(csProjData);
            csProjFile.close();
        }
        {
            fs::path directoryBuildpropsDir = csDir / "Directory.Build.props";
            std::fstream directoryBuildPropsFile;
            directoryBuildPropsFile.open(directoryBuildpropsDir, std::ios::out);

            directoryBuildPropsFile << directoryBuildPropsTemplate.render(csProjData);
            directoryBuildPropsFile.close();
        }

        std::string command = "dotnet build " + csProjDir.string();
        std::cout << command << std::endl;
        std::system(command.c_str());

        auto dllFilePath = csDir / "Binaries" /(libraryName + "_CS.dll");
        auto dllDstFilePath = workSpaceInfo.GetOutputPath() / (libraryName + "_CS.dll");
        fs::copy_file(dllFilePath, dllDstFilePath);
    }

}
