#include "pch.h"
#include "ASTGenerator.h"

namespace FJASTP{
    ASTGeneratorResult ASTGenerator::ParseExpression(Node* output, Node* replaceNode, bool allowArithmeticOperations)noexcept{
        Token t = GetToken(m_At);
        TokenType type = t.GetType();
        switch(type){
        case TokenType::EndOfFile:
            return ASTGeneratorResult(ASTGeneratorError::EndOfFile);
        case TokenType::ArithmeticOperator:{
            if(t.GetMetadata() == (uint8_t)ArithmeticOperations::Subtraction){
                //- operator
                uint32_t startAt = ++m_At;
                Node* exp = m_NodePool.Allocate();
                ASTGeneratorResult result = ParseExpression(exp, exp);
                if(!result)return result;

                NodeType type = exp->GetNodeType();
                if(type == NodeType::ArgumentList || type == NodeType::ParameterList)
                    return ASTGeneratorResult(startAt, ASTGeneratorError::InvalidNegationExpression);

                *output = std::move(Node(static_cast<void*>(exp), nullptr, NodeType::Negation, 0));
            }
            return ASTGeneratorResult(m_At, ASTGeneratorError::UnsupportedSyntax);
        }
        case TokenType::Identifier:{
            uint32_t startIdentifier = m_At;
            Token next = GetToken(m_At + 1);

            HBuffer nextValue = next.GetValue();
            std::cout << "CURrent IDENTIFIER" << GetToken(m_At).GetValue().SubString(0,-1).GetCStr()<<std::endl;
            if(nextValue == '.'){
                //Is a property access
                *output = Node(static_cast<void*>(&m_Input->at(m_At).GetValue()), static_cast<void*>(nullptr), NodeType::IdentifierExpression, 0);
                m_At++;
                /*
                Node* right = m_NodePool.Allocate();
                ASTGeneratorResult result = ParseExpression(right, replaceNode);*/
                do{
                    m_At++;
                    Token current = GetToken(m_At);
                    if(current.GetType() != TokenType::Identifier)
                        return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidPropertyAccessExpression);
                    Node* identifierExpression = m_NodePool.Allocate(static_cast<void*>(&m_Input->at(m_At).GetValue()),  static_cast<void*>(nullptr), NodeType::IdentifierExpression, 0);
                    *output = Node(m_NodePool.Allocate(std::move(*output)), static_cast<void*>(identifierExpression), NodeType::PropertyAccessExpression, 0);
                    m_At++;
                    next = GetToken(m_At);
                }while(next.GetValue() == '.');

                /// Confirm that new type is a valid property access
                NodeType type = output->GetNodeType();
                if(type != NodeType::PropertyAccessExpression)return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidPropertyAccessExpression);\

                //Check for tokens after for more kinds of syntactical analysis
                next = GetToken(m_At);

                //TODO: check if parse argument list
                if(next.GetValue() == '('){
                    //Is a method call
                    std::cout << "Method Call"<<std::endl;
                    Node* arguments = m_NodePool.Allocate();
                    ASTGeneratorResult result = ParseExpression(arguments, arguments);
                    std::cout << "Beofre"<<std::endl;
                    if(!result)return result;
                    std::cout<<"After"<<std::endl;
                    ///Treating anything inside the parenthesis as arguments
                    ///(5 + 5) isnt a list but is a argument
                    Node* newLeft = m_NodePool.Allocate(std::move(*output));
                    uint8_t metadata = 0;
                    *replaceNode = Node(static_cast<void*>(newLeft), static_cast<void*>(arguments), NodeType::MethodCall, metadata);
                    m_At++;
                }
            }
            else if(nextValue == '('){
                m_At++;
                std::cout << "Function Call"<<std::endl;
                //Function Call or Function Declaration
                /// TODO: Function Declaration
                Node* arguments = m_NodePool.Allocate();
                ASTGeneratorResult result = ParseExpression(arguments, arguments);
                if(!result)return result;
                ///Treating anything inside the parenthesis as arguments
                uint8_t metadata = 0;
                *output = Node(static_cast<void*>(&m_Input->at(startIdentifier).GetValue()), static_cast<void*>(arguments), NodeType::FunctionCall, metadata);
                //return ASTGeneratorResult();
            }
            else if(nextValue == "++"){
                //Unary Increment operator
                *output = Node(static_cast<void*>(&m_Input->at(startIdentifier).GetValue()), static_cast<void*>(nullptr), NodeType::UnaryExpression, (uint8_t)ArithmeticOperations::Increment);
                m_At+=2;
                //return ASTGeneratorResult();
            }
            else if(nextValue == "--"){
                //Unary Decrement operator
                //Node* identifierExpression = m_NodePool.Allocate(static_cast<void*>((void*)&AtToken(m_At).GetValue()), static_cast<void*>(nullptr), NodeType::IdentifierExpression, 0);
                *output = Node(static_cast<void*>(&m_Input->at(startIdentifier).GetValue()), static_cast<void*>(nullptr), NodeType::UnaryExpression, (uint8_t)ArithmeticOperations::Decrement);
                m_At+=2;
                //return ASTGeneratorResult();
            }else{
                std::cout << "IdentifierExpression"<<std::endl;
                std::cout << "before exp is " << GetToken(m_At).GetValue().SubString(0,-1).GetCStr()<<std::endl;
           
                *output = Node((void*)&m_Input->at(m_At).GetValue(), (void*)nullptr, NodeType::IdentifierExpression, 0);
                m_At++;
                std::cout << "after exp is " << GetToken(m_At).GetValue().SubString(0,-1).GetCStr()<<std::endl;
            }

            /// TODO: Check for operators
            Token potentialOperator = GetToken(m_At);
            TokenType potentialType = potentialOperator.GetType();

            bool continueLoop = potentialType == TokenType::ArithmeticOperator;
            std::cout << "Continue Loop : " << (continueLoop ? "Yes" : "No") << std::endl;
            
            while(continueLoop){
                std::cout << "In continue Loop"<<std::endl;
                m_At++;

                Node* newExpression = m_NodePool.Allocate();
                uint8_t metadata = potentialOperator.GetMetadata();
                ASTGeneratorResult result = ParseExpression(newExpression, newExpression, metadata != (uint8_t)ArithmeticOperations::Multiplication && metadata != (uint8_t)ArithmeticOperations::Division);
                potentialOperator = GetToken(m_At);
                potentialType = potentialOperator.GetType();
                std::cout << "RESULT IS " << (result ? "GOOD" : "BASD")<<std::endl;
                if(!result)return result;

                *output = std::move(Node(m_NodePool.Allocate(std::move(*output)), newExpression, NodeType::ArithmeticExpression, metadata));
                
                continueLoop = potentialType == TokenType::ArithmeticOperator;
            };
            std::cout << "after all is " << GetToken(m_At).GetValue().SubString(0,-1).GetCStr()<<std::endl;
            

            return ASTGeneratorResult();
        }
        case TokenType::GroupingSymbol:{
            HBuffer& value = t.GetValue();

            std::cout << "Parsing grouping symbol at " << t.GetValue().SubString(0,-1).GetCStr() << " " << t.GetLineNumber() << ":" << t.GetColumnNumber()<<std::endl;
            if(t.GetValue() == '('){
                //Parenthesis Expression Or Argument lists
                m_At++;
                if(GetToken(m_At).GetValue() == ')'){
                    //Empty Expression
                    m_At++;
                    *output = Node(nullptr, nullptr, NodeType::EmptyExpression, 0);
                    return ASTGeneratorResult();
                }

                //Getting the main or first part of the expression
                ASTGeneratorResult result = ParseExpression(output, output, true);
                if(!result)return result;

                Token potentialOperator = GetToken(m_At);
                TokenType potentialType = potentialOperator.GetType();

                bool isList = false;
                bool isArgumentList = false;
                bool continueLoop = potentialType == TokenType::ArithmeticOperator || potentialOperator.GetValue() == ',';

                Node* lastOutput = nullptr;
                while(continueLoop){
                    m_At++;

                    std::cout << "TEsting " << GetToken(m_At).GetValue().SubString(0,-1).GetCStr()<<std::endl;
                    if(potentialOperator.GetValue() == ','){
                        if(!isList){
                            /// @brief Convert output node to a list treating the current output as the first argument
                            std::vector<Node*> children;
                            children.emplace_back(m_NodePool.Allocate(std::move(*output)));
                            *output = Node(std::move(children), static_cast<void*>(nullptr), static_cast<void*>(nullptr), isArgumentList ? NodeType::ArgumentList : NodeType::ParameterList, 0);
                        }
                        isList = true;
                        continueLoop = true;
                    }
                    Node* newExpression = m_NodePool.Allocate();
                    uint8_t metadata = potentialOperator.GetMetadata();
                    bool allowedArithmeticOperations = metadata != (uint8_t)ArithmeticOperations::Multiplication && metadata != (uint8_t)ArithmeticOperations::Division;
                    std::cout << (allowedArithmeticOperations ? "Allowed arithmetic" : "Not allowed")<<std::endl;
                    std::cout << "Before is " <<GetToken(m_At).GetValue().SubString(0,-1).GetCStr()<<std::endl;
                    result = ParseExpression(newExpression, newExpression, allowedArithmeticOperations);
                    std::cout << "After at is " << GetToken(m_At).GetValue().SubString(0,-1).GetCStr()<<std::endl;
                    potentialOperator = GetToken(m_At);
                    potentialType = potentialOperator.GetType();
                    
                    //Must have valid expression followed by operator or seperator
                    if(!result)return result;

                    if(!isList)
                        *output = std::move(Node(m_NodePool.Allocate(std::move(*output)), newExpression, NodeType::ArithmeticExpression, metadata));
                    else{
                        isArgumentList |= newExpression->GetNodeType() != NodeType::IdentifierExpression;
                        output->m_Children.emplace_back(std::move(newExpression));
                    }
                    
                    continueLoop = potentialType == TokenType::ArithmeticOperator || potentialOperator.GetValue() == ',';
                };
                if(potentialOperator.GetValue() != ')'){
                    //Invalid End of list or expression
                    std::cout << "Invalid data : " << potentialOperator.GetValue().SubString(0,-1).GetCStr()<<std::endl;
                    return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidExpressionGrouping);
                }
                m_At++;

                /// ReAssign node type incase we detect a change from parameter to arguments
                if(isList)output->m_Type = isArgumentList ? NodeType::ArgumentList : NodeType::ParameterList;

                potentialOperator = GetToken(m_At);
                potentialType = potentialOperator.GetType();

                NodeType outputType = output->GetNodeType();

                if(potentialType == TokenType::UnaryOperator){
                    if(outputType != NodeType::IdentifierExpression){
                        return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidUnaryExpression);
                    }
                    m_At++;
                    //Node is a identifier expression so we consume its left
                    void* oldIdentifier = output->GetLeft();
                    *output = Node(oldIdentifier, static_cast<void*>(nullptr), NodeType::UnaryExpression, potentialOperator.GetMetadata());
                    potentialOperator = GetToken(m_At);
                    potentialType = potentialOperator.GetType();
                }

                //Are normal arithmetic expressions allowed on the current node?
                bool isArithmeticAllowed = true;//(outputType == NodeType::IdentifierExpression || outputType == NodeType::ArithmeticExpression || outputType == NodeType::UnaryExpression || outputType == NodeType::NumericalLiteralExpression);

                if(allowArithmeticOperations && isArithmeticAllowed && potentialType == TokenType::ArithmeticOperator){
                    //Check for () followed by operator
                    bool shouldContinue;

                    do{
                        m_At++;
                        std::cout << "Doing test1"<<std::endl;
                        Node* newExpression = m_NodePool.Allocate(nullptr, nullptr, NodeType::ArithmeticExpression, 0);
                        uint8_t potentialMetadata = potentialOperator.GetMetadata();
                        bool isDivisionOrMultiplication = potentialMetadata == (uint8_t)ArithmeticOperations::Multiplication || potentialMetadata == (uint8_t)ArithmeticOperations::Division;

                        ASTGeneratorResult result = ParseExpression(newExpression, newExpression, !isDivisionOrMultiplication);
                        if(!result)return result;
                        *output = Node(m_NodePool.Allocate(std::move(*output)), newExpression, NodeType::ArithmeticExpression, potentialMetadata);

                        potentialOperator = GetToken(m_At);
                        potentialType = potentialOperator.GetType();
                        shouldContinue = potentialType == TokenType::ArithmeticOperator;
                    }while(shouldContinue);
                }

                return ASTGeneratorResult();
            }

            return ASTGeneratorResult(m_At, ASTGeneratorError::ExpectedExpressionOrStatement);
        }
        case TokenType::NumericalLiteral:{
            size_t numericalLiteralAt = m_At;

            //Normal Numerical Literal
            Token& inputToken = m_Input->at(m_At);
            output->m_Left = static_cast<void*>(&inputToken.GetValue());
            output->m_Right = static_cast<void*>(nullptr);
            output->m_Type = NodeType::NumericalLiteralExpression;
            output->m_Metadata = inputToken.GetMetadata();
            
            //Check for operators
            Token potentialOperator = GetToken(m_At + 1);
            TokenType potentialType = potentialOperator.GetType();

            if(potentialType == TokenType::ArithmeticOperator && allowArithmeticOperations){
                //Arithmetic Operation
                m_At++;

                Node* newNode = nullptr;
                bool requiresExpression = true;
                ASTGeneratorResult result = ASTGeneratorResult();

                //Ive never made a arithmetic expression parser by myself before
                do{
                    m_At++;
                    newNode = m_NodePool.Allocate(nullptr, nullptr, NodeType::ArithmeticExpression, 0);
                    uint8_t potentialMetadata = potentialOperator.GetMetadata();
                    bool isDivisionOrMultiplication = potentialMetadata == (uint8_t)ArithmeticOperations::Multiplication || potentialMetadata == (uint8_t)ArithmeticOperations::Division;

                    result = ParseExpression(newNode, newNode, !isDivisionOrMultiplication);
                    if(!result)return result;
                    
                    *output = Node(m_NodePool.Allocate(std::move(*output)), newNode, NodeType::ArithmeticExpression, potentialMetadata);

                    potentialOperator = GetToken(m_At);
                    potentialType = potentialOperator.GetType();
                    requiresExpression = potentialType == TokenType::ArithmeticOperator;
                }while(requiresExpression);
                return ASTGeneratorResult();
            }
            else if(potentialType == TokenType::TernaryOperator){
                //Ternary Operator
                // exp ? true : false
                return ASTGeneratorResult();
            }
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
                Token next = GetToken(m_At + 1);
                if(next.GetValue() == '('){
                    //Method Declaration Or call
                    size_t parenthesisAt = ++m_At;
                    Node* passed = m_NodePool.Allocate();
                    ASTGeneratorResult result = ParseExpression(passed, passed);
                    if(!result){}

                    NodeType passedType = passed->GetNodeType();
                    Token current = GetToken(m_At);
                    if(current.GetValue() == '{'){
                        //Is a function
                        m_At++;

                        //Parse function body
                        std::vector<Node*> body;
                        ASTGeneratorResult result;
                        std::cout << "BEFORE ALL "<<std::endl;
                        while(true){
                            result = ParseCurrentToken(body);
                            if(!result)break;
                        }

                        if(GetToken(m_At).GetValue() != '}'){
                            if(!result){
                                return result;
                            }
                            return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidMethodBody);
                        }

                        m_At++;
                        HBuffer& buffer = GetToken(m_At).GetValue();
                        std::cout << (void*)buffer.GetData()<< " " << buffer.GetSize()<<std::endl;
                        std::cout << "CURRENT 2 IS "<< buffer.SubString(0,-1).GetCStr()<<std::endl;

                        std::cout <<"AFTER"<<std::endl;
                        
                        if(passedType != NodeType::IdentifierExpression && passedType != NodeType::ParameterList)
                            return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidParameterList);

                        /// @brief Function Identifier
                        void* left = &AtToken(startAt).GetValue();
                        /// @brief Function Parameters
                        uint8_t metadata = 0;
                        Node* methodDeclaration = m_NodePool.Allocate(std::move(body), left, static_cast<void*>(passed), NodeType::MethodDeclaration, metadata);
                        output.push_back(methodDeclaration);
                        return ASTGeneratorResult();
                    }

                    //Function call
                    //void* left = static_cast<void*>(m_NodePool.Allocate(static_cast<void*>(&m_Input->at(startAt).GetValue()), static_cast<void*>(nullptr), NodeType::IdentifierExpression, 0));
                    void* left = static_cast<void*>(&m_Input->at(startAt).GetValue());
                    uint8_t metadata = 0;
                    Node* functionCall = m_NodePool.Allocate(left, static_cast<void*>(passed), NodeType::FunctionCall, metadata);
                    output.push_back(functionCall);

                    //Check semicolon
                    if(GetToken(m_At).GetValue() == ';'){
                        m_At++;
                    }
                    Token after = GetToken(m_At);
                    return ASTGeneratorResult();

                    //delete allocatedParamatersOrArguments;
                    //return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidMethodBody);
                }

                Node* node = m_NodePool.Allocate();
                ASTGeneratorResult result = ParseExpression(node, node);
                if(result){
                    output.emplace_back(node);
                }

                return result;
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
                        if(next.GetValue() != '{')
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
                        if(t.GetValue() != '}'){
                            return result.m_ErrorCode != ASTGeneratorError::UnsupportedSyntax ? result : ASTGeneratorResult(m_At, ASTGeneratorError::InvalidClassDefinition);
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
                        return ASTGeneratorResult(m_At, ASTGeneratorError::UnsupportedSyntax);
                    }
                }
            }
        default:{
            Node* node = m_NodePool.Allocate();
            ASTGeneratorResult result = ParseExpression(node, node);

            if(result)
                output.push_back(node);
            return result;
            
        }
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