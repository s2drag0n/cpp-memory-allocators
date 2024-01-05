#include "../includes/FreeListAllocator.h"
#include "../includes/Utils.h"
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <limits>

FreeListAllocator::FreeListAllocator(const std::size_t totalSize,
                                     const PlacementPolicy pPolicy)
    : Allocator(totalSize) {
    m_pPolicy = pPolicy;
}

void FreeListAllocator::Init() {
    if (m_start_ptr != nullptr) {
        free(m_start_ptr);
        m_start_ptr = nullptr;
    }
    m_start_ptr = malloc(m_totalSize);
    this->Reset();
}

FreeListAllocator::~FreeListAllocator() {
    free(m_start_ptr);
    m_start_ptr = nullptr;
}

void *FreeListAllocator::Allocate(const std::size_t size,
                                  const std::size_t alignment) {
    const std::size_t allocationHeaderSize =
        sizeof(FreeListAllocator::AllocationHeader);
    const std::size_t freeHeaderSize = sizeof(FreeListAllocator::FreeHeader);
    assert("Allocation size must be bigger" && size >= sizeof(Node));
    assert("Alignment must be 8 at least" && alignment >= 8);

    // search free list for a free block that has enough space
    std::size_t padding;
    Node *affectedNode, *previousNode;
    this->Find(size, alignment, padding, previousNode, affectedNode);
    assert(affectedNode != nullptr && "not enough memory");

    const std::size_t alignmentPadding = padding - allocationHeaderSize;
    const std::size_t requiredSize = size + padding;

    const std::size_t rest = affectedNode->data.blockSize - requiredSize;

    if (rest > 0) {
        Node *newFreeNode = (Node *)((std::size_t)affectedNode + requiredSize);
        newFreeNode->data.blockSize = rest;
        m_freeList.insert(affectedNode, newFreeNode);
    }

    m_freeList.remove(previousNode, affectedNode);

    const std::size_t headerAddress =
        (std::size_t)affectedNode + alignmentPadding;

    const std::size_t dataAddress = headerAddress + allocationHeaderSize;
    ((FreeListAllocator::AllocationHeader *)headerAddress)->blockSize =
        requiredSize;
    ((FreeListAllocator::AllocationHeader *)headerAddress)->padding =
        alignmentPadding;

    m_used += requiredSize;
    m_peak = std::max(m_peak, m_used);

#ifdef _DEBUG
    std::cout
        << "A"
        << "\t@H " << (void *)headerAddress << "\tD@ " << (void *)dataAddress
        << "\tS "
        << ((FreeListAllocator::AllocationHeader *)headerAddress)->blockSize
        << "\tAP " << alignmentPadding << "\tP " << padding << "\tM " << m_used
        << "\tR " << rest << std::endl;
#endif

    return (void *)dataAddress;
}

void FreeListAllocator::Free(void *ptr) {
    const std::size_t currentAddress = (std::size_t)ptr;
    const std::size_t headerAddress =
        currentAddress - sizeof(FreeListAllocator::AllocationHeader);
    const FreeListAllocator::AllocationHeader *allocationHeader =
        (FreeListAllocator::AllocationHeader *)headerAddress;

    Node *freeNode = (Node *)(headerAddress - allocationHeader->padding);

    freeNode->data.blockSize = allocationHeader->blockSize;
    freeNode->next = nullptr;

    Node *it = m_freeList.head;
    Node *itPrev = nullptr;
    while (it != nullptr) {
        if (ptr < it) {
            m_freeList.insert(itPrev, freeNode);
            break;
        }
        itPrev = it;
        it = it->next;
    }
    m_used -= freeNode->data.blockSize;

    // Merge contiguous nodes
    Coalescence(itPrev, freeNode);

#ifdef _DEBUG
    std::cout << "F"
              << "\t@ptr " << ptr << "\tH@ " << (void *)freeNode << "\tS "
              << freeNode->data.blockSize << "\tM " << m_used << std::endl;
#endif
}

void FreeListAllocator::Reset() {
    m_used = 0;
    m_peak = 0;
    Node *firstNode = (Node *)m_start_ptr;
    firstNode->data.blockSize = m_totalSize;
    firstNode->next = nullptr;
    m_freeList.head = nullptr;
    m_freeList.insert(nullptr, firstNode);
}

void FreeListAllocator::Coalescence(Node *prevBlock, Node *freeBlock) {
    if (freeBlock->next != nullptr &&
        (std::size_t)freeBlock + freeBlock->data.blockSize ==
            (std::size_t)freeBlock->next) {
        freeBlock->data.blockSize += freeBlock->next->data.blockSize;
        m_freeList.remove(freeBlock, freeBlock->next);

#ifdef _DEBUG
        std::cout << "\tMerging(n) " << (void *)freeNode << " & "
                  << (void *)freeNode->next << "\tS "
                  << freeNode->data.blockSize << std::endl;
#endif
    }
    if (prevBlock != nullptr &&
        (std::size_t)prevBlock + prevBlock->data.blockSize ==
            (std::size_t)freeBlock) {
        prevBlock->data.blockSize += freeBlock->data.blockSize;
        m_freeList.remove(prevBlock, freeBlock);

#ifdef _DEBUG
        std::cout << "\tMerging(p) " << (void *)previousNode << " & "
                  << (void *)freeNode << "\tS " << previousNode->data.blockSize
                  << std::endl;
#endif
    }
}
void FreeListAllocator::Find(const std::size_t size,
                             const std::size_t alignment, std::size_t &padding,
                             Node *&previousNode, Node *&foundNode) {
    switch (m_pPolicy) {
    case FIND_FIRST:
        FindFirst(size, alignment, padding, previousNode, foundNode);
        break;
    case FIND_BEST:
        FindBest(size, alignment, padding, previousNode, foundNode);
        break;
    }
}

void FreeListAllocator::FindBest(const std::size_t size,
                                 const std::size_t alignment,
                                 std::size_t &padding, Node *&previousNode,
                                 Node *&foundNode) {
    std::size_t smallestDiff = std::numeric_limits<std::size_t>::max();
    Node *bestBlock = nullptr, *bestPrevBlock = nullptr;
    Node *it = m_freeList.head, *itPrev = nullptr;
    while (it != nullptr) {
        padding = Utils::CalculatePaddingWithHeader(
            (std::size_t)it, alignment,
            sizeof(FreeListAllocator::AllocationHeader));
        const std::size_t requiredSpace = size + padding;
        if (it->data.blockSize >= requiredSpace &&
            (it->data.blockSize < smallestDiff)) {
            bestBlock = it;
            bestPrevBlock = itPrev;
            smallestDiff = it->data.blockSize;
        }
        itPrev = it;
        it = it->next;
    }
    previousNode = bestPrevBlock;
    foundNode = bestBlock;
}

void FreeListAllocator::FindFirst(const std::size_t size,
                                  const std::size_t alignment,
                                  std::size_t &padding, Node *&previousNode,
                                  Node *&foundNode) {
    Node *it = m_freeList.head, *itPrev = nullptr;
    while (it != nullptr) {
        padding = Utils::CalculatePaddingWithHeader(
            (std::size_t)it, alignment,
            sizeof(FreeListAllocator::AllocationHeader));
        const std::size_t requiredSize = size + padding;
        if (it->data.blockSize >= requiredSize) {
            break;
        }
        itPrev = it;
        it = it->next;
    }
    previousNode = itPrev;
    foundNode = it;
}
