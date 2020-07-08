#pragma once
#ifndef _LEXICALANALYZER_H
#define _LEXICALANALYZER_H
#include<iostream>
#include<string>
#include<list>
#include <ext/hash_map>
using namespace __gnu_cxx;
int lineLen = 0;//缓冲区内数据长度
int readPos = -1;//读取位置
int lineNum = 1;//行号
int colNum = 0;//列号
char lastChar;//上一个字符
using namespace std;
enum Tag {
	ERR,					//错误，异常
	END,					//文件结束标记
	ID,						//标识符
	KW_INT, KW_VOID,			//数据类型
	CONST,					//常量
	NOT, LEA,				//单目运算符
	ADD, SUB, MUL, DIV, MOD,	//算数运算符
	INC, DEC,				//自加自减
	GT, GE, LT, LE, EQU, NEQU,	//比较运算符
	AND, OR,					//逻辑运算符
	LPAREN, RPAREN,			//（）
	LBRACK, RBRACK,			//[]
	LBRACE, RBRACE,			//{}
	COMMA, COLON, SEMICON,	//逗号，冒号，分号
	ASSIGN,					//赋值
	KW_IF, KW_ELSE,			//if-else
	KW_WHILE,				//while循环
	KW_BREAK, KW_CONTINUE, KW_RETURN //break,continue,return
};
//词法记号类
//作为基类
class Token
{
public:
	Tag tag;	//内部标签
	Token(Tag t);	//构造函数
	virtual string toString();
	virtual ~Token();
};

//以下类均为Token的派生类
//标识符记号类
class Id :public Token
{
public:
	string name;
	Id(string n);
	virtual string toString();
};
//数字记号类
class Num :public Token
{
public:
	int val;
	Num(int v);
	virtual string toString();

};
//派生类到此为止
class Scanner
{
	//文件指针
	char* fileName;//文件名
	FILE* file;//文件指针

	//内部状态
	static const int BUFLEN = 80;//扫描缓冲区长度
	char line[BUFLEN];
	int lineLen;//当前行的长度
	int readPos;//读取的位置
	char lastch;//上一个字符，主要用于判断换行位置	

	//读取状态
	int lineNum;//记录行号
	int colNum;//列号

	//显示字符
	void showChar(char ch);

public:

	//构造与初始化
	Scanner(char* name);
	~Scanner();

	//扫描
	int scan();//基于缓冲区的字符扫描算法,文件扫描接受后自动关闭文件

	//外部接口
	char* getFile();//获取文件名
	int getLine();//获取行号
	int getCol();//获取列号

};
//关键字表类
class Keywords
{
	struct string_hash {
		size_t operator()(const string& str) const {
			return __stl_hash_string(str.c_str());
		}
	};
	hash_map<string, Tag> keywords;
public:
	Keywords();					//关键字表初始化
	Tag getTag(string name);	//测试是否关键字
};

//词法分析器

class Lexer
{
	static Keywords keywords;//关键字列表

	Scanner& scanner;//扫描器
	char ch;//读入的字符
	bool scan(char need = 0);//扫描与匹配

	Token* token;//记录扫描的词法记号

public:
	Lexer(Scanner& sc);
	~Lexer();
	Token* tokenize();//有限自动机匹配，词法记号解析
};

//扫描器


#endif
