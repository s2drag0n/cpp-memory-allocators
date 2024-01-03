#include "../includes/Benchmark.h"

#include <cassert>
#include <cstdlib>
#include <iostream>

void Benchmark::SingleAllocation(Allocator *allocator, const std::size_t size,
                                 const std::size_t alignment) {
    std::cout << "BENCHMARK: ALLOCATION" << IO::endl;
    std::cout << "\tSize     \t" << size << IO::endl;
    std::cout << "\tAlignment\t" << alignment << IO::endl;

    StartRound();

    allocator->Init();

    auto operations = 0u;

    while (operations < m_nOperations) {
        allocator->Allocate(size, alignment);
        ++operations;
    }

    FinishRound();

    BenchmarkResults results =
        buildResults(m_nOperations, std::move(TimeElapsed), allocator->m_peak);

    PrintResult(results);
}

void Benchmark::SingleFree(Allocator *allocator, const std::size_t size,
                           const std::size_t alignment) {
    std::cout << "BENCHMARK: ALLOCATION" << IO::endl;
    std::cout << "\tSize     \t" << size << IO::endl;
    std::cout << "\tAlignment\t" << alignment << IO::endl;

    void *addresses[OPERATIONS];

    StartRound();

    allocator->Init();

    auto operations = 0u;

    while (operations < m_nOperations) {
        addresses[operations] = allocator->Allocate(size, alignment);
        ++operations;
    }

    while (operations) {
        allocator->Free(addresses[--operations]);
    }

    FinishRound();

    BenchmarkResults results =
        buildResults(m_nOperations, std::move(TimeElapsed), allocator->m_peak);

    PrintResult(results);
}

void Benchmark::MultipleAllocation(
    Allocator *allocator, const std::vector<std::size_t> &allocationSizes,
    const std::vector<std::size_t> &alignments) {
    assert(allocationSizes.size() == alignments.size() &&
           "Allocation sizes and Alignments must have same length");
    for (auto i = 0u; i < allocationSizes.size(); ++i) {
        SingleAllocation(allocator, allocationSizes[i], alignments[i]);
    }
}

void Benchmark::MultipleFree(Allocator *allocator,
                             const std::vector<std::size_t> &allocationSizes,
                             const std::vector<std::size_t> &alignments) {
    assert(allocationSizes.size() == alignments.size() &&
           "Allocation sizes and Alignments must have same length");
    for (auto i = 0u; i < allocationSizes.size(); ++i) {
        SingleFree(allocator, allocationSizes[i], alignments[i]);
    }
}

void Benchmark::RandomAllocation(
    Allocator *allocator, const std::vector<std::size_t> &allocationSizes,
    const std::vector<std::size_t> &alignments) {
    srand(1);

    std::cout << "\tBENCHMARK: ALLOCATION" << IO::endl;

    StartRound();

    std::size_t allocation_size;
    std::size_t alignment;

    allocator->Init();

    auto operations = 0u;

    while (operations < m_nOperations) {
        this->RandomAllocationAttr(allocationSizes, alignments, allocation_size,
                                   alignment);
        allocator->Allocate(allocation_size, alignment);
        ++operations;
    }

    FinishRound();

    BenchmarkResults results =
        buildResults(m_nOperations, std::move(TimeElapsed), allocator->m_peak);

    PrintResult(results);
}

void Benchmark::RandomFree(Allocator *allocator,
                           const std::vector<std::size_t> &allocationSizes,
                           const std::vector<std::size_t> &alignments) {
    srand(1);

    std::cout << "\tBENCHMARK: ALLOCATION/FREE" << IO::endl;

    StartRound();

    void *addresses[OPERATIONS];

    std::size_t allocation_size;
    std::size_t alignment;

    allocator->Init();

    auto operations = 0u;

    while (operations < m_nOperations) {
        this->RandomAllocationAttr(allocationSizes, alignments, allocation_size,
                                   alignment);
        addresses[operations] = allocator->Allocate(allocation_size, alignment);
        ++operations;
    }

    while (operations) {
        allocator->Free(addresses[--operations]);
    }

    FinishRound();

    BenchmarkResults results =
        buildResults(m_nOperations, std::move(TimeElapsed), allocator->m_peak);

    PrintResult(results);
}

void Benchmark::PrintResult(const BenchmarkResults &results) const {
    std::cout << "\tRESULT:" << IO::endl;
    std::cout << "\t\tOperations: \t" << results.Operations << IO::endl;
    std::cout << "\t\tTime elapsed: \t" << results.Milliseconds.count()
              << IO::endl;
    std::cout << "\t\tOp per sec: \t" << results.OperationsPerSec << IO::endl;
    std::cout << "\t\tTime per op: \t" << results.TimePerOperation << IO::endl;
    std::cout << "\t\tMemory peal: \t" << results.MemoryPeak << IO::endl;

    std::cout << IO::endl;
}

void Benchmark::RandomAllocationAttr(
    const std::vector<std::size_t> &allocationSizes,
    const std::vector<std::size_t> &alignments, std::size_t &size,
    std::size_t &alignment) {
    const int r = rand() % allocationSizes.size();
    size = allocationSizes[r];
    alignment = alignments[r];
}

const BenchmarkResults
Benchmark::buildResults(std::size_t nOperations,
                        std::chrono::milliseconds &&elapsedTime,
                        std::size_t memoryPeak) const {

    BenchmarkResults results;
    results.Operations = nOperations;
    results.Milliseconds = std::move(elapsedTime);
    results.OperationsPerSec =
        results.Operations / static_cast<double>(results.Milliseconds.count());
    results.TimePerOperation =
        static_cast<double>(results.Milliseconds.count()) /
        static_cast<double>(results.Operations);
    results.MemoryPeak = memoryPeak;

    return results;
}
