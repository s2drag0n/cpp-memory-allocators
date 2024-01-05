#include "../includes/PoolAllocator.h"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdlib>

#ifdef _DEBUG
#include <iostream>
#endif

PoolAllocator::PoolAllocator(const std::size_t totalSize,
                             const std::size_t chunkSize)
    : Allocator(totalSize) {
    assert(chunkSize >= 8 && "chunkSize must be greater or equal to 8");
    assert(totalSize % chunkSize == 0 &&
           "totalSize must be a mutiple of chunkSize");
    this->m_chunkSize = chunkSize;
}

void PoolAllocator::Init() {
    m_start_ptr = malloc(m_totalSize);
    this->ReSet();
}

PoolAllocator::~PoolAllocator() { free(m_start_ptr); }

void *PoolAllocator::Allocate(const std::size_t allocatorSize,
                              const std::size_t alignment) {
    assert(allocatorSize == m_chunkSize &&
           "allocatorSize must equal to m_chunkSize");
    Node *freePosition = m_freeList.pop();

    assert(freePosition != nullptr && "the pool allocator is full");

    m_used += m_chunkSize;
    m_peak = std::max(m_peak, m_used);

#ifdef _DEBUG
    std::cout << "A"
              << "\t@S " << m_start_ptr << "\t@R " << (void *)freePosition
              << "\tM " << m_used << std::endl;
#endif

    return (void *)freePosition;
}

void PoolAllocator::Free(void *ptr) {
    m_used -= m_chunkSize;
    m_freeList.push((Node *)ptr);

#ifdef _DEBUG
    std::cout << "F"
              << "\t@S " << m_start_ptr << "\t@F " << ptr << "\tM " << m_used
              << std::endl;
#endif // DEBUG
}

void PoolAllocator::ReSet() {
    m_used = 0;
    m_peak = 0;

    const int nChunks = m_totalSize / m_chunkSize;
    for (int i = 0; i < nChunks; ++i) {
        std::size_t address = (std::size_t)m_start_ptr + i * m_chunkSize;
        m_freeList.push((Node *)address);
    }
}
