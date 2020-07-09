#pragma once
#include "common.h"

class InterInst
{
    string label;

    Operator op;
    Var* arg1;
    Var* arg2;
    Var* result;

    Fun* fun;
    InterInst* target;

    void init();

public:

    //构造
    InterInst (Operator op,Var *rs,Var *arg1,Var *arg2=NULL);//一般运算指令
    InterInst (Operator op,Fun *fun,Var *rs=NULL);//函数调用指令,ENTRY,EXIT
    InterInst (Operator op,Var *arg1=NULL);//参数进栈指令,NOP
    InterInst ();//产生唯一标号
    InterInst (Operator op,InterInst *tar,Var *arg1=NULL,Var *arg2=NULL);//条件跳转指令,return

    //外部调用接口
    bool isJcond();//是否条件转移指令JT,JF,Jcond
    bool isJmp();//是否直接转移指令JMP,return
    bool isLb();//是否是标签
    bool isDec();//是否是声明
    bool isExpr();//是基本类型表达式运算,可以对指针取值
    bool unknown();//不确定运算结果影响的运算(指针赋值，函数调用)

    Operator getOp();//获取操作符
    InterInst* getTarget();//获取跳转指令的目标指令
    Var* getResult();//获取返回值
    Var* getArg1();//获取第一个参数
    Var* getArg2();//获取第二个参数
    string getLabel();//获取标签
    Fun* getFun();//获取函数对象
    void setArg1(Var*arg1);//设置第一个参数
    void toString();
};


class InterCode
{
    vector<InterInst*> code;

public:
    ~InterCode();

    void addInst(InterInst* inst);
    void toString();
    vector<InterInst*>& getCode();
};