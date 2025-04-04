#pragma once


#include "Node.h"
#include "Token.h"
#include "Memory/SegmentedPool.h"

namespace FJASTP{
    struct ASTGeneratorResult{
        uint32_t m_ErrorAt; /// @brief the token the current error is at if any
        ASTGeneratorError m_ErrorCode;
        HBuffer m_ErrorReason;
        
        ASTGeneratorResult()noexcept: m_ErrorAt(0), m_ErrorCode(ASTGeneratorError::Success){}
        ASTGeneratorResult(uint32_t errorAt, ASTGeneratorError errorCode)noexcept: m_ErrorAt(errorAt), m_ErrorCode(errorCode){}
        ASTGeneratorResult(ASTGeneratorError errorCode)noexcept: m_ErrorCode(errorCode){}

        uint32_t GetErrorAt() const noexcept{return m_ErrorAt;}
        ASTGeneratorError GetErrorCode() const noexcept{return m_ErrorCode;}
        
        constexpr operator bool() const noexcept{
            return m_ErrorCode == ASTGeneratorError::Success;
        }
    };
    
    /// @brief Does a Syntactic Analysis and generates ast. Only does slight semantic analysis
    class ASTGenerator{
    public:
        /// @brief Generates an abstract syntax tree from a list of tokens.
        /// @return Returns type of ASTGeneratorResult.
        ASTGeneratorResult Generate(std::vector<Token>& input, std::vector<Node*>& output) noexcept;

        Token GetToken(size_t at) const noexcept{if(at < 0 || at >= m_TokenCount)return Token(); return m_Input->at(at);}
        Token& AtToken(size_t at) const noexcept{return (Token&)m_Input[at];}
        /// @brief Allocates a new node on the node pool and returns its address
        /// @brief We allocate in pool so nodes are grouped close together. We also have a custom allocater
        Node* AllocateNode()const noexcept;
        SegmentedPool<Node>& GetNodePool() const noexcept{return (SegmentedPool<Node>&)m_NodePool;}
    private:
        /// @brief Parses an expression into the output node.
        /// @param output. The original output node to parse the expression into
        /// @param replaceNode a node to keep track of the first expression output to replace its data so we can easily differentiate between nodes like property access and function call with property access expressions and nest expressions.
        /// @param allowArithmeticOperations used to easily do operator precedence
        ASTGeneratorResult ParseExpression(Node* output, Node* replaceNode, bool allowArithmeticOperations = true)noexcept;
        
        ASTGeneratorResult ParseType(Node* output)noexcept;

        ASTGeneratorResult ParseCurrentToken(std::vector<Node*>& output)noexcept;
    private:
        std::vector<Token>* m_Input;
        std::vector<Node*>* m_Output;
        SegmentedPool<Node> m_NodePool;
        uint32_t m_TokenCount;
        uint32_t m_At;
    };
}