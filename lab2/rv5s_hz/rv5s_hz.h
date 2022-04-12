#pragma once

#include "VSRTL/core/vsrtl_adder.h"
#include "VSRTL/core/vsrtl_constant.h"
#include "VSRTL/core/vsrtl_design.h"
#include "VSRTL/core/vsrtl_logicgate.h"
#include "VSRTL/core/vsrtl_multiplexer.h"

#include "../../ripesprocessor.h"

// Functional units
#include "../riscv.h"
#include "../rv_alu.h"
#include "../rv_branch.h"
#include "../rv_control.h"
#include "../rv_decode.h"
#include "../rv_ecallchecker.h"
#include "../rv_immediate.h"
#include "../rv_memory.h"
#include "../rv_registerfile.h"
#include "rv_branch_id.h"
#include "rv_branch_predictor.h"
#include "rv_branch_target_buffer.h"
#include "rv_branch_checker.h"

// Stage separating registers
#include "rv5s_hz_ifid.h"
#include "rv5s_hz_exmem.h"
#include "rv5s_hz_idex.h"
#include "rv5s_hz_memwb.h"

// Forwarding & Hazard detection unit
#include "rv5s_hz_hazardunit.h"

namespace vsrtl {
namespace core {
using namespace Ripes;

class RV5S_HZ : public RipesProcessor {
public:
    enum Stage { IF = 0, ID = 1, EX = 2, MEM = 3, WB = 4, STAGECOUNT };
    RV5S_HZ(const QStringList& extensions) : RipesProcessor("5-Stage RISC-V Processor with hazard detection") {
        m_enabledISA = std::make_shared<ISAInfo<ISA::RV32I>>(extensions);
        decode->setISA(m_enabledISA);

        /*
         * Ripes overloaded the '>>' operator. It is used to represent the there is
         * a link between the two ports linked by '>>'.
         *
         * You do not need to know what is operation overloading in c++ as well as
         * how to implement operator overloading. But you need to know how to use '>>'.
         *
         * For example, PortA >> PortB means there is a link from A to B. And data
         * can be sent from A to B through this link.
         *
         * We denote the port on the left of '>>' as output port and the port on the
         * right as input port. Notice that there should be one and there is only one
         * link for each input port. While an output port can link to many links.
         *
         * For example:
         * PortA >> PortB; PortA >> PortC; (Right!)
         * PortX >> PortZ; PortY >> PortZ; (Wrong!)
         *
         * If you want a input port gets input related to more than one values, you can
         * use a Multiplexer or a Gate.
         *
         * See external/VSRTL/vsrtl_port.h for more information of '>>'.
         * See https://en.wikipedia.org/wiki/Operator_overloading for more information of
         * operator overloading.
         *
         */

        // -----------------------------------------------------------------------
        // Program counter
        pc_reg->out >> pc_4->op1;
        4 >> pc_4->op2;
        pc_src->out >> pc_reg->in;
        0 >> pc_reg->clear;
        hzunit->hazardFEEnable >> pc_reg->enable;

        // -----------------------------------------------------------------------
        // Instruction memory
        pc_reg->out >> instr_mem->addr;
        instr_mem->setMemory(m_memory);

        // -----------------------------------------------------------------------
        // Branch Checker
        instr_mem->data_out >> branch_checker->instr;
        branch_checker->is_branch >> branch_predictor->is_branch;

        // -----------------------------------------------------------------------
        // Branch Target Buffer
        pc_reg->out >> branch_target_buffer->branch_address;
        branch_target_buffer->target_address >> pc_src_if->get(PcSrcBranch::BRANCH);
        pc_4->out >> pc_src_if->get(PcSrcBranch::PC4);
        pc_src_if->out >> pc_src->get(PcSrcFinal::PREDICT);

        // -----------------------------------------------------------------------
        // Branch Predictor
        pc_reg->out >> branch_predictor->branch_address;
        branch_predictor->branch_predict >> pc_src_if->select;

        // -----------------------------------------------------------------------
        // Decode
        ifid_reg->instr_out >> decode->instr;

        // -----------------------------------------------------------------------
        // Control signals
        decode->opcode >> control->opcode;

        // -----------------------------------------------------------------------
        // Immediate
        decode->opcode >> immediate->opcode;
        ifid_reg->instr_out >> immediate->instr;

        // -----------------------------------------------------------------------
        // Registers
        decode->r1_reg_idx >> registerFile->r1_addr;
        decode->r2_reg_idx >> registerFile->r2_addr;

        memwb_reg->wr_reg_idx_out >> registerFile->wr_addr;
        memwb_reg->reg_do_write_out >> registerFile->wr_en;
        memwb_reg->mem_read_out >> reg_wr_src->get(RegWrSrc::MEMREAD);
        memwb_reg->alures_out >> reg_wr_src->get(RegWrSrc::ALURES);
        memwb_reg->pc4_out >> reg_wr_src->get(RegWrSrc::PC4);
        memwb_reg->reg_wr_src_ctrl_out >> reg_wr_src->select;
        reg_wr_src->out >> registerFile->data_in;

        registerFile->setMemory(m_regMem);

        // -----------------------------------------------------------------------
        // Branch
        control->comp_ctrl >> branch->comp_op;
        registerFile->r1_out >> branch->op1;
        registerFile->r2_out >> branch->op2;

        control->do_branch >> branch->do_branch_in;
        control->do_jump >> branch->do_jump_in;

        registerFile->r1_out >> branch_addr_op1_src->get(BranchAddrSrc1::REG1);
        ifid_reg->pc_out >> branch_addr_op1_src->get(BranchAddrSrc1::PC);
        control->alu_op1_ctrl >> branch_addr_op1_src->select;

        registerFile->r2_out >> branch_addr_op2_src->get(BranchAddrSrc2::REG2);
        immediate->imm >> branch_addr_op2_src->get(BranchAddrSrc2::IMM);
        control->alu_op2_ctrl >> branch_addr_op2_src->select;

        branch_addr_op1_src->out >> branch->pc_value;
        branch_addr_op2_src->out >> branch->offset_value;

        // ----------------- Part 4. TODO: modify PC4 source when using branch prediction ----------------
        // Hint: After you implement branch prediction, the PC4 source of pc_src_id should be changed.
        //       Think about why and how to change the PC4 source.
        pc_4->out >> pc_src_id->get(PcSrcBranch::PC4);
        branch->target_address >> pc_src_id->get(PcSrcBranch::BRANCH);
        branch->branch_actual_select >> pc_src_id->select;

        // Update results in BTB and BP
        ifid_reg->instr_out >> branch->instr;
        ifid_reg->pc_out >> branch_predictor->branch_address_update;
        branch->should_update >> branch_predictor->should_update;
        ifid_reg->pc_out >> branch_target_buffer->branch_address_update;
        branch->should_update >> branch_target_buffer->should_update;
        branch->taken >> branch_predictor->branch_result_update;
        branch->target_address >> branch_target_buffer->target_address_update;

        pc_src_id->out >> pc_src->get(PcSrcFinal::ACTUAL);
        branch->branch_final_select >> pc_src->select;

        // -----------------------------------------------------------------------
        // ALU
        idex_reg->r1_out >> alu_op1_src->get(AluSrc1::REG1);
        idex_reg->pc_out >> alu_op1_src->get(AluSrc1::PC);
        idex_reg->alu_op1_ctrl_out >> alu_op1_src->select;

        idex_reg->r2_out >> alu_op2_src->get(AluSrc2::REG2);
        idex_reg->imm_out >> alu_op2_src->get(AluSrc2::IMM);
        idex_reg->alu_op2_ctrl_out >> alu_op2_src->select;

        alu_op1_src->out >> alu->op1;
        alu_op2_src->out >> alu->op2;

        idex_reg->alu_ctrl_out >> alu->ctrl;

        // -----------------------------------------------------------------------
        // Data memory
        exmem_reg->alures_out >> data_mem->addr;
        exmem_reg->mem_do_write_out >> data_mem->wr_en;
        exmem_reg->r2_out >> data_mem->data_in;
        exmem_reg->mem_op_out >> data_mem->op;
        data_mem->mem->setMemory(m_memory);

        // -----------------------------------------------------------------------
        // Ecall checker
        idex_reg->opcode_out >> ecallChecker->opcode;
        ecallChecker->setSysCallSignal(&handleSysCall);
        hzunit->stallEcallHandling >> ecallChecker->stallEcallHandling;

        // -----------------------------------------------------------------------
        // IF/ID
        pc_4->out >> ifid_reg->pc4_in;
        pc_reg->out >> ifid_reg->pc_in;
        instr_mem->data_out >> ifid_reg->instr_in;
        hzunit->hazardFEEnable >> ifid_reg->enable;

        branch->wrong_predict_pc >> *wrong_pc_hazard_or->in[0];

        ecallChecker->syscallExit >> *wrong_pc_hazard_or->in[1];
        wrong_pc_hazard_or->out >> ifid_reg->clear;
        1 >> ifid_reg->valid_in;  // Always valid unless register is cleared

        // -----------------------------------------------------------------------
        // ID/EX
        hzunit->hazardIDEXEnable >> idex_reg->enable;
        hzunit->hazardIDEXClear >> idex_reg->stalled_in;

        ecallChecker->syscallExit >> *syscall_hazard_or->in[0];
        hzunit->hazardIDEXClear >> *syscall_hazard_or->in[1];
        syscall_hazard_or->out >> idex_reg->clear;

        // Data
        ifid_reg->pc4_out >> idex_reg->pc4_in;
        ifid_reg->pc_out >> idex_reg->pc_in;
        registerFile->r1_out >> idex_reg->r1_in;
        registerFile->r2_out >> idex_reg->r2_in;
        immediate->imm >> idex_reg->imm_in;

        // Control
        decode->wr_reg_idx >> idex_reg->wr_reg_idx_in;
        control->reg_wr_src_ctrl >> idex_reg->reg_wr_src_ctrl_in;
        control->reg_do_write_ctrl >> idex_reg->reg_do_write_in;
        control->alu_op1_ctrl >> idex_reg->alu_op1_ctrl_in;
        control->alu_op2_ctrl >> idex_reg->alu_op2_ctrl_in;
        control->mem_do_write_ctrl >> idex_reg->mem_do_write_in;
        control->mem_do_read_ctrl >> idex_reg->mem_do_read_in;
        control->alu_ctrl >> idex_reg->alu_ctrl_in;
        control->mem_ctrl >> idex_reg->mem_op_in;
        control->comp_ctrl >> idex_reg->br_op_in;
        control->do_branch >> idex_reg->do_br_in;
        control->do_jump >> idex_reg->do_jmp_in;
        decode->opcode >> idex_reg->opcode_in;

        ifid_reg->valid_out >> idex_reg->valid_in;

        // -----------------------------------------------------------------------
        // EX/MEM
        hzunit->hazardEXMEMClear >> exmem_reg->clear;
        hzunit->hazardEXMEMClear >> *mem_stalled_or->in[0];
        hzunit->hazardEXMEMEnable >> exmem_reg->enable;
        idex_reg->stalled_out >> *mem_stalled_or->in[1];
        mem_stalled_or->out >> exmem_reg->stalled_in;

        // Data
        idex_reg->pc_out >> exmem_reg->pc_in;
        idex_reg->pc4_out >> exmem_reg->pc4_in;
        idex_reg->r2_out >> exmem_reg->r2_in;
        alu->res >> exmem_reg->alures_in;

        // Control
        idex_reg->reg_wr_src_ctrl_out >> exmem_reg->reg_wr_src_ctrl_in;
        idex_reg->wr_reg_idx_out >> exmem_reg->wr_reg_idx_in;
        idex_reg->reg_do_write_out >> exmem_reg->reg_do_write_in;
        idex_reg->mem_do_write_out >> exmem_reg->mem_do_write_in;
        idex_reg->mem_do_read_out >> exmem_reg->mem_do_read_in;
        idex_reg->mem_op_out >> exmem_reg->mem_op_in;

        idex_reg->valid_out >> exmem_reg->valid_in;

        // -----------------------------------------------------------------------
        // MEM/WB
        0 >> memwb_reg->clear; // a more complete solution is to control all clear/enable signals by hzunit
        data_mem->data_invalid >> *memwb_stalled_or->in[0];
        exmem_reg->stalled_out >> *memwb_stalled_or->in[1];
        memwb_stalled_or->out >> memwb_reg->stalled_in;
        hzunit->hazardMEMWBEnable >> memwb_reg->enable;

        // Data
        exmem_reg->pc_out >> memwb_reg->pc_in;
        exmem_reg->pc4_out >> memwb_reg->pc4_in;
        exmem_reg->alures_out >> memwb_reg->alures_in;
        data_mem->data_out >> memwb_reg->mem_read_in;

        // Control
        exmem_reg->reg_wr_src_ctrl_out >> memwb_reg->reg_wr_src_ctrl_in;
        exmem_reg->wr_reg_idx_out >> memwb_reg->wr_reg_idx_in;
        exmem_reg->reg_do_write_out >> memwb_reg->reg_do_write_in;

        exmem_reg->valid_out >> memwb_reg->valid_in;

        // -----------------------------------------------------------------------
        // Hazard detection unit
        // --------------------------- Part 2, TODO: add links of ports of hazard unit ---------------------------
        // Hint: the existing ports of hzunit now is not enough for detecting hazard. So you need to find
        //       out what else you need and add input ports in hzunit.
        decode->r1_reg_idx >> hzunit->id_reg1_idx;
        decode->r2_reg_idx >> hzunit->id_reg2_idx;

        idex_reg->mem_do_read_out >> hzunit->ex_do_mem_read_en;
        idex_reg->wr_reg_idx_out >> hzunit->ex_reg_wr_idx;

        exmem_reg->reg_do_write_out >> hzunit->mem_do_reg_write;
        memwb_reg->reg_do_write_out >> hzunit->wb_do_reg_write;

        idex_reg->opcode_out >> hzunit->opcode;
        data_mem->data_invalid >> hzunit->mem_wait;
        alu->data_invalid >> hzunit->alu_wait;
    }

    // Design subcomponents
    SUBCOMPONENT(registerFile, RegisterFile<true>);
    SUBCOMPONENT(alu, ALU);
    SUBCOMPONENT(control, Control);
    SUBCOMPONENT(immediate, Immediate);
    SUBCOMPONENT(decode, Decode);
    SUBCOMPONENT(branch, BranchID);
    SUBCOMPONENT(pc_4, Adder<RV_REG_WIDTH>);
    SUBCOMPONENT(branch_predictor, BranchPredictor);
    SUBCOMPONENT(branch_target_buffer, BranchTargetBuffer);
    SUBCOMPONENT(branch_checker, BranchChecker);

    // Registers
    SUBCOMPONENT(pc_reg, RegisterClEn<RV_REG_WIDTH>);

    // Stage seperating registers
    SUBCOMPONENT(ifid_reg, RV5S_HZ_IFID);
    SUBCOMPONENT(idex_reg, RV5S_HZ_IDEX);
    SUBCOMPONENT(exmem_reg, RV5S_HZ_EXMEM);
    SUBCOMPONENT(memwb_reg, RV5S_HZ_MEMWB);

    // Multiplexers
    /*
     * A multiplexer (or mux; spelled sometimes as multiplexor), also known as a data selector,
     * is a device that selects between several input signals and forwards the selected input to
     * a single output.
     *
     * An EnumMultiplexer here is a multiplexer that choose from two values of two ports
     * using the specified Enum type to select the port.
     *
     * For example:
     * -·-·-·-·-·-·-·-·-·-·- Example -·-·-·-·-·-·-·-·-·-·-
     * Enum(ValueSrc, Src0, Src1);
     * SUBCOMPONENT(mp, TYPE(EnumMultiplexer<Value, ValueWidth>));
     * src0->out >> mp->get(ValueSrc::Src0);
     * src1->out >> mp->get(ValueSrc::Src1);
     * ValueSrc srcSelect() { return ValueSrc::Src0; }
     * srcSelect() >> mp->select;
     * Then the value of mp->out will be src0->out.
     * -·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-
     *
     * See external/VSRTL/vsrtl_multiplexer.h for more information of multiplexers in Ripes.
     * See https://en.wikipedia.org/wiki/Multiplexer for more information of multiplexers.
     *
     */
    SUBCOMPONENT(reg_wr_src, TYPE(EnumMultiplexer<RegWrSrc, RV_REG_WIDTH>));

    SUBCOMPONENT(pc_src, TYPE(EnumMultiplexer<PcSrcFinal, RV_REG_WIDTH>));
    SUBCOMPONENT(pc_src_if, TYPE(EnumMultiplexer<PcSrcBranch, RV_REG_WIDTH>));
    SUBCOMPONENT(pc_src_id, TYPE(EnumMultiplexer<PcSrcBranch, RV_REG_WIDTH>));

    SUBCOMPONENT(alu_op1_src, TYPE(EnumMultiplexer<AluSrc1, RV_REG_WIDTH>));
    SUBCOMPONENT(alu_op2_src, TYPE(EnumMultiplexer<AluSrc2, RV_REG_WIDTH>));
    SUBCOMPONENT(branch_addr_op1_src, TYPE(EnumMultiplexer<BranchAddrSrc1, RV_REG_WIDTH>));
    SUBCOMPONENT(branch_addr_op2_src, TYPE(EnumMultiplexer<BranchAddrSrc2, RV_REG_WIDTH>));

    // Memories
    SUBCOMPONENT(instr_mem, TYPE(ROM<RV_REG_WIDTH, RV_INSTR_WIDTH>));
    SUBCOMPONENT(data_mem, TYPE(RVMemory<RV_REG_WIDTH, RV_REG_WIDTH>));

    //hazard detection units
    SUBCOMPONENT(hzunit, HZ_HazardUnit);

    // Gates
    /*
     * A logic gate is an idealized model of computation or physical electronic device implementing a
     * Boolean function, a logical operation performed on one or more binary inputs that produces a
     * single binary output.
     *
     * In Ripes, there are four kinds of gates that can be used (Or, Xor, And, Not). It implements the
     * corresponding function just as the name of the gates.
     *
     * For example:
     * -·-·-·-·-·-·-·-·-·-·- Example -·-·-·-·-·-·-·-·-·-·-
     * SUBCOMPONENT(x_y_or, TYPE(Or<1, 2>))
     * x >> *x_y_or->in[0]
     * y >> *x_y_or->in[1]
     * Then x_y_or->out should be the value of (x || y).
     * -·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-·-
     *
     * See external/VSRTL/vsrtl_logicgate.h for more information of gates in Ripes.
     * See https://en.wikipedia.org/wiki/Logic_gate for more information of logic gates.
     *
     */
    SUBCOMPONENT(wrong_pc_hazard_or, TYPE(Or<1, 2>)); // wrong_pc_hazard_or (Or gate): the result is (syscall || the predicted result is wrong).
    SUBCOMPONENT(syscall_hazard_or, TYPE(Or<1, 2>)); // syscall_hazard_or (Or gate): the result is (syscall || ID/EX reg is clear due to hazard).

    SUBCOMPONENT(mem_stalled_or, TYPE(Or<1, 2>));
    SUBCOMPONENT(memwb_stalled_or, TYPE(Or<1, 2>));

    // Address spaces
    ADDRESSSPACE(m_memory);
    ADDRESSSPACE(m_regMem);

    SUBCOMPONENT(ecallChecker, EcallChecker);

    // Ripes interface compliance
    unsigned int stageCount() const override { return STAGECOUNT; }
    unsigned int getPcForStage(unsigned int idx) const override {
        // clang-format off
        switch (idx) {
            case IF: return pc_reg->out.uValue();
            case ID: return ifid_reg->pc_out.uValue();
            case EX: return idex_reg->pc_out.uValue();
            case MEM: return exmem_reg->pc_out.uValue();
            case WB: return memwb_reg->pc_out.uValue();
            default: assert(false && "Processor does not contain stage");
        }
        Q_UNREACHABLE();
        // clang-format on
    }
    unsigned int nextFetchedAddress() const override { return pc_src->out.uValue(); }
    QString stageName(unsigned int idx) const override {
        // clang-format off
        switch (idx) {
            case IF: return "IF";
            case ID: return "ID";
            case EX: return "EX";
            case MEM: return "MEM";
            case WB: return "WB";
            default: assert(false && "Processor does not contain stage");
        }
        Q_UNREACHABLE();
        // clang-format on
    }
    StageInfo stageInfo(unsigned int stage) const override {
        bool stageValid = true;
        // Has the pipeline stage been filled?
        stageValid &= stage <= m_cycleCount;

        switch(stage){
        case ID: stageValid &= ifid_reg->valid_out.uValue(); break;
        case EX: stageValid &= idex_reg->valid_out.uValue(); break;
        case MEM: stageValid &= exmem_reg->valid_out.uValue(); break;
        case WB: stageValid &= memwb_reg->valid_out.uValue(); break;
        default: case IF: break;
        }

        switch(stage){
        case ID: stageValid &= isExecutableAddress(ifid_reg->pc_out.uValue()); break;
        case EX: stageValid &= isExecutableAddress(idex_reg->pc_out.uValue()); break;
        case MEM: stageValid &= isExecutableAddress(exmem_reg->pc_out.uValue()); break;
        case WB: stageValid &= isExecutableAddress(memwb_reg->pc_out.uValue()); break;
        default: case IF: stageValid &= isExecutableAddress(pc_reg->out.uValue()); break;
        }

        if(stage < EX){
            stageValid &= !ecallChecker->isSysCallExiting();
        }

        // Gather stage state info
        StageInfo::State state = StageInfo ::State::None;
        switch (stage) {
            case IF:
                break;
            case ID:
                if (m_cycleCount > ID && ifid_reg->valid_out.uValue() == 0) {
                    state = StageInfo::State::Flushed;
                }
                break;
            case EX: {
                if (idex_reg->stalled_out.uValue() == 1) {
                    state = StageInfo::State::Stalled;
                } else if (m_cycleCount > EX && idex_reg->valid_out.uValue() == 0) {
                    state = StageInfo::State::Flushed;
                }
                break;
            }
            case MEM: {
                if (exmem_reg->stalled_out.uValue() == 1) {
                    state = StageInfo::State::Stalled;
                } else if (m_cycleCount > MEM && exmem_reg->valid_out.uValue() == 0) {
                    state = StageInfo::State::Flushed;
                }
                break;
            }
            case WB: {
                if (memwb_reg->stalled_out.uValue() == 1) {
                    state = StageInfo::State::Stalled;
                } else if (m_cycleCount > WB && memwb_reg->valid_out.uValue() == 0) {
                    state = StageInfo::State::Flushed;
                }
                break;
            }
        }

        return StageInfo({getPcForStage(stage), stageValid, state});
    }

    void setProgramCounter(uint32_t address) override {
        pc_reg->forceValue(0, address);
        propagateDesign();
    }
    void setPCInitialValue(uint32_t address) override { pc_reg->setInitValue(address); }
    SparseArray& getMemory() override { return *m_memory; }
    unsigned int getRegister(RegisterFileType rfid, unsigned i) const override { return registerFile->getRegister(i); }
    SparseArray& getArchRegisters() override { return *m_regMem; }
    void finalize(const FinalizeReason& fr) override {
        if (fr.exitSyscall && !ecallChecker->isSysCallExiting()) {
            // An exit system call was executed. Record the cycle of the execution, and enable the ecallChecker's system
            // call exiting signal.
            m_syscallExitCycle = m_cycleCount;
        }
        ecallChecker->setSysCallExiting(ecallChecker->isSysCallExiting() || fr.exitSyscall);
    }

    const Component* getDataMemory() const override { return data_mem; }
    const Component* getInstrMemory() const override { return instr_mem; }

    bool finished() const override {
        // The processor is finished when there are no more valid instructions in the pipeline
        bool allStagesInvalid = true;
        for (int stage = IF; stage < STAGECOUNT; stage++) {
            allStagesInvalid &= !stageInfo(stage).stage_valid;
            if (!allStagesInvalid)
                break;
        }
        return allStagesInvalid;
    }

    void setRegister(RegisterFileType rfid, unsigned i, uint32_t v) override {
        setSynchronousValue(registerFile->_wr_mem, i, v);
    }

    void clock() override {
        // An instruction has been retired if the instruction in the WB stage is valid and the PC is within the
        // executable range of the program
        if (memwb_reg->valid_out.uValue() != 0 && isExecutableAddress(memwb_reg->pc_out.uValue())) {
            m_instructionsRetired++;
        }

        RipesProcessor::clock();
    }

    void reverse() override {
        if (m_syscallExitCycle != -1 && m_cycleCount == m_syscallExitCycle) {
            // We are about to undo an exit syscall instruction. In this case, the syscall exiting sequence should
            // be terminate
            ecallChecker->setSysCallExiting(false);
            m_syscallExitCycle = -1;
        }
        RipesProcessor::reverse();
        if (memwb_reg->valid_out.uValue() != 0 && isExecutableAddress(memwb_reg->pc_out.uValue())) {
            m_instructionsRetired--;
        }
        branch_predictor->reset();
        branch_target_buffer->reset();
    }

    void reset() override {
        ecallChecker->setSysCallExiting(false);
        RipesProcessor::reset();
        m_syscallExitCycle = -1;
        branch_predictor->reset();
        branch_target_buffer->reset();
    }

    static const ISAInfoBase* ISA() {
        static auto s_isa = ISAInfo<ISA::RV32I>(QStringList{"M"});
        return &s_isa;
    }

    const ISAInfoBase* supportsISA() const override { return ISA(); };
    const ISAInfoBase* implementsISA() const override { return m_enabledISA.get(); };

    const std::set<RegisterFileType> registerFiles() const override {
        std::set<RegisterFileType> rfs;
        rfs.insert(RegisterFileType::GPR);

        if (implementsISA()->extensionEnabled("F")) {
            rfs.insert(RegisterFileType::FPR);
        }
        return rfs;
    }

private:
    /**
     * @brief m_syscallExitCycle
     * The variable will contain the cycle of which an exit system call was executed. From this, we may determine
     * when we roll back an exit system call during rewinding.
     */
    long long m_syscallExitCycle = -1;
    std::shared_ptr<ISAInfo<ISA::RV32I>> m_enabledISA;
};

}  // namespace core
}  // namespace vsrtl
