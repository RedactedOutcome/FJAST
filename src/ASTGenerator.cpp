#include "pch.h"
#include "ASTGenerator.h"

namespace FJASTP{

    ASTGeneratorResult ASTGenerator::ParseExpression(Node* output)noexcept{
        Token t = GetToken(m_At);
        TokenType type = t.GetType();
        switch(type){
        case TokenType::EndOfFile:
            return ASTGeneratorResult(ASTGeneratorError::EndOfFile);
        case TokenType::Identifier:{
            uint32_t startIdentifier = m_At;
            Token next = GetToken(m_At + 1);
            if(next.GetValue() == "."){
                //Is a property access
                /*
                next = GetToken(m_At + 2);
                if(next.GetType() != TokenType::Identifier){
                    return ASTGeneratorResult(m_AT, ASTGeneratorError::InvalidPropertyAccessExpression);
                }
                */
                Node* right = m_NodePool.Allocate();
                ASTGeneratorResult result = ParseExpression(right);
                if(!result)return result;

                NodeType type = right->GetNodeType();
                if(type != NodeType::PropertyAccessExpression && type != NodeType::IdentifierExpression)return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidPropertyAccessExpression);

                *output = Node(&m_Input->at(startIdentifier), right, NodeType::PropertyAccessExpression, 0);
                return ASTGeneratorResult();
            }
            /// TODO: Check for operators
            *output = Node((void*)&AtToken(m_At).GetValue(), (void*)nullptr, NodeType::IdentifierExpression, 0);
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
                    //Method Declaration Or call
                    m_At+=2;
                    /// @brief an allocated copy of parameters that exist on the heap and the node own
                    std::vector<Token*>* allocatedParameters = new std::vector<Token*>();

                    Token current = GetToken(m_At);
                    if(current.GetType() == TokenType::Identifier){
                        allocatedParameters->emplace_back(&m_Input->at(m_At++));

                        //Get Rest of the Parameters
                        while(true){
                            if(GetToken(m_At).GetValue() != ",")break;
                            current = GetToken(++m_At);
                            if((uint8_t)current.GetType() != (uint8_t)TokenType::Identifier){
                                delete allocatedParameters;
                                return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidParameterList);
                            }
                            allocatedParameters->emplace_back(&m_Input->at(m_At++));
                        }
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
                        uint32_t identifierAt = ++m_At;
                        Token identifier = GetToken(identifierAt);
                        void* derivedFrom = nullptr;
                        if(identifier.GetType() != TokenType::Identifier)return ASTGeneratorResult(identifierAt, ASTGeneratorError::InvalidClassDefinition);

                        Token next = GetToken(++m_At);
                        TokenType type = next.GetType();
                        if(type == TokenType::Keyword){
                            if(next.GetMetadata() == (uint8_t)Keyword::Extends){
                                /// Derived Class
                                uint32_t derivedFromAt = ++m_At;
                                if(GetToken(derivedFromAt).GetType() != TokenType::Identifier)return ASTGeneratorResult(derivedFromAt, ASTGeneratorError::InvalidClassDefinition);
                                derivedFrom = static_cast<void*>(&m_Input->at((size_t)derivedFromAt).GetValue());
                                m_At++;
                                next = GetToken(m_At);
                            }else{
                                return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidClassDefinition);
                            }
                        }
                        if(next.GetValue() != "{")
                            return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidClassDefinition);
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
                            return result;
                        }
                        m_At++;
                        void* left = &m_Input->at(identifierAt).GetValue();
                        void* right = nullptr;
                        uint8_t metadata = 0;

                        output.emplace_back(m_NodePool.Allocate(std::move(body), left, right, NodeType::ClassDeclaration, metadata));
                        return ASTGeneratorResult();
                        break;
                    }
                    default:{
                        Node* node = m_NodePool.Allocate();
                        ASTGeneratorResult result = ParseExpression(node);

                        if(result){
                            //Check for operators
                            
                        }
                        return result;
                    }
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