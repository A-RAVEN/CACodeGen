#include "Generator.h"
#include <LanguageTypes/CodeInfoContainer.h>
#include <string_utils.h>
#include <set>

static Mustache::mustache CreateMustacheNoEscape(const char* templateStr)
{
    Mustache::mustache mustache(templateStr);
    mustache.set_custom_escape([](const std::string& s) {return s;});
    return mustache;
}

bool IsOpaqueType(CursorType cursorType)
{
    return (cursorType.GetCanonicalType().GetKind() == CXType_Elaborated || 
        cursorType.GetCanonicalType().GetKind() == CXType_Record)
     && !cursorType.IsBuiltInType();
}


bool CCompatibleArgument(CursorType cursorType)
{
    if(cursorType.IsOneLayerPointer()||!cursorType.IsReferenceOrPointer())
    {
        if(cursorType.IsOneLayerPointer())
        {
            cursorType = cursorType.GetPointeeType();
        }
        bool classOrStruct = cursorType.GetKind() == CXType_Elaborated;
        return classOrStruct || cursorType.IsBuiltInType();
    }
    return false;
}

std::string GetArgumentTypeNameForC(CursorType cursorType)
{
    bool hasConst = cursorType.IsConst();
    bool needPointer = cursorType.IsReferenceOrPointer();
    if(needPointer)
    {
        cursorType = cursorType.GetPointeeType();
    }
    cursorType = cursorType.GetCanonicalType();
    bool isOpaqueType = IsOpaqueType(cursorType);

    return (hasConst ? "Const" : "") +  (isOpaqueType ? "VoidPtr" : Utils::CapitalToUpper(cursorType.GetCanonicalType().GetDisplayName()));
}

std::string GetArgumentTypeForC(CursorType cursorType)
{
    bool hasConst = cursorType.IsConst();
    bool needPointer = cursorType.IsReferenceOrPointer();
    if(needPointer)
    {
        cursorType = cursorType.GetPointeeType();
    }
    cursorType = cursorType.GetCanonicalType();
    bool isOpaqueType = IsOpaqueType(cursorType);
    needPointer = needPointer || isOpaqueType;

    Mustache::mustache argumentTemplate = CreateMustacheNoEscape("{{const}}{{typename}}{{pointer}}");
    Mustache::data data;
    data["const"] = hasConst ? "const " : "";
    data["typename"] = isOpaqueType ? "void" : cursorType.GetCanonicalType().GetDisplayName();
    data["pointer"] = needPointer ? "*" : "";
    return argumentTemplate.render(data);
}

std::string GetReturnTypeForC(CursorType cursorType)
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
    bool isOpaqueType = IsOpaqueType(cursorType);
    needPointer = needPointer || isOpaqueType;

    Mustache::mustache returnTypeTemplate = CreateMustacheNoEscape("{{const}}{{typename}}{{pointer}}");
    Mustache::data data;
    data["const"] = hasConst ? "const " : "";
    data["typename"] = isOpaqueType ? "void" : cursorType.GetDisplayName();
    data["pointer"] = needPointer ? "*" : "";
    return returnTypeTemplate.render(data);
}

std::string GetArgumentC2CPPConversion(Cursor argCursor)
{
    CursorType cursorType = argCursor.getType();
    bool hasConst = cursorType.IsConst();
    bool needPointer = cursorType.IsReferenceOrPointer() || IsOpaqueType(cursorType);
    bool needDereference = needPointer && !cursorType.IsPointer();
    if(cursorType.IsReferenceOrPointer())
    {
        cursorType = cursorType.GetPointeeType();
    }
    cursorType = cursorType.GetCanonicalType();
    Mustache::mustache conversionTmp = CreateMustacheNoEscape("{{dereference}}{{#need_convert}}({{const}}{{typename}}{{pointer}}){{/need_convert}}{{argname}}");
    Mustache::data data;
    data.set("need_convert", IsOpaqueType(cursorType));
    data["const"] = hasConst ? "const " : "";
    data["typename"] = cursorType.GetDisplayName();
    data["pointer"] = needPointer ? "*" : "";
    data["dereference"] = needDereference ? "*" : "";
    data["argname"] = argCursor.getDisplayName();
    return conversionTmp.render(data);
}

std::string GetFunctionNameForC(MethodInfo const& methodInfo)
{
    std::string functionName = methodInfo.GetSpelling();
    auto owningClass = methodInfo.GetOwnerClass();
    if(owningClass != nullptr)
    {
        functionName = owningClass->getCurosr().getDisplayName() + "_" + functionName;
    }
    auto ns = methodInfo.getCurrentNamespace();
    if(ns != nullptr)
    {
        functionName = ns->GetFullNameSpace("_") + "_" + functionName;
    }
    return functionName;
}

std::string GetCustructorNameForC(ClassOrStructInfo const& classInfo, std::string const& postfix)
{
    return "New_" + classInfo.getCurosr().getDisplayName() + postfix;
}

std::string CallCopyConstructor(ClassOrStructInfo const& classInfo, std::string const& srcPtr)
{
    std::string constructorName = GetCustructorNameForC(classInfo, "_Copy");
    return constructorName + "(" + srcPtr + ")";
}

std::string GetDestructorNameForC(ClassOrStructInfo const& classInfo)
{
    return "Release_" + classInfo.getCurosr().getDisplayName();
}

std::string GetCursorConversion(Cursor const& cursor, bool isConst, bool dereference = false)
{
    Mustache::mustache conversionTmp = CreateMustacheNoEscape("{{dereference}}({{const}}{{typename}}*)");
    std::string classTypeName = cursor.getType().GetCanonicalType().GetDisplayName();
    Mustache::data data;
    data["const"] = isConst ? "const " : "";
    data["typename"] = classTypeName;
    data["dereference"] = dereference ? "*" : "";
    return conversionTmp.render(data);
}

std::string GetCallerTypeC2CPPConversion(MethodInfo const& methodInfo)
{
    bool isConst = methodInfo.IsConst();
    auto owningClass = methodInfo.GetOwnerClass();
    std::string classTypeName = owningClass->getCurosr().getType().GetCanonicalType().GetDisplayName();
    Mustache::mustache conversionTmp = CreateMustacheNoEscape("({{const}}{{typename}}*)");
    Mustache::data data;
    data["const"] = isConst ? "const " : "";
    data["typename"] = classTypeName;
    return conversionTmp.render(data);
}

bool ReturnTypeNeedCopyAllocation(CursorType cursorType)
{
    //返回了一个不透明类型的拷贝，需要分配内存
    bool isOpaqueType = IsOpaqueType(cursorType);
    bool needPointer = cursorType.IsReferenceOrPointer();
    return isOpaqueType && !needPointer;
}

std::string GetFunctionBody(CodeInfoContainer const& codeInfoContainer, MethodInfo const& methodInfo)
{
    std::vector<std::string> args;
    for(auto& argInfo : methodInfo.GetArguments())
    {
        args.push_back(GetArgumentC2CPPConversion(argInfo.GetCursor()));
    }
    std::cout << "TEST: >" << std::endl;
    Mustache::mustache bodyTemplate = CreateMustacheNoEscape(R"(
    {{#should_return}}auto returnVal = {{/should_return}}({{caller_conversion}}thisPtr)->{{function_name}}({{arguments}});
    {{#should_return}}return {{#direct_return}}returnVal{{/direct_return}}{{#allocate_return}}{{copy_construct_return}}{{/allocate_return}};{{/should_return}}
    )");

    bool needCopyAllocation = ReturnTypeNeedCopyAllocation(methodInfo.GetReturnType());

    bodyTemplate.set_custom_escape([](const std::string& s) {return s;});

    Mustache::data data;
    data["caller_conversion"] = GetCallerTypeC2CPPConversion(methodInfo);
    data["function_name"] = methodInfo.GetSpelling();
    data["arguments"] = Utils::join(args, ", ");
    data.set("should_return", methodInfo.HasReturnType());
    data.set("direct_return", !needCopyAllocation);
    data.set("allocate_return", needCopyAllocation);

    if(needCopyAllocation)
    {
        auto returnValDecl = methodInfo.GetReturnType().GetDeclaration();
        auto returnValClassInfo = codeInfoContainer.FindClassOrStruct(returnValDecl);
        bool resultCopyConstructable = returnValClassInfo->CopyConstructable();
        assert(resultCopyConstructable);
        data["copy_construct_return"] = CallCopyConstructor(*returnValClassInfo.get(), "&returnVal");
    }

    return bodyTemplate.render(data);
}

void GenerateHandleDecl(CodeInfoContainer const& codeInfoContainer, ClassOrStructInfo const& classInfo
, Mustache::data& outHandleDeclarations)
{
    auto classFullName = classInfo.GetFullName("_");
    Mustache::mustache defaultDestructorImpl = CreateMustacheNoEscape(R"(
        struct {{handle_name}}
        {
            void* objPtr;
        };
    )");

    Mustache::data handleData;
    handleData["handle_name"] = "H_" + classFullName;
    outHandleDeclarations.push_back(Mustache::data{"handle_declaration", defaultDestructorImpl.render(handleData)});
}

void ICodeGenerator::GenerateMethods(CodeInfoContainer const& codeInfoContainer, MethodInfo const& methodInfo
, Mustache::data& outMethodDeclarations
, Mustache::data& outMethodImplementations)
{
    for(auto& argInfo : methodInfo.GetArguments())
    {
        if(!CCompatibleArgument(argInfo.GetCursor().getType()))
        {
            return;
        }
    }

    Mustache::mustache argumentsDeclTemplate = CreateMustacheNoEscape("{{thisPtrArgDecl}}{{#argDecls}}, {{argDecl}}{{/argDecls}}");
    Mustache::data argumentsDeclData;
    argumentsDeclData["thisPtrArgDecl"] = methodInfo.IsConst() ? "void const* thisPtr" : "void* thisPtr";
    Mustache::data argDecls = Mustache::data::type::list;

    for(auto& argInfo : methodInfo.GetArguments())
    {
        argDecls.push_back(Mustache::data{"argDecl", GetArgumentTypeForC(argInfo.GetCursor().getType()) + " " + argInfo.GetName()});
    }
    argumentsDeclData.set("argDecls", argDecls);

    Mustache::data methodData;
    methodData["method_name"] = GetFunctionNameForC(methodInfo);
    methodData["arguments"] = argumentsDeclTemplate.render(argumentsDeclData);
    methodData["return_value"] = GetReturnTypeForC(methodInfo.getCurosr().GetReturnType());
    methodData["method_body"] = GetFunctionBody(codeInfoContainer, methodInfo);

    outMethodDeclarations.push_back(Mustache::data{"method_declaration", methodDeclaration.render(methodData)});
    outMethodImplementations.push_back(Mustache::data{"method_implementation", methodTemplte.render(methodData)});

    std::cout << "\t declaration: " << methodDeclaration.render(methodData) << std::endl;
    std::cout << "\t method: " << methodTemplte.render(methodData) << std::endl;
}

void GenerateDestructor(ClassOrStructInfo const& classInfo
, Mustache::data& outMethodDeclarations
, Mustache::data& outMethodImplementations)
{
    std::string destructorName = GetDestructorNameForC(classInfo);
    std::string conversion = GetCursorConversion(classInfo.getCurosr(), false);
    Mustache::data defaultDestructorData;
    defaultDestructorData["method_name"] = destructorName;
    defaultDestructorData["conversion"] = conversion;
    Mustache::mustache defaultDestructorDecl = CreateMustacheNoEscape("CAINTERFACE void {{method_name}}(void* objPtr);");
    Mustache::mustache defaultDestructorImpl = CreateMustacheNoEscape(R"(
        CAINTERFACE void {{method_name}}(void* objPtr)
        {
            delete {{conversion}}objPtr;
        }
    )");
    outMethodDeclarations.push_back(Mustache::data{"method_declaration", defaultDestructorDecl.render(defaultDestructorData)});
    outMethodImplementations.push_back(Mustache::data{"method_implementation", defaultDestructorImpl.render(defaultDestructorData)});
}



void GenerateCopyConstructor(ClassOrStructInfo const& classInfo
, Mustache::data& outMethodDeclarations
, Mustache::data& outMethodImplementations)
{
    if(classInfo.CopyConstructable())
    {
        std::string constructorName = GetCustructorNameForC(classInfo, "_Copy");
        std::string conversion = GetCursorConversion(classInfo.getCurosr(), true, true);
        Mustache::data copyCtorData;
        copyCtorData["method_name"] = constructorName;
        copyCtorData["conversion"] = conversion;
        copyCtorData["class_name"] = classInfo.getCurosr().getType().GetCanonicalType().GetDisplayName();
        Mustache::mustache copyConstructorDeclTemplate = CreateMustacheNoEscape("CAINTERFACE void* {{method_name}}(const void* srcPtr);");
        Mustache::mustache copyConstructorImpl = CreateMustacheNoEscape(R"(
            CAINTERFACE void* {{method_name}}(const void* srcPtr)
            {
                return new {{class_name}}({{conversion}}srcPtr);
            }
        )");
        outMethodDeclarations.push_back(Mustache::data{"method_declaration", copyConstructorDeclTemplate.render(copyCtorData)});
        outMethodImplementations.push_back(Mustache::data{"method_implementation", copyConstructorImpl.render(copyCtorData)});
    }
}

void GenerateConstructors(ClassOrStructInfo const& classInfo
, Mustache::data& outMethodDeclarations
, Mustache::data& outMethodImplementations)
{
    //Default Constructor
    if(classInfo.DefaultConstructable())
    {
        std::string constructorName = GetCustructorNameForC(classInfo, "");
        Mustache::mustache defaultConstructorDeclTemplate = CreateMustacheNoEscape("CAINTERFACE void* {{method_name}}();");
        Mustache::mustache defaultConstructorImpl = CreateMustacheNoEscape(R"(
            CAINTERFACE void* {{method_name}}()
            {
                return new {{class_name}}();
            }
        )");
        Mustache::data defaultConstructorData;
        defaultConstructorData["method_name"] = constructorName;
        defaultConstructorData["class_name"] = classInfo.getCurosr().getType().GetCanonicalType().GetDisplayName();

        outMethodDeclarations.push_back(Mustache::data{"method_declaration", defaultConstructorDeclTemplate.render(defaultConstructorData)});
        outMethodImplementations.push_back(Mustache::data{"method_implementation", defaultConstructorImpl.render(defaultConstructorData)});
    }

    auto& methodInfos = classInfo.GetConstructors();
    for(auto const& methodInfo : methodInfos)
    {
        bool canGenerate = true;
        for(auto& argInfo : methodInfo.GetArguments())
        {
            if(!CCompatibleArgument(argInfo.GetCursor().getType()))
            {
                canGenerate = false;
                break;;
            }
        }
        if(canGenerate && !methodInfo.IsDefaultConstructor())
        {
            auto& constructorArgs = methodInfo.GetArguments();
            std::vector<std::string> argDecls;
            std::vector<std::string> argConversions;
            std::vector<std::string> argTypes;
            for(auto& argInfo : constructorArgs)
            {
                argDecls.push_back(GetArgumentTypeForC(argInfo.GetCursor().getType()) + " " + argInfo.GetName());
                argConversions.push_back(GetArgumentC2CPPConversion(argInfo.GetCursor()));
                argTypes.push_back(GetArgumentTypeNameForC(argInfo.GetCursor().getType()));
            }

            std::string constructorName = GetCustructorNameForC(classInfo, "_" + Utils::join(argTypes, "_"));

            Mustache::mustache constructorDeclTemplate = CreateMustacheNoEscape("CAINTERFACE void* {{method_name}}({{argDecls}});");
            Mustache::mustache constructorTemplate = CreateMustacheNoEscape(R"(
            CAINTERFACE void* {{method_name}}({{argDecls}})
            {
                return new {{class_name}}({{args}});
            }
            )");

            Mustache::data constructorData;
            constructorData["method_name"] = constructorName;
            constructorData["class_name"] = classInfo.getCurosr().getType().GetCanonicalType().GetDisplayName();
            constructorData["argDecls"] = Utils::join(argDecls, ", ");
            constructorData["args"] = Utils::join(argConversions, ", ");

            outMethodDeclarations.push_back(Mustache::data{"method_declaration", constructorDeclTemplate.render(constructorData)});
            outMethodImplementations.push_back(Mustache::data{"method_implementation", constructorTemplate.render(constructorData)});
        }
    }


}

void ICodeGenerator::GenerateCode(WorkSpaceInfo const& workSpaceInfo, CodeInfoContainer const& codeInfoContainer)
{

    methodDeclaration = CreateMustacheNoEscape("CAINTERFACE {{return_value}} {{method_name}}({{arguments}});");
    methodTemplte = CreateMustacheNoEscape(R"(
    CAINTERFACE {{return_value}} {{method_name}}({{arguments}})
    {
        {{method_body}} 
    }
    )");

    std::cout << "Generated C Functions: " << std::endl;

    Mustache::mustache headerTemplate = CreateMustacheNoEscape
(R"(
#ifndef {{header_guard}}
#define {{header_guard}}
// This file is generated by the code generator. Do not modify this file manually.
{{#header_exetern_includes}}
#include <{{headfile_name_extern}}>
{{/header_exetern_includes}}
{{#header_includes}}
#include "{{headfile_name}}"
{{/header_includes}}
#include <CAMeta/CAMetaCodeGenCommon.h>

#ifdef __cplusplus
extern "C"
{
#endif
{{#method_declarations}}
    {{method_declaration}}
{{/method_declarations}}
#ifdef __cplusplus
}
#endif
#endif // {{header_guard}}
)");

    Mustache::mustache bodyTemplate = CreateMustacheNoEscape
(R"(
// This file is generated by the code generator. Do not modify this file manually.
{{#source_exetern_includes}}
#include <{{headfile_name_extern}}>
{{/source_exetern_includes}}
{{#source_includes}}
#include "{{headfile_name}}"
{{/source_includes}}

#ifdef __cplusplus
extern "C"
{
#endif
{{#method_implementations}}
    {{method_implementation}}
{{/method_implementations}}
#ifdef __cplusplus
}
#endif
)");

    Mustache::mustache headerGuardTemplate = CreateMustacheNoEscape("C_{{project_name_upper}}_CODE_GEN_H");


    std::string projectName = workSpaceInfo.GetProjectName();
    projectName = Utils::replace(projectName, " ", "_");
    std::string headerFileName = "C_" + projectName + "_CodeGen.h";
    std::string sourceFileName = "C_" + projectName + "_CodeGen.cpp";

    Mustache::data includeFileData;
    includeFileData["header_guard"] = headerGuardTemplate.render(
        Mustache::data{"project_name_upper", Utils::toUpper(projectName)}
    );
    Mustache::data header_exetern_includes = Mustache::data::type::list;
    Mustache::data header_includes = Mustache::data::type::list;
    Mustache::data method_declarations = Mustache::data::type::list;

    Mustache::data sourceFileData;
    Mustache::data source_exetern_includes = Mustache::data::type::list;
    Mustache::data source_includes = Mustache::data::type::list;
    source_includes.push_back(Mustache::data{"headfile_name", headerFileName});
    Mustache::data method_implementations = Mustache::data::type::list;

    std::set<std::string> srcIncludeFiles;

    for(auto const& pclassInfo : codeInfoContainer.GetAllClasses())
    {
        auto pCodeGenAttribute = pclassInfo->GetPropertyInfoInParents("CodeGen");
        if(pCodeGenAttribute)
        {
            std::cout << "SOURCE FILE: " << workSpaceInfo.FullPathToWorkspace(pclassInfo->getSourceFile()) << std::endl;
            srcIncludeFiles.insert(workSpaceInfo.FullPathToWorkspace(pclassInfo->getSourceFile()).string());
            GenerateConstructors(*pclassInfo, method_declarations, method_implementations);
            GenerateCopyConstructor(*pclassInfo, method_declarations, method_implementations);
            GenerateDestructor(*pclassInfo, method_declarations, method_implementations);

            auto& methodInfos = pclassInfo->GetMethods();
            for(auto const& methodInfo : methodInfos)
            {
                GenerateMethods(codeInfoContainer, methodInfo, method_declarations, method_implementations);
            }
        }
    }

    includeFileData.set("header_exetern_includes", header_exetern_includes);
    includeFileData.set("header_includes", header_includes);
    includeFileData.set("method_declarations", method_declarations);

    for(auto& srcIncludeFile : srcIncludeFiles)
    {
        source_exetern_includes.push_back(Mustache::data{"headfile_name_extern", srcIncludeFile});
    }
    sourceFileData.set("source_exetern_includes", source_exetern_includes);
    sourceFileData.set("source_includes", source_includes);
    sourceFileData.set("method_implementations", method_implementations);

    std::cout << "header: \n" << headerTemplate.render(includeFileData) << std::endl;

    std::cout << "source: \n" << bodyTemplate.render(sourceFileData) << std::endl;

    fs::path headerFilePath = workSpaceInfo.GetCodeGenOutputPath() / headerFileName;
    std::fstream codegenfile;
    codegenfile.open(headerFilePath, std::ios::out);
    if (!codegenfile.is_open())
    {
        std::cout << "Could not create file: " << headerFilePath << std::endl;
    }
    else
    {
        codegenfile << headerTemplate.render(includeFileData);
        codegenfile.close();
    }

    fs::path sourceFilePath = workSpaceInfo.GetCodeGenOutputPath() / sourceFileName;
    codegenfile.open(sourceFilePath, std::ios::out);
    if (!codegenfile.is_open())
    {
        std::cout << "Could not create file: " << sourceFilePath << std::endl;
    }
    else
    {
        codegenfile << bodyTemplate.render(sourceFileData);
        codegenfile.close();
    }
}