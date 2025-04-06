#include "pch.h"
#include "Tokenizer.h"

namespace FJASTP{

    /*
    NUMBER METADATA
    76543210
    7 - IsNegative
    6 - IsDecimal
    5&4 - Format.
    3 - Notation sign

    Format -
    0 - base10
    1 - binary
    2 - hex
    3 - scientific
    
    */
    TokenizeResult Tokenizer::Tokenize(const HBuffer& input, std::vector<Token>& output, bool allowComments) noexcept{
        m_CurrentInput = input;
        m_CurrentOutput = &output;
        
        m_InputSize = static_cast<uint32_t>(input.GetSize());
        /// @brief getting the amount of extra tokens to allocate whenever we reallocate our list
        //m_ExtraReallocate = static_cast<uint32_t>(std::max(static_cast<uint32_t>(150), static_cast<uint32_t>(m_InputSize / 200)));

        while(true){
            if(m_At >= m_InputSize)return TokenizeResult();
            char c = input.At(m_At);
            
            switch(c){
                case '\t':
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
                case '/':{
                    //Check if comment
                    char nextChar = m_CurrentInput.Get(m_At + 1);
                    if(nextChar == '/'){
                        m_At+=2;
                        size_t commentAt = m_At;

                        while(true){
                            if(m_At >= m_InputSize){
                                //Last line
                                if(allowComments)PushBack(TokenType::Comment, m_CurrentInput.SubBuffer(commentAt, m_At - commentAt), m_Line, GetCurrentColumn());
                                return TokenizeResult();
                            }
                            char c = m_CurrentInput.At(m_At++);
                            if(c == '\n'){
                                m_Line++;
                                m_CurrentLineStart = m_At;
                                if(allowComments)PushBack(TokenType::Comment, m_CurrentInput.SubBuffer(commentAt, (m_At - commentAt) - 1), m_Line, GetCurrentColumn());
                                break;
                            }
                            if(c == '\12'){
                                nextChar = m_CurrentInput.Get(m_At++);
                                if(nextChar == '\n'){
                                    m_At++;
                                    m_Line++;
                                    m_CurrentLineStart = m_At;
                                    ///Havent tested this yet
                                    if(allowComments)PushBack(TokenType::Comment, m_CurrentInput.SubBuffer(commentAt, (m_At - commentAt) - 2), m_Line, GetCurrentColumn());
                                    break;
                                }
                                if(allowComments)PushBack(TokenType::Comment, m_CurrentInput.SubBuffer(commentAt, (m_At - commentAt) - 2), m_Line, GetCurrentColumn());

                                m_Line++;
                                m_CurrentLineStart = m_At;
                            }
                        }
                        break;
                    }
                    else if(nextChar == '*'){
                        m_At+=2;
                        while(true){
                            if(m_At >= m_InputSize){
                                return TokenizeResult(m_Line, GetCurrentColumn(), TokenizerError::EndOfFile);
                            }
                            char c = m_CurrentInput.At(m_At);
                            if(c == '\15'){
                                char next = m_CurrentInput.Get(++m_At);
                                m_Line++;
                                if(next == '\n'){
                                    m_CurrentLineStart=++m_At;
                                    continue;
                                }
                                m_CurrentLineStart=m_At;
                                continue;
                            }
                            if(c == '\n'){
                                m_Line++;
                                m_CurrentLineStart=++m_At;
                                continue;
                            }
                            if(c == '*'){
                                if(m_CurrentInput.Get(m_At + 1) == '/'){
                                    m_At+=2;
                                    break;
                                }
                                m_At+=2;
                                continue;
                            }
                            m_At++;
                        }
                        break;
                    }
                    else if(nextChar == '='){
                        PushBack(TokenType::AssignmentOperator, m_CurrentInput.SubBuffer(m_At, 2), (uint8_t)AssignmentOperator::DivisionAssignment, m_Line, GetCurrentColumn());
                        m_At+=2;
                        break;
                    }

                    //Operators that start with /
                    PushBack(TokenType::ArithmeticOperator, m_CurrentInput.SubBuffer(m_At, 1), (uint8_t)ArithmeticOperations::Division, m_Line, GetCurrentColumn());
                    m_At++;
                    break;
                }
                case '*':{
                    char next = m_CurrentInput.Get(m_At + 1);

                    if(next == '*'){
                        //Exponentiation
                        if(m_CurrentInput.Get(m_At + 2) == '='){
                            //Assignment
                            PushBack(TokenType::AssignmentOperator, m_CurrentInput.SubBuffer(m_At, 3), (uint8_t)AssignmentOperator::ExponentiationAssignment, m_Line, GetCurrentColumn());
                            m_At+=3;
                            break;
                        }
                        PushBack(TokenType::ArithmeticOperator, m_CurrentInput.SubBuffer(m_At, 2), (uint8_t)ArithmeticOperations::Exponentiation, m_Line, GetCurrentColumn());
                        m_At+=2;
                        break;
                    }
                    else if(next == '='){
                        //Multiplication Assignment
                        PushBack(TokenType::AssignmentOperator, m_CurrentInput.SubBuffer(m_At, 2), (uint8_t)AssignmentOperator::MultiplicationAssignment, m_Line, GetCurrentColumn());
                        m_At+=2;
                        break;
                    }

                    //Arithmetic Operator
                    PushBack(TokenType::ArithmeticOperator, m_CurrentInput.SubBuffer(m_At, 1), (uint8_t)ArithmeticOperations::Multiplication, m_Line, GetCurrentColumn());
                    m_At++;
                    break;
                }
                case '+':{
                    char c = m_CurrentInput.Get(m_At + 1);

                    if(c == '+'){
                        //Unary Operator
                        PushBack(TokenType::UnaryOperator, m_CurrentInput.SubBuffer(m_At, 2), m_Line, GetCurrentColumn());
                        m_At+=2;
                        break;
                    }
                    if(c == '='){
                        //Unary Operator
                        PushBack(TokenType::AssignmentOperator, m_CurrentInput.SubBuffer(m_At, 2), (uint8_t)AssignmentOperator::AdditionAssignment, m_Line, GetCurrentColumn());
                        m_At+=2;
                        break;
                    }
                    //Arithmetic Operator
                    PushBack(TokenType::ArithmeticOperator, m_CurrentInput.SubBuffer(m_At, 1), (uint8_t)ArithmeticOperations::Addition, m_Line, GetCurrentColumn());
                    m_At++;
                    break;
                }
                case '%':{
                    char nextToken = m_CurrentInput.Get(m_At + 1);
                    if(nextToken == '='){
                        PushBack(TokenType::AssignmentOperator, m_CurrentInput.SubBuffer(m_At, 2), (uint8_t)AssignmentOperator::ModulosAssignment, m_Line, GetCurrentColumn());
                        m_At+=2;
                        break;
                    }
                    PushBack(TokenType::ArithmeticOperator, m_CurrentInput.SubBuffer(m_At++, 1), (uint8_t)ArithmeticOperations::Modulus, m_Line, GetCurrentColumn());
                    break;
                }
                case '\'':
                case '"':{
                    //String Literals
                    uint32_t startLine = m_Line;
                    uint32_t startColumn = GetCurrentColumn();
                    uint32_t startAt = ++m_At;

                    HBuffer stringData;
                    stringData.Reserve(10);
                    
                    /// @brief the position in the buffer the string is currently being copied from
                    /// @brief the string becomes a copy so we dont include unwanted characters like line breaks from the original code
                    uint32_t stringAt = m_At;
                    uint32_t copyOffset = 0;
                    
                    while(true){
                        if(m_At >= m_InputSize)return TokenizeResult(m_Line, GetCurrentColumn(), TokenizerError::EndOfFile);
                        char current= m_CurrentInput.At(m_At++);
                        if(current == c){
                            stringData.Copy(copyOffset, m_CurrentInput.GetData() + stringAt, m_At - stringAt - 1);
                            break;
                        }
                        if(current == '\\'){
                            uint32_t size = m_At - stringAt - 1;
                            stringData.Copy(copyOffset, m_CurrentInput.GetData() + stringAt, size);
                            copyOffset+=size;
                            current = m_CurrentInput.Get(++m_At);

                            ///TODO: fix characters being skipped here
                            if(current == '\12'){
                                //New line
                                m_Line++;
                                m_CurrentLineStart = ++m_At;
                                current = m_CurrentInput.Get(m_At);
                                if(current == '\15'){
                                    m_CurrentLineStart++;
                                    m_At++;
                                }
                                stringAt = m_CurrentLineStart;
                            }else if(current == '\15'){
                                //New line
                                m_Line++;
                                m_CurrentLineStart = ++m_At;
                                stringAt = m_CurrentLineStart;
                            }
                            else    m_At++;
                        }

                        if(current == '\12' || current == '\15')return TokenizeResult(m_Line, GetCurrentColumn() - 1, TokenizerError::InvalidStringLiteral);
                    }
                    PushBack(TokenType::StringLiteral, std::move(stringData), startLine, startColumn);
                    break;
                }
                case '{':
                case '}':
                case '[':
                case ']':
                case '(':
                case ')':{
                    PushBack(TokenType::GroupingSymbol, m_CurrentInput.SubPointer(m_At, 1), m_Line, GetCurrentColumn());
                    m_At++;
                    break;
                }
                case ':':
                case ',':
                case ';':
                case '`':
                case '?':
                case '#':
                case '.':{
                    PushBack(TokenType::Punctuator, m_CurrentInput.SubPointer(m_At++, 1), m_Line, GetCurrentColumn());
                    break;
                }
                case '-':{
                    //Handle negation and urinary operators
                    char c = m_CurrentInput.Get(m_At + 1);
                    if(c == '-'){
                        PushBack(TokenType::UnaryOperator, m_CurrentInput.SubBuffer(m_At, 2), m_Line, GetCurrentColumn());
                        m_At+=2;
                        break;
                    }
                    if(c == '='){
                        PushBack(TokenType::AssignmentOperator, m_CurrentInput.SubBuffer(m_At, 2), (uint8_t)AssignmentOperator::SubtractionAssignment, m_Line, GetCurrentColumn());
                        m_At+=2;
                        break;
                    }
                    PushBack(TokenType::ArithmeticOperator, m_CurrentInput.SubBuffer(m_At, 1), (uint8_t)ArithmeticOperations::Subtraction, m_Line, GetCurrentColumn());
                    m_At++;
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
                    uint32_t column = GetCurrentColumn();
                    uint32_t startAt = m_At;
                    m_At++;

                    bool hasDecimal = false;
                    NumericalLiteralType format = NumericalLiteralType::Base10;
                    bool notationExponentNeg = false;
                    bool startedExponent=false;

                    while(true){
                        if(m_At >= m_InputSize)break;
                        char c = m_CurrentInput.At(m_At);
                        
                        bool shouldContinue = false;
                        switch(format){
                        case NumericalLiteralType::ScientificNotation:
                        case NumericalLiteralType::Base10:{
                            if(c >= '0' && c <= '9'){
                                m_At++;
                                shouldContinue = true;
                                startedExponent = true;
                                break;
                            }
                            break;
                        }
                        case NumericalLiteralType::Hexidecimal:{
                            if((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')){
                                m_At++;
                                shouldContinue = true;
                                break;
                            }
                            break;
                        }
                        case NumericalLiteralType::Binary:{
                            if((c >= '0' && c <= '1')){
                                m_At++;
                                shouldContinue = true;
                                break;
                            }
                            break;
                        }
                        }
                        if(shouldContinue)continue;

                        if(c == '.'){
                            if(hasDecimal || format != NumericalLiteralType::Base10)return TokenizeResult(m_Line, GetCurrentColumn(), TokenizerError::InvalidNumericalLiteral);
                            hasDecimal = true;
                            m_At++;
                            continue;
                        }
                        if(c == 'x' || c == 'X'){
                            if(hasDecimal || format != NumericalLiteralType::Base10)return TokenizeResult(m_Line, GetCurrentColumn(), TokenizerError::InvalidNumericalLiteral);
                            if(m_At - startAt > 2)return TokenizeResult(m_Line, GetCurrentColumn(), TokenizerError::InvalidNumericalLiteral);
                            if(m_CurrentInput.At(startAt + 1) != '0')return TokenizeResult(m_Line, GetCurrentColumn(), TokenizerError::InvalidNumericalLiteral);
                            format = NumericalLiteralType::Hexidecimal;
                            m_At++;
                            continue;
                        }
                        if(c == 'b' || c == 'B'){
                            if(hasDecimal || format != NumericalLiteralType::Base10)return TokenizeResult(m_Line, GetCurrentColumn(), TokenizerError::InvalidNumericalLiteral);
                            if(m_At - startAt > 2)return TokenizeResult(m_Line, GetCurrentColumn(), TokenizerError::InvalidNumericalLiteral);
                            if(m_CurrentInput.At(startAt + 1) != '0')return TokenizeResult(m_Line, GetCurrentColumn(), TokenizerError::InvalidNumericalLiteral);
                            format = NumericalLiteralType::Binary;
                            m_At++;
                            continue;
                        }

                        if(c == 'E' || c == 'e'){
                            if(format != NumericalLiteralType::Base10)return TokenizeResult(m_Line, GetCurrentColumn(), TokenizerError::InvalidNumericalLiteral);
                            format = NumericalLiteralType::ScientificNotation;
                            m_At++;
                            startedExponent = false;
                            continue;
                        }

                        if(c == '-'){
                            if(!startedExponent && format == NumericalLiteralType::ScientificNotation){
                                if(notationExponentNeg)return TokenizeResult(m_Line, GetCurrentColumn(), TokenizerError::InvalidNumericalLiteral);
                                notationExponentNeg = true;
                                m_At++;
                                continue;
                            }
                        }
                        if(!IsValidLiteralSplitter(c))return TokenizeResult(m_Line, GetCurrentColumn(), TokenizerError::InvalidNumericalLiteral);
                        break;
                    }

                    uint8_t metadata = 128 | (hasDecimal ? 64 : 0) | ((uint8_t)format << 3) || (notationExponentNeg ? 4 : 0);
                    PushBack(TokenType::NumericalLiteral, m_CurrentInput.SubPointer(startAt, m_At - startAt), metadata, m_Line, column);
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
                            PushBack(TokenType::ConditionalOperator, HBuffer("===", 3, false, false), m_Line, GetCurrentColumn());
                            continue;
                        }
                        PushBack(TokenType::ConditionalOperator, HBuffer("==", 2, false, false), m_Line, GetCurrentColumn());
                    }
                    else if (nextChar == '>'){
                        m_At++;
                        PushBack(TokenType::ConditionalOperator, HBuffer("=>", 2, false, false), m_Line, GetCurrentColumn());
                        continue;
                    }
                    else{
                        PushBack(TokenType::AssignmentOperator, HBuffer("=", 1, false, false), m_Line, GetCurrentColumn());
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
        uint32_t startAt = m_At;

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
        uint32_t startAt = m_At;

        if(startChar & 128){
            TokenizeResult result = ValidateUTF8();
            if(!result)return result;
        }
        else m_At++;

        while(true){
            if(m_At >= m_InputSize)break;
            char c = m_CurrentInput.At(m_At);

            //TODO: maybe run through a lookup
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

        uint32_t identifierSize = (m_At - startAt);
        HBuffer buff = m_CurrentInput.SubPointer(startAt, identifierSize);
        uint8_t metadata = FJastP::GetKeywordValue(buff);
        TokenType tokenType = TokenType::Identifier;
        if(metadata > 0)tokenType = TokenType::Keyword;
        else{
            if(buff == "Infinity"){
                metadata = static_cast<uint8_t>(NumericalLiteralType::Infinity);
                tokenType = TokenType::NumericalLiteral;
            }
            else if(buff == "NaN"){
                metadata = static_cast<uint8_t>(NumericalLiteralType::NaN);
                tokenType = TokenType::NumericalLiteral;
            }
        }
        PushBack(tokenType, std::move(buff), metadata, m_Line, GetCurrentColumn(startAt));
        //Success no need to return anything
        return TokenizeResult();
    }

    bool Tokenizer::IsValidLiteralSplitter(char c) const noexcept{
        switch(c){
        case '\12':
        case '\15':
        case '*':
        case ' ':
        case ',':
        case '(':
        case ')':
        case '{':
        case '}':
        case '$':
        case '%':
        case '+':
        case '-':
        case '=':
        case '!':
        case '`':
        case '/':
        case '@':
        case ';':
            return true;
        default:
            return false;
        }
    }
    
}