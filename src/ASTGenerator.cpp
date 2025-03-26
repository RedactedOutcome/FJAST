#include "pch.h"
#include "ASTGenerator.h"

namespace FJASTP{

    ASTGeneratorResult ASTGenerator::ParseExpression(Token& output)noexcept{
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
                    Token identifier = GetToken(m_At+1);
                    if(identifier.GetType() != TokenType::Identifier)return ASTGeneratorResult(m_At + 1, ASTGeneratorError::InvalidClassDefinition);

                    
                    m_At+=2;
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