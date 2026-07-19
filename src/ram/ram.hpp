#pragma once

#include <vector>
#include <cstdint>

struct Ram {
  std::vector<uint8_t> data;

  Ram(size_t size) : data(size, 0xCA) {}

  [[nodiscard]] uint32_t load32(uint32_t offset) const;
  void store32(uint32_t offset, uint32_t value);
};

