#pragma once
#include"lexicalanalyzer.h"
#include <sstream>
using namespace std;
//词法记号标签
/*
	编译参数初始化
*/
bool Args::showChar = false;
bool Args::showToken = false;
bool Args::showSym = false;
bool Args::showIr = false;
bool Args::showOr = false;
bool Args::showBlock = false;
bool Args::showHelp = false;
bool Args::opt = false;
const char* tokenName[] =
{
	"error",//错误，异常，结束标记等
	"文件结尾",//文件结束
	"标识符",//标识符
	"int","void",//数据类型
	"数字",//字面量
	"!","&",//单目运算 ! - & *
	"+","-","*","/","%",//算术运算符
	"++","--",
	">",">=","<","<=","==","!=",//比较运算符
	"&&","||",//逻辑运算
	"(",")",//()
	"[","]",//[]
	"{","}",//{}
	",",":",";",//逗号,冒号,分号
	"=",//赋值
	"if","else",//if-else
	"while",//循环
	"break","continue","return"//break,continue,return
};

//扫描器类函数的定义
Scanner::Scanner(char* name)
{
	file = fopen(name, "r");//打开指定的待扫描文件
	fileName = name;
	//初始化扫描状态
	lineLen = 0;
	readPos = -1;
	lastch = 0;
	lineNum = 1;
	colNum = 0;
}

Scanner::~Scanner()
{
	if (file)
	{
		fclose(file);
	}
}

/*
   显示字符
*/
void Scanner::showChar(char ch)
{
	if (ch == -1)printf("EOF");
	else if (ch == '\n')printf("\\n");
	else if (ch == '\t')printf("\\t");
	else if (ch == ' ')printf("<blank>");
	else printf("%c", ch);
	printf("\t\t<%d>\n", ch);
}

int Scanner::scan()
{
	if (!file)//没有文件
		return -1;
	if (readPos == lineLen - 1)//缓冲区读取完毕
	{
		lineLen = fread(line, 1, BUFLEN, file);//重新加载缓冲区数据
		if (lineLen == 0)//没有数据了
		{
			//标记文件结束,返回文件结束标记-1
			lineLen = 1;
			line[0] = -1;
		}
		readPos = -1;//恢复读取位置
	}
	readPos++;//移动读取点
	char ch = line[readPos];//获取新的字符
	if (lastch == '\n')//新行
	{
		lineNum++;//行号累加
		colNum = 0;//列号清空
	}
	if (ch == -1)//文件结束，自动关闭
	{
		fclose(file);
		file = NULL;
	}
	else if (ch != '\n')//不是换行
		colNum++;//列号递增
	lastch = ch;//记录上个字符
	if (Args::showChar)showChar(ch);
	return ch;
}

/*
   获取文件名
*/
char* Scanner::getFile()
{
	return fileName;
}

/*
   获取行号
*/
int Scanner::getLine()
{
	return lineNum;
}

/*
   获取列号
*/
int Scanner::getCol()
{
	return colNum;
}


//Token类的函数定义

Token::Token(Tag t) :tag(t)			//构造函数
{}

string Token::toString()
{
	return tokenName[tag];
}

Token::~Token()
{}

//标识符类的函数定义

Id::Id(string n) :Token(ID), name(n)
{}

string Id::toString()
{
	return Token::toString() + name;
}

//数字类的函数定义

Num::Num(int v) :Token(COSNT), val(v)
{}

string Num::toString()
{
	stringstream ss;
	ss << val;
	return string("[") + Token::toString() + "]:" + ss.str();
}



//关键字列表初始化
Keywords::Keywords()
{
	//add keyword mapping here ~
	keywords["int"] = KW_INT;
	keywords["void"] = KW_VOID;
	keywords["if"] = KW_IF;
	keywords["else"] = KW_ELSE;
	keywords["while"] = KW_WHILE;
	keywords["break"] = KW_BREAK;
	keywords["continue"] = KW_CONTINUE;
	keywords["return"] = KW_RETURN;
}

//测试是否是关键字

Tag Keywords::getTag(string name)
{
	return keywords.find(name) != keywords.end() ? keywords[name] : ID;
}


/*
词法分析器
*/

Keywords Lexer::keywords;
//自动机实现解析
Lexer::Lexer(Scanner& sc) :scanner(sc)
{
	token = NULL;//初始化词法记号记录，该变量被共享
	ch = ' ';//初始化为空格
}

Lexer::~Lexer()
{
	if (!token)//删除已经记录的词法记号变量的内存，防止内存溢出
	{
		delete token;
	}
}

/*
   封装的扫描方法
*/
bool Lexer::scan(char need)
{
	ch = scanner.scan();//扫描出字符
	if (need) {
		if (ch != need)//与预期不吻合
			return false;
		ch = scanner.scan();//与预期吻合，扫描下一个
		return true;
	}
	return true;
}

Token* Lexer::tokenize()
{
	for (; ch != -1;) {//过滤掉无效的词法记号，只保留正常词法记号或者NULL
		Token* t = NULL;//初始化一个词法记号指针
		while (ch == ' ' || ch == '\n' || ch == '\t')//忽略空白符
			scan();
		//标识符 关键字
		if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch == '_') {
			string name = "";
			do {
				name.push_back(ch);//记录字符
				scan();
			} while (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch == '_' || ch >= '0' && ch <= '9');
			//匹配结束
			Tag tag = keywords.getTag(name);
			if (tag == ID)//正常的标志符
				t = new Id(name);
			else//关键字
				t = new Token(tag);
		}
		//数字
		else if (ch >= '0' && ch <= '9') {
			int val = 0;
			if (ch != '0') {//10进制
				do {
					val = val * 10 + ch - '0';
					scan();
				} while (ch >= '0' && ch <= '9');
			}
			else {
				scan();
				if (ch == 'x') {//16进制
					scan();
					if (ch >= '0' && ch <= '9' || ch >= 'A' && ch <= 'F' || ch >= 'a' && ch <= 'f') {
						do {
							val = val * 16 + ch;
							if (ch >= '0' && ch <= '9')val -= '0';
							else if (ch >= 'A' && ch <= 'F')val += 10 - 'A';
							else if (ch >= 'a' && ch <= 'f')val += 10 - 'a';
							scan();
						} while (ch >= '0' && ch <= '9' || ch >= 'A' && ch <= 'F' || ch >= 'a' && ch <= 'f');
					}
					else {
						t = new Token(ERR);
					}
				}
				else if (ch == 'b') {//2进制
					scan();
					if (ch >= '0' && ch <= '1') {
						do {
							val = val * 2 + ch - '0';
							scan();
						} while (ch >= '0' && ch <= '1');
					}
					else {
						t = new Token(ERR);
					}
				}
				else if (ch >= '0' && ch <= '7') {//8进制
					do {
						val = val * 8 + ch - '0';
						scan();
					} while (ch >= '0' && ch <= '7');
				}
			}
			//最终数字
			if (!t)t = new Num(val);
		}
		//字符
		else if (ch == '\'') {
			char c;
			scan();
			if (ch == '\\') {//转义
				scan();
				if (ch == 'n')c = '\n';
				else if (ch == '\\')c = '\\';
				else if (ch == 't')c = '\t';
				else if (ch == '0')c = '\0';
				else if (ch == '\'')c = '\'';
				else if (ch == -1 || ch == '\n') {//文件结束 换行
					t = new Token(ERR);
				}
				else c = ch;//没有转义
			}
			else if (ch == '\n' || ch == -1) {//行 文件结束
				t = new Token(ERR);
			}
			else if (ch == '\'') {//没有数据
				t = new Token(ERR);
				scan();//读掉引号
			}
			else {
				switch (ch)//界符
				{
				case '#'://忽略行（忽略宏定义）
					while (ch != '\n' && ch != -1)
						scan();//行 文件不结束
					t = new Token(ERR);
					break;
				case '+':
					t = new Token(scan('+') ? INC : ADD); break;
				case '-':
					t = new Token(scan('-') ? DEC : SUB); break;
				case '*':
					t = new Token(MUL); scan(); break;
				case '/':
					scan();
					if (ch == '/') {//单行注释
						while (ch != '\n' && ch != -1)
							scan();//行 文件不结束
						t = new Token(ERR);
					}
					else if (ch == '*') {//多行注释,不允许嵌套注释
						while (!scan(-1)) {//一直扫描
							if (ch == '*') {
								if (scan('/'))break;
							}
						}
						if (ch == -1)//没正常结束注释

							t = new Token(ERR);
					}
					else
						t = new Token(DIV);
					break;
				case '%':
					t = new Token(MOD); scan(); break;
				case '>':
					t = new Token(scan('=') ? GE : GT); break;
				case '<':
					t = new Token(scan('=') ? LE : LT); break;
				case '=':
					t = new Token(scan('=') ? EQU : ASSIGN); break;
				case '&':
					t = new Token(scan('&') ? AND : LEA); break;
				case '|':
					t = new Token(scan('|') ? OR : ERR);
					if (t->tag == ERR)
						break;
				case '!':
					t = new Token(scan('=') ? NEQU : NOT); break;
				case ',':
					t = new Token(COMMA); scan(); break;
				case ':':
					t = new Token(COLON); scan(); break;
				case ';':
					t = new Token(SEMICON); scan(); break;
				case '(':
					t = new Token(LPAREN); scan(); break;
				case ')':
					t = new Token(RPAREN); scan(); break;
				case '[':
					t = new Token(LBRACK); scan(); break;
				case ']':
					t = new Token(RBRACK); scan(); break;
				case '{':
					t = new Token(LBRACE); scan(); break;
				case '}':
					t = new Token(RBRACE); scan(); break;
				case -1:scan(); break;
				default:
					t = new Token(ERR);//错误的词法记号
					scan();
				}
			}
			//词法记号内存管理
			if (token)delete token;
			token = t;//强制记录
			if (token && token->tag != ERR)//有效,直接返回
				return token;
			else
				continue;//否则一直扫描直到结束
		}
		//文件结束
		if (token)delete token;
		return token = new Token(END);
	}
}