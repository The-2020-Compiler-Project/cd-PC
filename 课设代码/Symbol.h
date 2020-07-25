#pragma once
#include <iostream>
#include "common.h"
#include "Token.h"
#include "Inter.h"
#include "GenInter.h"
#include "Symtab.h"
#include "platform.h"
#include "selector.h"

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
        switch (tk->tag) {
        case INTCONST:
            setType(KW_INT);
            name = "<int>";
            intVal = ((Num*)tk)->val;
            break;
        case STR:
            setType(KW_CHAR);
            name = GenIR::genLb();
            strVal = ((Str*)tk)->str;
            setArray(strVal.size() + 1);
        default:
            break;
        }
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

    //变量，指针
    Var(vector<int>&sp, Tag t, bool ptr, string name, Var*init) {
        clear();
        scopePath = sp;  //初始化路径
        setType(t);
        setPtr(ptr);
        setName(name);
        initData=init;
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

    //临时变量
    Var(vector<int>& scope, Tag t, bool isarray) {
        clear();
        scopePath = scope;
        setType(t);
        isArray = isarray;
        setName("");
        setLeft(false);
    }

    //构造变量
    Var(vector<int>& scope, Tag t, string name, Var* init = NULL) {
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
        if(s == "")
            s = GenIR::genLb();
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
        else if(this->type == KW_CHAR)
            size = 1;
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
        isLeft = false;
        arrayLength = sumLen;
        size *= arrayLength;                   //总大小=长度*类型大小
    }

    void setArray(int len)
    {
        if (len <= 0) {
            //SEMERROR(ARRAY_LEN_INVALID, name);//数组长度小于等于0错误
            return;
        }
        else {
            isArray = true;
            isLeft = false;                     //数组不能作为左值
            arrayLength = len;
            size *= len;                        //变量大小乘以数组长度
        }
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
            if (init->isArray) {
                if (init->type == KW_CHAR)             //字符数组只能是字符串了
                    ptrVal = init->name;
                else
                    arrayVal = init->arrayVal;
            }
            else
                intVal = init->intVal;
        }
        else 
            return true;
        return false;
    }

   //判断字符指针
   bool isCharPtr() {
       return type == KW_CHAR && !isBase();
   }

   //获取指针
   bool getPtr() {
       return isPtr;
   }

    //获取初始化值
    Var* getInitData() {
        return initData;
    }

    string getRawStr() {
        string raw;
        for (int i = 0; i < strVal.length(); i++) {
            switch (strVal[i]) {
            case '\n':
                raw.append("\\n");
                break;
            case '\t':
                raw.append("\\t");
                break;
            case '\0':
                raw.append("\\000");
                break;
            case '\\':
                raw.append("\\\\");
                break;
            case '\"':
                raw.append("\\\"");
                break;
            default:
                raw.push_back(strVal[i]);
                break;
            }
        }
        raw.append("\\000");
        return raw;
    }

    //设置变量的左值
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

    //基本类型常量（字符串除外）
    bool isLiteral() {
        return isConstant && isBase();
    }

    //获取值
    int getVal() {
        return intVal;
    }

    string getStrVal() {
        return strVal;
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
        if (!ptr)
            return;
        isPtr = true;
        size = 4;
    }

    //设置指针
    void setPointer(Var* ptr) {
        this->ptr = ptr;
    }

    //获取指针变量
    Var* getPointer() {
        return ptr;
    }

    string getPtrVal() {
        return ptrVal;
    }

    //输出变量的中间代码形式
    void value() {
        if(isConstant) {
            if(type == KW_INT) 
                cout << intVal;
            else if (type == KW_CHAR) {
                if (isArray)
                    cout << name;
            }
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
        isInited = false;
        isLeft = true;
        size = 0;
        offset = 0;
        index = -1;
        live = false;
        initData = nullptr;
        ptr = nullptr;
        regId = -1;
        inMem = false;
    }

    //获取特殊变量
    static Var* getVoid();

    //输出变量信息
    void toString() {
        cout << tokenName[type];
        if (isPtr)
            cout << "*";
        cout << " " << name;
        if (isArray)
            cout << "[" << arrayLength << "]";
        if(isInited) {
            cout << " = ";
            switch(type) {
                case KW_INT: cout << intVal; break;
                case KW_CHAR: 
                    if (isPtr)
                        cout << "<" << ptrVal << ">";
                    break;
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

    bool isChar() {return false;}

    //寄存器分配信息
    int regId;                                    //分配的寄存器编号，-1表示分配在内存
    bool inMem;                                   //被取地址的变量的标记，不分配寄存器
    
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
    string strVal;                                //字符串初值
    string ptrVal;                                //字符指针初值
    vector<int> arrayVal;                         //数组初值
    vector<int> dimension;                        //数组维度
    int offset;                                   //变量的栈帧偏移
    vector<int> scopePath;                        //作用域的嵌套路径
    int size;                                     //变量大小，单位：字节
    Var* ptr;                                     //指向当前变量指针变量

    //数据流分析
    int index;                                    //列表索引
    bool live;                                    //活跃性
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
        SP = Plat::stackBase;
        maxDepth = Plat::stackBase;
        for(int i = 0, argOff = 4; i < parameter.size(); i++, argOff += 4)  //为参数分配栈空间
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
       /* for(int i = 0; i < parameter.size(); i++) {
            if(!GenIR::typeCheck(parameter[i], args[i]))
                return false;
        } */
        return true;
    }

    Tag getType() {
        return type;
    }

    //获取参数列表
    vector<Var*>& getParaVar() {
        return parameter;
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

    void genAsm(FILE* file) {
        vector<InterInst*> code;
        code = interCode.getCode();
        const char* pname = name.c_str();
        fprintf(file, "#Code of %s Function\n", pname);
        fprintf(file, "\t.global %s\n", pname);//.global fun\n
        fprintf(file, "%s:\n", pname);//fun:\n
        ILoc il;//ILOC代码
        //将最终的中间代码转化为ILOC代码
        Selector sl(code, il);//指令选择器
        sl.select();
        //对ILOC代码进行窥孔优化
        //PeepHole ph(il);//窥孔优化器
        //将优化后的ILOC代码输出为汇编代码
        il.outPut(file);
    }

    int getMaxDep() {
        return maxDepth;
    }

    void setMaxDep(int dep) {
        maxDepth = dep;
        relocated = true;
    }

    bool isRelocated() {
        return relocated;
    }

private:    
    string name;                                  //函数名
    Tag type;                                     //返回值类型
    vector<Var*> parameter;                       //参数表
    int maxDepth;                                 //栈需要分配的最大空间
    int SP;                                       //当前栈指针，指向栈顶
    bool relocated;                               //栈帧重定位标记
    vector<int> scopeSP;                          //作用域栈指针
    InterCode interCode;                          //目标代码
    InterInst* retPoint;                          //返回点
};