#include "cpu.hpp"
#include "../common/panic.hpp"
#include "instruction.hpp"
#include <cassert>
#include <limits>

Cpu::Cpu(Bios bios, Ram ram) : resolver(std::move(bios), std::move(ram)), pc(BIOS.start), sr(0) {}

std::optional<uint32_t> Cpu::load32(uint32_t addr) const noexcept {
  if (addr % 4 != 0) {
    panic(std::format("Trying to load at an unaligned address {:x}", addr));
  }
  return resolver.load32(addr);
}

void Cpu::store32(uint32_t addr, uint32_t value) noexcept {
  if (addr % 4 != 0) {
    panic(std::format("Trying to store at an unaligned address {:x}", addr));
  }
  resolver.store32(addr, value);
}

void Cpu::store16(uint32_t addr, uint16_t value) noexcept {
  if (addr % 2 != 0) {
    panic(std::format("Trying to store16 at an unaligned address {:x}", addr));
  }
  resolver.store16(addr, value);
}

void Cpu::store8(uint32_t addr, uint8_t value) noexcept { resolver.store8(addr, value); }

void Cpu::run_next_instruction() {
  assert(input_regs.regs[0] == 0);
  assert(output_regs.regs[0] == 0);

  set_register(load_delay_value.first, load_delay_value.second);
  load_delay_value = {0, 0};

  auto instruction            = next_instruction;
  auto maybe_next_instruction = load32(pc);
  if (!maybe_next_instruction) {
    panic(std::format("Current addr in the program counter is incorrect ({})", reinterpret_cast<void *>(pc)));
  }

  next_instruction = Instruction{*maybe_next_instruction};
  pc += 4;

  decode_and_run(instruction);

  input_regs = output_regs;
}

// TO CHECK: need to order the switch cases in the most used order
// when emulator works I should count how many times each instructions is called
// Then sort here depending on it.
void Cpu::decode_and_run(Instruction instruction) {
  switch (instruction.type()) {
  case InstructionType::SPECIAL:
    switch (instruction.special_type()) {
    case SpecialInstructionType::ADDU:
      return add_unsigned(instruction);
    case SpecialInstructionType::SLTU:
      return set_on_less_than_unsigned(instruction);
    case SpecialInstructionType::SLL:
      return shift_left_logical(instruction);
    case SpecialInstructionType::OR:
      return bitwise_or(instruction);
    default:
      panic(std::format("Unhandled special instruction {:08x} ({:08x})", static_cast<uint32_t>(instruction.special_type()), instruction.value));
    };
  case InstructionType::COP0:
    return decode_and_run_coprocessor_0(instruction);
  case InstructionType::ADDI:
    return add_immediate(instruction);
  case InstructionType::BNE:
    return branch_if_not_equal(instruction);
  case InstructionType::J:
    return jump(instruction);
  case InstructionType::JAL:
    return jump_and_link(instruction);
  case InstructionType::ADDIU:
    return add_immediate_unsigned(instruction);
  case InstructionType::LUI:
    return load_upper_immediate(instruction);
  case InstructionType::ORI:
    return or_immediate(instruction);
  case InstructionType::ANDI:
    return and_immediate(instruction);
  case InstructionType::SW:
    return store_word(instruction);
  case InstructionType::SH:
    return store_halfword(instruction);
  case InstructionType::SB:
    return store_byte(instruction);
  case InstructionType::LW:
    return load_word(instruction);
  default:
    panic(std::format("Unhandled instruction {:08x}", instruction.value));
  };
}

void Cpu::load_upper_immediate(Instruction instruction) {
  assert(instruction.type() == InstructionType::LUI);

  auto t   = instruction.t();
  auto imm = instruction.imm();

  // lui $t, imm
  set_register(t, imm << 16);
}

void Cpu::or_immediate(Instruction instruction) {
  assert(instruction.type() == InstructionType::ORI);

  auto s   = instruction.s();
  auto t   = instruction.t();
  auto imm = instruction.imm();

  // ori $t, $s, imm
  set_register(t, get_register(s) | imm);
}

void Cpu::and_immediate(Instruction instruction) {
  assert(instruction.type() == InstructionType::ANDI);

  auto t   = instruction.t();
  auto s   = instruction.s();
  auto imm = instruction.imm();

  // andi $t, $s, imm
  set_register(t, get_register(s) & imm);
}

void Cpu::store_word(Instruction instruction) {
  assert(instruction.type() == InstructionType::SW);

  if (sr & 0x10000) {
    std::println("Ignoring store while cache is isolated");
    return;
  }

  auto s   = instruction.s();
  auto t   = instruction.t();
  auto imm = instruction.imm_se();

  // sw $t, offset($s, imm)
  store32(get_register(s) + imm, get_register(t));
}

void Cpu::store_halfword(Instruction instruction) {
  assert(instruction.type() == InstructionType::SH);

  if (sr & 0x10000) {
    std::println("Ignoring store16 while cache is isolated");
    return;
  }

  auto s   = instruction.s();
  auto t   = instruction.t();
  auto imm = instruction.imm_se();

  // sh $t, offset($s, imm)
  store16(get_register(s) + imm, static_cast<uint16_t>(get_register(t)));
}

void Cpu::store_byte(Instruction instruction) {
  assert(instruction.type() == InstructionType::SB);

  if (sr & 0x10000) {
    std::println("Ignoring store8 while cache is isolated");
    return;
  }

  auto s   = instruction.s();
  auto t   = instruction.t();
  auto imm = instruction.imm_se();

  // sh $t, offset($s, imm)
  store8(get_register(s) + imm, static_cast<uint8_t>(get_register(t)));
}

void Cpu::load_word(Instruction instruction) {
  assert(instruction.type() == InstructionType::LW);

  if (sr & 0x10000) {
    std::println("Ignoring load while cache is isolated");
    return;
  }

  auto t   = instruction.t();
  auto s   = instruction.s();
  auto imm = instruction.imm_se();

  auto addr = get_register(s) + imm;

  auto load = load32(addr);
  if (!load.has_value()) {
    panic(std::format("Trying to load incorrect memory addr {} in load_word", reinterpret_cast<void *>(addr)));
  }

  load_delay_value = std::make_pair(t, *load);
}

void Cpu::shift_left_logical(Instruction instruction) {
  assert(instruction.type() == InstructionType::SPECIAL);
  assert(instruction.special_type() == SpecialInstructionType::SLL);

  auto d = instruction.d();
  auto t = instruction.t();
  auto h = instruction.h();

  // ssl $d, $t, h
  set_register(d, get_register(t) << h);
}

void Cpu::add_immediate_unsigned(Instruction instruction) {
  assert(instruction.type() == InstructionType::ADDIU);

  auto t   = instruction.t();
  auto s   = instruction.s();
  auto imm = instruction.imm_se();

  // addiu $t, $s, imm
  set_register(t, get_register(s) + imm);
}

void Cpu::add_unsigned(Instruction instruction) {
  assert(instruction.type() == InstructionType::SPECIAL);
  assert(instruction.special_type() == SpecialInstructionType::ADDU);

  auto d = instruction.d();
  auto s = instruction.s();
  auto t = instruction.t();

  // addu $d, $s, $t
  set_register(d, get_register(s) + get_register(t));
}

void Cpu::jump(Instruction instruction) {
  assert(instruction.type() == InstructionType::J);

  auto imm = instruction.value & 0x3FFFFFF;
  // j imm
  pc = (pc & 0xF0000000) | (imm << 2);
}

void Cpu::jump_and_link(Instruction instruction) {
  assert(instruction.type() == InstructionType::JAL);

  auto imm = instruction.value & 0x3FFFFFF;

  // jal imm
  set_register(31, pc);
  pc = (pc & 0xF0000000) | (imm << 2);
}

void Cpu::bitwise_or(Instruction instruction) {
  assert(instruction.type() == InstructionType::SPECIAL);
  assert(instruction.special_type() == SpecialInstructionType::OR);

  auto d = instruction.d();
  auto s = instruction.s();
  auto t = instruction.t();

  // or $d, $s ,$t
  set_register(d, get_register(s) | get_register(t));
}

void Cpu::branch_if_not_equal(Instruction instruction) {
  assert(instruction.type() == InstructionType::BNE);

  auto s = instruction.s();
  auto t = instruction.t();

  // bne $s, $t, offset
  if (get_register(s) != get_register(t)) {
    // We need to compensate for the hardcoded pc += 4 in decode_and_run function
    pc = (pc - 4) + (instruction.imm_se() << 2);
  }
}

void Cpu::add_immediate(Instruction instruction) {
  assert(instruction.type() == InstructionType::ADDI);

  auto t = instruction.t();
  auto s = instruction.s();

  int64_t imm = static_cast<int64_t>(instruction.imm_se());
  int64_t v   = static_cast<int64_t>(get_register(s));

  if (v + imm > std::numeric_limits<uint32_t>::max() || v + imm < 0) {
    panic("Overflow happening during add_immediate");
  }

  // addi $t, $s, imm
  set_register(t, get_register(s) + imm);
}

void Cpu::set_on_less_than_unsigned(Instruction instruction) {
  assert(instruction.type() == InstructionType::SPECIAL);
  assert(instruction.special_type() == SpecialInstructionType::SLTU);

  auto d = instruction.d();
  auto s = instruction.s();
  auto t = instruction.t();

  // sltu $d, $s, $t
  set_register(d, get_register(s) < get_register(t));
}

void Cpu::decode_and_run_coprocessor_0(Instruction instruction) {
  assert(instruction.type() == InstructionType::COP0);

  switch (instruction.coprocessor_0_type()) {
  case Coprocessor0InstructionType::MTC0:
    return mtc0(instruction);
  default:
    panic(std::format("Unhandled coprocessor 0 instruction {:08x} ({:08x})", (uint32_t)instruction.coprocessor_0_type(), instruction.value));
  };
}

void Cpu::mtc0(Instruction instruction) {
  assert(instruction.coprocessor_0_type() == Coprocessor0InstructionType::MTC0);

  auto t = instruction.t();
  auto d = instruction.d();

  switch (d) {
  case 12: // Actually writing to the status register on the coprocessor 0
    sr = get_register(t);
    return;
  case 3:
  case 5:
  case 6:
  case 7:
  case 9:
  case 11:
  case 13:
    if (get_register(t) != 0) {
      panic(std::format("Unhandled write to cop0r{})", d));
    }
    std::println("mtc0 coprocessor 0 instruction trying to set breakpoints settings");
    return;
  default:
    panic(std::format("Unhandled mtc0 coprocessor 0 instruction {:08x} ({:08x})", (uint32_t)instruction.coprocessor_0_type(), instruction.value));
  }
}
