#include "Inter.h"
#include "Symbol.h"
#include "GenInter.h"

void InterInst::init()
{
    this->op = OP_NOP;
    this->result = NULL;
    this->target = NULL;
    this->arg1 = NULL;
    this->arg2 = NULL;
    this->fun = NULL;
}

// 完整四元式初始化
InterInst::InterInst(Operator op, Var* rs, Var* arg1, Var* arg2)
{
    init();
    this->op = op;
    this->result = rs;
    this->arg1 = arg1;
    this->arg2 = arg2;
}

// 函数调用指令
InterInst::InterInst(Operator op, Fun* fun, Var* rs)
{
    init();
    this->op = op;
    this->fun = fun;
    this->result = rs;
}

// 参数进栈
InterInst::InterInst(Operator op, Var* arg1)
{
    init();
    this->op = op;
    this->arg1 = arg1;
}

// 产生唯一标号
InterInst::InterInst()
{
    init();
    label = GenIR::genLb();
}

// 条件跳转
InterInst::InterInst(Operator op, InterInst* tar, Var* arg1, Var* arg2)
{
    init();
    this->op = op;
    this->target = tar;
    this->arg1 = arg1;
    this->arg2 = arg2;
}

// 是否为条件转移指令
bool InterInst::isJcond()
{
    return op >= OP_JT && op <= OP_JNE;
}

// 是否为直接跳转指令 jmp 或 return
bool InterInst::isJmp()
{
    return op==OP_JMP || op==OP_RET || op==OP_RETV;
}

// 是否为标签
bool InterInst::isLb()
{
    return label != "";
}

// 是基本类型的表达式运算，或可以对指针取值
bool InterInst::isExpr()
{
    return (op>=OP_AS && op<=OP_OR) || op==OP_GET;
}

// 不确定运算结果的运算，比如指针的赋值、函数调用
bool InterInst::unknown()
{
    return op==OP_SET || op==OP_PROC || op==OP_CALL;
}

// 是否为声明
bool InterInst::isDec()
{
    return op == OP_DEC;
}

// 获取跳转指令的目标指令
InterInst* InterInst::getTarget()
{
    return target;
}

// 获得四元式的 op 变量
Operator InterInst::getOp()
{
    return op;
}

// 获取四元式的 result 变量
Var* InterInst::getResult()
{
    return result;
}

// 获取四元式的 arg1 变量
Var* InterInst::getArg1()
{
    return arg1;
}

// 获取四元式的 arg2 变量
Var* InterInst::getArg2()
{
    return arg2;
}

// 获取四元式的标签
string InterInst::getLabel()
{
    return label;
}

// 获取四元式的函数对象
Fun* InterInst::getFun()
{
    return fun;
}

// 设置四元式的 arg1 变量
void InterInst::setArg1(Var* arg1)
{
    this->arg1 = arg1;
}

// 以字符串的形式输出指令
void InterInst::toString()
{
    if(label!=""){
        printf("%s:\n",label.c_str());
        return;
    }
    switch(op)
    {
        //case OP_NOP:printf("nop");break;
        case OP_DEC:printf("dec ");arg1->value();break;
        case OP_ENTRY:printf("entry");break;
        case OP_EXIT:printf("exit");break;
        case OP_AS:result->value();printf(" = ");arg1->value();break;
        case OP_ADD:result->value();printf(" = ");arg1->value();printf(" + ");arg2->value();break;
        case OP_SUB:result->value();printf(" = ");arg1->value();printf(" - ");arg2->value();break;
        case OP_MUL:result->value();printf(" = ");arg1->value();printf(" * ");arg2->value();break;
        case OP_DIV:result->value();printf(" = ");arg1->value();printf(" / ");arg2->value();break;
        case OP_MOD:result->value();printf(" = ");arg1->value();printf(" %% ");arg2->value();break;
        case OP_NEG:result->value();printf(" = ");printf("-");arg1->value();break;
        case OP_GT:result->value();printf(" = ");arg1->value();printf(" > ");arg2->value();break;
        case OP_GE:result->value();printf(" = ");arg1->value();printf(" >= ");arg2->value();break;
        case OP_LT:result->value();printf(" = ");arg1->value();printf(" < ");arg2->value();break;
        case OP_LE:result->value();printf(" = ");arg1->value();printf(" <= ");arg2->value();break;
        case OP_EQU:result->value();printf(" = ");arg1->value();printf(" == ");arg2->value();break;
        case OP_NE:result->value();printf(" = ");arg1->value();printf(" != ");arg2->value();break;
        case OP_NOT:result->value();printf(" = ");printf("!");arg1->value();break;
        case OP_AND:result->value();printf(" = ");arg1->value();printf(" && ");arg2->value();break;
        case OP_OR:result->value();printf(" = ");arg1->value();printf(" || ");arg2->value();break;
        case OP_JMP:printf("goto %s",target->label.c_str());break;
        case OP_JT:printf("if( ");arg1->value();printf(" )goto %s",target->label.c_str());break;
        case OP_JF:printf("if( !");arg1->value();printf(" )goto %s",target->label.c_str());break;
        case OP_JNE:printf("if( ");arg1->value();printf(" != ");arg2->value();printf(" )goto %s",
                                                                                     target->label.c_str());break;
        case OP_ARG:printf("arg ");arg1->value();break;
        case OP_PROC:printf("%s()",fun->getName().c_str());break;
        case OP_CALL:result->value();printf(" = %s()",fun->getName().c_str());break;
        case OP_RET:printf("return goto %s",target->label.c_str());break;
        case OP_RETV:printf("return ");arg1->value();printf(" goto %s",target->label.c_str());break;
        case OP_LEA:result->value();printf(" = ");printf("&");arg1->value();break;
        case OP_SET:printf("*");arg1->value();printf(" = ");result->value();break;
        case OP_GET:result->value();printf(" = ");printf("*");arg1->value();break;
        default: break;
    }
    printf("\n");
}

/**
* InterCode 类方法
*/

// 向 code 属性中添加一条中间代码
void InterCode::addInst(InterInst* inst)
{
    code.push_back(inst);
}

// 析构函数，释放 code 中的所有 InterInst 对象
InterCode::~InterCode()
{
    for (int i=0; i<code.size(); i++) {
        delete code[i];
    }
}

// 获取 code 属性，内部记录该属性中所具有的所有中间代码
vector<InterInst*>& InterCode::getCode()
{
    return code;
}

// 以字符串的形式输出 code 中所有的指令
void InterCode::toString()
{
    for (int i=0; i<code.size(); i++) {
        code[i]->toString();
    }
}
