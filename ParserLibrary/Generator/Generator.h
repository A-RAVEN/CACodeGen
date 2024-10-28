#pragma once
#include <Parser/parser_options.h>
#include <mustache.hpp>

class CodeInfoContainer;
class MethodInfo;
class ICodeGenerator
{
public:
    virtual void GenerateCode(WorkSpaceInfo const& workSpaceInfo, CodeInfoContainer const& codeInfoContainer) = 0;
    virtual ~ICodeGenerator() {}
    virtual std::vector<std::string> RequireGenerators() const { return {}; }
};