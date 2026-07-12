#include "bios.hpp"

#include <expected>
#include <fstream>

std::expected<Bios, BiosError> Bios::load_from_file(std::filesystem::path path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        return std::unexpected(BiosError::FileNotFound);
    }
    if (file.tellg() != PSX_BIOS_SIZE) {
        return std::unexpected(BiosError::WrongSize);
    }
    file.seekg(0);

    Bios bios;
    bios.data.resize(PSX_BIOS_SIZE);
    file.read(reinterpret_cast<char*>(bios.data.data()), PSX_BIOS_SIZE);

    if (!file) {
        return std::unexpected(BiosError::ReadFailed);
    }

    return bios;
}
