#pragma once
#include "pch.h"

class Cursor;
class CursorType
{
public:
    CursorType(const CXType& handle);

    std::string GetDisplayName(void) const;

    int GetArgumentCount(void) const;

    CursorType GetArgument(unsigned index) const;

    CursorType GetCanonicalType(void) const;

    Cursor GetDeclaration(void) const;

    CXTypeKind GetKind(void) const;

    bool IsConst(void) const;

private:
    CXType m_handle;
};

class Cursor
{
public:
    
    typedef CXCursorVisitor Visitor;

    Cursor(const CXCursor& handle);

    CXCursor get() const;

    CXCursorKind getKind() const;

    std::string getSpelling() const;
    std::string getDisplayName() const;

    std::string getSourceFile() const;

    bool isDefinition() const;

    CursorType getType() const;

    std::vector<Cursor> getChildren() const;
    void visitChildren(Visitor visitor, void* data = nullptr);

private:
    CXCursor m_handle;
};