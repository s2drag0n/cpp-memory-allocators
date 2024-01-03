#pragma once

#include <cstddef>

class Utils {
  public:
    static const std::size_t CalculatePadding(const std::size_t baseAddress,
                                              const std::size_t alignment) {
        const std::size_t multiplier = (baseAddress / alignment) + 1;
        const std::size_t alignedAddress = multiplier * alignment;
        const std::size_t padding = alignedAddress - baseAddress;
        return padding;
    }

    static const std::size_t
    CalculatePaddingWithHeader(const std::size_t baseAddress,
                               const std::size_t alignment,
                               const std::size_t headerSize) {
        std::size_t padding = CalculatePadding(baseAddress, alignment);
        std::size_t needSpace = headerSize;

        if (padding < needSpace) {
            // header not fit - Calculate next aligned address that header fits
            needSpace -= padding;

            // how many alignments I need to fit the header
            if (needSpace % alignment > 0) {
                padding += alignment * (1 + (needSpace / alignment));
            } else {
                padding += alignment * (needSpace / alignment);
            }
        }

        return padding;
    }
};
