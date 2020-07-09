#pragma once
#include "common.h"

class Parser
{
    //文法开始
    void CompUnit();
    void ConstDecl();
    void UnitTail();
    void VarOrFunc(Tag t,string name);
    void FuncFParams(vector<Var *>&list);
    void Block();
    Var* Array(Tag t,string name);
    void VardefTail(Var* v);
    void VarDeclTail(Tag t);

    //声明
    void Decl();
    void VarDecl();
    Tag Btype();
    void ConstDef(Tag t);
    void ConstDeclTail(Tag t);

    //常数定义
    void ConstInitVal(Var *v);
    void ConstInitValTail();
    void CVTTail();
    
    //变量声明
    void VarDef(Tag t);
    Var* initVal();
    void initValTail();
    void IVTTail();

    //函数定义
    void FuncDef();
    void FuncType();
    
    //函数形参表
    Var* FuncFParam();
    void FuncFParamsTail(vector<Var*>&list);
    Var* ArrayParam(Tag t,string name);
    
    //函数形参
    void ArrayParamTail();
    //语句块
    void BlockItem();
    //语句
    void Stmt();

    void LValOrFunc(string name);
    Var* Exp();
    Var* NewMulExp(Var* lval);
    Var* NewAddExp(Var* lval);
    Var* Num();
    Var* Cond();
    void ElseOpt(InterInst* _else,InterInst *exit);
    Var* ExpOpt();
    Var* FuncRParams(string name);
    void EqualOrMul();
    //条件式
    void LOrExp();
    //左值式
    void LVal();
    //一元式
    Var* UnaryExp();
    Var* PrimaryExp();
    Tag UnaryOp();
    //函数实参表
    void FuncRParamsTail(vector<Var *>& args);
    //乘除模
    Var* MulExp();
    //加减
    Var* AddExp();
    //关系
    void RelExp();
    void NewRelExp();
    //相等性
    void EqExp();
    void NewEqExp();
    //逻辑
    void LAndExp();
    void NewLAndExp();
    void NewLOrExp();
    //常量式
    void ConstExp();

    //词法分析
    Lexer &lexer;//词法分析器
    Token *look; //超前查看的字符

    //符号表
    SymTab &symtab;

    //中间代码生成器
    GenIR &ir;

    void move(); //移进
    bool match(Tag t); //匹配,成功则移进

public:
    //构造与初始化
    Parser(Lexer &lex,SymTab &tab,GenIR&inter);
    //外部调用接口
    void analyse();
};