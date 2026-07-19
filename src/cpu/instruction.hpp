#pragma once

#include <cstdint>

enum class InstructionType {
  SPECIAL = 0x0,
  BCONDZ  = 0x1,
  J       = 0x2,
  JAL     = 0x3,
  BEQ     = 0x4,
  BNE     = 0x5,
  BLEZ    = 0x6,
  BGTZ    = 0x7,
  ADDI    = 0x8,
  ADDIU   = 0x9,
  SLTI    = 0xA,
  SLTIU   = 0xB,
  ANDI     = 0xC,
  ORI     = 0xD,
  XORI    = 0xE,
  LUI     = 0xF,
  COP0,
  COP1,
  COP2,
  COP3,
  LB = 0x20,
  LH,
  LWL,
  LW,
  LBU,
  LHU,
  LWR,
  SB = 0x28,
  SH,
  SWL,
  SW,
  SWR  = 0x2E,
  LWC0 = 0x30
};

enum class SpecialInstructionType {
  SLL = 0x0,
  SRL = 0x2,
  SRA,
  SLLV,
  SRLV = 0x6,
  SRAV,
  JR,
  JALR,
  SYSCALL = 0xC,
  BREAK,
  MFHI = 0x10,
  MTHI,
  MFLO,
  MTLO,
  MULT = 0x18,
  MULTU,
  DIV,
  DIVU,
  ADD = 0x20,
  ADDU,
  SUB,
  SUBU,
  AND,
  OR,
  XOR,
  NOR,
  SLT = 0x2A,
  SLTU
};

enum class Coprocessor0InstructionType {
  MTC0 = 0x4,
};

struct Instruction {
  uint32_t value;

  [[nodiscard]] InstructionType type() const noexcept { return InstructionType(value >> 26); }

  // Only used in case Instruction.type() return the SPECIAL enum variant
  // Need to call this function to get more information
  [[nodiscard]] SpecialInstructionType special_type() const noexcept { return SpecialInstructionType(value & 0x3F); }

  // Only used in case Instruction.type() return the COP0 enum variant
  // Need to call this function to get more information
  [[nodiscard]] Coprocessor0InstructionType coprocessor_0_type() const noexcept { return Coprocessor0InstructionType(s()); }

  [[nodiscard]] uint32_t t() const noexcept { return (value >> 16) & 0x1F; }

  [[nodiscard]] uint32_t s() const noexcept { return (value >> 21) & 0x1F; }

  [[nodiscard]] uint32_t d() const noexcept { return (value >> 11) & 0x1F; }

  [[nodiscard]] uint32_t h() const noexcept { return (value >> 6) & 0x1F; }

  [[nodiscard]] uint32_t imm() const noexcept { return value & 0xFFFF; }

  // We want to extend a 16bits signed integer into a 32bits unsigned/signed integer.
  // We need to be explicit about it so that the compiler generate instructions to maintain
  // the sign coherence through two's complement
  [[nodiscard]] uint32_t imm_se() const noexcept { return static_cast<uint32_t>(static_cast<int16_t>(value & 0xFFFF)); }
};
