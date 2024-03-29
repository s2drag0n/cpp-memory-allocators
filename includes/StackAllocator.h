#pragma once

#include "Allocator.h"
#include <cstddef>

class StackAllocator : public Allocator {
  protected:
    void *m_start_ptr = nullptr;
    std::size_t m_offset;

  public:
    StackAllocator(const std::size_t totalSize);

    virtual ~StackAllocator();

    virtual void *Allocate(const std::size_t size,
                           const std::size_t alignment = 0) override;

    virtual void Free(void *ptr) override;

    virtual void Init() override;

    virtual void Reset();

  private:
    StackAllocator(StackAllocator &stackAllocator);

    struct AllocatorHeader {
        std::size_t padding;
    };
};
