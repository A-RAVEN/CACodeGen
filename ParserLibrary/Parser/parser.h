#pragma once
#include <pch.h>
#include <Cursor/namespace.h>
#include <Cursor/cursor.h>
#include "parser_options.h"

//Handle One 
class CodeParser
{
public:
    CodeParser(LanguageInfo const& languageInfo, WorkSpaceInfo const& workSpaceInfo);
    void Parse();
    void TraverseCodeSpace(const Cursor& cursor, NameSpace& current_namespace);
private:
    LanguageInfo m_LanguageInfo;
    WorkSpaceInfo m_WorkSpaceInfo;

    CXIndex           m_Index;
    CXTranslationUnit m_TranslationUnit;
};