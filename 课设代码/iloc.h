#pragma once
#include "common.h"

/*
 * 汇编指令的数据保存结构
 */
struct Arm {
    bool dead; //当前代码是否有效
    string opcode; //代码的助记符
    string result; //代码的结果
    string arg1; //第一操作数
    string arg2; //第二操作数
    string addition; //附加操作

    Arm(string op, string rs="", string a1="", string a2="", string add="");
    void replace(string op, string rs="", string a1="", string a2="", string add="");
    void setDead();
    string outPut();
};

/*
 * 用来提供接口生成并记录 Arm 结构体
 */
class ILoc {
    list<Arm*> code; //保存的代码序列，以函数为单位

    //将整数转成对应的立即数字符串，flag 控制井号
    string toStr(int num, int flag=true);

    //ldr 操作
    void ldr_imm(string rsReg, int num); //加载立即数到指定寄存器内
    void ldr_lb(string rsReg, string name); //加载符号值
    void leaStack(string rsReg, int off); //根据偏移来从当前栈中加载变量

public:
    //基址寻址加载，如 ldr r0, [fp, #100]
    void ldr_base(string rsReg, string base, int disp, bool isChar);
    //基址寻址保存，如 str r0, [fp, #100]
    void str_base(string srcReg, string base, int disp, string tmpReg, bool isChar);

    //产生标签
    void label(string name);
    //产生注释
    void comment(string str);
    //无操作数的指令
    void inst(string op, string rs);
    //一个操作数的指令
    void inst(string op, string rs, string arg1);
    //两个操作数的指令
    void inst(string op, string rs, string arg1, string arg2);

    //初始化变量
    void init_var(Var* var, string initReg, string tmpReg);
    //加载一个变量到寄存器
    void ldr_var(string rsReg, Var* var);
    //加载一个变量的地址到寄存器
    void lea_var(string rsReg, Var* var);
    //保存寄存器到变量
    void str_var(string srcReg, Var* var, string addrReg);

    //调用库函数
    void call_lib(string fun, string rsReg, string reg1, string reg2);
    //调用一般函数
    void call_fun(Fun* fun, string tmpReg);

    //分配一个栈帧
    void allocStack(Fun* fun, string tmpReg);
    void ldr_args(Fun* fun);

    //逻辑运算
    void logic_and(string rsReg, string reg1, string reg2);
    void logic_or(string rsReg, string reg1, string reg2);
    void logic_not(string rsReg, string reg1);

    //比较
    void cmp(string cmp, string cmpnot, string rsReg, string reg1, string reg2);

    //占位
    void nop();

    //输出汇编
    void outPut(FILE* file);
    //获取汇编代码
    list<Arm*>& getCode();
    ~ILoc();
};