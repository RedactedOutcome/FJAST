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
                    std::vector<Token*> parameters;

                    while(true){
                        current = GetToken(m_At);
                        if((uint8_t)current.GetType() == (uint8_t)TokenType::Identifier){
                            parameters.emplace_back(&m_Input->at(m_At++));
                            continue;
                        }
                        break;
                    }

                    current = GetToken(m_At);
                    if(current.GetValue() != ")")
                        return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidMethodDeclaration);
                    current = GetToken(++m_At);
                    if(current.GetValue() != "{")
                        return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidMethodBody);
                    m_At++;
                    std::cout << "Getting method"<<std::endl;
                    std::cout << "Current token is " << GetToken(m_At).GetValue().SubString(0,-1).GetCStr()<<std::endl;

                    //Parse function body
                    std::vector<Node*> body;
                    while(true){
                        ASTGeneratorResult result = ParseCurrentToken(body);
                        if(!result)break;
                    }
                    std::cout << "After body"<<std::endl;
                    std::cout << "1 Current token is " << GetToken(m_At).GetValue().SubString(0,-1).GetCStr()<<std::endl;

                    if(GetToken(m_At).GetValue() != "}")return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidMethodBody);
                    m_At++;
                    std::cout << "2 Current token is " << GetToken(m_At).GetValue().SubString(0,-1).GetCStr()<<std::endl;

                    //APpend an actual node

                    /// @brief an allocated copy of parameters that exist on the heap and the node own
                    std::vector<Token*>* allocatedParameters = new std::vector<Token*>();
                    allocatedParameters->reserve(parameters.size());
                    for(size_t i = 0; i < parameters.size(); i++)
                        allocatedParameters->emplace_back(parameters[i]);
                    
                    /// @brief Function Identifier
                    void* left = &AtToken(startAt).GetValue();
                    /// @brief Function Parameters
                    void* right = static_cast<void*>(allocatedParameters);
                    uint8_t metadata = 0;
                    Node* methodDeclaration = m_NodePool.Allocate(std::move(body), left, right, NodeType::MethodDeclaration, metadata);
                    output.push_back(methodDeclaration);
                }

                return ASTGeneratorResult(m_At, ASTGeneratorError::ExpectedExpressionOrStatement);
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

                                std::vector<Node*> body;
                                ASTGeneratorResult result;
                                while(true){
                                    std::cout << "Current token " << GetToken(m_At).GetValue().SubString(0,-1).GetCStr()<<std::endl;
                                    std::cout << "At before " << m_At<<std::endl;
                                    result = ParseCurrentToken(body);
                                    std::cout << "At after " << m_At <<std::endl;
                                    if(!result)break;
                                }

                                std::cout <<"Showing body"<<std::endl;
                                for(size_t i = 0; i < body.size(); i++){
                                    Node& node = *body[i];
                                    switch((uint8_t)node.GetNodeType()){
                                    case (uint8_t)NodeType::Expression:{
                                        if(node.m_Metadata == 0){
                                            HBuffer* buffer = ((HBuffer*)node.GetLeft());
                                            std::cout << "Got buffer " << buffer->SubString(0,-1).GetCStr() << std::endl;
                                        }
                                    }
                                    }
                                    std::cout<<std::endl;
                                }
    
                                Token t = GetToken(m_At);
                                std::cout <<"a Current token value is "<< t.GetValue().SubString(0,-1).GetCStr()<<std::endl;
                                if(t.GetValue() != "}"){
                                    return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidClassDefinition);
                                }
                                m_At++;
                                std::cout << "Got Class decleration"<<std::endl;
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