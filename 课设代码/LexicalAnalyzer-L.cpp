#pragma once
#include"lexicalanalyzer.h"
#include <sstream>
using namespace std;
//�ʷ��Ǻű�ǩ
/*
	���������ʼ��
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
	"error",//�����쳣��������ǵ�
	"�ļ���β",//�ļ�����
	"��ʶ��",//��ʶ��
	"int","void",//��������
	"����",//������
	"!","&",//��Ŀ���� ! - & *
	"+","-","*","/","%",//���������
	"++","--",
	">",">=","<","<=","==","!=",//�Ƚ������
	"&&","||",//�߼�����
	"(",")",//()
	"[","]",//[]
	"{","}",//{}
	",",":",";",//����,ð��,�ֺ�
	"=",//��ֵ
	"if","else",//if-else
	"while",//ѭ��
	"break","continue","return"//break,continue,return
};

//ɨ�����ຯ���Ķ���
Scanner::Scanner(char* name)
{
	file = fopen(name, "r");//��ָ���Ĵ�ɨ���ļ�
	fileName = name;
	//��ʼ��ɨ��״̬
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
   ��ʾ�ַ�
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
	if (!file)//û���ļ�
		return -1;
	if (readPos == lineLen - 1)//��������ȡ���
	{
		lineLen = fread(line, 1, BUFLEN, file);//���¼��ػ���������
		if (lineLen == 0)//û��������
		{
			//����ļ�����,�����ļ��������-1
			lineLen = 1;
			line[0] = -1;
		}
		readPos = -1;//�ָ���ȡλ��
	}
	readPos++;//�ƶ���ȡ��
	char ch = line[readPos];//��ȡ�µ��ַ�
	if (lastch == '\n')//����
	{
		lineNum++;//�к��ۼ�
		colNum = 0;//�к����
	}
	if (ch == -1)//�ļ��������Զ��ر�
	{
		fclose(file);
		file = NULL;
	}
	else if (ch != '\n')//���ǻ���
		colNum++;//�кŵ���
	lastch = ch;//��¼�ϸ��ַ�
	if (Args::showChar)showChar(ch);
	return ch;
}

/*
   ��ȡ�ļ���
*/
char* Scanner::getFile()
{
	return fileName;
}

/*
   ��ȡ�к�
*/
int Scanner::getLine()
{
	return lineNum;
}

/*
   ��ȡ�к�
*/
int Scanner::getCol()
{
	return colNum;
}


//Token��ĺ�������

Token::Token(Tag t) :tag(t)			//���캯��
{}

string Token::toString()
{
	return tokenName[tag];
}

Token::~Token()
{}

//��ʶ����ĺ�������

Id::Id(string n) :Token(ID), name(n)
{}

string Id::toString()
{
	return Token::toString() + name;
}

//������ĺ�������

Num::Num(int v) :Token(COSNT), val(v)
{}

string Num::toString()
{
	stringstream ss;
	ss << val;
	return string("[") + Token::toString() + "]:" + ss.str();
}



//�ؼ����б��ʼ��
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

//�����Ƿ��ǹؼ���

Tag Keywords::getTag(string name)
{
	return keywords.find(name) != keywords.end() ? keywords[name] : ID;
}


/*
�ʷ�������
*/

Keywords Lexer::keywords;
//�Զ���ʵ�ֽ���
Lexer::Lexer(Scanner& sc) :scanner(sc)
{
	token = NULL;//��ʼ���ʷ��Ǻż�¼���ñ���������
	ch = ' ';//��ʼ��Ϊ�ո�
}

Lexer::~Lexer()
{
	if (!token)//ɾ���Ѿ���¼�Ĵʷ��Ǻű������ڴ棬��ֹ�ڴ����
	{
		delete token;
	}
}

/*
   ��װ��ɨ�跽��
*/
bool Lexer::scan(char need)
{
	ch = scanner.scan();//ɨ����ַ�
	if (need) {
		if (ch != need)//��Ԥ�ڲ��Ǻ�
			return false;
		ch = scanner.scan();//��Ԥ���Ǻϣ�ɨ����һ��
		return true;
	}
	return true;
}

Token* Lexer::tokenize()
{
	for (; ch != -1;) {//���˵���Ч�Ĵʷ��Ǻţ�ֻ���������ʷ��ǺŻ���NULL
		Token* t = NULL;//��ʼ��һ���ʷ��Ǻ�ָ��
		while (ch == ' ' || ch == '\n' || ch == '\t')//���Կհ׷�
			scan();
		//��ʶ�� �ؼ���
		if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch == '_') {
			string name = "";
			do {
				name.push_back(ch);//��¼�ַ�
				scan();
			} while (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch == '_' || ch >= '0' && ch <= '9');
			//ƥ�����
			Tag tag = keywords.getTag(name);
			if (tag == ID)//�����ı�־��
				t = new Id(name);
			else//�ؼ���
				t = new Token(tag);
		}
		//����
		else if (ch >= '0' && ch <= '9') {
			int val = 0;
			if (ch != '0') {//10����
				do {
					val = val * 10 + ch - '0';
					scan();
				} while (ch >= '0' && ch <= '9');
			}
			else {
				scan();
				if (ch == 'x') {//16����
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
				else if (ch == 'b') {//2����
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
				else if (ch >= '0' && ch <= '7') {//8����
					do {
						val = val * 8 + ch - '0';
						scan();
					} while (ch >= '0' && ch <= '7');
				}
			}
			//��������
			if (!t)t = new Num(val);
		}
		//�ַ�
		else if (ch == '\'') {
			char c;
			scan();
			if (ch == '\\') {//ת��
				scan();
				if (ch == 'n')c = '\n';
				else if (ch == '\\')c = '\\';
				else if (ch == 't')c = '\t';
				else if (ch == '0')c = '\0';
				else if (ch == '\'')c = '\'';
				else if (ch == -1 || ch == '\n') {//�ļ����� ����
					t = new Token(ERR);
				}
				else c = ch;//û��ת��
			}
			else if (ch == '\n' || ch == -1) {//�� �ļ�����
				t = new Token(ERR);
			}
			else if (ch == '\'') {//û������
				t = new Token(ERR);
				scan();//��������
			}
			else {
				switch (ch)//���
				{
				case '#'://�����У����Ժ궨�壩
					while (ch != '\n' && ch != -1)
						scan();//�� �ļ�������
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
					if (ch == '/') {//����ע��
						while (ch != '\n' && ch != -1)
							scan();//�� �ļ�������
						t = new Token(ERR);
					}
					else if (ch == '*') {//����ע��,������Ƕ��ע��
						while (!scan(-1)) {//һֱɨ��
							if (ch == '*') {
								if (scan('/'))break;
							}
						}
						if (ch == -1)//û��������ע��

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
					t = new Token(ERR);//����Ĵʷ��Ǻ�
					scan();
				}
			}
			//�ʷ��Ǻ��ڴ����
			if (token)delete token;
			token = t;//ǿ�Ƽ�¼
			if (token && token->tag != ERR)//��Ч,ֱ�ӷ���
				return token;
			else
				continue;//����һֱɨ��ֱ������
		}
		//�ļ�����
		if (token)delete token;
		return token = new Token(END);
	}
}