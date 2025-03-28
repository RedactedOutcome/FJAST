#include "pch.h"
#include "ASTGenerator.h"

namespace FJASTP{

    ASTGeneratorResult ASTGenerator::ParseExpression(Node* output)noexcept{
        Token t = GetToken(m_At);
        TokenType type = t.GetType();
        switch(type){
        case TokenType::Identifier:{
            output->m_Left = &(m_Input->at(m_At).GetValue());
            output->m_Type = NodeType::Expression;
            output->m_Metadata = 0;
            return ASTGeneratorResult();
        }
        default:
            return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidExpression);
        }
        return ASTGeneratorResult();
    }

    ASTGeneratorResult ASTGenerator::ParseCurrentToken(std::vector<Node*>& output) noexcept{
        Token currentToken = GetToken(m_At);

        switch(currentToken.GetType()){
            case TokenType::EndOfFile:
                return ASTGeneratorResult(ASTGeneratorError::EndOfFile);
            case TokenType::Identifier:{
                output.emplace_back(m_NodePool.Allocate());
                break;
            }
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
                                m_At++;

                                std::vector<Node*> body;
                                while(true){
                                    ASTGeneratorResult result = ParseCurrentToken(body);
                                    if(result)continue;
                                    if(result.m_ErrorCode == ASTGeneratorError::EndOfFile)break;
                                    return result;
                                }
    
                                    Token t = GetToken(m_At);
                                    if(t.GetValue() != "}"){
                                        return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidClassDefinition);
                                    }
                                std::cout << "Got expression"<<std::endl;
                                std::cout << "EXP is " << ((HBuffer*)expression.GetLeft())->SubString(0,-1).GetCStr()<<std::endl;
                                return ASTGeneratorResult();
                            }
                            return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidClassDefinition);
                        }

                        if(next.GetValue() != "{")
                            return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidClassDefinition);
                        std::cout << "Normal class definition"<<std::endl;

                        return ASTGeneratorResult();
                        break;
                    }
                    default:
                        return ASTGeneratorResult(m_At, ASTGeneratorError::UnsupportedSyntax);
                }
            }
        default:
            return ASTGeneratorResult(m_At, ASTGeneratorError::UnsupportedSyntax);
            break;
        }
    }

    ASTGeneratorResult ASTGenerator::Generate(std::vector<Token>& input, std::vector<Node*>& output) noexcept{
        m_Input = &input;
        m_Output= &output;

        m_At = 0;
        m_TokenCount = static_cast<uint32_t>(input.size());

        while(m_At < m_TokenCount){
            ASTGeneratorResult result = ParseCurrentToken(*m_Output);
            if(!result)return result;
        }
        return ASTGeneratorResult();
    }
}