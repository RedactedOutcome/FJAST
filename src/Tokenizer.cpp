#include "pch.h"
#include "Tokenizer.h"

namespace FJASTP{

    TokenizeResult Tokenizer::Tokenize(const HBuffer& input, std::vector<Token>& output) noexcept{
        m_CurrentInput = input;
        m_CurrentOutput = &output;

        m_InputSize = input.GetSize();

        while(true){
            if(m_At >= m_InputSize)return TokenizeResult();
            char c = input.At(m_At);
            
            switch(c){
                case ' ':{
                    m_At++;
                    continue;
                }
                case '\15':{
                    char next = m_CurrentInput.Get(++m_At);
                    m_Line++;
                    if(next == '\n'){
                        m_CurrentLineStart=++m_At;
                        continue;
                    }
                    m_CurrentLineStart=m_At;
                    continue;
                }
                case '\n':{
                    m_Line++;
                    m_CurrentLineStart=++m_At;
                    continue;
                }
                case '{':
                case '}':
                case '[':
                case ']':
                case '(':
                case ')':{
                    m_CurrentOutput->emplace_back(Token(TokenType::GroupingSymbol, m_CurrentInput.SubPointer(m_At, 1), m_Line, GetCurrentColumn()));
                    m_At++;
                    break;
                }
                case ':':
                case ',':
                case ';':
                case '`':
                case '?':{
                    m_CurrentOutput->emplace_back(Token(TokenType::Punctuator, m_CurrentInput.SubPointer(m_At, 1), m_Line, GetCurrentColumn()));
                    m_At++;
                    break;
                }
                //TODO: handle numerical literals that start with a .
                case '.':{
                    /// TODO: if we break the character loop we need to verify if a valid seperator/token is following it. Identifiers cant immediately follow without a white space or valid token
                    char nextChar = m_CurrentInput.Get(++m_At);
                    if(nextChar >= '0' && nextChar <= '9'){
                        //Floating Point Numerical Literal
                        size_t startAt = m_At-1;
                        m_At++;

                        //Scientific Notation
                        bool usesNotation = false;
                        bool isNegative = false;
                        uint8_t notationAt=0;
                        std::cout<<"Starting"<<std::endl;
                        while(true){
                            if(m_At >= m_InputSize)break;
                            char current = m_CurrentInput.At(m_At);

                            if(current >= '0' && current <= '9'){
                                m_At++;
                                continue;
                            }

                            if(current == 'e' || current == 'E'){
                                if(usesNotation)return TokenizeResult(m_Line, GetCurrentColumn(), TokenizerError::InvalidNumericalLiteral);
                                usesNotation = true;

                                if(++m_At >= m_InputSize)return TokenizeResult(m_Line, GetCurrentColumn(), TokenizerError::EndOfFile);
                                current = m_CurrentInput.At(m_At);
                                if(current == '-'){
                                    isNegative = true;
                                    if(++m_At >= m_InputSize)return TokenizeResult(m_Line, GetCurrentColumn(), TokenizerError::EndOfFile);
                                    current = m_CurrentInput.At(m_At);
                                }
                                else if(current == '+'){
                                    if(++m_At >= m_InputSize)return TokenizeResult(m_Line, GetCurrentColumn(), TokenizerError::EndOfFile);
                                    current = m_CurrentInput.At(m_At);
                                }

                                size_t exponentStart = m_At;

                                //Check for digits
                                while(true){
                                    current = m_CurrentInput.Get(m_At);
                                    if(current >= '0' && current <= '9'){
                                        m_At++;
                                        continue;
                                    }
                                    break;
                                }
                                //Maybe have it be <=. Changed this code a daya fter writing and not sure
                                if(m_At - exponentStart < 1)return TokenizeResult(m_Line, GetCurrentColumn(), TokenizerError::InvalidNumericalLiteral);
                                continue;
                            }
                            break;
                        }

                        if(m_At < m_InputSize){
                            char c = m_CurrentInput.At(m_At);
                            if(c != ' ' && IsValidLiteralSplitter(c)){
                                return TokenizeResult(m_Line, GetCurrentColumn(), TokenizerError::InvalidNumericalLiteral);
                            }
                        }
                        
                        uint8_t metadata = (isNegative ? 128 : 0) | (usesNotation ? 64 : 0);
                        m_CurrentOutput->emplace_back(Token(TokenType::NumericalLiteral, m_CurrentInput.SubPointer(startAt, m_At - startAt), metadata, m_Line, GetCurrentColumn()));
                        break;
                    }
                    m_CurrentOutput->emplace_back(Token(TokenType::Punctuator, m_CurrentInput.SubPointer(m_At, 1), m_Line, GetCurrentColumn()));
                    break;
                }
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':{
                    //Numerical Literal
                    size_t m_StartAt = m_At++;
                    bool isFloatingPoint = false;
                    
                    break;
                }
                case 'A':
                case 'B':
                case 'C':
                case 'D':
                case 'E':
                case 'F':
                case 'G':
                case 'H':
                case 'I':
                case 'J':
                case 'K':
                case 'L':
                case 'M':
                case 'N':
                case 'O':
                case 'P':
                case 'Q':
                case 'R':
                case 'S':
                case 'T':
                case 'U':
                case 'V':
                case 'W':
                case 'X':
                case 'Y':
                case 'Z':
                case 'a':
                case 'b':
                case 'c':
                case 'd':
                case 'e':
                case 'f':
                case 'g':
                case 'h':
                case 'i':
                case 'j':
                case 'k':
                case 'l':
                case 'm':
                case 'n':
                case 'o':
                case 'p':
                case 'q':
                case 'r':
                case 's':
                case 't':
                case 'u':
                case 'v':
                case 'w':
                case 'x':
                case 'y':
                case 'z':
                case '_':{
                    //Is valid Identifier Start
                    TokenizeResult result = ParseIdentifier(c);
                    if(!result)return result;
                    break;
                }
                case '=':{
                    char nextChar = m_CurrentInput.Get(++m_At);
                    if(nextChar == '='){
                        char afterNext = m_CurrentInput.Get(++m_At);
                        if(afterNext == '='){
                            m_At++;
                            m_CurrentOutput->emplace_back(Token(TokenType::AssignmentOperator, HBuffer("===", 3, false, false), m_Line, GetCurrentColumn()));
                            continue;
                        }
                        m_CurrentOutput->emplace_back(Token(TokenType::AssignmentOperator, HBuffer("==", 2, false, false), m_Line, GetCurrentColumn()));
                    }
                    else if (nextChar == '>'){
                        m_At++;
                        m_CurrentOutput->emplace_back(Token(TokenType::AssignmentOperator, HBuffer("=>", 2, false, false), m_Line, GetCurrentColumn()));
                        continue;
                    }
                    else{
                        m_CurrentOutput->emplace_back(Token(TokenType::AssignmentOperator, HBuffer("=", 1, false, false), m_Line, GetCurrentColumn()));
                    }
                    break;
                }
                default:
                    if(c & 0b10000000){
                        TokenizeResult result = ParseIdentifier(c);
                        if(!result)return result;
                        continue;
                    }
                    std::cout << "Unsupported token " << (size_t)c << " " << c <<std::endl;
                    return TokenizeResult(m_Line, GetCurrentColumn(), TokenizerError::UnsupportedToken);
                }
        }

        return TokenizeResult();
    }

    TokenizeResult Tokenizer::ValidateUTF8() noexcept{
        //Unicode character
        char startChar = m_CurrentInput.At(m_At);
        int8_t bytes;
        if((startChar & 0b11100000) == 0b11000000)
            bytes = 2;
        else if((startChar & 0b11110000) == 0b11100000)
            bytes = 3;
        else if((startChar & 0b11111000) == 0b11110000)
            bytes = 4;
        else
            return TokenizeResult(m_Line, GetCurrentColumn(), TokenizerError::UnsupportedToken);
        
        if(m_At + bytes >= m_InputSize)return TokenizeResult(m_Line, GetCurrentColumn(), TokenizerError::EndOfFile);

        uint32_t character = startChar & ((2 ^ (8 - bytes))- 1);
        size_t startAt = m_At;

        for(uint8_t i = 1; i < bytes; i++){
            character<<=6;
            uint8_t currentByte = m_CurrentInput.At(m_At + i);
            if((currentByte & 0b11000000) != 0b10000000)
                return TokenizeResult(m_Line, GetCurrentColumn(startAt), TokenizerError::InvalidUTF8Character);
            
            character |= (currentByte & 0b111111);
        }
        m_At+=bytes;
        m_UnicodeBytesInLine+=bytes - 1;
        
        bool valid = (character >= 0x80 && character <= 0x7FF) || ((character >= 0x800 && character <= 0xFFFF) && character != 0xD800 && character != 0xDFFF) || (character >= 0x10000 && character <= 0x10FFFF);
        return valid ? TokenizeResult() : TokenizeResult(m_Line, GetCurrentColumn(), TokenizerError::InvalidUTF8Character);
    }

    TokenizeResult Tokenizer::ParseIdentifier(char startChar) noexcept{
        size_t startAt = m_At;

        if(startChar & 128){
            TokenizeResult result = ValidateUTF8();
            if(!result)return result;
        }
        else m_At++;

        while(true){
            if(m_At >= m_InputSize)break;
            char c = m_CurrentInput.At(m_At);

            if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == '$'){
                m_At++;
                continue;
            }

            if(c & 0b10000000){
                TokenizeResult result = ValidateUTF8();
                if(!result)return result;
                continue;
            }
            break;
        }

        size_t identifierSize = (m_At - startAt);
        HBuffer buff = m_CurrentInput.SubPointer(startAt, identifierSize);
        TokenType tokenType = FJASTP::IsKeyword(buff) ? TokenType::Keyword : TokenType::Identifier;
        m_CurrentOutput->emplace_back(Token(tokenType, std::move(buff), m_Line, GetCurrentColumn(startAt)));
        //Success no need to return anything
        return TokenizeResult();
    }
    bool Tokenizer::IsValidLiteralSplitter(char c) const noexcept{
        switch(c){
        case '\12':
        case '\15':
        case '*':
        case ' ':
        case '(':
        case ')':
        case '{':
        case '}':
        case '$':
        case '%':
        case '+',
        case '-',
        case '=',
        case '!',
        case '`':
        case '@':
            return true;
        default:
            return false;
        }
    }
}