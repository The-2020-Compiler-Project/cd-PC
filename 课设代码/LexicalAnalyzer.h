#pragma once
#include "common.h"
#include <ext/hash_map>
using namespace __gnu_cxx;

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
	struct string_hash {
		size_t operator()(const string& str) const {
			return __stl_hash_string(str.c_str());
		}
	};
	hash_map<string, Tag, string_hash> keywords;
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