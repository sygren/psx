#pragma once

#include <cstdint>
#include <expected>
#include <filesystem>
#include <vector>

enum class BiosError {
  FileNotFound,
  WrongSize,
  ReadFailed,
};

struct Bios {
  std::vector<uint8_t> data;

  static std::expected<Bios, BiosError> load_from_file(std::filesystem::path path);

  uint32_t load32(uint32_t offset) const;

  Bios()  = default;
  ~Bios() = default;

  Bios(const Bios &)            = delete;
  Bios &operator=(const Bios &) = delete;
  Bios(Bios &&)                 = default;
  Bios &operator=(Bios &&)      = default;
};
