#pragma once
#include "common.h"
#include "Token.h"
#include "Inter.h"
#include "GenInter.h"
#include "Symtab.h"
#include <iostream>

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
        setType(v->getType());
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
    Var(vector<int>& scope, Tag t, string name, Var* init=NULL) {
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
        if(init->isConstant) {
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

    bool isBase() {
        return !isArray && !isPtr;
    }

    bool isRef() {
        return !!ptr;
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
        return "";
    }

    //获取特殊变量
    static Var* getVoid();

    //输出变量信息
    void toString() {
        cout << tokenName[type];
        cout << " " << name;
        if(isInited) {
            cout << " = ";
            switch(type) {
                case KW_INT: cout << intVal; break;
            }
        }
        cout << "; size=" << size << " scope=\"";
        for(int i = 0; i < (int)scopePath.size(); i++) {
            cout << "/" << scopePath[i];
        }
        cout << "\" ";
        if(offset > 0)
            cout << "addr=[ebp+" << offset << "]";
        else if(offset < 0)
            cout << "addr=[ebp" << offset << "]";
        else if(name[0] != '<')
            cout << "addr=<" << name << ">";
        else
            cout << "value=\'" << getVal() << "\'";
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
        if(parameter.size() != args.size())
            return false;
        for(int i = 0; i < parameter.size(); i++) {
            /*if(!GenIR::typeCheck(parameter[i], args[i]))
                return false;*/
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

    void printInterCode() {
        cout << "-------------<" << name << ">Start--------------\n";
        interCode.toString();
        cout << "--------------<" << name << ">End---------------\n";
    }

    //函数信息
    void toString() {
        cout << tokenName[type];
        cout << " " << name;
        cout << "(";
        for(int i = 0; i < (int)parameter.size(); i++) {
            cout << "<" << parameter[i]->getName() << ">";
            if(i != (int)parameter.size() - 1)
                cout << ",";
        }
        cout << ")";
        cout << ":\n";
        cout << "\t\tmaxDepth=" << maxDepth << endl;
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