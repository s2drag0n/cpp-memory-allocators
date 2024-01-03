#include "../includes/CAllocator.h"
#include <cstdlib>

CAllocator::CAllocator() : Allocator(0) {}

CAllocator::~CAllocator() {}

void *CAllocator::Allocate(std::size_t size, std::size_t alignment) {
    return malloc(size);
}

void CAllocator::Free(void *ptr) { free(ptr); }

void CAllocator::Init() {}
