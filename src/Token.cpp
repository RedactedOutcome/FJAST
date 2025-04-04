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
    
    Token::Token(const Token& token)noexcept
        : m_Type(token.m_Type), m_Value(token.m_Value), m_Metadata(token.m_Metadata), m_LineNumber(token.m_LineNumber), m_ColumnNumber(token.m_ColumnNumber){}

    Token::Token(Token&& token)noexcept
    : m_Type(token.m_Type), m_Value(std::move(token.m_Value)), m_Metadata(token.m_Metadata), m_LineNumber(token.m_LineNumber), m_ColumnNumber(token.m_ColumnNumber){}
    
    Token& Token::operator=(const Token& token)noexcept{
        m_Type = token.m_Type;
        m_Metadata = token.m_Metadata;
        m_Type = token.m_Type;
        m_Value = token.m_Value;
        m_LineNumber = token.m_LineNumber;
        m_ColumnNumber = token.m_ColumnNumber;
        return *this;
    }
    Token& Token::operator=(Token&& token)noexcept{
        m_Type = token.m_Type;
        m_Metadata = token.m_Metadata;
        m_Type = token.m_Type;
        m_Value = std::move(token.m_Value);
        m_LineNumber = token.m_LineNumber;
        m_ColumnNumber = token.m_ColumnNumber;
        return *this;
    }
}