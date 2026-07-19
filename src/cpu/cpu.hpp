#pragma once

#include "../bios/bios.hpp"
#include "../map/resolver.hpp"
#include "../ram/ram.hpp"
#include "instruction.hpp"
#include "register.hpp"
#include <cstdint>
#include <optional>

struct Cpu {
  Cpu(Bios bios, Ram ram);
  ~Cpu() = default;

  [[nodiscard]] std::optional<uint32_t> load32(uint32_t addr) const noexcept;
  void                                  store32(uint32_t addr, uint32_t value) noexcept;
  void                                  store16(uint32_t addr, uint16_t value) noexcept;
  void store8(uint32_t addr, uint8_t value) noexcept;

  void run_next_instruction();
  void decode_and_run(Instruction instruction);

  // Operations on main CPU
  void load_upper_immediate(Instruction instruction);      // LUI
  void or_immediate(Instruction instruction);              // ORI
  void and_immediate(Instruction instruction);             // ANDI
  void store_word(Instruction instruction);                // SW
  void store_halfword(Instruction instruction);            // SH
                                                           void store_byte(Instruction instruction); // SB
  void load_word(Instruction instruction);                 // LW
  void shift_left_logical(Instruction instruction);        // SLL
  void add_immediate_unsigned(Instruction instruction);    // ADDIU
  void add_unsigned(Instruction instruction);              // ADDU
  void jump(Instruction instruction);                      // J
  void jump_and_link(Instruction instruction);             // JAL
  void bitwise_or(Instruction instruction);                // OR
  void branch_if_not_equal(Instruction instruction);       // BNE
  void add_immediate(Instruction instruction);             // ADDI
  void set_on_less_than_unsigned(Instruction instruction); // SLTU

  // Operations on coprocessors
  void decode_and_run_coprocessor_0(Instruction instruction);
  void mtc0(Instruction instruction);

  uint32_t get_register(uint32_t index) { return input_regs.get(index); }
  void     set_register(uint32_t index, uint32_t value) { output_regs.set(index, value); }

  Resolver resolver;

  // We have two copies of the registers in order to properly handle the load delay slot on MIPS cpu
  // load_delay_value contains the register index and the value to write to after load delay
  std::pair<uint32_t, uint32_t> load_delay_value = {0, 0};
  Registers                     input_regs;
  Registers                     output_regs;

  uint32_t    pc; // program counter
  uint32_t    sr; // status register (coprocessor 0 register)
  Instruction next_instruction{0};
};
