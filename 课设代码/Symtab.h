#pragma once
#include "common.h"
#include <ext/hash_map>
#include "Symbol.h"
#include "GenInter.h"
#include <unordered_map>
#include <vector>
#include <fstream>
using namespace __gnu_cxx;

class Var;

class SymTab {
public:
    SymTab();//初始化符号表
	~SymTab();//清除内存

    void enter();

    void leave();

    //添加变量
    void addVar(Var* var);

    //获取变量
    Var* getVar(string name);

    //定义函数
    void defFun(Fun* fun);

    //结束定义函数
    void endDefFun();

    //添加一条中间代码
    void addInst(InterInst *inst);

    //获取函数对象
    Fun* getFun(string name, vector<Var*>& args);

    //获取当前分析的函数
    Fun* getCurFun();

    vector<int>& getScopePath();
    /******************************数据段**********************************/

    //获取全局变量
    vector<Var*> getGlbVars();

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

    /*void genData(ofstream file) {
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
    }*/

    void genData(FILE* file);

    void setIr(GenIR*ir);

    //输出符号表信息
    void toString();

    void printInterCode();

    /*void genAsm(char* fileName) {
        string newName=fileName;
        int pos = newName.rfind(".c");
        if(pos>0&&pos==newName.length()-2){
            newName.replace(pos,2,".s");
        }
        else newName=newName+".s";
        FILE* file=fopen(newName.c_str(),"w");//创建输出文件
        //生成数据段
        genData(file);
        //生成代码段
        //if(Args::opt)fprintf(file,"#优化代码\n");
        //else fprintf(file,"#未优化代码\n");
        fprintf(file,".text\n");
        for(int i=0;i<funList.size();i++){
            //printf("-------------生成函数<%s>--------------\n",funTab[funList[i]]->getName().c_str());
            funTab[funList[i]]->genAsm(file);
        }
        fclose(file);
    }*/

public:
    static Var* voidVar;

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