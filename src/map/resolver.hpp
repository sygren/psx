#pragma once

#include <cstdint>
#include <optional>
#include <print>

#include "../bios/bios.hpp"
#include "../common/panic.hpp"
#include "../ram/ram.hpp"
#include "map.hpp"

struct Resolver {
public:
  Resolver(Bios bios, Ram ram) : bios(std::move(bios)), ram(std::move(ram)) {}

  [[nodiscard]] constexpr std::optional<uint32_t> load32(uint32_t addr) const noexcept {
    addr = mask_region(addr);

    if (auto offset = BIOS.try_offset_of(addr)) {
      return bios.load32(*offset);
    }
    if (auto offset = RAM.try_offset_of(addr)) {
      return ram.load32(*offset);
    }
    return std::nullopt;
  };

  constexpr void store32(uint32_t addr, uint32_t value) noexcept {
    addr = mask_region(addr);

    if (addr == CACHE_CONTROL_ADDR) {
      std::println("Unhandled store32 into CACHE_CONTROL address at {}", reinterpret_cast<void*>(addr));
      return;
    }

    if (addr == RAM_SIZE_ADDR) {
      std::println("Unhandled store32 into RAM_SIZE address at {}", reinterpret_cast<void*>(addr));
      return;
    }

    if (auto offset = RAM.try_offset_of(addr)) {
      ram.store32(*offset, value);
      return;
    }

    if (auto offset = MEMORY_CONTROL.try_offset_of(addr)) {
      switch (*offset) {
      case 0:
        if (value != 0x1f000000)
          panic();
        return;
        break;
      case 4:
        if (value != 0x1f802000)
          panic();
        return;
        break;
      default:
        std::println("Unhandled store32 into memory control region at address {:x}, offset {:x}", addr, *offset);
        return;
      };
    }

    panic(std::format("Unhandled store32 into address {:x}", addr));
  }

  void store16(uint32_t addr, uint16_t value) { 
    addr = mask_region(addr);
    if (auto offset = SPU.try_offset_of(addr)) {
      std::println("Unhandled store16 into spu control region at address {:08x}, offset {:x}", addr, *offset);
      return;
    }
    panic(std::format("Unhandled store16 into address {:x}", addr));
  }

  void store8(uint32_t addr, uint8_t value) {
    addr = mask_region(addr);

    if (auto offset = EXPENSION_2.try_offset_of(addr)) {
      std::println("Unhandled store8 into expension2 region at address {:08x}, offset {:x}", addr, *offset);
      return;
    }

    panic(std::format("Unhandled store8 into address {:x}", addr));
  }

private:
  Bios bios;
  Ram  ram;
};
