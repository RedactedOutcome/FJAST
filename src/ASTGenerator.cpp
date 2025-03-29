#include "pch.h"
#include "ASTGenerator.h"

namespace FJASTP{

    ASTGeneratorResult ASTGenerator::ParseExpression(Node* output)noexcept{
        Token t = GetToken(m_At);
        TokenType type = t.GetType();
        switch(type){
        case TokenType::Identifier:{
            //std::cout << "Current node size is " << (size_t)currentToken.GetValue().GetData() << std::endl;
            //Node* node = m_NodePool.Allocate((void*)&AtToken(m_At).GetValue(), (void*)nullptr, NodeType::Expression, 0);
            *output = Node((void*)&AtToken(m_At).GetValue(), (void*)nullptr, NodeType::Expression, 0);
            m_At++;
            return ASTGeneratorResult();
        }
        default:
            return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidExpression);
        }
        return ASTGeneratorResult();
    }

    ASTGeneratorResult ASTGenerator::ParseCurrentToken(std::vector<Node*>& output) noexcept{
        if(m_At >= m_TokenCount)return ASTGeneratorResult(m_At, ASTGeneratorError::EndOfFile);

        Token currentToken = GetToken(m_At);
        switch(currentToken.GetType()){
            case TokenType::EndOfFile:
                return ASTGeneratorResult(ASTGeneratorError::EndOfFile);
            case TokenType::Identifier:{
                size_t startAt = m_At;
                Token current = GetToken(m_At + 1);

                if(current.GetValue() == "("){
                    //Method Declaration
                    m_At+=2;
                    /// @brief an allocated copy of parameters that exist on the heap and the node own
                    std::vector<Token*>* allocatedParameters = new std::vector<Token*>();

                    while(true){
                        current = GetToken(m_At);
                        if((uint8_t)current.GetType() == (uint8_t)TokenType::Identifier){
                            allocatedParameters->emplace_back(&m_Input->at(m_At++));
                            continue;
                        }
                        break;
                    }

                    current = GetToken(m_At);
                    if(current.GetValue() != ")"){
                        delete allocatedParameters;
                        return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidMethodDeclaration);
                    }
                    current = GetToken(++m_At);
                    if(current.GetValue() != "{"){
                        delete allocatedParameters;
                        return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidMethodBody);
                    }
                    m_At++;

                    //Parse function body
                    std::vector<Node*> body;
                    while(true){
                        ASTGeneratorResult result = ParseCurrentToken(body);
                        if(!result)break;
                    }

                    if(GetToken(m_At).GetValue() != "}"){
                        delete allocatedParameters;
                        return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidMethodBody);
                    }
                    m_At++;
                    
                    /// @brief Function Identifier
                    void* left = &AtToken(startAt).GetValue();
                    /// @brief Function Parameters
                    void* right = static_cast<void*>(allocatedParameters);
                    uint8_t metadata = 0;
                    Node* methodDeclaration = m_NodePool.Allocate(std::move(body), left, right, NodeType::MethodDeclaration, metadata);
                    output.push_back(methodDeclaration);
                    return ASTGeneratorResult();
                }

                return ASTGeneratorResult(m_At, ASTGeneratorError::ExpectedExpressionOrStatement);
            }
            case TokenType::Keyword:{
                uint8_t keyword = currentToken.GetMetadata();
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
                                std::vector<Node*> body;
                                ASTGeneratorResult result;
                                while(true){
                                    result = ParseCurrentToken(body);
                                    if(!result)break;
                                }

                                Token t = GetToken(m_At);
                                if(t.GetValue() != "}"){
                                    return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidClassDefinition);
                                }
                                m_At++;
                                void* left = nullptr;
                                void* right = nullptr;
                                uint8_t metadata = 0;

                                output.emplace_back(m_NodePool.Allocate(std::move(body), left, right, NodeType::ClassDeclaration, metadata));
                                //std::cout << "EXP is " << ((HBuffer*)expression.GetLeft())->SubString(0,-1).GetCStr()<<std::endl;
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
            if(!result){
                return result.m_ErrorCode == ASTGeneratorError::EndOfFile ? ASTGeneratorResult() : result;
            }
        }
        return ASTGeneratorResult();
    }
}