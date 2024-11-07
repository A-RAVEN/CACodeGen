#include "CInterfaceGeneratorCommon.h"
#include <LanguageTypes/CodeInfoContainer.h>
#include <Cursor/cursor.h>
#include <string_utils.h>

namespace CInterfaceCodeGenCommon
{
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

    bool IsOpaqueType(CursorType const& cursorType)
    {
        return (cursorType.GetCanonicalType().GetKind() == CXType_Elaborated || 
            cursorType.GetCanonicalType().GetKind() == CXType_Record)
        && !cursorType.IsBuiltInType();
    }

    std::string GetCustructorNameForC(ClassOrStructInfo const& classInfo, std::string const& postfix)
    {
        return "New_" + classInfo.getCurosr().getDisplayName() + postfix;
    }

    std::string GetDestructorNameForC(ClassOrStructInfo const& classInfo)
    {
        return "Release_" + classInfo.getCurosr().getDisplayName();
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
}