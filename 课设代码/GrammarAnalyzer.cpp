#include "GrammarAnalyzer.h"
#include "Token.h"
#include "LexicalAnalyzer.h"
#include "Symbol.h"
#include "Symtab.h"
#include "GenInter.h"

Parser::Parser(Lexer &lex,SymTab &tab,GenIR&inter)
    :lexer(lex),symtab(tab),ir(inter)
{}

//语法分析主程序
void Parser::analyse()
{
    move(); //预先读入
    CompUnit();
}

//移进
void Parser::move()
{
    look = lexer.tokenize();
}

bool Parser::match(Tag need)
{
    if(look->tag==need){
        move();
        return true;
    }
    else
        return false;
}

void Parser::CompUnit()
{
    Tag t;
    string name = "";
    if(match(KW_INT)){
        t = KW_INT;
        if(look->tag == ID){
            name = ((Id *)look)->name;
            move();
            VarOrFunc(t,name);
            UnitTail();
        }
    }else if(match(KW_VOID)){
        t = KW_VOID;
        if(look->tag == ID){
            name = ((Id *)look)->name;
            move();
            if(match(LPAREN)){
                symtab.enter();
                vector<Var *> paraList;//参数列表
                FuncFParams(paraList);
                Fun *f = new Fun(t, name, paraList);
                if(match(RPAREN)){ //函数定义
                    symtab.defFun(f);
                    Block();
                    symtab.endDefFun();
                    symtab.leave();
                    UnitTail();
                }
            }
        }
    }else if(look->tag==KW_CONST){
        t = KW_CONST;
        ConstDecl();
        UnitTail();
    }
}

void Parser::VarOrFunc(Tag t,string name)
{
    if(match(LPAREN)){
        symtab.enter();
        vector<Var *> paraList;
        FuncFParams(paraList);
        Fun *f = new Fun(t, name,paraList);
        if(match(RPAREN)){
            symtab.defFun(f);
            Block();
            symtab.endDefFun();
            symtab.leave();
        }
    }
    else{
        Var* init = NULL;
        if(match(EQU))
            init = initVal();
        Var*v=new Var(symtab.getScopePath(), t, name, init);
        symtab.addVar(v);
        VarDeclTail(t);
        if(match(SEMICON))
            return;
    }
}

void Parser::UnitTail()
{
    Tag t;
    string name = "";
    if(look->tag==END)
        return;
    else if(look->tag==KW_CONST){
        t = KW_CONST;
        ConstDecl();
        UnitTail();
    }else if(match(KW_INT)){
        t = KW_INT;
        if(look->tag == ID){
            name = ((Id *)look)->name;
            move();
            VarOrFunc(t,name);
            UnitTail();
        }
    }else if(match(KW_VOID)){
        t = KW_VOID;
        if(look->tag == ID){
            name = ((Id *)look)->name;
            move();
            if(match(LPAREN)){
                symtab.enter();
                vector<Var *> paralist;
                FuncFParams(paralist);
                Fun *f = new Fun(t, name, paralist);
                if(match(RPAREN)){
                    symtab.defFun(f);
                    Block();
                    symtab.endDefFun();
                    symtab.leave();
                    UnitTail();
                }
            }
        }
    }
}

void Parser::Decl()
{
    if(look->tag==KW_CONST){
        ConstDecl();
    }
    else if(look->tag==KW_INT){
        VarDecl();
    }
}

void Parser::ConstDecl()
{
    Tag t;
    if(match(KW_CONST)){
        t=Btype();
        ConstDef(t);
        ConstDeclTail(t);
        match(SEMICON);
    }
}

void Parser::ConstDeclTail(Tag t)
{
    if(match(COMMA)){
        ConstDef(t);
        ConstDeclTail(t);
    }
}

Tag Parser::Btype()
{
    Tag tmp = KW_INT;
    if(look->tag==KW_INT){
        move();
        return tmp;
    }
}

void Parser::ConstDef(Tag t)
{
    string name = "";
    if(look->tag == ID){
        name = ((Id *)look)->name;
        move();
        Var* init = NULL;
        if(match(EQU))
            init = ConstInitVal();//不考虑数组，跳过Array
        Var* v = new Var(symtab.getScopePath(), t, name, init);
        symtab.addVar(v);
    }
}


/*————————无用函数——————————*/
Var* Parser::Array(Tag t,string name)
{
    //先不考虑数组
    if(match(LBRACK)){
        ConstExp();
        if(match(RBRACK))
            Array(t,name);
    }//变量
    else
        return new Var(symtab.getScopePath(), t, name, NULL);
}
/*————————无用函数——————————*/


Var* Parser::ConstInitVal()
{
    if(match(LBRACE)){
        ConstInitValTail();
        if(match(RBRACE))
            return NULL;
    }else{
        return ConstExp();
    }
}


/*————————无用函数——————————*/
void Parser::ConstInitValTail()
{
    if(look->tag==RBRACE)
        return;
    else{
        //ConstInitVal();
        CVTTail();
    }
}

void Parser::CVTTail()
{
    if(match(COMMA)){
        //ConstInitVal();
        CVTTail();
    }
}
/*————————无用函数——————————*/


void Parser::VarDecl()
{
    Tag t=Btype();
    VarDef(t);
    VarDeclTail(t);
    if(match(SEMICON))
        return;
}

void Parser::VarDeclTail(Tag t)
{
    if(match(COMMA)){
        VarDef(t);
        VarDeclTail(t);
    }
}

void Parser::VarDef(Tag t)
{
    string name = "";
    if(look->tag == ID){
        name = ((Id*)look)->name;
        move();
        Var* init = NULL;
        if(match(ASSIGN))
            init = initVal();
        Var*v=new Var(symtab.getScopePath(), t, name, init);
        symtab.addVar(v);
    }
}


/*————————无用函数——————————*/
void Parser::VardefTail(Var* v)
{
    Var *initV = NULL;
    
    if(match(ASSIGN)){
        initV=initVal();
    }
}
/*————————无用函数——————————*/


Var* Parser::initVal()
{
    if(match(LBRACE)){
        initValTail();
        if(match(RBRACE))
            ;
    }else
        return Exp();
}


/*————————无用函数——————————*/
void Parser::initValTail()
{
    if(look->tag==RBRACE)
        return;
    else{
        initVal();
        IVTTail();
    }
}

void Parser::IVTTail()
{
    if(match(COMMA)){
        initVal();
        IVTTail();
    }
}

void Parser::FuncDef()
{
    Tag t = FuncType();
    if(match(ID)&&match(LPAREN)){
        vector<Var *>paralist;
        FuncFParams(paralist);
        if(match(RPAREN))
            Block();
    }
}

Tag Parser::FuncType()
{
    Tag t = KW_INT;
    if(look->tag==KW_VOID||look->tag==KW_INT){
        t = look->tag;
        move();
    }
    return t;
}
/*————————无用函数——————————*/


void Parser::FuncFParams(vector<Var *>&list)
{
    if (look->tag == KW_VOID || look->tag == KW_INT) {
        Var *v = FuncFParam();
        symtab.addVar(v);
        list.push_back(v);
        FuncFParamsTail(list);
    }
}

void Parser::FuncFParamsTail(vector<Var*>&list)
{
    if(match(COMMA)){
        Var *v = FuncFParam();
        symtab.addVar(v);
        list.push_back(v);
        FuncFParamsTail(list);
    }
}

Var* Parser::FuncFParam()
{
    Tag t = Btype();
    string name = "";
    if(look->tag == ID){
        name = ((Id *)look)->name;
        move();
        return ArrayParam(t,name);
    }
}

Var* Parser::ArrayParam(Tag t,string name)
{
    //只考虑一维数组
    if(match(LBRACK)&&match(RBRACK)){
        ArrayParamTail();
    }else{
        return new Var(symtab.getScopePath(), t, name);
    }
}


/*————————无用函数——————————*/
void Parser::ArrayParamTail(){
    if(match(LBRACK)){
        Exp();
        if(match(RBRACK)){
            ArrayParamTail();
        }
    }
}
/*————————无用函数——————————*/


void Parser::Block()
{
    if(match(LBRACE)){
        BlockItem();
        if(match(RBRACE))
            ;
    }
}

void Parser::BlockItem()
{
    if(look->tag==RBRACE)
        return;
    else if(look->tag == KW_CONST || look->tag == KW_INT){
        Decl();
        BlockItem();
    }else{
        Stmt();
        BlockItem();
    }
}

void Parser::Stmt()
{
    if(look->tag == ID){
        string name=((Id *)look)->name;
        move();
        LValOrFunc(name);
        if(match(SEMICON))
            ;
    }else if(match(LPAREN)){
        Var *lval= Exp();
        if(match(RPAREN)){
            lval=NewMulExp(lval);
            lval=NewAddExp(lval);
            if(match(SEMICON))
                ;
        }
    }else if(look->tag == INTCONST){
        Var *lval = Num();
        lval = NewMulExp(lval);
        lval = NewAddExp(lval);
    }else if(look->tag==ADD||look->tag==SUB||look->tag==NOT){
        Tag t = UnaryOp();
        Var *lval = UnaryExp();
        lval = NewMulExp(lval);
        lval = NewAddExp(lval);
    }else if(match(SEMICON))
        return;
    else if(look->tag == LBRACE){
        Block();
    }
    else if(match(KW_IF)){
        symtab.enter();
        InterInst *_else, *_exit;
        if(match(LPAREN)){
            Var *cond = Cond();
            ir.genIfHead(cond, _else);
            if(match(RPAREN)){
                Stmt();
                symtab.leave();
                ElseOpt(_else,_exit);
            }
        }
    }else if(match(KW_WHILE)){
        symtab.enter();
        InterInst *_while, *_exit;
        ir.genWhileHead(_while, _exit);
        if(match(LPAREN)){
            Var *cond = Cond();
            if(match(RPAREN)){
                Stmt();
            }
        }
        ir.genWhileTail(_while, _exit);
        symtab.leave();
    }else if(match(KW_BREAK)){
        ir.genBreak();
        match(SEMICON);
    }else if(match(KW_CONTINUE)){
        ir.genContinue();
        match(SEMICON);
    }else if(match(KW_RETURN)){
        ir.genReturn(ExpOpt());
        match(SEMICON);
    }
}

Var* Parser::ExpOpt(){
    if(match(SEMICON)){
        //return Var::getVoid();
        return NULL;
    }else
        return Exp();
}

void Parser::ElseOpt(InterInst* _else,InterInst *_exit){
    ir.genElseHead(_else, _exit);
    if(match(KW_ELSE))
    {
        symtab.enter();
        Stmt();
        symtab.leave();
    }
    ir.genElseTail(_exit);
}

void Parser::LValOrFunc(string name){
    if(match(LPAREN)){
        Var *lval = FuncRParams(name);
        if(match(RPAREN)){
            lval=NewMulExp(lval);
            lval=NewAddExp(lval);
        }
    }else{
        //ArrayParamTail();//不支持数组
        EqualOrMul();
    }
}

void Parser::EqualOrMul()
{
    if(match(ASSIGN)){
        Exp();
    }else{
        //NewMulExp();
        //NewAddExp();
    }
}

Var* Parser::Exp()
{
    return AddExp();
}

Var* Parser::Cond()
{
    return LOrExp();
}

void Parser::LVal()
{
    if(match(ID)){
        ArrayParamTail();
    }
}

Var* Parser::PrimaryExp()
{
    Var *v = NULL;
    if(match(LPAREN)){
        v=Exp();
        match(RPAREN);
    }else if(look->tag==INTCONST){
        v=Num();
    }else{
        LVal();
    }
    return v;
}

Var* Parser::Num()
{
    Var *v = NULL;
    if(look->tag==INTCONST){
        v = new Var(look);
        symtab.addVar(v);
        move();
    }
    return v;
}

Var* Parser::UnaryExp()
{
    Var *v;
    if(match(ID)){
        string name = ((Id *)look)->name;
        if(match(LPAREN)){
            v=FuncRParams(name);
            if(match(RPAREN))
                ;
        }
    }else if(look->tag==ADD||look->tag==SUB||look->tag==NOT){
        Tag t=UnaryOp();
        Var *v = UnaryExp();
        return ir.genOneOpLeft(t, v);
    }else{
        return PrimaryExp();
    }
    return v;
}

Tag Parser::UnaryOp(){
    Tag tmp;
    if(look->tag == AND || look->tag == SUB || look->tag == NOT){
        tmp = look->tag;
        move();
    }
    return tmp;

}//单目运算符

Var* Parser::FuncRParams(string name)
{
    vector<Var *> args;
    if(look->tag==RPAREN)
        ;
    else{
        args.push_back(Exp());
        FuncRParamsTail(args);
    }
    Fun *function = symtab.getFun(name, args);
    return ir.genCall(function, args);
}

void Parser::FuncRParamsTail(vector<Var *>& args)
{
    if(match(COMMA)){
        args.push_back(Exp());
        FuncRParamsTail(args);
    }
}

Var* Parser::MulExp()
{
    Var*lval=UnaryExp();
    return NewMulExp(lval);
}

Var* Parser::NewMulExp(Var*lval)
{
    Var *rval;
    Var *result;
    if(match(MUL)){
        rval = UnaryExp();
        result = ir.genTwoOp(lval, MUL, rval);
        return NewMulExp(result);
    }else if(match(DIV)){
        rval = UnaryExp();
        result = ir.genTwoOp(lval, DIV, rval);
        return NewMulExp(result);
    }else if(match(MOD)){
        rval = UnaryExp();
        result = ir.genTwoOp(lval, MOD, rval);
        return NewMulExp(result);
    }
    return lval;
}

Var* Parser::AddExp()
{
    Var*lval=MulExp();
    return NewAddExp(lval);
}

Var* Parser::NewAddExp(Var* lval)
{
    Var *rval;
    Var *result;
    if(match(ADD)){
        rval=MulExp();
        result = ir.genTwoOp(lval, ADD, rval);
        return NewAddExp(result);
    }else if(match(SUB)){
        rval=MulExp();
        result = ir.genTwoOp(lval, SUB, rval);
        return NewAddExp(result);
    }
    return lval;
}

Var* Parser::RelExp()
{
    Var* lval = AddExp();
    return NewRelExp(lval);
}

Var* Parser::NewRelExp(Var* lval)
{
    Var *rval;
    Var *result;
    if(match(LT)){
        rval = AddExp();
        result = ir.genTwoOp(lval, LT, rval);
        return NewRelExp(result);
    }else if(match(GT)){
        rval = AddExp();
        result = ir.genTwoOp(lval, GT, rval);
        return NewRelExp(result);
    }else if(match(LE)){
        rval = AddExp();
        result = ir.genTwoOp(lval, LE, rval);
        return NewRelExp(result);
    }else if(match(GE)){
        rval = AddExp();
        result = ir.genTwoOp(rval, GE, lval);
        return NewRelExp(result);
    }
    return lval;
}

Var* Parser::EqExp()
{
    Var* lval = RelExp();
    return NewEqExp(lval);
}

Var* Parser::NewEqExp(Var* lval)
{
    Var *rval;
    Var *result;
    if(match(EQU)){
        rval = RelExp();
        result = ir.genTwoOp(lval, EQU, rval);
        return NewEqExp(result);
    }else if(match(NEQU)){
        rval = RelExp();
        result = ir.genTwoOp(lval, NEQU, rval);
        return NewEqExp(result);
    }
    return lval;
}

Var* Parser::LAndExp()
{
    Var* lval = EqExp();
    return NewLAndExp(lval);
}

Var* Parser::NewLAndExp(Var* lval)
{
    Var *rval;
    Var *result;
    if(match(AND)){
        rval = EqExp();
        result = ir.genTwoOp(lval, AND, rval);
        return NewLAndExp(result);
    }
    return lval;
}

Var* Parser::LOrExp()
{
    Var * lval = LAndExp();
    return NewLOrExp(lval);
}

Var* Parser::NewLOrExp(Var* lval)
{
    Var *rval;
    Var *result;
    if(match(OR)){
        rval = LAndExp();
        result = ir.genTwoOp(lval, OR, rval);
        return NewLOrExp(result);
    }
}

Var* Parser::ConstExp()
{
    return AddExp();
}
