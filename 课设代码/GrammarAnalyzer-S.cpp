#include <stdio.h>
#include "GrammarAnalyzer-S.h"

//构造函数
Parser::Parser(Lexer &lex, SymTab &tab, GenIR &inter)
:lexer(lex), symtab(tab), ir(inter){}

//超前读入
void Parser::Move(){
    look = lexer.tokenize();
}

//匹配并移动
bool Parser::Match(Tag need){
    if(look->tag == need){
        Move();
        return true;
    }
    else
        return false;
}

//主调程序
void Parser::Analyse(){
    Move();
    CompUnit();
}





//声明与定义
Tag Parser::BType(){
    Tag tmp = KW_INT;
    if(look->tag == KW_INT)
        Move();
    else
        Recovery();
    return tmp;
}//基本类型

//函数
Tag Parser::FuncType(){
    Tag tmp = KW_INT;
    if(look->tag == KW_INT || look->t == KW_VOID){
        tmp = look->tag;
        Move();
    }
    else
        Recovery();
    return tmp;
}//函数类型

Tag Parser::UnaryOp(){
    Tag tmp;
    if(look->tag == AND || look->tag == SUB || look->tag == NOT){
        tmp = look->tag;
        Move();
    }
    else
        Recovery();
    return tmp;
}//单目运算符

//代码段
void Parser::CompUnit(){
    if(Match(KW_INT)){
        if(Match(ID)){
            VarOrFun();
            UnitTail();
        }
    }
    else if(Match(KW_VOID)){
        if(Match(ID)){
            if(Match(LPAREN)){
                FuncFParam();
                if(Match(RPAREN)){
                    Block();
                    UnitTail();
                }
            }
        }
    }
    ConstDecl();
    UnitTail();
}//编译单元
void Parser::VarOrFun(){
    if(Match(LPAREN)){
        FuncFParam();
        if(Match(LPAREN)){
            Block();
        }
    }
    else{
        Array();
        VarDefTail();
        VarDeclTail();
        if(Match(SEMICON)){
            return;
        }
    }
}//函数判定
void Parser::UnitTail(){
    if(Match(KW_INT)){
        if(Match(ID)){
            VarOrFun();
            UnitTail();
        }
    }
    else if(Match(KW_VOID)){
        if(Match(ID)){
            if(Match(LPAREN)){
                FuncFParam();
                if(Match(RPAREN)){
                    Block();
                    UnitTail();
                }
            }
        }
    }
    else if(look->tag == END)
        return;
    ConstDecl();
    UnitTail();
}//变量判定

//声明与定义
void Parser::Decl(){
    if(look->tag == KW_CONST)
        ConstDecl();
    VarDecl();
}//声明
void Parser::ConstDecl(){
    if(Match(KW_CONST)){
        Tag tmp = BType();
        ConstDef();
        ConstDeclTail();
        if(Match(SEMICON))
            return;
    }
}//常量声明
void Parser::ConstDeclTail(){
    if(Match(COMMA)){
        ConstDef();
        ConstDeclTail();
    }
}//常量尾
void Parser::ConstDef(){
    if(Match(ID)){
        Array();
        if(Match(EQU)){
            ConstInitVal();
        }
    }
}//常数定义
void Parser::ConstInitVal(){
    if(Match(LBRACE)){
        ConstInitValTail();
        if(Match(RBRACE)){
            return;
        }
    }
    ConstExp();
}//常量初值
void Parser::ConstInitValTail(){
    if(look->tag == RBRACE){
        return;
    }
    ConstInitVal();
    CVTTail();
}//常初值尾
void Parser::CVTTail(){
    if(Match(COMMA)){
        ConstInitVal();
        CVTTail();
    }
}//常初值尾
void Parser::Array(){
    if(Match(LBRACK)){
        ConstExp();
        if(Match(RBRACK)){
            Array();
        }
    }
}//数组部分
void Parser::VarDecl(){
    BType();
    VarDef();
    VarDeclTail();
    if(Match(SEMICON)){
        return;
    }
}//变量声明
void Parser::VarDeclTail(){
    if(Match(COMMA)){
        VarDef();
        VarDeclTail();
    }
}//变量声明尾
void Parser::VarDef(){
    if(Match(ID)){
        Array();
        VarDeclTail();
    }
}//变量定义
void Parser::VarDefTail(){
    if(Match(EQU)){
        InitVal();
    }
}//变量定义尾
void Parser::InitVal(){
    if(Match(LBRACE)){
        InitValTail();
        if(Match(RBRACE)){
            return;
        }
    }
    Exp();
}//变量初值
void Parser::InitValTail(){
    if(look->tag == RBRACE){
        return;
    }
    InitVal();
    IVTTail();
}//变量初值尾
void Parser::IVTTail(){
    if(Match(COMMA)){
        InitVal();
        IVTTail();
    }
}//变量初值尾

//函数
void Parser::FuncDef(){
    FuncType();
    if(Match(ID)){
        if(Match(LPAREN)){
            FuncFParams();
            if(Match(RPAREN)){
                Block();
            }
        }
    }
}//函数定义
void Parser::FuncFParams(){
    if (look->tag == KW_VOID || look->tag == KW_INT) {
        FuncFParam();
        FuncFParamsTail();
    }
}//函数形参表
void Parser::FuncFParamsTail(){
    if(Match(COMMA)){
        FuncFParam();
        FuncFParamsTail();
    }
}//函数形参表尾
void Parser::FuncFParam(){
    Tag tmp = BType();
    if(Match(ID)){
        ArrayParam();
    }
}//函数形参
void Parser::ArrayParam(){
    if(Match(LBRACK)){
        if(Match(RBRACK)){
            ArrayParamTail();
        }
    }
}//数组形参
void Parser::ArrayParamTail(){
    if(Match(LBRACK)){
        Exp();
        if(Match(RBRACK)){
            ArrayParamTail();
        }
    }
}//数组形参尾
void Parser::Block(){
    if(Match(LBRACE)){
        BlockItem();
        if(Match(RBRACE)){
            return;
        }
    }
}//语句块
void Parser::BlockItem(){
    if(look->tag == KW_CONST || look->tag == KW_INT){
        Decl();
        BlockItem();
    }
    else if(look->tag == RBRACE){
        return;
    }
    else{
        Stmt();
        BlockItem();
    }
}//语句块项
void Parser::Stmt(){
    if(Match(ID)){
        LValOrFunc();
        if(Match(SEMICON)){
            return;
        }
    }
    else if(Match(LPAREN)){
        Exp();
        NewMulExp();
        NewAddExp();
        if(Match(SEMICON)){
            return;
        }
    }
    else if(Match(INC)){
        Num();
        NewMulExp();
        NewAddExp();
        if(Match(SEMICON)){
            return;
        }
    }
    else if(Match(SEMICON)){
        return;
    }
    else if(Match(LBRACE)){
        Block();
    }
    else if(Match(KW_IF)){
        if(Match(LPAREN)){
            Cond();
            if(Match(RPAREN)){
                Stmt();
                ElseOpt();
            }
        }
    }
    else if(Match(KW_WHILE)){
        if(Match(LPAREN)){
            Cond();
            if(Match(RPAREN)){
                Stmt();
            }
        }
    }
    else if(Match(KW_BREAK)){
        if(Match(SEMICON)){
            return;
        }
    }
    else if(Match(KW_CONTINUE)){
        if(Match(SEMICON)){
            return;
        }
    }
    else if(Match(KW_RETURN)){
        ExpOpt();
        if(Match(SEMICON)){
            return;
        }
    }
}//语句
void Parser::LValOrFunc(){
    if(Match(LPAREN)){
        FuncRParams();
        if(Match(RPAREN)){
            NewMulExp();
            NewAddExp();
        }
    }
    else{
        ArrayParamTail();
        EqualOrMul();
    }
}//语句一级判断
void Parser::EqualOrMul(){
    if(Match(EQU)){
        Exp();
    }
    else{
        NewMulExp();
        NewAddExp();
    }
}//语句二级判断
void Parser::ExpOpt(){
    if(Match(SEMICON)){
        return;
    }
    else{
        Exp();
    }
}//表达式语句
void Parser::ElseOpt(){
    if(Match(KW_ELSE)){
        Stmt();
    }
}//else语句

//表达式
void Parser::Exp(){
    AddExp();
}//表达式
void Parser::Cond(){
    LOrExp();
}//条件表达式
void Parser::LVal(){
    if(Match(ID)){
        ArrayParamTail();
    }
}//左值表达式
void Parser::PrimaryExp(){
    if(Match(LPAREN)){
        Exp();
        if(Match(RPAREN)){
            return;
        }
    }
    else if(look->tag == ID){
        Num();
    }
    else{
        LVal();
    }
}//基本表达式
void Parser::Num(){
    if(Match(INTCONST)){
        return;
    }
}//数值
void Parser::UnaryExp(){
    if(Match(ID)){
        if(Match(LPAREN)){
            FuncRParams();
            if(Match(RPAREN)){
                return;
            }
        }
    }
    else if(Match(LPAREN)){
        PrimaryExp();
    }
    else{
        UnaryOp();
        UnaryExp();
    }
}//一元表达式
void Parser::MulExp(){
    UnaryExp();
    NewMulExp();
}//乘除模表达式
void Parser::NewMulExp(){
    if(MUL){
        UnaryExp();
        NewMulExp();
    }
    else if(MOD){
        UnaryExp();
        NewMulExp();
    }
    else if(DIV){
        UnaryExp();
        NewMulExp();
    }
    else{
        return;
    }
}//乘除模表达式
void Parser::AddExp(){
    MulExp();
    NewAddExp();
}//加减表达式
void Parser::NewAddExp(){
    if(Match(ADD)){
        MulExp();
        NewAddExp();
    }
    else if(Match(SUB)){
        MulExp();
        NewAddExp();
    }
    else{
        return;
    }
}//加减表达式尾
void Parser::RelExp(){
    AddExp();
    NewRelExp();
}//关系表达式
void Parser::NewRelExp(){
    if(Match(LT)){
        AddExp();
        NewRelExp();
    }
    else if(Match(GT)){
        AddExp();
        NewRelExp();
    }
    else if(Match(LE)){
        AddExp();
        NewRelExp();
    }
    else if(Match(GE)){
        AddExp();
        NewRelExp();
    }
    else{
        return;
    }
}//关系表达式尾
void Parser::EqExp(){
    RelExp();
    NewEqExp();
}//相等性表达式
void Parser::NewEqExp(){
    if(Match(EQU)){
        RelExp();
        NewEqExp();
    }
    else if(NEQU){
        RelExp();
        NewEqExp();
    }
    else{
        return;
    }
}//相等性表达式尾
void Parser::LAndExp(){
    EqExp();
    NewLAndExp();
}//逻辑与表达式
void Parser::NewLAndExp(){
    if(Match(AND)){
        EqExp();
        NewLAndExp();
    }
    else{
        return;
    }
}//逻辑与表达式尾
void Parser::LOrExp(){
    EqExp();
    NewLOrExp();
}//逻辑或表达式
void Parser::NewLOrExp(){
    if(Match(OR)){
        EqExp();
        NewLOrExp();
    }
    else{
        return;
    }
}//逻辑或表达式尾
void Parser::ConstExp(){
    AddExp();
}//常量表达式
void Parser::FuncRParams(){
    if(look->tag == RPAREN){
        return;
    }
    else{
        Exp();
        FuncRParamsTail();
    }
}//函数实参表
void Parser::FuncRParamsTail(){
    if(Match(COMMA)){
        Exp();
        FuncRParamsTail();
    }
    else{
        return;
    }
}//函数实参表尾

