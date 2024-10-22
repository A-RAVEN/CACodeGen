#pragma once
#include <Parser/parser_options.h>
#include <mustache.hpp>

class CodeInfoContainer;
class MethodInfo;
class ICodeGenerator
{
public:
    void GenerateCode(WorkSpaceInfo const& workSpaceInfo, CodeInfoContainer const& codeInfoContainer);
    void GenerateMethods(CodeInfoContainer const& codeInfoContainer,MethodInfo const& methodInfo
    , Mustache::data& outMethodDeclarations
    , Mustache::data& outMethodImplementations);

    Mustache::mustache methodDeclaration;
    Mustache::mustache methodTemplte;
};