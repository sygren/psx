#include "cpu.hpp"

#define BIOS_START_ADDR 0xbfc00000

Cpu::Cpu() : m_pc(BIOS_START_ADDR) {}

void Cpu::run_next_instruction() {
    m_pc += 4;
}
