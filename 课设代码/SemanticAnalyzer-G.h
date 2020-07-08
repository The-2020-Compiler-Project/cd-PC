#pragma once
#include "common.h"
#include <sstream>

// 是否打印语义错误信息
#define SHOW_SEMERROR false

#if (SHOW_SEMERROR)
    #define SEMERROR(code) Error::semError(code);
#else
    #define SEMERROR(code)
#endif

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


class GenIR
{
    static int lbNum;
    SymTab &symtab;

    vector< InterInst* > heads;
    vector< InterInst* > tails;
    void push(InterInst* head, InterInst* tail);
    void pop();

    // 函数调用
    void genPara(Var* arg);

    // 双目运算
    Var* genAssign(Var* lvar, Var* rval);

    Var* genAdd(Var* lval, Var* rval);
    Var* genSub(Var* lval, Var* rval);
    Var* twoOpExceptAddSub(Var* lval, Tag opt, Var* rval);

    Var* genNot(Var* val);
    Var* genMinus(Var* val);
    Var* genPtr(Var* val);
    Var* genIncLorDecL(Tag opt, Var* val);
    Var* genIncRorDecR(Tag opt, Var* val);

public:

    GenIR(SymTab &tab);

    Var* genAssign(Var* val);

    Var* genArray(Var* array, Var* index);
    Var* genCall(Fun* func, vector<Var*> & args);
    Var* genTwoOp(Var* lval, Tag opt, Var* rval);
    Var* genOneOpLeft(Tag opt, Var* val);
    Var* genOneOpRight(Var* val, Tag opt);

    //产生复合语句
    void genWhileHead(InterInst*& _while,InterInst*& _exit);//while循环头部
    void genWhileCond(Var*cond,InterInst* _exit);//while条件
    void genWhileTail(InterInst*& _while,InterInst*& _exit);//while尾部
    void genDoWhileHead(InterInst*& _do,InterInst*& _exit);//do-while头部
    void genDoWhileTail(Var*cond,InterInst* _do,InterInst* _exit);//do-while尾部
    void genForHead(InterInst*& _for,InterInst*& _exit);//for循环头部
    void genForCondBegin(Var*cond,InterInst*& _step,InterInst*& _block,InterInst* _exit);//for条件开始
    void genForCondEnd(InterInst* _for,InterInst* _block);//for循环条件结束部分
    void genForTail(InterInst*& _step,InterInst*& _exit);//for循环尾部
    void genIfHead(Var*cond,InterInst*& _else);//if头部
    void genIfTail(InterInst*& _else);//if尾部
    void genElseHead(InterInst* _else,InterInst*& _exit);//else头部
    void genElseTail(InterInst*& _exit);//else尾部
    void genSwitchHead(InterInst*& _exit);//switch头部
    void genSwitchTail(InterInst* _exit);//switch尾部
    void genCaseHead(Var*cond,Var*lb,InterInst*& _case_exit);//case头部
    void genCaseTail(InterInst* _case_exit);//case尾部

    //产生特殊语句
    void genBreak();//产生break语句
    void genContinue();//产生continue语句
    void genReturn(Var*ret);//产生return语句
    bool genVarInit(Var*var);//产生变量初始化语句
    void genFunHead(Fun*function);//产生函数入口语句
    void genFunTail(Fun*function);//产生函数出口语句

    //全局函数
    static string genLb();//产生唯一的标签
    static bool typeCheck(Var*lval,Var*rval);//检查类型是否可以转换
};