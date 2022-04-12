#pragma once

#include "VSRTL/core/vsrtl_component.h"
#include "VSRTL/core/vsrtl_wire.h"
#include "rv_endpoint.h"

#include "../riscv.h"

namespace vsrtl {
namespace core {
using namespace Ripes;

class BranchPredictor : public Component {
public:
    BranchPredictor(std::string name, SimComponent* parent) : Component(name, parent) {
        update_wire->setSensitiveTo(branch_address_update);
        update_wire->setSensitiveTo(branch_result_update);
        update_wire->setSensitiveTo(should_update);

        branch_address_update >> last_address_update->in;

        taken << [=] {
            return branchPredict();
        };

        branch_predict << [=] {
            if (branchPredict()) {
                return PcSrcBranch::BRANCH;
            } else {
                return PcSrcBranch::PC4;
            }
        };

        // --------------------------- Part 4. TODO: initialize the branch history table here if you need to ---------------------------


        update_wire->out << [=] {
            // Not branch instructions.
            if (not should_update.uValue())
                return 1;
            // Pipeline stalled, no need to update twice.
            if (last_address_update->out.uValue() == branch_address_update.uValue())
                return 1;

            // --------------------------- Part 4. TODO: update branch history table here ---------------------------
            return 1;
        };

        update_wire->out >> endpoint->end_port;
    }

    INPUTPORT(is_branch, 1); // is_branch: whether the instruction is a branch/jump instruciton (JAL, JALR, BEQ, BNE, BGE, BLT, BLTU, BGEU).
    INPUTPORT(branch_address, RV_REG_WIDTH); // branch_address: the address of the branch instruction.
    INPUTPORT(branch_address_update, RV_REG_WIDTH); // branch_address_update: the address to update.
    INPUTPORT(branch_result_update, 1); // branch_result_update: the result to update.
    INPUTPORT(should_update, 1); // should_update: whether we should update the predictor.

    SUBCOMPONENT(last_address_update, Register<RV_REG_WIDTH>); // last_address_update: the last PC address in the ID stage.

    // update_wire and endpoint are used to update the branch predictor at runtime.
    // Their return values are meaningless. You do not need to modify this.
    WIRE(update_wire, 1);
    SUBCOMPONENT(endpoint, Endpoint);

    OUTPUTPORT(taken, 1); // taken: the predicted result (0: not taken. 1: taken).
    OUTPUTPORT_ENUM(branch_predict, PcSrcBranch); // branch_predict: the selection of the source of pc in IF stage.

    void reset() {
        // --------------------------- Part 4. TODO: reset the branch history table here---------------------------
        // This function is called when click the 'reset' button in Ripes. You may need to do things such as clearing
        // the tables.
        // If you do not need to reset the table, you can leave this function empty.
        return;
    }

private:
    bool branchPredict() {
        if (is_branch.uValue() == 0) {
            return 0;
        }

        // --------------------------- Part 4. TODO: implement branch predict policy ---------------------------
        return 1;
    }


    // --------------------------- Part 4. TODO: define your branch history table here ---------------------------
    // Hint: You may need to add some input ports to get the data used to update the branch history.


};

}  // namespace core
}  // namespace vsrtl
