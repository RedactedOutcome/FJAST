#pragma once

#ifdef FJASTP_USE_PCH
#include FJASTP_PCH_DIR
#else
#include <HBuffer/HBuffer.hpp>
#include <vector>
#endif

#include "FJASTP.h"
#include "Token.h"
#include "Memory/SegmentedPool.h"

namespace FJASTP{
    /// TODO: multi threaded chunk parsing
    struct TokenizeResult{
        uint32_t m_Line;
        uint32_t m_Column;
        TokenizerError m_ErrorCode;    //Type of TokenizerError
        /// TODO: have a Buffer that gives a reason on why the syntax failed.
        /// HBuffer m_ErrorReason;

        TokenizeResult() noexcept: m_Line(0), m_Column(0), m_ErrorCode(TokenizerError::Success){}
        TokenizeResult(uint32_t line, uint32_t column) noexcept: m_Line(line), m_Column(column), m_ErrorCode(TokenizerError::Success){}
        TokenizeResult(uint32_t line, uint32_t column, TokenizerError errorCode) noexcept: m_Line(line), m_Column(column), m_ErrorCode(errorCode){}
        
        constexpr operator bool() const noexcept{
            return m_ErrorCode == TokenizerError::Success;
        }
    };

    class Tokenizer{
    public:
        /// @brief Parses the input file as javascript and appends tokens to output. Doesnt allocate new data just points to offset of the input buffer
        /// @param input the hbuffer that contains the buffer that the ouput datas buffers point to
        /// @return returns a empty TokenizeResult state with a success value if tokizing was successfull, else returns the line, column, and status code
        [[nodiscard]] TokenizeResult Tokenize(const HBuffer& input, std::vector<Token>& output) noexcept;
    private:
        /// @brief Validates a utf8 character at m_At and jumps past it if valid.
        /// @return returns success if is valid utf8 javascript identifier character.
        TokenizeResult ValidateUTF8() noexcept;

        /// @brief Checks if characer is valid LiteralSplitter. For example if we have 0x00hello. if the next character followed by the literal isnt a grouping symbol, special character, whitespace, newline, etc they we have a parse error
        bool IsValidLiteralSplitter(char c) const noexcept;

        /// @brief starts parsing the rest of an identifier
        /// @param startChar must either be a valid alphabetical character, number, underscore, else must be some sort of utf8 character
        TokenizeResult ParseIdentifier(char startChar)noexcept;

        uint32_t GetCurrentColumn() const noexcept{return (m_At - m_CurrentLineStart - m_UnicodeBytesInLine) + 1;}
        /// @brief returns the column number of a position inside the buffer. at must be atleast the start of the current line
        uint32_t GetCurrentColumn(uint32_t at) const noexcept{return (at - m_CurrentLineStart - m_UnicodeBytesInLine) + 1;}
    private:
        template <typename... Args>
        void PushBack(Args&&... args) noexcept{
            if(m_CurrentOutput->size() == m_CurrentOutput->capacity()){
                size_t newSize = static_cast<size_t>(m_CurrentOutput->size() * 1.65f);
                m_CurrentOutput->reserve(newSize);
            }
            m_CurrentOutput->emplace_back(std::forward<Args>(args)...);
        }
    private:
        uint32_t m_At = 0;
        uint32_t m_Line = 1;     //What line the tokenizer is currently on
        uint32_t m_CurrentLineStart=0;    //Determines where the current line started in the buffer to calculate the column
        uint32_t m_UnicodeBytesInLine=0;    //Determines where the current line started in the buffer to calculate the column
        /// @brief the amount of extra tokens to reserve whenever we reallocate the vector. This way less reallocations. This is set whenever we start reading depending on filesize.
        //uint32_t m_ExtraReallocate = 0;
        HBuffer m_CurrentInput;
        uint32_t m_InputSize;
        std::vector<Token>* m_CurrentOutput = nullptr;
    };
}