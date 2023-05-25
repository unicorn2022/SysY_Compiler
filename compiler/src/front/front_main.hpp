#pragma once
#include <memory>
#include <cassert>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include "AST.hpp"

using namespace std;

// 声明 lexer 的输入, 以及 parser 函数
extern FILE *yyin;
extern int yyparse(unique_ptr<BaseAST> &ast);

// 全局符号表，用于在 parse 前添加库函数
extern SymbolTable globalSymbolTable;

void front_main(const char input[], const char output[]);

