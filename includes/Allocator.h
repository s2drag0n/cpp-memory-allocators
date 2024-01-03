#pragma once

#include <cstddef>
class Allocator {
  protected:
    std::size_t m_totalSize;
    std::size_t m_used;
    std::size_t m_peak;

  public:
    Allocator(const std::size_t totalSize)
        : m_totalSize{totalSize}, m_used{0}, m_peak{0} {}

    virtual ~Allocator() { m_totalSize = 0; }

    // size 代表这一次分配所需要的内存大小
    // alignment代表这一次分配需要内存对齐参数
    virtual void *Allocate(const std::size_t size,
                           const std::size_t alignment = 0) = 0;

    virtual void Free(void *ptr) = 0;

    virtual void Init() = 0;

    friend class Benchmark;
};
