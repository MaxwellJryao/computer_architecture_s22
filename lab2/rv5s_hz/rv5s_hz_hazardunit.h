#pragma once

#include "../riscv.h"

#include "VSRTL/core/vsrtl_component.h"
#include "stdio.h"

namespace vsrtl {
namespace core {
using namespace Ripes;

class HZ_HazardUnit : public Component {
public:
    HZ_HazardUnit(std::string name, SimComponent* parent) : Component(name, parent) {
        hazardFEEnable << [=] { return !hasHazard() && !hasMemWait() && !hasALUWait(); };
        hazardIDEXEnable << [=] { return !hasEcallHazard() && !hasMemWait() && !hasALUWait(); };
        hazardEXMEMEnable << [=] { return !hasMemWait() && !hasALUWait(); };
        hazardMEMWBEnable << [=] { return !hasMemWait() && !hasALUWait(); };
        hazardEXMEMClear << [=] { return hasEcallHazard(); };
        hazardIDEXClear << [=] { return hasDataHazard(); };
        stallEcallHandling << [=] { return hasEcallHazard(); };
    }

    // For the result of the following input ports, you may find /src/processors/RISC-V/rv-control.h helpful.
    INPUTPORT(id_reg1_idx, RV_REGS_BITS); // id_reg1_idx: index of the first register of ID stage.
    INPUTPORT(id_reg2_idx, RV_REGS_BITS); // id_reg2_idx: index of the second register of ID stage.

    INPUTPORT(ex_reg_wr_idx, RV_REGS_BITS); // ex_reg_wr_idx: index of the write register of EX stage.
    INPUTPORT(ex_do_mem_read_en, 1); // ex_do_mem_read_en: whether it is a load instruction (LB, LH, LW, LBU, LHU).

    INPUTPORT(mem_do_reg_write, 1); // mem_do_reg_write: whether the current instruction at MEM stage will write registers.

    INPUTPORT(wb_do_reg_write, 1); // wb_do_reg_write: whether the current instruction at WB stage will write registers.

    INPUTPORT_ENUM(opcode, RVInstr);
    INPUTPORT(mem_wait, 1); // mem_wait: whether the pipeline is waiting for memory access result.
    INPUTPORT(alu_wait, 1); // alu_wait: whether the pipeline is waiting for alu operation result (MUL, DIV).

    OUTPUTPORT(hazardFEEnable, 1); // Enable IF stage.
    OUTPUTPORT(hazardIDEXEnable, 1); // Enable EX stage.
    OUTPUTPORT(hazardEXMEMEnable, 1); // Enable MEM stage.
    OUTPUTPORT(hazardMEMWBEnable, 1); // Enable WB stage.

    OUTPUTPORT(hazardEXMEMClear, 1); // Clear data in EX/MEM register.
    OUTPUTPORT(hazardIDEXClear, 1); // Clear data in ID/EX register.

    // Stall Ecall Handling: High whenever we are about to handle an ecall, but have outstanding writes in the pipeline
    // which must be comitted to the register file before handling the ecall.
    OUTPUTPORT(stallEcallHandling, 1);

private:
    bool hasHazard() { return hasEcallHazard() || hasDataHazard(); }

    bool hasMemWait() const {
        return (mem_wait.uValue() == 1);
    }

    bool hasALUWait() const {
        return (alu_wait.uValue() == 1);
    }

    bool hasEcallHazard() const {
        // Check for ECALL hazard. We are implictly dependent on all registers when performing an ECALL operation. As
        // such, all outstanding writes to the register file must be performed before handling the ecall. Hence, the
        // front-end of the pipeline shall be stalled until the remainder of the pipeline has been cleared and there are
        // no more outstanding writes.
        const bool isEcall = opcode.uValue() == RVInstr::ECALL;
        return isEcall && (mem_do_reg_write.uValue() || wb_do_reg_write.uValue());
    }


    bool hasDataHazard() const {
        // --------------------------- Part 2, TODO: implement data hazard detection ---------------------------
        // Hint: the existing ports of hzunit now is not enough for detecting hazard. So you need to find
        //       out what else you need and add input ports in hzunit.
        return false;
    }
};
}  // namespace core
}  // namespace vsrtl
