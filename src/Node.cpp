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
        CleanUp();
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

    void Node::CleanUp()noexcept{
        switch(m_Type){

        }
        
        m_Children.clear();
        m_Metadata = 0;
        m_Left = nullptr;
        m_Right = nullptr;
        m_Type = NodeType::None;
    }
}