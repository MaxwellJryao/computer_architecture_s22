# pragma once

#include "VSRTL/core/vsrtl_component.h"

#include "../riscv.h"

namespace vsrtl {
namespace core {
using namespace Ripes;

class Endpoint : public Component {
public:
    Endpoint(std::string name, SimComponent* parent) : Component(name, parent) {
        return;
    }

    INPUTPORT(end_port, 1);
};

}  // namespace core
}  // namespace vsrtl
