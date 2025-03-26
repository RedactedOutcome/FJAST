#pragma once


#include "Node.h"
#include "Token.h"

namespace FJASTP{
    struct ASTGeneratorResult{
        uint32_t m_ErrorAt; /// @brief the token the current error is at if any
        ASTGeneratorError m_ErrorCode;

        ASTGeneratorResult()noexcept: m_ErrorAt(0), m_ErrorCode(ASTGeneratorError::Success){}
        ASTGeneratorResult(uint32_t errorAt, ASTGeneratorError errorCode)noexcept: m_ErrorAt(errorAt), m_ErrorCode(errorCode){}

        uint32_t GetErrorAt() const noexcept{return m_ErrorAt;}
        ASTGeneratorError GetErrorCode() const noexcept{return m_ErrorCode;}

        constexpr operator bool() const noexcept{
            return m_ErrorCode == ASTGeneratorError::Success;
        }
    };

    class ASTGenerator{
    public:
        /// @brief Generates an abstract syntax tree from a list of tokens.
        /// @return Returns type of ASTGeneratorResult.
        ASTGeneratorResult Generate(std::vector<Token>& input, std::vector<Node>& output) noexcept;

        Token GetToken(size_t at) const noexcept{if(at < 0 || at >= m_TokenCount)return Token(); return m_Input->at(at);}
    private:
        ASTGeneratorResult ParseExpression(Token& output)noexcept;
    private:
        std::vector<Token>* m_Input;
        std::vector<Node>* m_Output;
        uint32_t m_TokenCount;
        uint32_t m_At;
    };
}