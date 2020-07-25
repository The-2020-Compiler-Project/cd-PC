#include "Symtab.h"
#include "Symbol.h"

Var* SymTab::voidVar = nullptr;

SymTab::SymTab() {
    voidVar = new Var();
    addVar(voidVar);
    scopeNum = 0;
    currentFun = nullptr;
    inter = nullptr;
    scopePath.push_back(0);
}

SymTab::~SymTab() {
    //清楚函数
    unordered_map<string, Fun*>::iterator funIt, funEnd = funTab.end();
    for(funIt = funTab.begin(); funIt != funEnd; funIt++) {
        delete funIt->second;
    }
    //清楚变量
    unordered_map<string, vector<Var*>*>::iterator varIt, varEnd = varTab.end();
    for(varIt = varTab.begin(); varIt != varEnd; varIt++) {
        vector<Var*>& list = *varIt->second;
        for(int i = 0; i < (int)list.size(); i++)
            delete list[i];
        delete &list;
    }
    //清除字符串
    unordered_map<string, Var*>::iterator strIt, strEnd = strTab.end();
    for (strIt = strTab.begin(); strIt != strEnd; strIt++)
        delete strIt->second;
}

void SymTab::enter() {
    scopeNum++;
    scopePath.push_back(scopeNum);
    if(currentFun != nullptr)
        currentFun->enterScope();
}

void SymTab::leave() {
    scopePath.pop_back();
    if(currentFun != nullptr)
        currentFun->leaveScope();
}

//添加变量
void SymTab::addVar(Var* var) {
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
        if(i == list.size() || var->getName()[0] == '<')    //数据类型只有int
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

void SymTab::addStr(Var* v) {
    strTab[v->getName()] = v;
}

//获取变量
Var* SymTab::getVar(string name) {
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
    return res;
}

//定义函数
void SymTab::defFun(Fun* fun) {
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
void SymTab::endDefFun() {
    inter->genFunTail(currentFun);            //生成函数出口代码
    currentFun = nullptr;                     //复位
}

//添加一条中间代码
void SymTab::addInst(InterInst *inst)
{
    if(currentFun)
        currentFun->addInst(inst);
    else 
        delete inst;
}

//获取函数对象
Fun* SymTab::getFun(string name, vector<Var*>& args) {
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
Fun* SymTab::getCurFun() {
    return currentFun;
}

vector<int>& SymTab::getScopePath()
{
    return scopePath;
}
/******************************数据段**********************************/

//获取全局变量
vector<Var*> SymTab::getGlbVars() {
    vector<Var*> glbVars;
    for(int i = 0; i < (int)varList.size(); i++) {
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

/*void SymTab:: genData(ofstream file) {
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

void SymTab::genData(FILE* file) {
    fprintf(file, ".section .rodata\n");
    unordered_map<string, Var*>::iterator strIt, strEnd = strTab.end();
    for (strIt = strTab.begin(); strIt != strEnd; strIt++) {
        Var* str = strIt->second;//常量字符串变量
        fprintf(file, "%s:\n", str->getName().c_str());//var:
        fprintf(file, "\t.ascii \"%s\"\n", str->getRawStr().c_str());//.ascii "abc\000"
    }
    //生成数据段和bss段
    fprintf(file, ".data\n");
    vector<Var*> glbVars = getGlbVars();//获取所有全局变量
    for(unsigned int i = 0; i < glbVars.size(); i++) {
        Var*var=glbVars[i];
        fprintf(file, "\t.global %s\n",var->getName().c_str());//.global var
        if(!var->unInit()){//变量初始化了,放在数据段
            fprintf(file, "%s:\n", var->getName().c_str());//var:
            if(var->isBase()){//基本类型初始化 100 'a'
                fprintf(file, "\t.word %d\n", var->getVal());//.byte 65  .word 100
            }
            else{//字符指针初始化
                fprintf(file, "\t.word %s\n",var->getPtrVal().c_str());//.word .L0
            }
        }
        else{//放在bss段
            fprintf(file, "\t.comm %s,%d\n", var->getName().c_str(), var->getSize());//.comm var,4
        }
    }
}

void SymTab::setIr(GenIR*ir) {
    this->inter=ir;
}

//输出符号表信息
void SymTab:: toString() {
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
    cout << "----------串表-----------\n";
    unordered_map<string, Var*>::iterator strIt, strEnd = strTab.end();
    for (strIt = strTab.begin(); strIt != strEnd; strIt++)
        cout << strIt->second->getName() << "=" << strIt->second->getStrVal() << endl;
    cout << "----------函数表----------\n";
    for(int i = 0; i < (int)funList.size(); i++) {
        funTab[funList[i]]->toString();
    }
}

void SymTab::printInterCode() {
    for(int i = 0; i < (int)funList.size(); i++){
        funTab[funList[i]]->printInterCode();
    }
}

void SymTab::genAsm(char* fileName) {
    string newName = fileName;
    int pos = newName.rfind(".sysy");
    if(pos > 0 && pos == newName.length() - 5){
        newName.replace(pos, 5, ".s");
    }
    else newName = newName + ".s";
    FILE* file = fopen(newName.c_str(), "w");//创建输出文件
    //生成数据段
    genData(file);
    //生成代码段
    //if(Args::opt)fprintf(file,"#优化代码\n");
    //else fprintf(file,"#未优化代码\n");
    fprintf(file, ".text\n");
    for(int i = 0; i < funList.size(); i++){
        //printf("-------------生成函数<%s>--------------\n",funTab[funList[i]]->getName().c_str());
        funTab[funList[i]]->genAsm(file);
    }
    fclose(file);
}