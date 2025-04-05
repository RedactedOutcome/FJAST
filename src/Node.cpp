#include "pch.h"
#include "Node.h"

namespace FJASTP{
    Node::Node() noexcept{}
    Node::Node(std::vector<Node*>&& children, void* left, void* right, NodeType type, uint8_t metadata)noexcept{
        m_Children = std::move(children);
        m_Type = type;
        m_Metadata = metadata;
        m_Left = left;
        m_Right = right;
    }
    Node::Node(void* left, void* right, NodeType type, uint8_t metadata)noexcept{
        m_Left = left;
        m_Right = right;
        m_Type = type;
        m_Metadata = metadata;
    }
    Node::Node(Node&& node)noexcept{
        m_Children = std::move(node.m_Children);
        m_Type = node.m_Type;
        m_Metadata = node.m_Metadata;
        m_Left = node.m_Left;
        m_Right = node.m_Right;
        
        node.m_Left = nullptr;
        node.m_Right = nullptr;
        node.m_Metadata = 0;
        node.m_Type = NodeType::None;
    }

    Node& Node::operator=(Node&& node) noexcept{
        m_Children = std::move(node.m_Children);
        m_Type = node.m_Type;
        m_Metadata = node.m_Metadata;
        m_Left = node.m_Left;
        m_Right = node.m_Right;
        
        node.m_Left = nullptr;
        node.m_Right = nullptr;
        node.m_Metadata = 0;
        node.m_Type = NodeType::None;
        return *this;
    }

    Node::~Node()noexcept{
        /// @brief no need to do anything except since we are deconstructing and children deallocator will auto be called
    }

    void Node::CleanUp()noexcept{
        m_Children.clear();
        m_Metadata = 0;
        m_Left = nullptr;
        m_Right = nullptr;
        m_Type = NodeType::None;
    }

    void Node::Print(int padStart)noexcept{
        //std::cout << std::endl;
        switch((uint8_t)m_Type){
            case (uint8_t)NodeType::ClassDeclaration:{
                std::vector<Node*>& children = m_Children;
                std::cout << std::string(padStart, ' ') << "Class child count is " << children.size()<<std::endl;
                for(size_t d = 0; d < children.size(); d++){
                    std::cout << std::string(padStart, ' ') << "Child Node Node Type : " << (int)children[d]->GetNodeType()<<std::endl;
                    //PrintNode(*children[d], padStart + 2);
                    static_cast<Node*>(children[d])->Print(padStart + 2);
                }
                break;
            }
            case (uint8_t)NodeType::MethodDeclaration:{
                std::vector<Node*>& body = m_Children;
                std::cout << std::string(padStart, ' ') << "Method Body Count is " << (int)body.size()<<std::endl;
                for(size_t d = 0; d < body.size(); d++){
                    std::cout << std::string(padStart, ' ') << "Body Node is : " << (int)body[d]->GetNodeType()<<std::endl;
                    static_cast<Node*>(body[d])->Print(padStart + 2);
                }
                break;
            }
            case (uint8_t)NodeType::FunctionCall:{
                std::cout << std::string(padStart, ' ') << "Function Call" <<std::endl;
                std::cout << std::string(padStart, ' ') << "Function Name " << static_cast<HBuffer*>(m_Left)->SubString(0,-1).GetCStr()<<std::endl;
                static_cast<Node*>(m_Right)->Print(padStart);
                break;
            }
            case (uint8_t)NodeType::MethodCall:{
                std::cout << std::string(padStart, ' ') << "Method Call" <<std::endl;
                static_cast<Node*>(m_Left)->Print(padStart);
                static_cast<Node*>(m_Right)->Print(padStart);
                break;
            }
            case (uint8_t)NodeType::ParameterList:
            case (uint8_t)NodeType::ArgumentList:{
                size_t listCount = m_Children.size();
                std::cout << std::string(padStart, ' ') << "Argument/Parameter List with " << listCount << " Nodes"<<std::endl;
                for(size_t i = 0; i < listCount; i++)
                    m_Children[i]->Print(padStart + 2);
                break;
            }
            case (uint8_t)NodeType::IdentifierExpression:{
                std::cout << std::string(padStart, ' ') << "Identifier Expression : " << static_cast<HBuffer*>(m_Left)->SubString(0,-1).GetCStr()<<std::endl;
                break;
            }
            case (uint8_t)NodeType::ArithmeticExpression:{
                std::cout << std::string(padStart, ' ') << "Arithmetic Expression" <<std::endl;
                std::cout << std::string(padStart, ' ') << "Left Side" <<std::endl;
                static_cast<Node*>(m_Left)->Print(padStart + 2);
                uint8_t metadata = (uint8_t)GetMetadata();
                const char* op = "";
                switch(metadata){
                    case (uint8_t)ArithmeticOperations::Subtraction:{
                        op = "Subtraction";
                        break;
                    }
                    case (uint8_t)ArithmeticOperations::Addition:{
                        op = "Addition";
                        break;
                    }
                    case (uint8_t)ArithmeticOperations::Multiplication:{
                        op = "Multiplication";
                        break;
                    }
                    case (uint8_t)ArithmeticOperations::Division:{
                        op = "Division";
                        break;
                    }
                }
                std::cout << std::string(padStart, ' ') << "Arithmetic Operator " << op << std::endl;
                std::cout << std::string(padStart, ' ') << "Right Side" <<std::endl;
                static_cast<Node*>(m_Right)->Print(padStart + 2);
                break;
            }
            case (uint8_t)NodeType::PropertyAccessExpression:{
                std::cout << std::string(padStart, ' ') << "PropertyAccessExpression" <<std::endl;
                static_cast<Node*>(m_Left)->Print(padStart + 2);
                static_cast<Node*>(m_Right)->Print(padStart + 2);
                break;
            }
            case (uint8_t)NodeType::UnaryExpression:{
                std::cout << std::string(padStart, ' ') << "Unary Expression" <<std::endl;
                std::cout << std::string(padStart, ' ') << "Unary Expression On : " << static_cast<HBuffer*>(m_Left)->SubString(0,-1).GetCStr() <<std::endl;
                std::cout << std::string(padStart, ' ') << "Operator : " << (m_Metadata == (uint8_t)ArithmeticOperations::Increment ? "increment" : "decrement")<<std::endl;
                break;
            }
            case (uint8_t)NodeType::NumericalLiteralExpression:{
                std::cout << std::string(padStart, ' ') << "Numerical Literal : " << static_cast<HBuffer*>(m_Left)->SubString(0,-1).GetCStr() << std::endl;
                break;
            }
            case (uint8_t)NodeType::AssignmentExpression:{
                const char* type = "";
                switch(m_Metadata){
                    case (uint8_t)AssignmentOperator::Assignment:
                        type = "=";
                        break;
                    case (uint8_t)AssignmentOperator::MultiplicationAssignment:
                        type = "*=";
                        break;
                    case (uint8_t)AssignmentOperator::DivisionAssignment:
                        type = "/=";
                        break;
                    case (uint8_t)AssignmentOperator::ModulosAssignment:
                        type = "%=";
                        break;
                    case (uint8_t)AssignmentOperator::AdditionAssignment:
                        type = "+=";
                        break;
                    case (uint8_t)AssignmentOperator::SubtractionAssignment:
                        type = "-=";
                        break;
                }
                std::cout << std::string(padStart, ' ') << "Assignment Expression : " << type << std::endl;
                std::cout << std::string(padStart, ' ') << "Left Expression : " << std::endl;
                static_cast<Node*>(m_Left)->Print(padStart + 2);
                std::cout << std::string(padStart, ' ') << "Right Expression : " << std::endl;
                static_cast<Node*>(m_Right)->Print(padStart + 2);
            }
        default:
            std::cout << std::string(padStart, ' ') << "Dont know how to print node : " << (int)m_Type << std::endl;
        }
    }
}