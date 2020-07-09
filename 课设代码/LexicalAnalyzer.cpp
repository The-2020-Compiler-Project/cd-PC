#include "LexicalAnalyzer.h"
#include "Token.h"

Scanner::Scanner(char* name)
{
	file = fopen(name, "r");
	if (!file) {
		printf("文件未完全扫描");
		fclose(file);
	}
	fileName = name;
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
		printf("文件未完全扫描");
		fclose(file);
	}
}

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
	if (!file)
		return -1;
	if (readPos == lineLen - 1)
	{
		lineLen = fread(line, 1, BUFLEN, file);
		if (lineLen == 0)
		{
			lineLen = 1;
			line[0] = -1;
		}
		readPos = -1;
	}
	readPos++;
	char ch = line[readPos];
	if (lastch == '\n')
	{
		lineNum++;
		colNum = 0;
	}
	if (ch == -1)
	{
		fclose(file);
		file = NULL;
	}
	else if (ch != '\n')
		colNum++;
	lastch = ch;
	return ch;
}

char* Scanner::getFile()
{
	return fileName;
}

int Scanner::getLine()
{
	return lineNum;
}

int Scanner::getCol()
{
	return colNum;
}

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

Tag Keywords::getTag(string name)
{
	return keywords.find(name) != keywords.end() ? keywords[name] : ID;
}

Keywords Lexer::keywords;

Lexer::Lexer(Scanner& sc) :scanner(sc)
{
	token = NULL;
	ch = ' ';
}

Lexer::~Lexer()
{
	if (!token)
	{
		delete token;
	}
}

bool Lexer::scan(char need)
{
	ch = scanner.scan();
	if (need) {
		if (ch != need)
			return false;
		ch = scanner.scan();
		return true;
	}
	return true;
}

Token* Lexer::tokenize()
{
	for (; ch != -1;) {
		Token* t = NULL;
		while (ch == ' ' || ch == '\n' || ch == '\t')
			scan();
		if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch == '_') {
			string name = "";
			do {
				name.push_back(ch);//��¼�ַ�
				scan();
			} while (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch == '_' || ch >= '0' && ch <= '9');
			Tag tag = keywords.getTag(name);
			if (tag == ID)
				t = new Id(name);
			else
				t = new Token(tag);
		}
		else if (ch >= '0' && ch <= '9') {
			int val = 0;
			if (ch != '0') {
				do {
					val = val * 10 + ch - '0';
					scan();
				} while (ch >= '0' && ch <= '9');
			}
			else {
				scan();
				if (ch == 'x') {
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
				else if (ch == 'b') {
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
				else if (ch >= '0' && ch <= '7') {
					do {
						val = val * 8 + ch - '0';
						scan();
					} while (ch >= '0' && ch <= '7');
				}
			}
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
				else if (ch == -1 || ch == '\n') {
					t = new Token(ERR);
				}
				else c = ch;//û��ת��
			}
			else if (ch == '\n' || ch == -1) {
				t = new Token(ERR);
			}
			else if (ch == '\'') {
				t = new Token(ERR);
				scan();
			}
			else c = ch;//�����ַ�
			if (!t) {
				if (scan('\'')) {
					//t = new Char(c);
				}
				else {
					t = new Token(ERR);
				}
			}
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