#ifndef GrammarAnalyzer_h
#define GrammarAnalyzer_h

//文法分析器
class Parser {
    //代码段
    void CompUnit();//编译单元
    void UnitTail();//单元尾
    
    //声明与定义
    Tag BType();//基本类型
    /*****************常量这三个的返回值有待确认****************/
    void ConstDecl();//常量声明
    Var* ConstDef();//常量定义
    Var* ConstInitVal(bool isList, string name);//常量初值
    void VarDecl();//变量声明
    Var* VarDef();//变量定义
    Var* InitVal(bool isList, string name);//变量初值
    
    //函数
    Tag FuncType();//函数类型
    void FuncDef();//函数定义
    void FuncFParams(vector<Var*>&list);//函数形参表
    Vat* FuncFParam(string name);//函数形参
    void Block();//语句块
    void BlockItem();//语句块项
    void Stmt();//语句
    
    //表达式
    Var* Exp();//表达式
    Var* Cond();//条件表达式
    Var* LVal();//左值表达式
    Var* PrimaryExp();//基本表达式
    Var* Number();//数值
    Var* UnaryExp();//一元表达式
    Var* MulExp();//乘除模表达式
    Var* AddExp();//加减表达式
    Var* RelExp();//关系表达式
    Var* EqExp();//相等性表达式
    Var* LAndExp();//逻辑与表达式
    Var* LOrExp();//逻辑或表达式
    Var* ConstExp();//常量表达式
    Tag UnaryOp();//单目运算符
    void FuncRParams();//函数实参表
    
    
    //词法分析器
    Lexer &lexer;//词法分析器
    Token* look;//超前查看的字符
    
    //符号表
    
    //中间代码生成器
    
    //语法分析与错误修复
    void Move();//移进
    bool Match(Tag t);//匹配,成功则移进
    void Recovery(bool cond,SynError lost,SynError wrong);//错误修复

public:
    //构造函数
    Parser(Lexer&lex,SymTab&tab,GenIR&inter);
    //外部接口
    void Analyse();//语法分析主程序
};

#endif /* GrammarAnalyzer_h */

