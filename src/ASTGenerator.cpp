#include "pch.h"
#include "ASTGenerator.h"

namespace FJASTP{

    ASTGeneratorResult ASTGenerator::ParseExpression(Node** output)noexcept{
        Token t = GetToken(m_At);
        TokenType type = t.GetType();
        switch(type){
        case TokenType::Identifier:{
            output = /*Generate*/nullptr;
            return ASTGeneratorResult();
        }
        default:
            return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidExpression);
        }
        return ASTGeneratorResult();
    }
    ASTGeneratorResult ASTGenerator::Generate(std::vector<Token>& input, std::vector<Node>& output) noexcept{
        m_Input = &input;
        m_Output= &output;

        m_At = 0;
        m_TokenCount = static_cast<uint32_t>(input.size());

        while(m_At < m_TokenCount){
            const Token& currentToken = input[m_At];

            switch(currentToken.GetType()){
            case TokenType::Keyword:{
                uint8_t keyword = currentToken.GetMetadata();
                std::cout << "KEYWORD"<<std::endl;
                switch(keyword){
                case (uint8_t)Keyword::Class:{
                    Token identifier = GetToken(++m_At);
                    if(identifier.GetType() != TokenType::Identifier)return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidClassDefinition);

                    Token next = GetToken(++m_At);
                    TokenType type = next.GetType();
                    if(type == TokenType::Keyword){
                        if(next.GetMetadata() == (uint8_t)Keyword::Extends){
                            //Derived Class
                            Token extendedClass = GetToken(++m_At);
                            if(extendedClass.GetType() != TokenType::Identifier)return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidClassDefinition);
                            
                            if(GetToken(++m_At).GetValue() != "{"){
                                return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidClassDefinition);
                            }
                            m_At++;

                            //Get Class body
                            Node expression;
                            ASTGeneratorResult result = ParseExpression(&expression);
                            if(!result)return result;

                            std::cout << "Got expression"<<std::endl;
                            continue;
                        }
                        else{
                            return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidClassDefinition);
                        }
                    }
                    break;
                }
                }
                break;
            }
            default:
                return ASTGeneratorResult(m_At, ASTGeneratorError::UnsupportedSyntax);
                break;
            }
        }
        return ASTGeneratorResult();
    }
}