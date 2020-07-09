#pragma once
#include "common.h"
#include <ext/hash_map>
#include "Symbol.h"
#include "GenInter.h"
#include <unordered_map>
#include <vector>
#include <fstream>
using namespace __gnu_cxx;

class SymTab {
public:
    SymTab() {
        voidVar = new Var();
        addVar(voidVar);
        scopeNum = 0;
        currentFun = nullptr;
        inter = nullptr;
        scopePath.push_back(0);
    }

    ~SymTab() {
        unordered_map<string, Fun*>::iterator funIt, funEnd = funTab.end();
        for(funIt = funTab.begin(); funIt != funEnd; funIt++) {
            delete funIt->second;
        }
        unordered_map<string, vector<Var*>*>::iterator varIt, varEnd = varTab.end();
        for(varIt = varTab.begin(); varIt != varEnd; varIt++) {
            vector<Var*>& list = *varIt->second;
            for(int i = 0; i < (int)list.size(); i++)
                delete list[i];
            delete &list;
        }
    }

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
            varList.push_back(var->getName());
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
            delete fun;
            fun = funTab[n];
        }
        currentFun = fun;
        inter->genFunHead(currentFun);           //函数入口
    }

    //结束定义函数
    void endDefFun() {
        inter->genFunTail(currentFun);            //生成函数出口代码
        currentFun = nullptr;                     //复位
    }

    //添加一条中间代码
    void addInst(InterInst *inst)
    {
        if(currentFun)currentFun->addInst(inst);
        else delete inst;
    }

    //获取函数对象
    Fun* getFun(string name, vector<Var*>& args) {
        if(funTab.find(name) != funTab.end()) {
            Fun* res = funTab[name];
            if(!res->match(args)) {
                return nullptr;
            }
            return res;
        }
        return nullptr;
    }

    //获取当前分析的函数
    Fun* getCurFun() {
        return currentFun;
    }

    vector<int>& getScopePath()
    {
        return scopePath;
    }
    /******************************数据段**********************************/

    //获取全局变量
    vector<Var*> getGlbVars() {
        vector<Var*> glbVars;
        for(int i = 0; i < varList.size(); i++) {
            string varName = varList[i];
            if(varName[0] == '<')
                continue;
            vector<Var*>& List = *varTab[varName];
            for(int j = 0; j < (int)List.size(); j++) {
                if(List[j]->getPath().size() == 1) {
                    glbVars.push_back(List[j]);
                    break;
                }
            }
        }
        return glbVars;
    }

    /*vector<Var*> getGlbVars() {
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
    }*/

    void genData(ofstream file) {
        vector<Var*> glbVars = getGlbVars();
        file << ".data" << endl;
        for(int i = 0; i < (int)glbVars.size(); i++) {
            Var* tmp = glbVars[i];
            file << "\t.global " << tmp->getName() << endl;
            if(!tmp->unInit()) {
                file << tmp->getName() << ":\n";
                //file << "\t" << tmp->getName() << ":\t" << ".word ";  //var:   .word 6
                if(!tmp->getArray())
                    file << "\t" << ".word" << " " << tmp->getVal() << endl;
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

    /*void genData(FILE* file) {
        //生成数据段和bss段
        fprintf(file, ".data\n");
        vector<Var*> glbVars=getGlbVars();//获取所有全局变量
        for(unsigned int i=0;i<glbVars.size();i++)
        {
            Var*var=glbVars[i];
            fprintf(file, "\t.global %s\n",var->getName().c_str());//.global var
            if(!var->unInit()){//变量初始化了,放在数据段
                fprintf(file, "%s:\n", var->getName().c_str());//var:
                if(!var->getArray()){//基本类型初始化 100 'a'
                    fprintf(file, "\t.word %d\n", var->getVal());//.byte 65  .word 100
                }
                else{//字符指针初始化
                    //fprintf(file, "\t.word %s\n",var->getPtrVal().c_str());//.word .L0
                }
            }
            else{//放在bss段
                fprintf(file, "\t.comm %s,%d\n", var->getName().c_str(), var->getSize());//.comm var,4
            }
        }
    }*/

    /*void genData(ofstream file) {
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
    }*/

    void setIr(GenIR*ir) {
	    this->inter = ir;
    }

    //输出符号表信息
    void toString() {
        cout << "----------变量表----------\n";
        for(int i = 0; i < (int)varList.size(); i++) {
            string varName = varList[i];
            vector<Var*>& List = *varTab[varName];
            cout << varName << ":\n";
            for(int j = 0; j < List.size(); j++) {
                cout << '\t';
                List[j]->toString();
                cout << endl;
            }
        }
        cout << "----------函数表----------\n";
        for(int i = 0; i < (int)funList.size(); i++) {
            funTab[funList[i]]->toString();
        }
    }
    
    void printInterCode() {
        for(int i = 0; i < (int)funList.size(); i++){
            funTab[funList[i]]->printInterCode();
        }
    }

public:
    static Var* voidVar = nullptr;

private:
    unordered_map<string, vector<Var*>*> varTab;  //变量表，值为同名变量链
    unordered_map<string, Fun*> funTab;           //函数表
    Fun* currentFun;                              //当前分析的函数
    int scopeNum;                                 //作用域编号
    vector<int> scopePath;                        //作用域的嵌套路径
    vector<string> varList;                       //变量名
    vector<string> funList;                       //所有的函数名，按定义顺序存放
    GenIR* inter;                                 
};
