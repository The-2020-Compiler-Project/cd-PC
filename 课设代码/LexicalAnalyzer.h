#ifndenf _LEXICALANALYZER_H
#define _LEXICALANALYZER_H
#include<iostream>
#include<hash_map>
#include<string>
#define BUFLEN 80;//缓冲区大小
int lineLen = 0;//缓冲区内数据长度
int readPos = -1;//读取位置
char line[BUFLEN];//缓冲区
int lineNum = 1;//行号
int colNum = 0;//列号
char lastChar = ch;//上一个字符
char scan(FILE* file);//用于读文件

enum Tag {
	ERR,					//错误，异常
	END,					//文件结束标记
	ID,						//标识符
	KW_INT, KW_VOID,			//数据类型
	NUM,					//常量
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

//关键字表类
class Keywords
{
	struct string_hash {
		size_t operator()(const string& str) const {
			return __stl_hash_string(str.c_str());
		}
	};
	hash_map<string, tag, string_hash> keywords;
public:
	Keywords();					//关键字表初始化
	Tag getTag(string name);	//测试是否关键字
}
};
#endif