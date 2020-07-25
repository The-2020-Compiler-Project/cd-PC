#include<iostream>
using namespace std;
#include"Compiler.h"

int main(int argc,char *argv[])
{
    if (argc == 1) exit(-1);
	Compiler Compiler;
	Compiler.compile(argv[1]);
	return 0;
}
