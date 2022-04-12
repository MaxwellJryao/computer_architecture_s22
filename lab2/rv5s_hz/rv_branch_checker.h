# pragma once

#include "VSRTL/core/vsrtl_component.h"

#include "../riscv.h"

namespace vsrtl {
namespace core {
using namespace Ripes;

class BranchChecker : public Component {
public:
    BranchChecker(std::string name, SimComponent* parent) : Component(name, parent) {
        is_branch << [=] {
            const unsigned l7 = instr.uValue() & 0b1111111;
            switch(l7) {
            case 0b1101111: // JAL
            case 0b1100111: // JALR
            case 0b1100011: // Branch Instruction
                return 1;
            default:
                return 0;
            }
        };

    }

    INPUTPORT(instr, RV_INSTR_WIDTH); // instr: the instruction.
    OUTPUTPORT(is_branch, 1); // is_branch: whether the instruction is a branch instruction (JAL, JALR, BEQ, BNE, BGE, BLT, BLTU, BGEU).
};

}  // namespace core
}  // namespace vsrtl
