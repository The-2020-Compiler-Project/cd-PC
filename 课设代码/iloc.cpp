#include "iloc.h"
#include "platform.h"
#include "Symbol.h"
#include <sstream>

Arm::Arm(string op, string rs, string a1, string a2, string add)
: opcode(op), result(rs), arg1(a1), arg2(a2), addition(add), dead(false)
{}

// 替换指令内容
void Arm::replace(string op, string rs, string a1, string a2, string add)
{
    opcode = op;
    result = rs;
    arg1 = a1;
    arg2 = a2;
    addition = add;
}

//设置为无效指令
void Arm::setDead()
{
    dead = true;
}

//输出函数
string Arm::outPut()
{
    if (dead) return "";
    if (opcode == "") return "";

    string ret = opcode + " " + result;
    if (arg1 != "") ret += "," + arg1;
    if (arg2 != "") ret += "," + arg2;
    if (addition != "") ret += "," + addition;
    return ret;
}

#define emit(args...) code.push_back(new Arm(args))

string ILoc::toStr(int num, int flag)
{
    string ret = flag? "#": "";
    stringstream ss;
    ss<<num;
    ret += ss.str();
    return ret;
}

void ILoc::label(string name)
{
    emit(name, ":");
}

void ILoc::comment(string str)
{
    emit("#", str);
}

void ILoc::inst(string op, string rs)
{
    emit(op, rs);
}

void ILoc::inst(string op, string rs, string arg1)
{
    emit(op, rs, arg1);
}

void ILoc::inst(string op, string rs, string arg1, string arg2)
{
    emit(op, rs, arg1, arg2);
}

//如果立即数合法用 mov，否则用 ldr 伪指令
void ILoc::ldr_imm(string rsReg, int num)
{
    if (Plat::constExpr(num)) {
        emit("mov", rsReg, toStr(num));
    } else {
        ldr_lb(rsReg, toStr(num, false));
    }
}

//加载标号处的值
void ILoc::ldr_lb(string rsReg, string name)
{
    emit("ldr", rsReg, "="+name);
}

void ILoc::ldr_base(string rsReg, string base, int disp, bool isChar)
{
    if (Plat::isDisp(disp)) {
        if (disp) base += ","+toStr(disp);
    } else {
        ldr_imm(rsReg, disp);
        base += ","+rsReg;
    }
    base = "[" + base + "]";
    emit(isChar?"ldrb":"ldr", rsReg, base);
}

void ILoc::str_base(string srcReg, string base, int disp, string tmpReg, bool isChar)
{
    if (Plat::isDisp(disp)) {
        if (disp) base += ","+toStr(disp);
    } else {
        ldr_imm(tmpReg, disp);
        base += ","+tmpReg;
    }
    base = "[" + base + "]";
    emit(isChar?"strb":"str", srcReg, base);
}

void ILoc::init_var(Var* var, string initReg, string tmpReg)
{
    if (! var->unInit()) {
        if (var->isBase()) //int变量
            ldr_imm(initReg, var->getVal());
        else //指针变量
            ldr_lb(initReg, var->getPtrVal());
        str_var(initReg, var, tmpReg);
    }
}

void ILoc::ldr_var(string rsReg, Var* var)
{
    if (! var) return;
    if (var->notConst()) {
        int id = var->regId;
        if (id != -1) {
            emit("mov", rsReg, Plat::regName[id]);
        } else {
            int off = var->getOffset();
            bool isGlb = !off; //当 off 为 0 时表示全局变量
            bool isVar = !var->getArray(); //不是数组
            bool isChar = var->isChar(); //是否为字符变量

            if (isGlb) {
                ldr_lb(rsReg, var->getName()); //ldr rx, =glb
                if (isVar) ldr_base(rsReg, rsReg, 0, isChar);
            } else {
                if (isVar)
                    ldr_base(rsReg, "fp", off, isChar); //ldr r8, [fp, #-8]
                else
                    leaStack(rsReg, off); //add r8, fp, #-8
            }
        }
    } else {
        if (var->isBase())
            ldr_imm(rsReg, var->getVal());
        else
            ldr_lb(rsReg, var->getName());
    }
}

void ILoc::lea_var(string rsReg, Var* var)
{
    int off = var->getOffset();
    bool isGlb = !off;
    if (isGlb)
        ldr_lb(rsReg, var->getName());
    else
        leaStack(rsReg, off);
}

void ILoc::str_var(string srcReg, Var* var, string tmpReg)
{
    int id = var->regId;
    bool isChar = var->isChar();
    if (id != -1) {
        emit("mov", Plat::regName[id], srcReg);
    } else {
        int off = var->getOffset();
        bool isGlb = !off;
        if (isGlb) {
            ldr_lb(tmpReg, var->getName());
            str_base(srcReg, tmpReg, 0, tmpReg, isChar); //str r8, [r9]
        } else {
            str_base(srcReg, "fp", off, tmpReg, isChar); //str r8, [fp, #-16]
        }
    }
    nop();
}

void ILoc::leaStack(string rsReg, int off)
{
    if (Plat::constExpr(off)) {
        emit("add", rsReg, "fp", toStr(off)); //add r8, fp, #-16
    } else {
        ldr_imm(rsReg, off); //ldr r8, =100
        emit("add", rsReg, "fp", rsReg); //add r8, fp, r8
    }
}

void ILoc::allocStack(Fun* fun, string tmpReg)
{
    int base = (fun->isRelocated()? Plat::stackBase_protect: Plat::stackBase);
    int off = fun->getMaxDep() - base;
    if (Plat::constExpr(off)) {
        emit("sub", "sp", "sp", toStr(off)); //sub sp, sp, #16
    } else {
        ldr_imm(tmpReg, off); //ldr r8, =257
        emit("sub", "sp", "sp", tmpReg); //sub sp, sp, r8
    }
}

void ILoc::ldr_args(Fun* fun)
{
    vector<Var*> args = fun->getParaVar();
    for (int i = 0; i < args.size(); i++) {
        Var* arg = args[i];
        if (arg->regId != -1)
            ldr_base(Plat::regName[arg->regId], "fp", arg->getOffset(), false);
    }
}

void ILoc::call_lib(string fun, string rsReg, string reg1, string reg2)
{
    emit("stmfd", "sp!", "{r0-r7}");
    emit("mov", "r0", reg1);
    emit("mov", "r1", reg2);
    emit("bl", fun);
    emit("mov", rsReg, "r0");
    emit("ldmfd", "sp!", "{r0-r7}");
}

void ILoc::call_fun(Fun* fun, string tmpReg)
{
    string funName = fun->getName();
    emit("bl", funName);
    int off = fun->getParaVar().size() * 4;
    if (Plat::constExpr(off)) {
        emit("add", "sp", "sp", toStr(off)); //add sp, sp, #4
    } else {
        ldr_imm(tmpReg, off); //ldr r8, =512
        emit("add", "sp", "sp", tmpReg); //add sp, sp, r8
    }
}

void ILoc::logic_and(string rsReg, string reg1, string reg2)
{
    emit("cmp", reg1, "#0");
    emit("moveq", rsReg, "#0");
    emit("movne", rsReg, "#1");
    emit("cmpne", reg2, "#0");
    emit("moveq", rsReg, "#0");
}

void ILoc::logic_or(string rsReg, string reg1, string reg2)
{
    emit("cmp", reg1, "#0");
    emit("moveq", rsReg, "#0");
    emit("movne", rsReg, "#1");
    emit("cmpeq", reg2, "#0");
    emit("movne", rsReg, "#1");
}

void ILoc::logic_not(string rsReg, string reg1)
{
    emit("cmp", reg1, "#0");
    emit("moveq", rsReg, "#0");
    emit("movne", rsReg, "#1");
}

void ILoc::cmp(string cmp, string cmpnot, string rsReg, string reg1, string reg2)
{
    emit("cmp", reg1, reg2);
    emit("mov"+cmp, rsReg, "#1");
    emit("mov"+cmpnot, rsReg, "#0");
}

void ILoc::nop() {emit("");}

ILoc::~ILoc()
{
    for (list<Arm*>::iterator it = code.begin(); it != code.end(); it++)
        delete (*it);
}

void ILoc::outPut(FILE* fp)
{
    for (list<Arm*>::iterator it = code.begin(); it != code.end(); it++) {
        string s = (*it)->outPut();
        if (s != "") fprintf(fp, "\t%s\n", s.c_str());
    }
}

list<Arm*>& ILoc::getCode() {return code;}