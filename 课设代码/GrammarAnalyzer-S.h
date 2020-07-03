#pragma once

#include "common.h"

//文法分析器
class Parser {
    //代码段
    void CompUnit();//编译单元
    void VarOrFun();//函数判定
    void UnitTail();//变量判定
    
    //声明与定义
    Tag BType();//基本类型
    void Decl();//声明
    void ConstDecl();//常量声明
    void ConstDeclTail();//常量尾
    void ConstDef();//常数定义
    void ConstInitVal();//常量初值
    void ConstInitValTail();//常初值尾
    void CVTTail();//常初值尾
    void Array();//数组部分
    void VarDecl();//变量声明
    void VarDeclTail();//变量声明尾
    void VarDef();//变量定义
    void VarDefTail();//变量定义尾
    void InitVal();//变量初值
    void InitValTail();//变量初值尾
    void IVTTail();//变量初值尾
    
    //函数
    Tag FuncType();//函数类型
    void FuncDef();//函数定义
    void FuncFParams();//函数形参表
    void FuncFParamsTail();//函数形参表尾
    void FuncFParam();//函数形参
    void ArrayParam();//数组形参
    void ArrayParamTail();//数组形参尾
    void Block();//语句块
    void BlockItem();//语句块项
    void Stmt();//语句
    void LValOrFunc();//语句一级判断
    void EqualOrMul();//语句二级判断
    void ExpOpt();//表达式语句
    void ElseOpt();//else语句
    
    //表达式
    void Exp();//表达式
    void Cond();//条件表达式
    void LVal();//左值表达式
    void PrimaryExp();//基本表达式
    void Num();//数值
    void UnaryExp();//一元表达式
    void MulExp();//乘除模表达式
    void NewMulExp();//
    void AddExp();//加减表达式
    void NewAddExp();//加减表达式尾
    void RelExp();//关系表达式
    void NewRelExp();//关系表达式尾
    void EqExp();//相等性表达式
    void NewEqExp();//相等性表达式尾
    void LAndExp();//逻辑与表达式
    void NewLAndExp();//逻辑与表达式尾
    void LOrExp();//逻辑或表达式
    void NewLOrExp();//逻辑或表达式尾
    void ConstExp();//常量表达式
    Tag UnaryOp();//单目运算符
    void FuncRParams();//函数实参表
    void FuncRParamsTail();//函数实参表尾
    
    
    //词法分析器
    Lexer &lexer;//词法分析器
    Token* look;//超前查看的字符
    
    //符号表
    SymTab &symtab;
    
    //中间代码生成器
    GenIR &ir;
    
    //语法分析与错误修复
    void Move();//移进
    bool Match(Tag t);//匹配,成功则移进
    void Recovery();//错误修复

public:
    //构造函数
    Parser(Lexer&lex, SymTab&tab, GenIR&inter);
    //外部接口
    void Analyse();//语法分析主程序
};
