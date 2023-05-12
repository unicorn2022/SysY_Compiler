#pragma once
#include "koopa.h"

void back_main(const char input[], const char output[]);

// 从文本IR中解析KoopaIR
void GetKoopaIR(const char str[]);


// 访问 raw program
void Visit_Program(const koopa_raw_program_t &program);


// 访问 raw slice
void Visit_Slice(const koopa_raw_slice_t &slice);

/*====================  指令部分 =======================*/ 
// 访问函数
void Visit_Function(const koopa_raw_function_t &func);
// 遍历当前基本块的所有指令, 计算当前基本块可能用到的栈空间大小
int32_t Get_Basic_Block_Need_Stack(const koopa_raw_basic_block_t &bbs);
// 访问基本块
void Visit_Basic_Block(const koopa_raw_basic_block_t &bb);


/*====================  指令部分 =======================*/ 
// 访问指令
int32_t Visit_Inst(const koopa_raw_value_t &value);
// 访问 integer 指令, 返回整数值 (tag = 0)
int32_t Visit_Inst_Integer(const koopa_raw_integer_t &integer);
// 访问 alloc 指令, 返回结果所在的sp+x (tag = 6)
int32_t Visit_Inst_Alloc(const koopa_raw_type_t &alloc_type);
// 访问 load 指令, 返回结果所在的sp+x (tag = 8)
int32_t Visit_Inst_Load(const koopa_raw_load_t &load);
// 访问 store 指令 (tag = 9)
int32_t Visit_Inst_Store(const koopa_raw_store_t &store);
// 访问 binary 指令, 返回结果所在的sp+x (tag = 12)
int32_t Visit_Inst_Binary(const koopa_raw_binary_t &binary);
// 访问 branch 指令 (tag = 13)
int32_t Visit_Inst_Branch(const koopa_raw_branch_t &branch);
// 访问 jump 指令 (tag = 14)
int32_t Visit_Inst_Jump(const koopa_raw_jump_t &jump);
// 访问 return 指令 (tag = 16)
int32_t Visit_Inst_Return(const koopa_raw_return_t &ret);
