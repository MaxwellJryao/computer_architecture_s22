#pragma once

#include "VSRTL/core/vsrtl_component.h"
#include "VSRTL/core/vsrtl_wire.h"
#include "../riscv.h"
#include "rv_endpoint.h"

namespace vsrtl {
namespace core {
using namespace Ripes;

class BranchTargetBuffer : public Component {
public:
    BranchTargetBuffer(std::string name, SimComponent* parent) : Component(name, parent) {
        update_wire->setSensitiveTo(branch_address_update);
        update_wire->setSensitiveTo(should_update);
        update_wire->setSensitiveTo(target_address_update);

        branch_address_update >> last_address_update->in;

        target_address << [=] {
            // --------------------------- Part 4. TODO: implement BTB policy ---------------------------
            return branch_address.uValue() + 4;
        };

        // --------------------------- Part 4. TODO: initialize the BTB here if you need to ---------------------------


        update_wire->out << [=] {
            // Not branch instructions.
            if (not should_update.uValue())
                return 1;
            // Pipeline stalled, no need to update twice.
            if (last_address_update->out.uValue() == branch_address_update.uValue())
                return 1;

            // --------------------------- Part 4. TODO: update BTB here ---------------------------
            return 1;
        };

        update_wire->out >> endpoint->end_port;
    }

    INPUTPORT(branch_address, RV_REG_WIDTH); // branch_address: the address of the branch instruction. It is used as the index of BTB.
    INPUTPORT(branch_address_update, RV_REG_WIDTH); // branch_address_update: the address to update.
    INPUTPORT(target_address_update, RV_REG_WIDTH); // target_address_update: the target address to update.
    INPUTPORT(should_update, 1); // should_update: whether we should update the BTB.

    SUBCOMPONENT(last_address_update, Register<RV_REG_WIDTH>); // last_address_update: the last PC address in the ID stage.

    // update_wire and endpoint are used to update the branch predictor at runtime.
    // Their return values are meaningless. You do not need to modify this.
    WIRE(update_wire, 1); // T
    SUBCOMPONENT(endpoint, Endpoint);

    // target_address: the predicted target address.
    OUTPUTPORT(target_address, RV_REG_WIDTH);

    void reset() {
        // --------------------------- Part 4. TODO: reset the branch target buffer here---------------------------
        // This function is called when click the 'reset' button in Ripes. You may need to do things such as clearing
        // the tables.
        // If you do not need to reset the table, you can leave this function empty.
        return;
    }
private:

    // --------------------------- Part 4. TODO: put your BTB data structure here ---------------------------
    // Hint: For example: use a map<branch address, target address>
    // Hint: You may need to add some input ports to get the data used to update the branch history.
};

}  // namespace core
}  // namespace vsrtl
