#ifndenf _LEXICALANALYZER_H
#define _LEXICALANALYZER_H
#include<iostream>
#include<hash_map>
#include<string>
#define BUFLEN 80;//��������С
int lineLen = 0;//�����������ݳ���
int readPos = -1;//��ȡλ��
char line[BUFLEN];//������
int lineNum = 1;//�к�
int colNum = 0;//�к�
char lastChar = ch;//��һ���ַ�
char scan(FILE* file);//���ڶ��ļ�

enum Tag {
	ERR,					//�����쳣
	END,					//�ļ��������
	ID,						//��ʶ��
	KW_INT, KW_VOID,			//��������
	NUM,					//����
	NOT, LEA,				//��Ŀ�����
	ADD, SUB, MUL, DIV, MOD,	//���������
	INC, DEC,				//�Լ��Լ�
	GT, GE, LT, LE, EQU, NEQU,	//�Ƚ������
	AND, OR,					//�߼������
	LPAREN, RPAREN,			//����
	LBRACK, RBRACK,			//[]
	LBRACE, RBRACE,			//{}
	COMMA, COLON, SEMICON,	//���ţ�ð�ţ��ֺ�
	ASSIGN,					//��ֵ
	KW_IF, KW_ELSE,			//if-else
	KW_WHILE,				//whileѭ��
	KW_BREAK, KW_CONTINUE, KW_RETURN //break,continue,return
};
//�ʷ��Ǻ���
//��Ϊ����
class Token
{
public:
	Tag tag;	//�ڲ���ǩ
	Token(Tag t);	//���캯��
	virtual string toString();
	virtual ~Token();
};

//�������ΪToken��������
//��ʶ���Ǻ���
class Id :public Token
{
public:
	string name;
	Id(string n);
	virtual string toString();
};
//���ּǺ���
class Num :public Token
{
public:
	int val;
	Num(int v);
	virtual string toString();

};
//�����ൽ��Ϊֹ

//�ؼ��ֱ���
class Keywords
{
	struct string_hash {
		size_t operator()(const string& str) const {
			return __stl_hash_string(str.c_str());
		}
	};
	hash_map<string, tag, string_hash> keywords;
public:
	Keywords();					//�ؼ��ֱ��ʼ��
	Tag getTag(string name);	//�����Ƿ�ؼ���
}
};
#endif