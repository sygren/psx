#include <print>

#include "cpu/cpu.hpp"
#include "bios/bios.hpp"

int main() {
    Cpu cpu;
    auto bios = Bios::load_from_file("C:\\Users\\mikep\\Desktop\\psx\\bios\\SCPH1001.bin");
    if (bios.has_value()) {
        std::println("Bios loaded successfully");
    }
    return 0;
}
