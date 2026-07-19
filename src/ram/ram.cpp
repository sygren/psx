#include "ram.hpp"

[[nodiscard]] uint32_t Ram::load32(uint32_t offset) const {
  return data[offset] | data[offset + 1] << 8 | data[offset + 2] << 16 | data[offset + 3] << 24;
}

void Ram::store32(uint32_t offset, uint32_t value) {
  data[offset + 0] = value >> 0;
  data[offset + 1] = value >> 8;
  data[offset + 2] = value >> 16;
  data[offset + 3] = value >> 24;
}
