#pragma once


class CodeInfoContainer;
class ICodeGenerator
{
public:
    void GenerateCode(CodeInfoContainer const& codeInfoContainer);
};