#pragma once
#include <cassert>
#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include "koopa.h"

using namespace std;

void back_main(const char input[], const char output[]);

// 从文本IR中解析KoopaIR
void GetKoopaIR(const char str[]);


// 访问 raw program
void Visit(const koopa_raw_program_t &program);
// 访问 raw slice
void Visit(const koopa_raw_slice_t &slice);
// 访问函数
void Visit(const koopa_raw_function_t &func);
// 访问基本块
void Visit(const koopa_raw_basic_block_t &bb);
// 访问指令
void Visit(const koopa_raw_value_t &value);
// 访问 return 指令
void Visit(const koopa_raw_return_t &ret);
// 访问 integer 指令
void Visit(const koopa_raw_integer_t &integer);