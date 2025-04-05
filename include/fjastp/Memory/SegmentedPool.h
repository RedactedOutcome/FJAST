#pragma once

#ifdef FJASTP_USE_PCH
#include FJASTP_PCH_DIR
#else
#include <array>
#include <vector>
#include <memory>
#endif

namespace FJASTP{
    ///@brief a class that is basically a vector of memory segments where each memory segment holds a chunk of nodes

    ///@brief a pool of arrays where each array is a fixed size of type T. The way we "allocate" is by using a index into these preallocated arrays. If we need more memory we just allocate a new array and reallocate our list of vectors. This way we dont need to change old pointers and we dont need to reserve a lot of unnecessary nodes.
    ///@brief
    template<typename T, size_t PoolSize=1024>
    class SegmentedPool{
    public:
        SegmentedPool()noexcept{
            m_Pool.reserve(5);
            m_Pool.push_back(new std::array<T, PoolSize>());
            memset(&m_Pool[0]->at(0), 0, sizeof(T) * sizeof(PoolSize));
        }
        ~SegmentedPool()noexcept{
            for(size_t i = 0; i < m_Pool.size(); i++){
                delete m_Pool[i];
            }
        }
        
        T* Allocate() noexcept{
            if(m_PoolIndex >= PoolSize){
                m_PoolIndex = 0;
                m_Pool.push_back(new std::array<T, PoolSize>());
                memset(&m_Pool[0]->at(0), 0, sizeof(T) * sizeof(PoolSize));
            }
            return &(m_Pool[m_Pool.size() - 1]->at(m_PoolIndex++));
        }
        template <typename... Args>
        T* Allocate(Args&&... args) noexcept{
            if(m_PoolIndex >= PoolSize){
                m_PoolIndex = 0;
                m_Pool.push_back(new std::array<T, PoolSize>());
                memset(&m_Pool[0]->at(0), 0, sizeof(T) * sizeof(PoolSize));
            }
            T* t = &(m_Pool[m_Pool.size() - 1]->at(m_PoolIndex++));
            *t = std::move(T(std::forward<Args>(args)...));
            return t;
        }

        size_t GetPoolCount()const noexcept{return m_Pool.size();}
        size_t GetPoolCapacity()const noexcept{return m_Pool.capacity();}

        /// @brief gets the current index that we are using like a stack pointer into the current pool
        size_t GetPoolIndex()const noexcept{return m_PoolIndex;}
    private:
        std::vector<std::array<T, PoolSize>*> m_Pool;

        /// @brief the index inside the current segment pool that we are at. Think of this like a stack pointer except its not.
        size_t m_PoolIndex=0;
    };
}