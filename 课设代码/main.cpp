#include<iostream>
using namespace std;
#include"Compiler.h"

int main(int argc,char *argv[])
{
	Compiler Compiler;
	char file[] = "test.txt";
	Compiler.compile(file);
}
