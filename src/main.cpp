#include "bios/bios.hpp"
#include "common/panic.hpp"
#include "cpu/cpu.hpp"
#include "map/map.hpp"

int main() {
  auto bios_path = "C:\\Users\\mikep\\Desktop\\psx\\bios\\SCPH1001.bin";
  auto bios      = Bios::load_from_file(bios_path);
  if (!bios)
    panic(std::format("Bios couldn't be loaded from {}", bios_path));

  Ram ram(RAM.size);

  Cpu cpu(std::move(*bios), std::move(ram));

  while (true) {
    cpu.run_next_instruction();
  }
  return 0;
}
