#include "../includes/StackAllocator.h"
#include "../includes/Utils.h"
#include <algorithm>
#include <cstddef>
#include <cstdlib>

StackAllocator::StackAllocator(const std::size_t totalSize)
    : Allocator(totalSize) {}

StackAllocator::~StackAllocator() {
    free(m_start_ptr);
    m_start_ptr = nullptr;
}

void *StackAllocator::Allocate(const std::size_t size,
                               const std::size_t alignment) {
    const std::size_t currentAddress = (std::size_t)m_start_ptr + m_offset;

    std::size_t padding = 0;
    if (alignment == 0) {
        padding = sizeof(AllocatorHeader);
    } else {
        padding = Utils::CalculatePaddingWithHeader(currentAddress, alignment,
                                                    sizeof(AllocatorHeader));
    }

    if (m_offset + size + padding > m_totalSize) {
        return nullptr;
    }

    m_offset += padding;
    const std::size_t nextAddress = currentAddress + padding;
    const std::size_t headerAddress = nextAddress - sizeof(AllocatorHeader);
    AllocatorHeader allocatorHeader{padding};
    AllocatorHeader *headerPtr = (AllocatorHeader *)headerAddress;
    *headerPtr = allocatorHeader;

    m_offset += size;
#ifdef _DEBUG
    std::cout << "A"
              << "\t@C " << (void *)currentAddress << "\t@R "
              << (void *)nextAddress << "\tO " << m_offset << "\tP " << padding
              << std::endl;
#endif

    m_used = m_offset;
    m_peak = std::max(m_peak, m_used);

    return (void *)nextAddress;
}

void StackAllocator::Free(void *ptr) {
    const std::size_t currentAddress = (std::size_t)ptr;
    const std::size_t headerAddress = currentAddress - sizeof(AllocatorHeader);
    const AllocatorHeader *allocatorHeader = (AllocatorHeader *)headerAddress;

    m_offset =
        currentAddress - allocatorHeader->padding - (std::size_t)(m_start_ptr);
    m_used = m_offset;
}

void StackAllocator::Init() {
    if (m_start_ptr) {
        free(m_start_ptr);
    }
    m_start_ptr = malloc(m_totalSize);
    m_offset = 0;
}

void StackAllocator::Reset() {
    m_offset = 0;
    m_used = 0;
    m_peak = 0;
}
