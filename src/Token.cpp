#include "pch.h"
#include "Token.h"

namespace FJASTP{
    Token::Token(TokenType type, const HBuffer& value, uint32_t lineNumber, uint32_t columnNumber) noexcept
        : m_Type(type), m_Value(value), m_Metadata(0), m_LineNumber(lineNumber), m_ColumnNumber(columnNumber){}
    Token::Token(TokenType type, const HBuffer& value, uint8_t metadata, uint32_t lineNumber, uint32_t columnNumber) noexcept
        : m_Type(type), m_Value(value), m_Metadata(metadata), m_LineNumber(lineNumber), m_ColumnNumber(columnNumber){}
    Token::Token(TokenType type, HBuffer&& value, uint32_t lineNumber, uint32_t columnNumber) noexcept
        : m_Type(type), m_Value(std::move(value)), m_Metadata(0), m_LineNumber(lineNumber), m_ColumnNumber(columnNumber){}
    Token::Token(TokenType type, HBuffer&& value, uint8_t metadata, uint32_t lineNumber, uint32_t columnNumber) noexcept
        : m_Type(type), m_Value(std::move(value)), m_Metadata(metadata), m_LineNumber(lineNumber), m_ColumnNumber(columnNumber){}
}