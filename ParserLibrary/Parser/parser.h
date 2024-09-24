#pragma once
#include <pch.h>
#include "parser_options.h"

//Handle One 
class CodeParser
{
public:
    CodeParser(LanguageInfo const& languageInfo, WorkSpaceInfo const& workSpaceInfo);
    void Parse();
private:
    LanguageInfo m_LanguageInfo;
    WorkSpaceInfo m_WorkSpaceInfo;

    CXIndex           m_Index;
    CXTranslationUnit m_TranslationUnit;
};