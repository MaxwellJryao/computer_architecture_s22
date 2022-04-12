#pragma once

#include "VSRTL/core/vsrtl_component.h"
#include "../riscv.h"

namespace vsrtl {
namespace core {
using namespace Ripes;

class BranchID : public Component {
public:
    BranchID(std::string name, SimComponent* parent) : Component(name, parent) {
        taken << [=] {
            return branchTaken();
        };

        target_address << [=] {
            return computeTargetAddress();
        };

        wrong_predict_pc << [=] {
            return wrongPredictPc();
        };

        branch_actual_select << [=] {
            if (branchTaken())
                return PcSrcBranch::BRANCH;
            else
                return PcSrcBranch::PC4;
        };

        branch_final_select << [=] {
            // ------------------ Part 4. TODO: implement selection of the final source of pc -------------------
            return PcSrcFinal::ACTUAL;
        };

        should_update << [=] {
            const unsigned l7 = instr.uValue() & 0b1111111;
            // JAL, JALR or other branch instructions
            return l7 == 0b1101111 || l7 == 0b1100111 || l7 == 0b1100011;
        };
    }

    INPUTPORT_ENUM(comp_op, CompOp); // comp_op: the compare operator (NOP, EQ, NE, LT, LTU, GE, GEU);
    INPUTPORT(op1, RV_REG_WIDTH); // op1:
    INPUTPORT(op2, RV_REG_WIDTH); // op2:
    INPUTPORT(do_jump_in, 1); // do_jump_in: whether this instruction may do jump operations (JAL, JALR).
    INPUTPORT(do_branch_in, 1); // do_branch_in: whether this instruction may do other branch operations (JAL, JALR, BEQ, BNE, BGE, BLT, BLTU, BGEU).
    INPUTPORT(pc_value, RV_REG_WIDTH); // pc_value: the value of pc register.
    INPUTPORT(offset_value, RV_REG_WIDTH); // offset_value: the offset to generated the branch target address.
    INPUTPORT(instr, RV_INSTR_WIDTH); // instr: the instruction.

    OUTPUTPORT(should_update, 1); // should_update: whether we should update predictor or BTB.
    OUTPUTPORT(taken, 1); // taken: the actual result of the branch instruction (0: not taken. 1: taken).
    OUTPUTPORT(target_address, RV_REG_WIDTH); // target_address: the actual target address of the branch instruction.
    OUTPUTPORT(wrong_predict_pc, 1); // wrong_predict_pc: whether the predicted result of this branch instruction is wrong. (0: not wrong. 1: wrong).
    OUTPUTPORT_ENUM(branch_actual_select, PcSrcBranch); // branch_actual: the selection of the source of pc in ID stage.
    OUTPUTPORT_ENUM(branch_final_select, PcSrcFinal); // branch_final: the selection of the source of pc (pc in IF stage or pc in ID stage).

private:
    bool branchTaken() {
        if (do_jump_in.uValue()) {
            return true;
        }
        switch(comp_op.uValue()){
            case CompOp::NOP: return false;
            case CompOp::EQ: return op1.uValue() == op2.uValue() && do_branch_in.uValue();
            case CompOp::NE: return op1.uValue() != op2.uValue() && do_branch_in.uValue();
            case CompOp::LT: return op1.sValue() < op2.sValue() && do_branch_in.uValue();
            case CompOp::LTU: return op1.uValue() < op2.uValue() && do_branch_in.uValue();
            case CompOp::GE: return op1.sValue() >= op2.sValue() && do_branch_in.uValue();
            case CompOp::GEU: return op1.uValue() >= op2.uValue() && do_branch_in.uValue();
            default: assert("Comparator: Unknown comparison operator"); return false;
        }
        return false;
    }

    bool wrongPredictPc() {
        // ------------------ Part 4. TODO: decide whether the predict result is wrong -------------------
        // Hint: You may need to add some input ports to decide whether the prediction is wrong.
        return branchTaken();
    }

    VSRTL_VT_U computeTargetAddress() {
        return pc_value.uValue() + offset_value.uValue();
    }
};

}  // namespace core
}  // namespace vsrtl
