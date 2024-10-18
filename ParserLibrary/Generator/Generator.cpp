#include "Generator.h"
#include <LanguageTypes/CodeInfoContainer.h>
#include <mustache.hpp>
#include <string_utils.h>
#include <set>

Mustache::mustache CreateMustacheNoEscape(std::string const& templateStr)
{
    Mustache::mustache mustache(templateStr);
    mustache.set_custom_escape([](const std::string& s) {return s;});
    return mustache;
}

bool IsOpaqueType(CursorType cursorType)
{
    return cursorType.GetCanonicalType().GetKind() == CXType_Elaborated && !cursorType.IsBuiltInType();
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

    Mustache::mustache argumentTemplate = CreateMustacheNoEscape("{{const}}{{typename}}{{pointer}}");
    Mustache::data data;
    data["const"] = hasConst ? "const " : "";
    data["typename"] = isOpaqueType ? "void*" : cursorType.GetCanonicalType().GetDisplayName();
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

    Mustache::mustache returnTypeTemplate = CreateMustacheNoEscape("{{const}}{{typename}}{{pointer}}");
    Mustache::data data;
    data["const"] = hasConst ? "const " : "";
    data["typename"] = isOpaqueType ? "void*" : cursorType.GetDisplayName();
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

std::string GetFunctionBody(MethodInfo const& methodInfo)
{
    std::string args = "";
    for(auto& argInfo : methodInfo.GetArguments())
    {
        bool first = args.empty();
        args += (first ? "" : ", ") + GetArgumentC2CPPConversion(argInfo.GetCursor());
    }
    std::cout << "TEST: >" << std::endl;
    Mustache::mustache bodyTemplate = CreateMustacheNoEscape(R"(
    {{#should_return}}auto returnVal = {{/should_return}}({{caller_conversion}}thisPtr)->{{function_name}}({{arguments}});
    {{#should_return}}return {{#direct_return}}returnVal{{/direct_return}}{{#allocate_return}}returnVal{{/allocate_return}};{{/should_return}}
    )");

    bool needCopyAllocation = ReturnTypeNeedCopyAllocation(methodInfo.GetReturnType());

    bodyTemplate.set_custom_escape([](const std::string& s) {return s;});

    Mustache::data data;
    data["caller_conversion"] = GetCallerTypeC2CPPConversion(methodInfo);
    data["function_name"] = methodInfo.GetSpelling();
    data["arguments"] = args;
    data.set("should_return", methodInfo.HasReturnType());
    data.set("direct_return", !needCopyAllocation);
    data.set("allocate_return", needCopyAllocation);
    return bodyTemplate.render(data);
}

void ICodeGenerator::GenerateCode(WorkSpaceInfo const& workSpaceInfo, CodeInfoContainer const& codeInfoContainer)
{
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
    Mustache::mustache methodDeclaration = CreateMustacheNoEscape("{{return_value}} {{method_name}}({{arguments}});");
    Mustache::mustache methodTemplte = CreateMustacheNoEscape(R"(
    {{return_value}} {{method_name}}({{arguments}})
    {
        {{method_body}} 
    }
    )");

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
        auto pCodeGenAttribute = pclassInfo->getMetaData().GetPropertyInfo("CodeGen");
        if(pCodeGenAttribute)
        {
            std::cout << "SOURCE FILE: " << workSpaceInfo.FullPathToWorkspace(pclassInfo->getSourceFile()) << std::endl;
            srcIncludeFiles.insert(workSpaceInfo.FullPathToWorkspace(pclassInfo->getSourceFile()).string());
            auto& methodInfos = pclassInfo->GetMethods();
            for(auto const& methodInfo : methodInfos)
            {
                bool canGenerate = true;
                for(auto& argInfo : methodInfo.GetArguments())
                {
                    if(!CCompatibleArgument(argInfo.GetCursor().getType()))
                    {
                        canGenerate = false;
                        break;
                    }
                }
                if(!canGenerate)
                {
                    continue;
                }

                std::string argumentStr = "";
                if(methodInfo.IsConst())
                {
                    argumentStr += "void const* thisPtr";
                }
                else
                {
                    argumentStr += "void* thisPtr";
                }

                for(auto& argInfo : methodInfo.GetArguments())
                {
                    argumentStr += ", " + GetArgumentTypeForC(argInfo.GetCursor().getType()) + " " + argInfo.GetName();
                }

                Mustache::data methodData;
                methodData["method_name"] = GetFunctionNameForC(methodInfo);
                methodData["arguments"] = argumentStr;
                methodData["return_value"] = GetReturnTypeForC(methodInfo.getCurosr().GetReturnType());
                methodData["method_body"] = GetFunctionBody(methodInfo);

                methodInfo.GetArgumentCount();

                method_declarations.push_back(Mustache::data{"method_declaration", methodDeclaration.render(methodData)});
                method_implementations.push_back(Mustache::data{"method_implementation", methodTemplte.render(methodData)});

                std::cout << "\t declaration: " << methodDeclaration.render(methodData) << std::endl;
                std::cout << "\t method: " << methodTemplte.render(methodData) << std::endl;
            }
            std::cout << "Generating code for class: " << pclassInfo->getCurosr().getDisplayName() << std::endl;
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