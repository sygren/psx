#include "bios.hpp"

#include <expected>
#include <fstream>

#include "../map/map.hpp"

std::expected<Bios, BiosError> Bios::load_from_file(std::filesystem::path path) {
  std::ifstream file(path, std::ios::binary | std::ios::ate);

  if (!file.is_open()) {
    return std::unexpected(BiosError::FileNotFound);
  }
  if (file.tellg() != BIOS.size) {
    return std::unexpected(BiosError::WrongSize);
  }
  file.seekg(0);

  Bios bios;
  bios.data.resize(BIOS.size);
  file.read(reinterpret_cast<char *>(bios.data.data()), BIOS.size);

  if (!file) {
    return std::unexpected(BiosError::ReadFailed);
  }

  return bios;
}

uint32_t Bios::load32(uint32_t offset) const { return data[offset + 0] | (data[offset + 1] << 8) | (data[offset + 2] << 16) | (data[offset + 3] << 24); }
