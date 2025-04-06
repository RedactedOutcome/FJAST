#pragma once

#ifdef FJASTP_USE_PCH
#include FJASTP_PCH_DIR
#else
#include <HBuffer/HBuffer.hpp>
#include <cstdint>
#endif

#include "FJASTP.h"

namespace FJASTP{
    /// @brief A class for storing javascript tokens.
    class Token{
    public:
        Token() noexcept: m_Type(TokenType::EndOfFile){}
        Token(TokenType type, const HBuffer& value, uint32_t lineNumber, uint32_t columnNumber) noexcept;
        Token(TokenType type, const HBuffer& value, uint8_t metadata, uint32_t lineNumber, uint32_t columnNumber) noexcept;
        Token(TokenType type, HBuffer&& value, uint32_t lineNumber, uint32_t columnNumber) noexcept;
        Token(TokenType type, HBuffer&& value, uint8_t metadata, uint32_t lineNumber, uint32_t columnNumber) noexcept;

        Token(const Token& token) noexcept;
        Token(Token&& token)noexcept;
        ~Token()noexcept{}
    public:
        TokenType GetType() const noexcept{return (TokenType)m_Type;}
        HBuffer& GetValue() const noexcept{return (HBuffer&)m_Value;}
        uint8_t GetMetadata() const noexcept{return (uint8_t)m_Metadata;}
        uint32_t GetLineNumber() const noexcept{return m_LineNumber;}
        uint32_t GetColumnNumber() const noexcept{return m_ColumnNumber;}
    public:
        Token& operator=(const Token& token)noexcept;
        Token& operator=(Token&& token)noexcept;
    private:
        TokenType m_Type;
        /// @brief stores extra metadata that is interpreted differently depending on the tokenType
        uint8_t m_Metadata;
        uint32_t m_LineNumber;
        uint32_t m_ColumnNumber;
        
        HBuffer m_Value;
    };
}