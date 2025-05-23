#include "pch.h"
#include "ASTGenerator.h"

namespace FJASTP{
    ASTGeneratorResult ASTGenerator::ParseVariableDeclaration(VariableDeclarationType type, Node* currentOutput) noexcept{
        m_At++;

        Node identifier(static_cast<void*>(&m_Input->at(m_At).GetValue()), static_cast<void*>(nullptr), NodeType::IdentifierExpression, 0);
        *currentOutput = Node(static_cast<void*>(m_NodePool.Allocate(std::move(identifier))), static_cast<void*>(nullptr), NodeType::VariableDeclaration, static_cast<uint8_t>(type));

        Token currentToken = GetToken(++m_At);
        TokenType nextTokenType = currentToken.GetType();

        HBuffer& nextTokenValue = currentToken.GetValue();
        void* right = nullptr;

        if(nextTokenType == TokenType::AssignmentOperator){
            //Check for valid Assignment operator with variable declaration
            uint8_t assignmentOperator = currentToken.GetMetadata();
            if(assignmentOperator != (uint8_t)AssignmentOperator::Assignment){
                /// TODO: add error messages
                return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidVariableAssignment);
            }

            m_At++;
            Node* expression = m_NodePool.Allocate();
            ASTGeneratorResult result = ParseExpression(expression, expression);
            if(!result)return result;

            Node* newLeft = m_NodePool.Allocate(std::move(*currentOutput));
            *currentOutput = Node(static_cast<void*>(newLeft), static_cast<void*>(expression), NodeType::VariableAssignment, static_cast<uint8_t>(type));
            currentToken = GetToken(m_At);
        }

        if(currentToken.GetValue() == ';'){
            m_At++;
        }

        return ASTGeneratorResult();
    }
    
    ASTGeneratorResult ASTGenerator::ParseAssignmentExpression(Node* output, Token assignmentOperator) noexcept{
        m_At++;
        Node* right = m_NodePool.Allocate();
        ASTGeneratorResult result = ParseExpression(right, right);
        if(!result)return result;

        NodeType nodeType = right->GetNodeType();
        *output = Node(static_cast<void*>(m_NodePool.Allocate(std::move(*output))), static_cast<void*>(right), NodeType::AssignmentExpression, assignmentOperator.GetMetadata());
        if(GetToken(m_At).GetValue() == ';')
            m_At++;
        return ASTGeneratorResult();
    }
    ASTGeneratorResult ASTGenerator::ParseExpression(Node* output, Node* replaceNode, bool allowArithmeticOperations)noexcept{
        Token currentToken = GetToken(m_At);
        TokenType currentTokenType = currentToken.GetType();
        switch(currentTokenType){
        case TokenType::EndOfFile:
            return ASTGeneratorResult(ASTGeneratorError::EndOfFile);
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
                            if(!result)return result;
                            return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidClassDefinition);
                        }
                        m_At++;
                        void* left = &m_Input->at(identifierAt).GetValue();
                        void* right = nullptr;
                        uint8_t metadata = 0;

                        output->m_Children.emplace_back(m_NodePool.Allocate(std::move(body), left, right, NodeType::ClassDeclaration, metadata));
                        return ASTGeneratorResult();
                        break;
                    }
                    case (uint8_t)Keyword::Function:{
                        m_At++;
                        if(m_At >= m_Input->size())return ASTGeneratorResult(m_At, ASTGeneratorError::EndOfFile);
                        Token& identifier = m_Input->at(m_At);
                        
                        if(identifier.GetType() != TokenType::Identifier){
                            return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidFunctionDeclaration);
                        }
                        if(GetToken(++m_At).GetValue() != '('){
                            return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidParameterList);
                        }
                        uint32_t parametersAt = m_At;
                        Node* parameterList = m_NodePool.Allocate();
                        ASTGeneratorResult result = ParseExpression(parameterList, parameterList);
                        if(!result)return result;

                        NodeType parameterListType = parameterList->GetNodeType();
                        if(parameterListType != NodeType::EmptyExpression && parameterListType != NodeType::IdentifierExpression && parameterListType != NodeType::ParameterList)
                            return ASTGeneratorResult(parametersAt, ASTGeneratorError::InvalidParameterList);
                        if(GetToken(m_At).GetValue() != '{'){
                            return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidFunctionBody);
                        }
                        m_At++;

                        std::vector<Node*> body;
                        do{
                            result = ParseCurrentToken(body);
                        }while(result);

                        if(GetToken(m_At).GetValue() != '}'){
                            if(result != ASTGeneratorError::UnsupportedSyntax)
                                return result;
                            return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidFunctionEncapsulation);
                        }
                        m_At++;

                        Node* identifierExpression = m_NodePool.Allocate(static_cast<void*>(&identifier.GetValue()), static_cast<void*>(nullptr), NodeType::IdentifierExpression, 0);
                        *output = Node(static_cast<void*>(identifierExpression), static_cast<void*>(parameterList), NodeType::FunctionDeclaration, 0);
                    }
                    default:{
                        return ASTGeneratorResult(m_At, ASTGeneratorError::UnsupportedSyntax);
                    }
                }
            }
        case TokenType::ArithmeticOperator:{
            if(currentToken.GetMetadata() == (uint8_t)ArithmeticOperations::Subtraction){
                //- operator
                uint32_t startAt = ++m_At;
                Node* exp = m_NodePool.Allocate();
                ASTGeneratorResult result = ParseExpression(exp, exp);
                if(!result)return result;

                NodeType nodeType = exp->GetNodeType();
                if(nodeType == NodeType::ArgumentList || nodeType == NodeType::ParameterList)
                    return ASTGeneratorResult(startAt, ASTGeneratorError::InvalidNegationExpression);

                *output = std::move(Node(static_cast<void*>(exp), nullptr, NodeType::Negation, 0));
            }
            return ASTGeneratorResult(m_At, ASTGeneratorError::UnsupportedSyntax);
        }
        case TokenType::Identifier:{
            uint32_t startIdentifier = m_At;
            Token nextToken = GetToken(m_At + 1);
            TokenType nextTokenType = nextToken.GetType();

            HBuffer nextValue = nextToken.GetValue();
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
                    nextToken = GetToken(m_At);
                }while(nextToken.GetValue() == '.');
                /// Confirm that new type is a valid property access
                NodeType type = output->GetNodeType();
                if(type != NodeType::PropertyAccessExpression)return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidPropertyAccessExpression);\

                //Check for tokens after for more kinds of syntactical analysis
                nextToken = GetToken(m_At);

                //TODO: check if parse argument list
                if(nextToken.GetValue() == '('){
                    //Is a method call
                    Node* arguments = m_NodePool.Allocate();
                    ASTGeneratorResult result = ParseExpression(arguments, arguments);
                    if(!result)return result;
                    ///Treating anything inside the parenthesis as arguments
                    ///(5 + 5) isnt a list but is a argument
                    Node* newLeft = m_NodePool.Allocate(std::move(*output));
                    uint8_t metadata = 0;
                    *replaceNode = Node(static_cast<void*>(newLeft), static_cast<void*>(arguments), NodeType::MethodCall, metadata);
                    m_At++;
                }
                else if(nextToken.GetType() == TokenType::AssignmentOperator){
                    ParseAssignmentExpression(output, nextToken);
                }
            }
            else if(nextValue == '('){
                m_At++;
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
            else if(nextTokenType == TokenType::AssignmentOperator){
                *output = Node(static_cast<void*>(&m_Input->at(m_At).GetValue()), static_cast<void*>(nullptr), NodeType::IdentifierExpression, 0);
                Token assignmentOperator = GetToken(++m_At);
                ASTGeneratorResult result = ParseAssignmentExpression(output, assignmentOperator);
                return result;
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
                *output = Node((void*)&m_Input->at(m_At).GetValue(), (void*)nullptr, NodeType::IdentifierExpression, 0);
                m_At++;
            }

            /// TODO: Check for operators
            Token potentialOperator = GetToken(m_At);
            TokenType potentialType = potentialOperator.GetType();

            bool continueLoop = potentialType == TokenType::ArithmeticOperator;
            while(continueLoop){
                m_At++;

                Node* newExpression = m_NodePool.Allocate();
                uint8_t metadata = potentialOperator.GetMetadata();
                ASTGeneratorResult result = ParseExpression(newExpression, newExpression, metadata != (uint8_t)ArithmeticOperations::Multiplication && metadata != (uint8_t)ArithmeticOperations::Division);
                potentialOperator = GetToken(m_At);
                potentialType = potentialOperator.GetType();
                if(!result)return result;

                *output = std::move(Node(m_NodePool.Allocate(std::move(*output)), newExpression, NodeType::ArithmeticExpression, metadata));
                
                continueLoop = potentialType == TokenType::ArithmeticOperator;
            };

            return ASTGeneratorResult();
        }
        case TokenType::GroupingSymbol:{
            HBuffer& value = currentToken.GetValue();

            if(currentToken.GetValue() == '('){
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
                    result = ParseExpression(newExpression, newExpression, allowedArithmeticOperations);
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
        case TokenType::StringLiteral:{
            *output = Node(static_cast<void*>(&m_Input->at(m_At).GetValue()), static_cast<void*>(nullptr), NodeType::StringLiteralExpression, 0);
            m_At++;
            return ASTGeneratorResult();
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

    ASTGeneratorResult ASTGenerator::ParseCurrentToken(std::vector<Node*>& outputNodes) noexcept{
        if(m_At >= m_TokenCount)return ASTGeneratorResult(m_At, ASTGeneratorError::EndOfFile);

        Token currentToken = GetToken(m_At);
        switch(currentToken.GetType()){
            case TokenType::EndOfFile:
                return ASTGeneratorResult(ASTGeneratorError::EndOfFile);
            case TokenType::Keyword:{
                switch(currentToken.GetMetadata()){
                    case (uint8_t)Keyword::Var:{
                        Token nextToken = GetToken(m_At + 1);
                        TokenType nextTokenType = nextToken.GetType();
                        
                        if(nextTokenType == TokenType::Identifier){
                            /// @brief Parse variable declaration may return a variable declaration or an assignment with a varaible declaration
                            Node* expression = m_NodePool.Allocate();
                            ASTGeneratorResult result = ParseVariableDeclaration(VariableDeclarationType::Var, expression);
                            if(!result)return result;
                            outputNodes.emplace_back(expression);
                            return ASTGeneratorResult();
                        }
                        return ASTGeneratorResult(m_At, ASTGeneratorError::UnsupportedSyntax);
                    }
                    case (uint8_t)Keyword::Let:{
                        Token nextToken = GetToken(m_At + 1);
                        TokenType nextTokenType = nextToken.GetType();
                        
                        if(nextTokenType == TokenType::Identifier){
                            /// @brief Parse variable declaration may return a variable declaration or an assignment with a varaible declaration
                            Node* expression = m_NodePool.Allocate();
                            ASTGeneratorResult result = ParseVariableDeclaration(VariableDeclarationType::Let, expression);
                            if(!result)return result;
                            outputNodes.emplace_back(expression);
                            return ASTGeneratorResult();
                        }
                        return ASTGeneratorResult(m_At, ASTGeneratorError::UnsupportedSyntax);
                    }
                    case (uint8_t)Keyword::Const:{
                        Token nextToken = GetToken(m_At + 1);
                        TokenType nextTokenType = nextToken.GetType();

                        if(nextTokenType == TokenType::Identifier){
                            /// @brief Parse variable declaration may return a variable declaration or an assignment with a varaible declaration
                            Node* expression = m_NodePool.Allocate();
                            ASTGeneratorResult result = ParseVariableDeclaration(VariableDeclarationType::Const, expression);
                            if(!result)return result;
                            outputNodes.emplace_back(expression);
                            return ASTGeneratorResult();
                        }
                        return ASTGeneratorResult(m_At, ASTGeneratorError::UnsupportedSyntax);
                    }
                    default:{
                        Node* expression = m_NodePool.Allocate();
                        ASTGeneratorResult result = ParseExpression(expression, expression);
                        if(!result)return result;
                        outputNodes.emplace_back(expression);
                        return ASTGeneratorResult();
                    }
                }
            }
            case TokenType::Identifier:{
                size_t startAt = m_At;
                Token nextToken = GetToken(m_At + 1);
                TokenType nextType = nextToken.GetType();

                if(nextToken.GetValue() == '('){
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
                        while(result){
                            result = ParseCurrentToken(body);
                        }

                        if(GetToken(m_At).GetValue() != '}'){
                            /// TODO: this wont get called right?
                            if(!result){
                                return result;
                            }
                            return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidMethodBody);
                        }

                        m_At++;
                        if(passedType != NodeType::IdentifierExpression && passedType != NodeType::ParameterList)
                            return ASTGeneratorResult(m_At, ASTGeneratorError::InvalidParameterList);

                        /// @brief Function Identifier
                        void* left = &AtToken(startAt).GetValue();
                        /// @brief Function Parameters
                        uint8_t metadata = 0;
                        Node* methodDeclaration = m_NodePool.Allocate(std::move(body), left, static_cast<void*>(passed), NodeType::MethodDeclaration, metadata);
                        outputNodes.push_back(methodDeclaration);
                        return ASTGeneratorResult();
                    }

                    //Function call
                    //void* left = static_cast<void*>(m_NodePool.Allocate(static_cast<void*>(&m_Input->at(startAt).GetValue()), static_cast<void*>(nullptr), NodeType::IdentifierExpression, 0));
                    void* left = static_cast<void*>(&m_Input->at(startAt).GetValue());
                    uint8_t metadata = 0;
                    Node* functionCall = m_NodePool.Allocate(left, static_cast<void*>(passed), NodeType::FunctionCall, metadata);
                    outputNodes.push_back(functionCall);

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
                    outputNodes.emplace_back(node);
                }

                return result;
            }
            
        default:{
            Node* node = m_NodePool.Allocate();
            ASTGeneratorResult result = ParseExpression(node, node);

            if(result)
                outputNodes.push_back(node);
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