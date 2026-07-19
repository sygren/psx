#pragma once

#include <array>
#include <cstdint>
#include <optional>

constexpr auto REGIONS_MASK = std::to_array<uint32_t>({// KUSEG: 2048MB
                                                       0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
                                                       // KSEG0: 512MB
                                                       0x7fffffff,
                                                       // KSEG1: 512MB
                                                       0x1fffffff,
                                                       // KSEG2: 1024MB
                                                       0xffffffff, 0xffffffff});

[[nodiscard]] uint32_t mask_region(uint32_t addr);

struct Range {
  uint32_t start;
  uint32_t size;

  [[nodiscard]] constexpr std::optional<uint32_t> try_offset_of(uint32_t item) const noexcept {
    if ((start <= item) && item - start < size) {
      return item - start;
    }
    return std::nullopt;
  }
};

// MEMORY RANGES
constexpr Range BIOS{0x1fc00000, 512 * 1024};
constexpr Range MEMORY_CONTROL{0x1f801000, 36};
constexpr Range RAM{0x00000000, 2 * 1024 * 1024};
constexpr Range SPU{0x1f801c00, 640};
constexpr Range EXPENSION_2{0x1f802000, 0x1fe000};

// MEMORY ADDRESSES
constexpr uint32_t RAM_SIZE_ADDR{0x1f801060};
constexpr uint32_t CACHE_CONTROL_ADDR{0xfffe0130};
