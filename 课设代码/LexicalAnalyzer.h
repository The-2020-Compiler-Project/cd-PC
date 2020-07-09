#pragma once
#include "common.h"
#include <unordered_map>

class Scanner
{
	char* fileName;
	FILE* file;

	static const int BUFLEN = 80;
	char line[BUFLEN];
	int lineLen;
	int readPos;
	char lastch;	

	
	int lineNum;
	int colNum;

	
	void showChar(char ch);

public:

	Scanner(char* name);
	~Scanner();

	
	int scan();

	
	char* getFile();
	int getLine();
	int getCol();

};

class Keywords
{
	unordered_map<string, Tag> keywords;
public:
	Keywords();					
	Tag getTag(string name);	
};

class Lexer
{
	static Keywords keywords;

	Scanner& scanner;
	char ch;
	bool scan(char need = 0);

	Token* token;

public:
	Lexer(Scanner& sc);
	~Lexer();
	Token* tokenize();
};