#pragma once
#include "common.h"

class Parser
{
    //文法开始
    void CompUnit();
    void ConstDecl();
    void UnitTail();
    void VarOrFunc();
    void FuncFParams();
    void Block();
    void Array();
    void VardefTail();
    void VarDeclTail();

    //声明
    void Decl();
    void VarDecl();
    void Btype();
    void ConstDef();
    void ConstDeclTail();

    //常数定义
    void ConstInitVal();
    void ConstExp();
    void ConstInitValTail();
    void CVTTail();
    
    //变量声明
    void VarDef();
    void initVal();
    void initValTail();
    void IVTTail();

    //函数定义
    void FuncDef();
    void FuncType();
    
    //函数形参表
    void FuncFParam();
    void FuncFParamsTail();
    void ArrayParam();
    
    //函数形参
    void ArrayParamTail();
    //语句块
    void BlockItem();
    //语句
    void Stmt();

    void LValOrFunc();
    void Exp();
    void NewMulExp();
    void NewAddExp();
    void Num();
    void UnaryOp();
    void UnaryExp();
    void Cond();
    void ElseOpt();
    void ExpOpt();
    void FuncRParams();
    void EqualOrMul();
    //表达式
    void AddExp();
    //条件式
    void LOrExp();
    //左值式
    void LVal();
    //一元式
    void UnaryExp();
    void PrimaryExp();
    void UnaryOp();
    //函数实参表
    void FuncRParamsTail();
    //乘除模
    void MulExp();
    //加减
    void AddExp();
    //关系
    void RelExp();
    void NewRelExp();
    //相等性
    void EqExp();
    void NewEqExp();
    //逻辑
    void LAndExp();
    void NewLAndExp();
    void LOrExp();
    void NewLOrExp();
    //常量式
    void ConstExp();

    //词法分析
    Lexer &lexer;//词法分析器
    Token *look; //超前查看的字符

    void move(); //移进
    bool match(Tag t); //匹配,成功则移进

public:
    //构造与初始化
    Parser(Lexer &lex);
    //外部调用接口
    void analyse();
};