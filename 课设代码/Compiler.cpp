#include "Compiler.h"
#include "LexicalAnalyzer.h"
#include "Symtab.h"
#include "GenInter.h"
#include "GrammarAnalyzer.h"


/*
	编译一个文件
*/
void Compiler::compile(char*file)
{
	//准备
	Scanner scanner(file);//扫描器
	Lexer lexer(scanner);//词法分析器
	SymTab symtab;//符号表
	GenIR ir(symtab);//中间代码生成器
	Parser parser(lexer,symtab,ir);//语法分析器
	//分析
	parser.analyse();//分析
	//中间结果
	symtab.toString();//输出符号表
	symtab.printInterCode();//输出中间代码
}
