#pragma once

#include "Allocator.h"
#include <cstddef>

class CAllocator : public Allocator {
  public:
    CAllocator();
    virtual ~CAllocator();
    virtual void *Allocate(std::size_t size, std::size_t alignment) override;
    virtual void Free(void *ptr) override;
    virtual void Init() override;
};
