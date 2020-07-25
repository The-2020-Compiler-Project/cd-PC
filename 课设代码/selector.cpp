#include "selector.h"
#include "Inter.h"
#include "Symbol.h"
#include "platform.h"

Selector::Selector(vector<InterInst*>& irCode, ILoc& ilocCode)
: ir(irCode), iloc(ilocCode) {}

void Selector::select()
{
    for (int i = 0; i < ir.size(); i++)
        translate(ir[i]);
}

void Selector::translate(InterInst* inst)
{
    string label = inst->getLabel();
    if (label != "") {
        iloc.label(label);
        return;
    }

    Operator op = inst->getOp();
    if (op == OP_ENTRY) {
        iloc.comment("Fun Entry");
        Fun* fun = inst->getFun();
        iloc.inst("mov", "ip", "sp");
        if (fun->isRelocated())
            iloc.inst("stmfd", "sp!", "{r0-r7, fp, ip, lr, pc}");
        else
            iloc.inst("stmfd", "sp!", "{fp, ip, lr, pc}");
        iloc.inst("sub", "fp", "ip", "#4");
        iloc.comment("Alloc StackFrame");
        iloc.allocStack(fun, "r8");
        iloc.comment("Load Args to Registers");
        iloc.ldr_args(fun);
        iloc.comment("Code of Fun");
    }
    else if (op == OP_EXIT) {
        iloc.comment("Fun Exit");
        if (inst->getFun()->isRelocated())
            iloc.inst("ldmea", "fp", "{r0-r7, fp, sp, pc}");
        else
            iloc.inst("ldmea", "fp", "{fp,sp,pc}");
    }
    else if (op == OP_DEC) {
        iloc.init_var(inst->getArg1(), "r8", "r9");
    }
    else if (op == OP_LEA) {
        iloc.lea_var("r8", inst->getArg1());
        iloc.str_var("r8", inst->getResult(), "r9");
    }
    else if (op == OP_SET) {
        iloc.ldr_var("r8", inst->getResult());
        iloc.ldr_var("r9", inst->getArg1());
        iloc.str_base("r8", "r9", 0, "r9", inst->getArg1()->isCharPtr());
        iloc.nop();
    }
    else if (op==OP_JMP || op==OP_JT || op==OP_JF || op==OP_JNE || op==OP_RET || op==OP_RETV) {
        string des = inst->getTarget()->getLabel();
        iloc.ldr_var("r8", inst->getArg1());
        iloc.ldr_var("r9", inst->getArg2());
        switch (op) {
            case OP_JMP: iloc.inst("b", des); break;
            case OP_JT: iloc.inst("cmp", "r8", "#0"); iloc.inst("bne", des); break;
            case OP_JF: iloc.inst("cmp", "r8", "#0"); iloc.inst("beq", des); break;
            case OP_RET: iloc.inst("b", des); break;
            case OP_RETV: iloc.inst("b", des); break;
            case OP_JNE:
                iloc.cmp("ne", "eq", "r8", "r8", "r9");
                iloc.inst("cmp", "r8", "#0");
                iloc.inst("bne", des);
                break;
        }
    }
    else if (op == OP_ARG) {
        iloc.ldr_var("r8", inst->getArg1());
        iloc.inst("stmfd", "sp!", "{r8}");
    }
    else if (op == OP_CALL) {
        iloc.call_fun(inst->getFun(), "r9");
        iloc.str_var("r8", inst->getResult(), "r9");
    }
    else if (op == OP_PROC) {
        iloc.call_fun(inst->getFun(), "r9");
    }
    else {
        Var* rs = inst->getResult();
        Var* arg1 = inst->getArg1();
        Var* arg2 = inst->getArg2();
        iloc.ldr_var("r8", arg1);
        iloc.ldr_var("r9", arg2);
        switch (op) {
            case OP_AS: break;
            case OP_ADD: iloc.inst("add", "r8", "r8", "r9"); break;
            case OP_SUB: iloc.inst("sub", "r8", "r8", "r9"); break;
            case OP_MUL: iloc.inst("mul", "r10", "r8", "r9"); iloc.inst("mov", "r8", "r10"); break;
            case OP_DIV: iloc.call_lib("__divsi3", "r8", "r8", "r9"); break;
            case OP_MOD: iloc.call_lib("__modsi3", "r8", "r8", "r9"); break;
            case OP_NEG: iloc.inst("rsb", "r8", "r8", "#0"); break;

            case OP_GT:iloc.cmp("gt","le","r8","r8","r9");break;
            case OP_GE:iloc.cmp("ge","lt","r8","r8","r9");break;
            case OP_LT:iloc.cmp("lt","ge","r8","r8","r9");break;
            case OP_LE:iloc.cmp("le","gt","r8","r8","r9");break;
            case OP_EQU:iloc.cmp("eq","ne","r8","r8","r9");break;
            case OP_NE:iloc.cmp("ne","eq","r8","r8","r9");break;

            case OP_AND:iloc.logic_and("r8","r8","r9");break;
            case OP_OR:iloc.logic_or("r8","r8","r9");break;
            case OP_NOT:iloc.logic_not("r8","r8");break;

            case OP_GET:iloc.ldr_base("r8","r8",0,rs->isChar());break;//a=*p
        }
        iloc.str_var("r8", rs, "r9");
    }
}