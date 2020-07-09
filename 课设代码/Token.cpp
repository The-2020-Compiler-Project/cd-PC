#include "Token.h"
#include <sstream>

const char* tokenName[] =
{
	"error",
	"文件结尾",
	"标识符",
	"int","void",
	"const",
	"数字",
	"!","&",
	"+","-","*","/","%",
	"++","--",
	">",">=","<","<=","==","!=",
	"&&","||",
	"(",")",//()
	"[","]",//[]
	"{","}",//{}
	",",":",";",
	"=",//��ֵ
	"if","else",//if-else
	"while",//
	"break","continue","return"//break,continue,return
};

Token::Token(Tag t) :tag(t)
{}

string Token::toString()
{
	return tokenName[tag];
}

Token::~Token()
{}

Id::Id(string n) :Token(ID), name(n)
{}

string Id::toString()
{
	return Token::toString() + name;
}


Num::Num(int v) :Token(INTCONST), val(v)
{}

string Num::toString()
{
	stringstream ss;
	ss << val;
	return string("[") + Token::toString() + "]:" + ss.str();
}