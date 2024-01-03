#include "../includes/Benchmark.h"
#include "../includes/LinearAllocator.h"
#include <cstddef>
#include <iostream>

int main() {
    const std::size_t A = static_cast<std::size_t>(1e9);
    const std::size_t B = static_cast<std::size_t>(1e8);

    const std::vector<std::size_t> ALLOCATION_SIZES{32,   64,   256, 512,
                                                    1024, 2048, 4096};
    const std::vector<std::size_t> ALIGNMENTS{8, 8, 8, 8, 8, 8, 8};

    Allocator *linearAllocator = new LinearAllocator(A);

    Benchmark benchmark(OPERATIONS);

    std::cout << "LINEAR" << std::endl;
    benchmark.MultipleAllocation(linearAllocator, ALLOCATION_SIZES, ALIGNMENTS);
    benchmark.MultipleFree(linearAllocator, ALLOCATION_SIZES, ALIGNMENTS);
    benchmark.RandomAllocation(linearAllocator, ALLOCATION_SIZES, ALIGNMENTS);
    benchmark.RandomFree(linearAllocator, ALLOCATION_SIZES, ALIGNMENTS);

    delete linearAllocator;

    return 1;
}
