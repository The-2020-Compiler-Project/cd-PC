#include "SemanticAnalyzer-G.h"

/**
* InterInst 类方法，都在修改或判断四元式的属性，所以不写注释了
*/

// 基本初始化，让四元式具备默认参数，并作为空指令
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

/**
* GenIR 类方法
*/

// 打印语义错误信息

int GenIR::lbNum = 0;

// 初始化生成器，绑定符号表并初始化作用域
GenIR::GenIR(SymTab& tab):
    symtab(tab)
{
    symtab.setIr(this);
    lbNum = 0;
    push(NULL, NULL);
}

// 获取一个全局唯一的标签字符串（如果全局只有一个 GenIR 对象）
string GenIR::genLb()
{
    lbNum++;
    // TODO: 标签格式待商议
    string lb = ".L";
    stringstream ss;

    ss<<lbNum;
    return lb + ss.str();
}

// 数组索引语句
Var* GenIR::genArray(Var* array, Var* index)
{
    // TODO: 待加入多维索引，以及数组下标数量不对时直接报错
    if (!array || !index) return NULL;
    // 均为 void 类型
    if (array->isVoid() || index->isVoid()) {
        SEMERROR(EXPR_IS_VOID);
        return NULL;
    }
    // 均为基本类型
    if (array->isBase() || !index->isBase()) {
        SEMERROR(ARR_TYPE_ERR);
        return index;
    }

    return genPtr(genAdd(array, index));
}

// 函数的参数传递
void GenIR::genPara(Var* arg)
{
    if (arg->isRef()) arg = genAssign(arg);
    InterInst* argInst = new InterInst(OP_ARG, arg);
    symtab.addInst(argInst);
}

// 函数的调用语句
Var* GenIR::genCall(Fun* function, vector<Var*>& args)
{
    if (!function) return NULL;
    // 逆向生成参数传递语句
    for (int i = args.size()-1; i>=0; i--) genPara(args[i]);

    if (function->getType() == KW_VOID) {
        // 函数返回值不会作为右值
        symtab.addInst(new InterInst(OP_PROC, function));
        return Var::getVoid();
    } else {
        // 函数返回值可能作为右值
        Var* ret = new Var(symtab.getScopePath(), function->getType(), false);
        symtab.addInst(new InterInst(OP_CALL, function, ret));
        symtab.addVar(ret);
        return ret;
    }
}

// 所有的双目运算，其他操作返回 rval
Var* GenIR::genTwoOp(Var* lval, Tag opt, Var* rval)
{
    if (!lval || !rval) return NULL;

    // 避免在表达式中出现 void 返回值的函数
    if (lval->isVoid() || rval->isVoid()) {
        SEMERROR(EXPR_IS_VOID);
        return NULL;
    }

    // 赋值运算，数组索引赋值也被考虑进对应的函数里，所以这里不对其进行展开
    if (opt == ASSIGN) return genAssign(lval, rval);

    // 展开数组索引
    lval = (lval->isRef()? genAssign(lval): lval);
    rval = (rval->isRef()? genAssign(rval): rval);
    // 数组和基本类型均可作为算子的运算
    switch (opt) {
        case OR:
        case AND:
        case EQU:
        case NEQU:
            return twoOpExceptAddSub(lval, opt, rval);
        case ADD:
            return genAdd(lval, rval);
        case SUB:
            return genSub(lval, rval);
    }

    // 过滤掉数组变量，下面的运算均需基本类型
    if (!lval->isBase() || !rval->isBase()) {
        SEMERROR(EXPR_NOT_BASE);
        return lval;
    }
    switch (opt) {
        case GT:  //大于
        case GE:  //大于等于
        case LT:  //小于
        case LE:  //小于等于
        case MUL: //乘
        case DIV: //除
        case MOD: //取模
            return twoOpExceptAddSub(lval, opt, rval);
    }

    return lval; //保险措施，其他运算返回右值
}

// 赋值语句，内部考虑了合法数组索引的赋值（不合法的表达式在 genArray 中过滤掉了）
// 中途遇到不合法操作都会直接返回右值
Var* GenIR::genAssign(Var* lval, Var* rval)
{
    // 被赋值的对象必须是左值
    if (!lval->getLeft()) {
        SEMERROR(EXPR_NOT_LEFT_VAL);
        return rval;
    }
    // 类型检查
    if (!typeCheck(lval, rval)) {
        SEMERROR(ASSIGN_TYPE_ERR);
        return rval;
    }

    if (lval->isRef()) {
        //*(lval->ptr) = rval 或 *(lval->ptr) = *(rval->ptr)
        rval = (rval->isRef()? genAssign(rval): rval);
        symtab.addInst(new InterInst(OP_SET, rval, lval->getPointer()));
    } else {
        if (rval->isRef()) {
            // lval = *(rval->ptr)
            symtab.addInst(new InterInst(OP_GET, lval, rval->getPointer()));
        } else {
            // lval = rval
            symtab.addInst(new InterInst(OP_AS, lval, rval));
        }
    }
    return lval;
}

// 处理引用类型，得到其实际的指针指向的值并返回
Var* GenIR::genAssign(Var* val)
{
    Var* tmp = new Var(symtab.getScopePath(), val);
    symtab.addVar(tmp);

    if (val->isRef())
        symtab.addInst(new InterInst(OP_GET, tmp, val->getPointer()));
    else
        symtab.addInst(new InterInst(OP_AS, tmp, val));

    return tmp;
}

// 除了加法与减法外的二元运算
Var* GenIR::twoOpExceptAddSub(Var* lval, Tag opt, Var* rval)
{
    Var* tmp = new Var(symtab.getScopePath(), KW_INT, false);
    symtab.addVar(tmp);

    Operator inst;
    switch (opt) {
        case OR: inst = OP_OR; break;
        case AND: inst = OP_AND; break;
        case EQU: inst = OP_EQU; break;
        case NEQU: inst = OP_NE; break;
        case GT: inst = OP_GT; break;
        case GE: inst = OP_GE; break;
        case LT: inst = OP_LT; break;
        case LE: inst = OP_LE; break;
        case MUL: inst = OP_MUL; break;
        case DIV: inst = OP_DIV; break;
        case MOD: inst = OP_MOD; break;
    }
    symtab.addInst(new InterInst(inst, tmp, lval, rval));
    return tmp;
}

// 加法运算，和减法做区分是因为加法的左右操作数都可以为引用类型，而减法只能左操作数为引用类型
// 操作数类型错误时，返回 rval；否则返回计算后的临时结果
Var* GenIR::genAdd(Var* lval, Var* rval)
{
    Var* tmp = NULL;
    if ((lval->getArray()||lval->getPtr()) && rval->isBase()) {

        tmp = new Var(symtab.getScopePath(), lval);
        rval = twoOpExceptAddSub(rval, MUL, new Var(4));

    } else if ((rval->getArray()||rval->getPtr()) && lval->isBase()) {

        tmp = new Var(symtab.getScopePath(), rval);
        lval = twoOpExceptAddSub(rval, MUL, new Var(4));

    } else if (lval->isBase() && rval->isBase()) {
        tmp = new Var(symtab.getScopePath(), KW_INT, false);
    } else {
        SEMERROR(EXPR_NOT_BASE);
        return lval;
    }

    symtab.addVar(tmp);
    symtab.addInst(new InterInst(OP_ADD, tmp, lval, rval));
    return tmp;
}

// 减法语句
Var* GenIR::genSub(Var* lval, Var* rval)
{
    Var* tmp = NULL;
    if (!rval->isBase()) {
        SEMERROR(EXPR_NOT_BASE);
        return lval;
    }
    if (lval->getArray() || lval->getPtr()) {
        tmp = new Var(symtab.getScopePath(), lval);
        rval = twoOpExceptAddSub(rval, MUL, new Var(4));
    } else {
        tmp = new Var(symtab.getScopePath(), KW_INT, false);
    }

    symtab.addVar(tmp);
    symtab.addInst(new InterInst(OP_SUB, tmp, lval, rval));
    return tmp;
}

// 左单目运算语句
Var* GenIR::genOneOpLeft(Tag opt, Var* val)
{
    if (!val) return NULL;

    if (val->isVoid()) {
        SEMERROR(EXPR_IS_VOID);
        return NULL;
    }

    if (opt==INC or opt==DEC) return genIncLorDecL(opt, val);

    if (val->isRef()) val = genAssign(val);
    switch (opt) {
        case NOT: return genNot(val);
        case SUB: return genMinus(val);
    }

    return NULL;
}

// 取反运算
Var* GenIR::genNot(Var* val)
{
    Var* tmp = new Var(symtab.getScopePath(), KW_INT, false);

    symtab.addVar(tmp);
    symtab.addInst(new InterInst(OP_NOT, tmp, val));

    return tmp;
}

// 取负运算
Var* GenIR::genMinus(Var* val)
{
    if (!val->isBase()) {
        SEMERROR(EXPR_NOT_BASE);
        return val;
    }

    Var* tmp = new Var(symtab.getScopePath(), KW_INT, false);

    symtab.addVar(tmp);
    symtab.addInst(new InterInst(OP_NEG, tmp, val));

    return tmp;
}

// 左自加和左自减
Var* GenIR::genIncLorDecL(Tag opt, Var* val)
{
    if (!val->getLeft()) {
        SEMERROR(EXPR_NOT_LEFT_VAL);
        return val;
    }
    // ++nums[i] = t1=nums[i], t2=t1+1, nums[i]=t2
    // --同上
    if (val->isRef()) {
        Var* t1 = genAssign(val);
        Var* t2 = (opt==INC? genAdd(t1, new Var(4)): genSub(t1, new Var(4)));
        return genAssign(val, t2);
    }

    Operator tmp = (opt==INC? OP_ADD: OP_SUB);
    symtab.addInst(new InterInst(tmp, val, val, new Var(1)));
    return val;
}

// 指针取值语句，在数组索引处会用到
Var* GenIR::genPtr(Var* val)
{
    if (val->isBase()) {
        SEMERROR(EXPR_IS_BASE);
        return val;
    }
    Var* tmp = new Var(symtab.getScopePath(), val->getType(), false);
    tmp->setLeft(true);
    tmp->setPointer(val);
    symtab.addVar(tmp);
    return tmp;
}

// 右单目运算语句
Var* GenIR::genOneOpRight(Var* val, Tag opt)
{
    if (!val) return NULL;
    if (val->isVoid()) {
        SEMERROR(EXPR_IS_VOID);
        return NULL;
    }
    if (!val->getLeft()) {
        SEMERROR(EXPR_NOT_LEFT_VAL);
        return val;
    }
    genIncRorDecR(opt, val);
    return val;
}

Var* GenIR::genIncRorDecR(Tag opt, Var* val)
{
    Var* tmp = genAssign(val);

    Operator opt_tmp = (opt==INC? OP_ADD: OP_SUB);
    symtab.addInst(new InterInst(opt_tmp, val, val, new Var(1)));

    return tmp;
}

// 类型是否可以相互转换
bool GenIR::typeCheck(Var* lval, Var* rval)
{
    if (!lval || !rval) return false;

    if (lval->isBase() && rval->isBase()) return true;

    return lval->getType()==rval->getType();
}

/**
 * while 中间代码
 *
 * _while:
 *      // 计算 cond 作为条件
 *      OP_JF cond _exit
 *      // 循环体
 *      OP_JMP _while
 * _exit:
 */

// while 循环的头部
void GenIR::genWhileHead(InterInst*& _while, InterInst*& _exit)
{
    _while = new InterInst();
    symtab.addInst(_while);

    _exit = new InterInst(); //产生标签但不添加到符号表中，在genWhileTail中添加
    push(_while, _exit);
}

// 产生 while 的条件
void GenIR::genWhileCond(Var* cond, InterInst* _exit)
{
    if (cond) {
        // 不处理空表达式
        if (cond->isRef()) cond = genAssign(cond);
        symtab.addInst(new InterInst(OP_JF, _exit, cond));
    }
}

// 产生 while 尾部
void GenIR::genWhileTail(InterInst*& _while, InterInst*& _exit)
{
    symtab.addInst(new InterInst(OP_JMP, _while));
    symtab.addInst(_exit);
    pop();
}


/**
 * do-while 中间代码
 *
 * _do:
 *      // 循环体
 *      // 计算 cond 作为条件
 *      OP_JT cond _do
 * _exit:
 */

// do-while 头部
void GenIR::genDoWhileHead(InterInst*& _do, InterInst*& _exit)
{
    _do = new InterInst();
    _exit = new InterInst(); // exit 的处理同 while
    symtab.addInst(_do);
    push(_do, _exit);
}

// do-while 尾部
void GenIR::genDoWhileTail(Var* cond, InterInst* _do, InterInst* _exit)
{
    if (cond) {
        // 不处理空表达式
        if (cond->isRef()) cond = genAssign(cond);
        symtab.addInst(new InterInst(OP_JT, _do, cond));
    }
    symtab.addInst(_exit);
    pop();
}


/**
 * for 循环中间代码
 *
 * _for:
 *      // 算 cond 做条件
 *      OP_JF _exit, cond
 *      OP_JMP _block
 * _step:
 *      // 执行 for 循环第二个分号后面的更新
 *      OP_JMP _for
 * _block:
 *      // 循环体
 *      OP_JMP, _step
 * _exit:
 */

// for 循环头部
void GenIR::genForHead(InterInst*& _for, InterInst*& _exit)
{
    _for = new InterInst();
    _exit = new InterInst();
    symtab.addInst(_for);
}

// for 条件开始部分
void GenIR::genForCondBegin(Var* cond, InterInst*& _step, InterInst*& _block, InterInst* _exit)
{
    _block = new InterInst();
    _step = new InterInst();

    if (cond) {
        // 不处理空表达式
        if (cond->isRef()) cond = genAssign(cond);
        symtab.addInst(new InterInst(OP_JF, _exit, cond));
        symtab.addInst(new InterInst(OP_JMP, _block));
    }

    symtab.addInst(_step);
    push(_step, _exit);
}

// for 条件结束部分
void GenIR::genForCondEnd(InterInst* _for, InterInst* _block)
{
    symtab.addInst(new InterInst(OP_JMP, _for));
    symtab.addInst(_block);
}

// for 尾部
void GenIR::genForTail(InterInst*& _step, InterInst*& _exit)
{
    symtab.addInst(new InterInst(OP_JMP, _step));
    symtab.addInst(_exit);
    pop();
}

/**
 * if 中间代码
 *
 *      // 计算 cond 作为条件
 *      OP_JF cond _else
 *      // 条件为 true 时执行
 *      OP_JMP _exit
 * _else:
 *      // 条件为 false 时执行
 * _exit:
 */

// 产生 if 头部
void GenIR::genIfHead(Var* cond, InterInst*& _else)
{
    _else = new InterInst();
    if (cond) {
        if (cond->isRef()) cond = genAssign(cond);
        symtab.addInst(new InterInst(OP_JF, _else, cond));
    }
}

// if 尾部
void GenIR::genIfTail(InterInst*& _else)
{
    symtab.addInst(_else);
}

// else 头部
void GenIR::genElseHead(InterInst* _else, InterInst*& _exit)
{
    _exit = new InterInst();
    symtab.addInst(new InterInst(OP_JMP, _exit));
    symtab.addInst(_else);
}

// else 尾部
void GenIR::genElseTail(InterInst*& _exit)
{
    symtab.addInst(_exit);
}

/**
 * switch 中间代码
 *
 *      OP_JNE exit_1 lb_1 cond
 *      // 条件命中时执行，break 另议
 * exit_1:
 *      OP_JNE exit_2 lb_2 cond
 *      // 条件命中时执行
 * exit_2:
 *      ...
 *      default
 * exit:
 */

// switch 头部
void GenIR::genSwitchHead(InterInst*& _exit)
{
    _exit = new InterInst();
    push(NULL, _exit);
}

// switch 尾部
void GenIR::genSwitchTail(InterInst* _exit)
{
    symtab.addInst(_exit);
    pop();
}

// case 头部
void GenIR::genCaseHead(Var* cond, Var* lb, InterInst*& _case_exit)
{
    _case_exit = new InterInst();
    if (lb)
        symtab.addInst(new InterInst(OP_JNE, _case_exit, cond, lb));
}

// case 尾部
void GenIR::genCaseTail(InterInst* _case_exit)
{
    symtab.addInst(_case_exit);
}

// 添加一个作用域，第一个参数服务于 continue，第二个参数服务于 break
void GenIR::push(InterInst* head, InterInst* tail)
{
    heads.push_back(head);
    tails.push_back(tail);
}

// 删除一个作用域
void GenIR::pop()
{
    heads.pop_back();
    tails.pop_back();
}

// break 语句, 跳转到当前作用域(while, for, do-while, switch)的结尾
void GenIR::genBreak()
{
    InterInst* tail = tails.back();
    if (tail)
        symtab.addInst(new InterInst(OP_JMP, tail));
    else
        SEMERROR(BREAK_ERROR);
}

// continue 语句，跳转到当前作用域(while, for, do-while)的开头
void GenIR::genContinue()
{
    InterInst* head = heads.back();
    if (head)
        symtab.addInst(new InterInst(OP_JMP, head));
    else
        SEMERROR(CONTINUE_ERR);
}

// 变量初始化语句
// 对于临时变量返回 false
// 普通变量如果仅声明则只产生声明语句，如果在声明时初始化则产生声明和赋值语句
bool GenIR::genVarInit(Var* var)
{
    if (var->getName()[0] == '<') return 0;
    symtab.addInst(new InterInst(OP_DEC, var));
    if (var->setInit())
        genTwoOp(var, ASSIGN, var->getInitData());
    return 1;
}

// 函数入口语句
void GenIR::genFunHead(Fun* function)
{
    function->enterScope();
    symtab.addInst(new InterInst(OP_ENTRY, function));
    function->setReturnPoint(new InterInst);
}

// 函数出口语句
void GenIR::genFunTail(Fun* function)
{
    symtab.addInst(function->getReturnPoint());
    symtab.addInst(new InterInst(OP_EXIT, function));
    function->leaveScope();
}

// return 语句
void GenIR::genReturn(Var* ret)
{
    if (!ret) return;
    Fun* fun = symtab.getCurFun();
    // 返回值类型和函数声明不匹配暂不检查
    InterInst* returnPoint = fun->getReturnPoint();
    if (ret->isVoid()) {
        symtab.addInst(new InterInst(OP_RET, returnPoint));
    } else {
        ret = (ret->isRef()? genAssign(ret): ret);
        symtab.addInst(new InterInst(OP_RETV, returnPoint, ret));
    }
}