#include "Generator.h"
#include <LanguageTypes/CodeInfoContainer.h>
#include <mustache.hpp>

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
    std::string argumentType = "";
    if(cursorType.IsConst())
    {
        argumentType += "const ";
    }
    bool needPointer = cursorType.IsReferenceOrPointer();
    if(needPointer)
    {
        cursorType = cursorType.GetPointeeType();
    }
    if(cursorType.IsBuiltInType())
    {
        argumentType += cursorType.GetCanonicalType().GetDisplayName();
    }
    else
    {
        needPointer = true;
        argumentType += "void";
    }
    if(needPointer)
    {
        argumentType += "*";
    }
    return argumentType;
}

std::string GetArgumentC2CPPConversion(Cursor argCursor)
{
    CursorType cursorType = argCursor.getType();
    bool hasConst = cursorType.IsConst();
    bool needPointer = cursorType.IsReferenceOrPointer() || (cursorType.GetKind() == CXType_Elaborated);
    bool needDereference = needPointer && !cursorType.IsPointer();
    if(cursorType.IsReferenceOrPointer())
    {
        cursorType = cursorType.GetPointeeType();
    }
    Mustache::mustache conversionTmp = "{{dereference}}({{const}}{{typename}}{{pointer}}){{argname}}";
    Mustache::data data;
    data["const"] = hasConst ? "const " : "";
    data["typename"] = cursorType.GetCanonicalType().GetDisplayName();
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
    Mustache::mustache conversionTmp = "({{const}} {{typename}}*)";
    Mustache::data data;
    data["const"] = isConst ? "const" : "";
    data["typename"] = classTypeName;
    return conversionTmp.render(data);
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
    Mustache::mustache bodyTemplate("{{caller_conversion}} thisPtr{{{pointing}}}{{function_name}}({{arguments}});");

    Mustache::data data;
    data["caller_conversion"] = GetCallerTypeC2CPPConversion(methodInfo);
    data["function_name"] = methodInfo.GetSpelling();
    data["arguments"] = args;
    data["pointing"] = "->";
    return bodyTemplate.render(data);
}

void ICodeGenerator::GenerateCode(CodeInfoContainer const& codeInfoContainer)
{
    std::cout << "Generated C Functions: " << std::endl;
    for(auto const& pclassInfo : codeInfoContainer.GetAllClasses())
    {
        auto pCodeGenAttribute = pclassInfo->getMetaData().GetPropertyInfo("CodeGen");
        if(pCodeGenAttribute)
        {
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

                Mustache::data data;
                data["method_name"] = GetFunctionNameForC(methodInfo);
                data["arguments"] = argumentStr;
                data["return_value"] = "void";
                data["method_body"] = GetFunctionBody(methodInfo);

                methodInfo.GetArgumentCount();
                Mustache::mustache methodDeclaration = "{{return_value}} {{method_name}}({{arguments}});";
                Mustache::mustache methodTemplte = "{{return_value}} {{method_name}}({{arguments}}){ {{{method_body}}} }";

                std::cout << "\t declaration: " << methodDeclaration.render(data) << std::endl;
                std::cout << "\t method: " << methodTemplte.render(data) << std::endl;
            }
            std::cout << "Generating code for class: " << pclassInfo->getCurosr().getDisplayName() << std::endl;
        }
    }
}
