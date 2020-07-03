#include"GrammarAnalyzer-W"

Parser::Parser(Lexer &lex)
    :lexer(lex)
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
    if(look->tag==KW_INT){
        move();
        if(match(ID)){
            VarOrFunc();
            UnitTail();
        }
    }else if(look->tag==KW_VOID){
        move();
        if(match(ID)){
            if(match(LPAREN)){
                FuncFParams();
                if(match(RPAREN)){
                    Block();
                    UnitTail();
                }
            }
        }
    }else if(look->tag==KW_CONST){
        ConstDecl();
        UnitTail();
    }
}

void Parser::VarOrFunc()
{
    if(look->tag==LBRACK){
        Array();
        VardefTail();
        VarDeclTail();
        if(match(SEMICON))
            ;
    }else if(look->tag==LPAREN){
        FuncFParams();
        if(match(RPAREN))
            Block();
    }
}

void Parser::UnitTail()
{
    if(look->tag==END)
        return;
    else if(look->tag==KW_CONST){
        ConstDecl();
        UnitTail();
    }else if(look->tag==KW_INT){
        move();
        if(match(ID)){
            VarOrFunc();
            UnitTail();
        }
    }else if(look->tag==KW_VOID){
        move();
        if(match(ID)){
            if(match(LPAREN)){
                FuncFParams();
                if(match(RPAREN)){
                    Block();
                    UnitTail();
                }
            }
        }
    }
}

void Parser::Decl()
{
    if(look->tag==KW_CONST)
        ConstDecl();
    else if(look->tag==KW_INT)
        VarDecl();
}

void Parser::ConstDecl()
{
    match(KW_CONST);
    if(look->tag==KW_INT){
        Btype();
        ConstDef();
        ConstDeclTail();
        match(SEMICON);
    }
}

void Parser::ConstDeclTail()
{
    if(match(COMMA)){
        ConstDef();
        ConstDeclTail();
    }
}

void Parser::Btype()
{
    if(look->tag==KW_INT){
        move();
    }
}

void Parser::ConstDef()
{
    if(match(ID)){
        Array();
        if(match(EQU))
            ConstInitVal();
    }
}

void Parser::Array()
{
    if(match(LBRACK)){
        ConstExp();
        if(match(RBRACK))
            Array();
    }
}

void Parser::ConstInitVal()
{
    if(match(LBRACE)){
        ConstInitValTail();
        if(match(RBRACE))
            ;
    }else{
        ConstExp();
    }
}

void Parser::ConstInitValTail()
{
    if(look->tag==RBRACE)
        return;
    else{
        ConstInitVal();
        CVTTail();
    }
}

void Parser::CVTTail()
{
    if(match(COMMA)){
        VarDef();
        VarDeclTail();
    }
}

void Parser::VarDecl()
{
    if(match(KW_INT)){
        Btype();
        VarDef();
        VarDeclTail();
        if(match(SEMICON))
            ;
    }
}

void Parser::VarDeclTail()
{
    if(match(COMMA)){
        VarDef();
        VarDeclTail();
    }
}

void Parser::VarDef()
{
    if(match(ID)){
        Array();
        VardefTail();
    }
}

void Parser::VardefTail()
{
    if(match(ASSIGN)){
        initVal();
    }
}

void Parser::initVal()
{
    if(match(LBRACE)){
        initValTail();
        if(match(RBRACE))
            ;
    }else
        Exp();
}

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
    FuncType();
    if(match(ID)&&match(LPAREN)){
        FuncFParams();
        if(match(RPAREN))
            Block();
    }
}

void Parser::FuncType()
{
    if(look->tag==KW_VOID||look->tag==KW_INT){
        move();
    }
}

void Parser::FuncFParams()
{
   if(look->tag==RPAREN)
       return;
   FuncFParam();
   FuncFParamsTail();
}

void Parser::FuncFParamsTail()
{
    if(match(COMMA)){
        FuncFParam();
        FuncFParamsTail();
    }
}

void Parser::FuncFParam()
{
    Btype();
    if(match(ID)){
        ArrayParam();
    }
}

void Parser::ArrayParam()
{
    if(match(LBRACK)&&match(RBRACK)){
        ArrayParamTail();
    }
}

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
    else if(look->tag==ID||look->tag==LPAREN||look->tag==INTCONST
    ||look->tag==ADD||look->tag==SUB||look->tag==NOT||look->tag==SEMICON
    ||look->tag==LBRACE||look->tag==KW_IF||look->tag==KW_WHILE||look->tag==KW_BREAK
    ||look->tag==KW_CONTINUE||look->tag==KW_RETURN){
        Stmt();
        BlockItem();
    }else{
        Decl();
        BlockItem();
    }
}

void Parser::Stmt()
{
    if(match(ID)){
        LValOrFunc();
    }else if(match(LPAREN)){
        NewMulExp();
        NewAddExp();
    }else if(match(INTCONST)){
        NewMulExp();
        NewAddExp();
    }else if(look->tag==ADD||look->tag==SUB||look->tag==NOT){
        UnaryOp();
        UnaryExp();
        NewMulExp();
        NewAddExp();
    }else if(match(SEMICON))
        return;
    else if(match(KW_IF)){
        if(match(LPAREN)){
            Cond();
            if(match(RPAREN)){
                Stmt();
                ElseOpt();
            }
        }
    }else if(match(KW_WHILE)){
        if(match(LPAREN)){
            Cond();
            if(match(RPAREN)){
                Stmt();
            }
        }
    }else if(match(KW_BREAK)){
        match(SEMICON);
    }else if(match(KW_CONTINUE)){
        match(SEMICON);
    }else if(match(KW_RETURN)){
        ExpOpt();
        match(SEMICON);
    }
}

void Parser::LValOrFunc(){
    if(match(LPAREN)){
        FuncRParams();
        if(match(RPAREN)){
            NewMulExp();
            NewAddExp();
        }
    }else{
        ArrayParamTail();
        EqualOrMul();
    }
}

void Parser::EqualOrMul()
{
    if(match(ASSIGN)){
        Exp();
    }else{
        NewMulExp();
        NewAddExp();
    }
}

void Parser::Exp()
{
    AddExp();
}

void Parser::Cond()
{
    LOrExp();
}

void Parser::LVal()
{
    if(match(ID)){
        ArrayParamTail();
    }
}

void Parser::PrimaryExp()
{
    if(match(LPAREN)){
        Exp();
        match(RPAREN);
    }else if(look->tag==INTCONST){
        Num();
    }else{
        LVal();
    }
}

void Parser::Num()
{
    match(INTCONST);
}

void Parser::UnaryExp()
{
    if(match(ID)){
        if(match(LPAREN)){
            FuncRParams();
            if(match(RPAREN))
                ;
        }
    }else if(look->tag==ADD||look->tag==SUB||look->tag==NOT){
        UnaryOp();
        UnaryExp();
    }else{
        PrimaryExp();
    }
}

void Parser::FuncRParams()
{
    if(look->tag==RPAREN)
        return;
    else{
        Exp();
        FuncRParamsTail();
    }
}

void Parser::FuncRParamsTail()
{
    if(match(COMMA)){
        Exp();
        FuncRParamsTail();
    }
}

void Parser::MulExp()
{
    UnaryExp();
    NewMulExp();
}

void Parser::NewMulExp()
{
    if(match(MUL)){
        UnaryExp();
        NewMulExp();
    }else if(match(DIV)){
        UnaryExp();
        NewMulExp();
    }else if(match(MOD)){
        UnaryExp();
        NewMulExp();
    }
}

void Parser::AddExp()
{
    MulExp();
    NewAddExp();
}

void Parser::NewAddExp()
{
    if(match(ADD)){
        MulExp();
        NewAddExp();
    }else if(match(SUB)){
        MulExp();
        NewAddExp();
    }
}

void Parser::RelExp()
{
    AddExp();
    NewRelExp();
}

void Parser::NewRelExp()
{
    if(match(MUL)){
        AddExp();
        NewRelExp();
    }else if(match(DIV)){
        AddExp();
        NewRelExp();
    }else if(match(MOD)){
        AddExp();
        NewRelExp();
    }
}

void Parser::EqExp()
{
    RelExp();
    NewEqExp();
}

void Parser::NewEqExp()
{   
    if(match(EQU)){
        RelExp();
        NewEqExp();
    }else if(match(NEQU)){
        RelExp();
        NewEqExp();
    }
}

void Parser::LAndExp()
{
    EqExp();
    NewLAndExp();
}

void Parser::NewLAndExp()
{
    if(match(AND)){
        EqExp();
        NewLAndExp();
    }
}

void Parser::LOrExp()
{
    LAndExp();
    NewLOrExp();
}

void Parser::NewLOrExp()
{
    if(match(OR)){
        LAndExp();
        NewLOrExp();
    }
}

void Parser::ConstExp()
{
    AddExp();
}