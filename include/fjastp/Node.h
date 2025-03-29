#pragma once

#ifdef FJASTP_USE_PCH
#include FJASTP_PCH_DIR
#else
#include <vector>
#include <cstdint>
#endif

#include "fjastp.h"

namespace FJASTP{
    class Node{
    public:
        friend class ASTGenerator;
        Node() noexcept;
        Node(std::vector<Node*>&& children, void* left, void* right, NodeType type, uint8_t metadata)noexcept;
        Node(void* left, void* right, NodeType type, uint8_t metadata)noexcept;
        Node(NodeType type)noexcept: m_Type(type){}
        Node(const Node& node) = delete;
        Node(Node&& node)noexcept;

        //Automatically handles freeing memory
        void CleanUp()noexcept;
    public:
        std::vector<Node*>& GetChildren() const noexcept{return (std::vector<Node*>&)m_Children;}
        void* GetLeft() const noexcept{return m_Left;}
        void* GetRight() const noexcept{return m_Right;}
        NodeType GetNodeType() const noexcept{return m_Type;}
        uint8_t GetMetadata() const noexcept{return m_Metadata;}
    public:
        Node& operator=(const Node& node) = delete;
        Node& operator=(Node&&) noexcept;
    private:
        std::vector<Node*> m_Children;
        void* m_Left = nullptr;
        void* m_Right = nullptr;
        NodeType m_Type = NodeType::None;
        uint8_t m_Metadata = 0;
    };
}