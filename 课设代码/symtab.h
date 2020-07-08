#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#define SEMERROR(code, name) Error::semError(code, name)
using namespace std;

class Var;
class Fun;

class Symtab {
public:
    void enter() {
        scopeNum++;
        scopePath.push_back(scopeNum);
        if(currentFun != nullptr)
            currentFun->enterScope();
    }

    void leave() {
        scopePath.pop_back();
        if(currentFun != nullptr)
            currentFun->leaveScope();
    }

    //添加变量
    void addVar(Var* var) {
        if(varTab.find(var->getName()) == varTab.end()) {
            varTab[var->getName()] = new vector<Var*>;
            varTab[var->getName()]->push_back(var);
        }
        else {
            vector<Var*>& list = *varTab[var->getName()];
            int i;
            for(i = 0; i < list.size(); i++) {
                if(list[i]->getPath().back() == var->getPath().back())
                    break;
            }
            if(i == list.size() || var->getName() == "<int>")    //数据类型只有int
                list.push_back(var);
            else {
                SEMERROR(VAR_RE_DEF, var->getName());
                delete var;
                return;
            }
        }
        if(inter) {
            int flag = inter->genVarInit(var);
            if(currentFun && flag)
                currentFun->locate(var);
        }
    }

    //获取变量
    Var* getVar(string name) {
        Var* res = nullptr;
        if(varTab.find(name) != varTab.end()) {
            vector<Var*>& list = *varTab[name];
            int pathLen = scopePath.size();
            int maxLen = 0;
            for(int i = 0; i < list.size(); i++) {
                int len = list[i]->getPath().size();
                if(len <= pathLen && list[i]->getPath()[len - 1] == scopePath[len - 1]) {
                    if(len > maxLen) {            //选择离当前作用域最近的
                        maxLen = len;
                        res = list[i];
                    }
                }
            }
        }
        if(res == nullptr)
            SEMERROR(VAR_UN_DEC, name);           //变量未声明
        return res;
    }

    //定义函数
    void defFun(Fun* fun) {
        string n = fun->getName();
        if(funTab.find(n) == funTab.end()) {
            funTab[n] = fun;
            funList.push_back(n);
        }
        else {
            SEMERROR(FUN_RE_DEF, n)               //函数重定义
            delete fun;
            fun = funTab[n];
        }
        currentFun = fun;
        inter->genFunEntry(currentFun);           //函数入口
    }

    //结束定义函数
    void endDefFun() {
        inter->genFunExit(currentFun);            //生成函数出口代码
        currentFun = nullptr;                     //复位
    }

    //获取函数对象
    Fun* getFun(string name, vector<Var*>& args) {
        if(funTab.find(name) != funTab.end()) {
            Fun* res = funTab[name];
            if(!res->match(args)) {
                SEMERROR(FUN_CALL_ERR, name);
                return nullptr;
            }
            return res;
        }
        SEMERROR(FUN_UN_DEC, name);
        return nullptr;
    }

    //获取当前分析的函数
    Fun* getCurFun() {
        return currentFun;
    }

    vector<int>& getScopePath() {
        return scopePath;
    } 

    void setIr(GenIR* ir) {
        inter = ir;
    }
    
    /******************************数据段**********************************/

    //获取全局变量
    vector<Var*> getGlbVars() {
        vector<Var*> glbVars;
        unordered_map<string, vector<Var*>*>::iterator it, varEnd = varTab.end();
        for(it = varTab.begin(); it != varEnd; it++) {
            string varName = it->first;
            if(varName[0] == '<')
                continue;
            vector<Var*>& list = *it->second;
            for(int j = 0; j < list.size(); j++) {
                if(list[j]->getPath().size() == 1) {
                    glbVars.push_back(list[j]);
                    break;                        //全局变量同名只有一个
                }
            }
        }
        return glbVars;
    }

    void genData(ofstream file) {
        vector<Var*> glbVars = getGlbVars();
        file << ".data" << endl;
        for(int i = 0; i < (int)glbVars.size(); i++) {
            Var* tmp = glbVars[i];
            if(!tmp->unInit()) {
                file << "\t" << tmp->getName() << ":\t" << ".word ";  //var:   .word 6
                if(!tmp->getArray())
                    file << tmp->getVal();
                else {
                    vector<int> a = tmp->getArrayVal();
                    file << a[0];
                    for(int i = 1; i < a.size(); i++) 
                        file << ", " << a[i];
                }
            }
            else {
                file << "\t.comm " << tmp->getName() << "," << tmp->getSize() << endl;  //.bss
            }            
        }
    }

public:
    static Var* voidVar;

private:
    unordered_map<string, vector<Var*>*> varTab;  //变量表，值为同名变量链
    unordered_map<string, Fun*> funTab;           //函数表
    Fun* currentFun;                              //当前分析的函数
    int scopeNum;                                 //作用域编号
    vector<int> scopePath;                        //作用域的嵌套路径
    vector<string> funList;                       //所有的函数名，按定义顺序存放
    GenIR* inter;                                 //
};

class Var {
public:
    Var() {
        clear();
        setName("<void>");
        setLeft(false);
        intVal = 0;
        isConstant = false;
        setType(KW_VOID);
        isPtr = true;
    }

    //创建常量的变量，无作用域
    Var(Token* tk) {
        clear();
        isConstant = true;
        setType(KW_INT);
        setLeft(false);
        name = "<int>";
        intVal = ((Num*)tk)->val;
    }

    //整数
    Var(int v) {
        clear();
        setName("<int>");
        isConstant = true;
        setType(KW_INT);
        intVal = v;
        setLeft(false);
    }

    //数组
    Var(vector<int>& scope, Tag t, string name, vector<int>& dime) {
        clear();
        scopePath = scope;
        setName(name);
        setType(t);
        setArray(dime);
    }

    //拷贝出一个临时变量
    Var(vector<int>& scope, Var* v) {
        clear();
        scopePath = scope;
        setType(v->getType);
        if(v->isArray)
            setArray(v->dimension);
        setPtr(v->isArray || v->isPtr);
        setName("");
        setLeft(false);
    }

    //临时变量（会有数组吗？）
    Var(vector<int>& scope, Tag t, bool isarray) {
        clear();
        scopePath = scope;
        setType(t);
        isArray = isarray;
        setName("");
        setLeft(false);
    }

    //构造变量
    Var(vector<int>& scope, Tag t, string name, Var* init) {
        clear();
        scopePath = scope;
        setType(t);
        setName(name);
        initData = init;
    }

    //获取变量名
    string getName() {
        return name;
    }

    //设置变量名
    void setName(string s) {
        name = s;
    }

    //获取作用域路径
    vector<int>& getPath() {
        return scopePath;
    }

    //获取大小
    int getSize() {
        return size;
    }

    //获取类型
    Tag getType() {
        return type;
    }

    void setType(Tag type) {
        this->type = type;
        if(this->type == KW_VOID) {
            SEMERROR(VOID_VAR, "");
            this->type = KW_INT;
        }
        if(this->type == KW_INT)
            size = 4;
    }

    //设置数组维度
    void setArray(vector<int> dimension) {
        this->dimension = dimension;
        int sumLen = 1;
        bool isAllPos = true;                  //数组维度是否大于零
        for(int i: this->dimension) {
            if(i <= 0) {
                isAllPos = false;
                break;
            }
            sumLen *= i;
        }
        if(!isAllPos) {
            SEMERROR(ARRAY_LEN_INVALID, name);
            return;
        }
        isArray = true;
        arrayLength = sumLen;
        size *= arrayLength;                   //总大小=长度*类型大小
    }

    //获取数组
    bool getArray() {
        return isArray;
    }

    //设置偏移
    void setOffset(int d) {
        offset = d;
    }

    //获取偏移
    int getOffset() {
        return offset;
    }

    //变量初始化
    bool setInit() {
        Var* init = initData;
        if(!init)
            return false;
        isInited = false;
        if(!GenIR::typeCheck(this, init))
            SEMERROR(VAR_INIT_ERR, name);
        else if(init->isConstant) {
            isInited = true;
            if(init->isArray)
                arrayVal = init->arrayVal;
            else
                intVal = init->intVal;
        }
        else 
            return true;
        return false;
    }

    //获取初始化值
    Var* getInitData() {
        return initData;
    }

    void setLeft(bool L) {
        isLeft = L;
    }

    //能否作左值
    bool getLeft() {
        return !isConstant && !isArray;
    }

    //是否初始化
    bool unInit() {
        return !isInited;
    }

    //是否不是常量
    bool notConst() {
        return !isConstant;
    }

    //获取值
    int getVal() {
        return intVal;
    }

    //获取数组值
    vector<int> getArrayVal() {
        return arrayVal;
    }

    vector<int> getDimension() {
        return dimension;
    }

    //
    bool isVoid() {
        return type == KW_VOID;
    }

    //设置是否为指针
    void setPtr(bool ptr) {
        isPtr = ptr;
        size = 4;
    }

    //是否为指针
    bool getPtr() {
        return isPtr;
    }

    //是否为引用
    bool isRef() {
        return !!ptr;
    }

    //设置指针
    void setPointer(Var* ptr) {
        this->ptr = ptr;
    }

    //获取指针变量
    Var* getPointer() {
        return ptr;
    }

    //输出变量的中间代码形式
    void value() {
        if(isConstant) {
            cout << intVal;
        }
        else
            cout << getName();
    }
    
    //
    void clear() {
        scopePath.push_back(-1);
        isArray = false;
        isPtr = false;
        isConstant = false;
        isInited = true;
        isLeft = false;
        size = 0;
        offset = 0;
        index = -1;
        live = false;
        initData = nullptr;
    }

    //字符串转化
    string getRawStr() {

    }

    //获取特殊变量
    Var* getVoid() {
        return Symtab::voidVar;
    }

private:
    string name;                                  //变量名
    Tag type;                                     //变量类型
    bool isConstant;                              //是否为常量
    bool isArray;                                 //是否为数组
    bool isPtr;                                   //是否为指针
    bool isLeft;                                  //能否作左值
    int arrayLength;                              //数组长度
    Var* initData;                                //初值
    bool isInited;                                //是否初始化
    int intVal;                                   //整型初值
    vector<int> arrayVal;                         //数组初值
    vector<int> dimension;                        //数组维度
    int offset;                                   //变量的栈帧偏移
    vector<int> scopePath;                        //作用域的嵌套路径
    int size;                                     //变量大小，单位：字节
    Var* ptr;                                     //指向当前变量指针变量

    //数据流分析
    int index;                                    //列表索引
    bool live;                                    //活跃性

    //寄存器分配信息
    int regId;                                    //分配的寄存器编号，-1表示分配在内存
    bool inMem;                                   //被取地址的变量的标记，不分配寄存器
};

/*****************************************************************
 *                            函数
 * **************************************************************/
class Fun {
public:
    Fun(Tag t, string name, vector<Var*>& paraList) {
        type = t;
        this->name = name;
        parameter = paraList;
        SP = 0;
        maxDepth = 0;
        for(int i = 0, argOff = 8; i < parameter.size(); i++, argOff += 4)  //为参数分配栈空间
            parameter[i]->setOffset(argOff);
    }

    void enterScope() {
        scopeSP.push_back(0);
    }

    void leaveScope() {
        maxDepth = SP > maxDepth ? SP : maxDepth;
        SP -= scopeSP.back();
        scopeSP.pop_back();
    }

    void locate(Var* var) {
        int size = var->getSize();
        size += (4 - size % 4) % 4;               //4字节对齐
        scopeSP.back() += size;
        SP += size;
        var->setOffset(-SP);
    }

    string getName() {
        return name;
    }

    //形参与实参的匹配检查
    bool match(vector<Var*>& args) {
        if(parameter.size() != args)
            return false;
        for(int i = 0; i < parameter.size(); i++) {
            if(!GenIR::typeCheck(parameter[i], args[i]))
                return false;
        }
        return true;
    }

    Tag getType() {
        return type;
    }

    //获取返回点
    InterInst* getReturnPoint() {
        return retPoint;
    }

    //设置返回点
    void setReturnPoint(InterInst* inst) {
        retPoint = inst;
    }

    //添加中间代码
    void addInst(InterInst* inst) {
        interCode.addInst(inst);
    }

    bool isVoid() {
        return type == KW_VOID;
    }

private:    
    string name;                                  //函数名
    Tag type;                                     //返回值类型
    vector<Var*> parameter;                       //参数表
    int maxDepth;                                 //栈需要分配的最大空间
    int SP;                                       //当前栈指针，指向栈顶
    vector<int> scopeSP;                          //作用域栈指针
    InterCode interCode;                          //目标代码
    InterInst* retPoint;                          //返回点
};