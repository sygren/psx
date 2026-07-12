#pragma once

#include <cstdint>
#include <vector>
#include <expected>
#include <filesystem>

constexpr size_t PSX_BIOS_SIZE = 512 * 1024;

enum class BiosError {
    FileNotFound, WrongSize, ReadFailed,
};

struct Bios {
    std::vector<uint8_t> data;

    static std::expected<Bios, BiosError> load_from_file(std::filesystem::path path);

    Bios() = default;
    ~Bios() = default;

    Bios(const Bios&) = delete;
    Bios& operator=(const Bios&) = delete;
    Bios(Bios&&) = default;
    Bios& operator=(Bios&&) = default;
};
