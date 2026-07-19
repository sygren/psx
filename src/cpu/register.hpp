#pragma once

#include "../common/panic.hpp"
#include <array>
#include <cstdint>

struct Registers {
  std::array<uint32_t, 32> regs;

  Registers() {
    regs.fill(0xdeadbeef);
    regs[0] = 0;
  }

  [[nodiscard]] constexpr uint32_t get(size_t index) {
    if (index > 31) {
      panic(std::format("Trying to access index {} of cpu registers", index));
    }
    return regs[index];
  }
  void set(size_t index, uint32_t value) {
    if (index > 31) {
      panic(std::format("Trying to access index {} of cpu registers", index));
    }
    regs[index] = value;
    regs[0]     = 0;
  }
};
