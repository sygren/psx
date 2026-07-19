#include "map.hpp"

[[nodiscard]] uint32_t mask_region(uint32_t addr) {
  return addr & REGIONS_MASK[addr >> 29];
}
