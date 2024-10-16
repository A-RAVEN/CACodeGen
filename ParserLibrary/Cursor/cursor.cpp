#include <pch.h>
#include <string_utils.h>
#include "cursor.h"

CursorType::CursorType(const CXType& handle) : m_handle(handle) {}

std::string CursorType::GetDisplayName(void) const
{
    std::string display_name;

    Utils::toString(clang_getTypeSpelling(m_handle), display_name);

    return display_name;
}

int CursorType::GetArgumentCount(void) const { return clang_getNumArgTypes(m_handle); }

CursorType CursorType::GetArgument(unsigned index) const { return clang_getArgType(m_handle, index); }

CursorType CursorType::GetCanonicalType(void) const { return clang_getCanonicalType(m_handle); }

CursorType CursorType::GetPointeeType(void) const { return clang_getPointeeType(m_handle); }

Cursor CursorType::GetDeclaration(void) const { return clang_getTypeDeclaration(m_handle); }

CXTypeKind CursorType::GetKind(void) const { return m_handle.kind; }

std::string CursorType::GetKindSpelling() const
{
    std::string kind_spelling;

    Utils::toString(clang_getTypeKindSpelling(m_handle.kind), kind_spelling);

    return kind_spelling;
}


bool CursorType::IsConst(void) const { return clang_isConstQualifiedType(m_handle) ? true : false; }

bool CursorType::IsOneLayerPointer() const
{
    return IsReferenceOrPointer() && !GetPointeeType().IsReferenceOrPointer();
}

bool CursorType::IsBuiltInType() const
{
    return m_handle.kind >= CXType_FirstBuiltin && m_handle.kind <= CXType_LastBuiltin;
}

bool CursorType::IsReference() const
{
    return m_handle.kind == CXType_LValueReference || m_handle.kind == CXType_RValueReference;
}
bool CursorType::IsRValueReference() const
{
    return m_handle.kind == CXType_RValueReference;
}
bool CursorType::IsLValueReference() const
{
    return m_handle.kind == CXType_LValueReference;
}
bool CursorType::IsPointer() const
{
    return m_handle.kind == CXType_Pointer;
}

bool CursorType::IsReferenceOrPointer() const
{
    return IsReference() || IsPointer();
}

Cursor::Cursor(const CXCursor& handle) : m_handle(handle) {}

CXCursor Cursor::get() const
{
    return m_handle;
}

CXCursorKind Cursor::getKind(void) const { return m_handle.kind; }

std::string Cursor::getSpelling(void) const
{
    std::string spelling;

    Utils::toString(clang_getCursorSpelling(m_handle), spelling);

    return spelling;
}

std::string Cursor::getDisplayName(void) const
{
    std::string display_name;

    Utils::toString(clang_getCursorDisplayName(m_handle), display_name);

    return display_name;
}

std::string Cursor::getSourceFile(void) const
{
    auto range = clang_Cursor_getSpellingNameRange(m_handle, 0, 0);

    auto start = clang_getRangeStart(range);

    CXFile   file;
    unsigned line, column, offset;

    clang_getFileLocation(start, &file, &line, &column, &offset);

    std::string filename;

    Utils::toString(clang_getFileName(file), filename);

    return filename;
}

bool Cursor::isDefinition(void) const { return clang_isCursorDefinition(m_handle); }

CursorType Cursor::getType(void) const { return clang_getCursorType(m_handle); }

CursorType Cursor::GetReturnType(void) const
{
    return clang_getCursorResultType( m_handle );
}

std::vector<Cursor> Cursor::getChildren(void) const
{
    std::vector<Cursor> children;

    auto visitor = [](CXCursor cursor, CXCursor parent, CXClientData data) {
        auto container = static_cast<std::vector<Cursor>*>(data);

        container->emplace_back(cursor);

        if (cursor.kind == CXCursor_LastPreprocessing)
            return CXChildVisit_Break;

        return CXChildVisit_Continue;
    };

    clang_visitChildren(m_handle, visitor, &children);

    return children;
}

void Cursor::visitChildren(Visitor visitor, void* data) { clang_visitChildren(m_handle, visitor, data); }
