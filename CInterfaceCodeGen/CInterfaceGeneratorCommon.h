#pragma once
#include <string>
class MethodInfo;
class CursorType;
class ClassOrStructInfo;
namespace CInterfaceCodeGenCommon
{
    std::string GetFunctionNameForC(MethodInfo const& methodInfo);

    bool IsOpaqueType(CursorType const& cursorType);

    std::string GetCustructorNameForC(ClassOrStructInfo const& classInfo, std::string const& postfix);

    std::string GetDestructorNameForC(ClassOrStructInfo const& classInfo);

    std::string GetArgumentTypeNameForC(CursorType cursorType);
}