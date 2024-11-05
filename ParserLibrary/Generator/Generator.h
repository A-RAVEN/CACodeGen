#pragma once
#include <Parser/parser_options.h>
#include <mustache.hpp>

class CodeInfoContainer;
class MethodInfo;
class GeneratedCodeResults;
class ICodeGenerator
{
public:
    virtual void GenerateCode(WorkSpaceInfo const& workSpaceInfo, CodeInfoContainer const& codeInfoContainer, GeneratedCodeResults& codeResults) = 0;
    virtual ~ICodeGenerator() {}
    virtual std::vector<std::string> RequireGenerators() const { return {}; }
};