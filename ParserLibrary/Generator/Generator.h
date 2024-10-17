#pragma once
#include <Parser/parser_options.h>

class CodeInfoContainer;
class ICodeGenerator
{
public:
    void GenerateCode(WorkSpaceInfo const& workSpaceInfo, CodeInfoContainer const& codeInfoContainer);
};