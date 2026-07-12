#pragma once

#include <cstdint>

class Cpu {
    public:
        Cpu();
        ~Cpu() = default;

        void run_next_instruction();

        uint32_t m_pc;
    private:
};
