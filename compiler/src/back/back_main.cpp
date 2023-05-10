#include "back_main.hpp"
std::map<koopa_raw_value_t, int32_t> hasDone;

void back_main(const char input[], const char output[]){
    // 从input中读取IR树
    ifstream fin(input);
    std::istreambuf_iterator<char> beg(fin), end;
    std::string IRTree(beg, end);
    
    // 解析KoopaIR
    GetKoopaIR(IRTree.c_str());
}

// 从文本IR中解析KoopaIR, 生成raw program
// 通过Visit(raw program), 得到最后的RISCV指令
void GetKoopaIR(const char str[]){
    // 解析字符串 str, 得到 Koopa IR 程序
    koopa_program_t program;
    koopa_error_code_t ret = koopa_parse_from_string(str, &program);
    assert(ret == KOOPA_EC_SUCCESS);  // 确保解析时没有出错
    
    // 创建一个 raw program builder, 用来构建 raw program
    koopa_raw_program_builder_t builder = koopa_new_raw_program_builder();
    
    // 将 Koopa IR 程序转换为 raw program
    koopa_raw_program_t raw = koopa_build_raw_program(builder, program);
    
    // 释放 Koopa IR 程序占用的内存
    koopa_delete_program(program);

    // 处理 raw program
    Visit_Program(raw);
    

    // 处理完成, 释放 raw program builder 占用的内存
    // 注意, raw program 中所有的指针指向的内存均为 raw program builder 的内存
    // 所以不要在 raw program 处理完毕之前释放 builder
    koopa_delete_raw_program_builder(builder);
}


// 访问 raw program
void Visit_Program(const koopa_raw_program_t &program) {
    // printf("-----------Visit_Program---------------\n");

    // 访问所有全局变量
    Visit_Slice(program.values);

    // 执行一些其他的必要操作
    cout << "\t.text\n";        // 声明之后的数据需要被放入代码段中
    cout << "\t.globl main\n";  // 声明全局符号 main, 以便链接器处理
    
    // 访问所有函数
    Visit_Slice(program.funcs);
}

// 访问 raw slice
void Visit_Slice(const koopa_raw_slice_t &slice) {
    // printf("-----------Visit_Slice---------------\n");

    for (size_t i = 0; i < slice.len; ++i) {
        // 当前slice的内容
        auto ptr = slice.buffer[i];
        
        // 当前slice的类型
        switch (slice.kind) {
            // 当前slice的类型为function
            case KOOPA_RSIK_FUNCTION:{
                Visit_Function(reinterpret_cast<koopa_raw_function_t>(ptr));
                break;
            }
            
            // 当前slice的类型为basic_block
            case KOOPA_RSIK_BASIC_BLOCK:{
                Visit_Basic_Block(reinterpret_cast<koopa_raw_basic_block_t>(ptr));
                break;
            }
            
            // 当前slice的类型为value(即指令)
            case KOOPA_RSIK_VALUE:{
                Visit_Inst(reinterpret_cast<koopa_raw_value_t>(ptr));
                break;
            }
            
            // 其他情况
            default:{
                printf("[Visit_Slice]: slice.kind = %d\n", slice.kind);
                assert(false);
            }
        }
    }
}

// 访问函数
void Visit_Function(const koopa_raw_function_t &func) {
    // printf("-----------Visit_Function---------------\n");

    // 输出当前函数的函数名, 标记当前函数的入口
    // 由于KoopaIR中函数名均为@name, 因此只需要输出name+1即可
    cout << func->name+1 << ":\n";

    // 访问当前函数的所有参数
    // koopa_raw_slice_t params, 需要通过Slice进行进一步划分
    // Visit_Slice(func->params)

    // 访问当前函数的所有基本块
    // koopa_raw_slice_t bbs, 需要通过Slice进行进一步划分
    Visit_Slice(func->bbs);

    // 判断当前函数的返回值
    // koopa_raw_type_t Return_Type = func->ty
}

// 访问基本块
void Visit_Basic_Block(const koopa_raw_basic_block_t &bb) {
    // printf("-----------Visit_Basic_Block---------------\n");
    
    // 输出当前基本块的名成, 标记当前基本块的入口
    // 由于KoopaIR中基本块均为@name, 因此只需要输出name+1即可
    cout << bb->name+1 << ":\n";  // 标记 基本块 的入口点
    
    // 访问当前基本块的所有参数
    // koopa_raw_slice_t params, 需要通过Slice进行进一步划分
    // Visit_Slice(bb->params)

    // 访问当前基本块的所有用到的value
    // koopa_raw_slice_t used_by, 需要通过Slice进行进一步划分
    // Visit_Slice(bb->used_by) 

    // 访问所有指令
    // koopa_raw_slice_t insts, 需要通过Slice进行进一步划分
    Visit_Slice(bb->insts);
}

// 访问指令
int32_t Visit_Inst(const koopa_raw_value_t &value) {
    if(hasDone.find(value) != hasDone.end()) return hasDone[value];
    // printf("-----------Visit_Inst, addr = %x---------------\n", value);


    // 根据指令类型判断后续需要如何访问
    const auto &kind = value->kind;
    
    switch (kind.tag) {
        // 访问 return 指令
        case KOOPA_RVT_RETURN:{
            return hasDone[value] = Visit_Inst_Return(kind.data.ret);
            break;
        }
        
        // 访问 integer 指令
        case KOOPA_RVT_INTEGER:{
            return hasDone[value] = Visit_Inst_Integer(kind.data.integer);
            break;
        }
        
        // 访问双目运算符
        case KOOPA_RVT_BINARY:{
            return hasDone[value] = Visit_Inst_Binary(kind.data.binary);
            break;
        }
        
        // 其他类型
        default:{
            printf("Visit_Inst kind.tag = %d\n", kind.tag);
            assert(false);
        }
    }
}

// 访问 return 指令
int32_t Visit_Inst_Return(const koopa_raw_return_t &ret){
    // printf("-----------Visit_Inst_Return---------------\n");

    // return 指令中, value 代表返回值
    koopa_raw_value_t ret_value = ret.value;
    
    // 根据ret_value的类型, 判断使用什么指令
    switch (ret_value->kind.tag) {
        // ret_value为integer
        case KOOPA_RVT_INTEGER:{
            // 取出整数的值, 并将其存放在a0中, 然后返回
            cout << "\tli a0, " << Visit_Inst_Integer(ret_value->kind.data.integer) << "\n";
            cout << "\tret\n";
            break;
        }

        // 其他情况下, 将寄存器中的值放到a0中, 然后返回
        default:{
            int reg = Visit_Inst(ret_value);
            cout << "\tmv a0, t" << reg << "\n";
            cout << "\tret\n";
        }
    }    
    return 0;
}

// 访问 integer 指令, 返回整数值
int32_t Visit_Inst_Integer(const koopa_raw_integer_t &integer){
    // printf("Visit_Inst_Integer: value = %d\n", integer.value);
    return integer.value;
}

// 访问 binary 指令, 返回寄存器编号
int32_t Visit_Inst_Binary(const koopa_raw_binary_t &binary){
    // printf("-----------Visit_Inst_Binary, op = %d---------------\n", binary.op);

    // 取出两个操作数
    koopa_raw_value_t lhs = binary.lhs;
    koopa_raw_value_t rhs = binary.rhs;
    bool lhs_is_integer = false, rhs_is_integer = false;
    int32_t lhs_value = 0, rhs_value = 0;

    // lhs是整数指令
    if(lhs->kind.tag == KOOPA_RVT_INTEGER){
        lhs_is_integer = true;
        lhs_value = Visit_Inst_Integer(lhs->kind.data.integer);
    }
    else{
        lhs_is_integer = false;
        lhs_value = Visit_Inst(lhs);
    }
    // rhs是整数指令
    if(rhs->kind.tag == KOOPA_RVT_INTEGER){
        rhs_is_integer = true;
        rhs_value = Visit_Inst_Integer(rhs->kind.data.integer);
    }else{
        rhs_is_integer = false;
        rhs_value = Visit_Inst(rhs);
    }

    // 找到结果所在的寄存器编号now
    int now = 0;
    if(!lhs_is_integer) now = std::max(now, lhs_value+1);
    if(!rhs_is_integer) now = std::max(now, rhs_value+1);

    // lhs是整数且不为0, 需要将其移动到寄存器中
    if(lhs_is_integer && lhs_value != 0) {
        cout << "\tli t" << now << ", " << lhs_value << "\n";
        lhs_is_integer = false;
        lhs_value = now;
    }
    // rhs是整数且不为0, 需要将其移动到寄存器中
    if(rhs_is_integer && rhs_value != 0) {
        cout << "\tli t" << now + 1 << ", " << rhs_value << "\n";
        rhs_is_integer = false;
        rhs_value = now + 1;
    }
    
    // printf("lhs.name = %s is_integer = %d value = %d\n", lhs->name, lhs_is_integer, lhs_value);
    // printf("rhs.name = %s is_integer = %d value = %d\n", rhs->name, rhs_is_integer, rhs_value);
    // printf("now = %d\n", now);

    // 根据op判断是哪一个操作
    switch (binary.op){
        // lhs != rhs
        case KOOPA_RBO_NOT_EQ:{
            // 通过sub后与0判相等, 模拟!=操作
            cout << "\tsub t" << now << ", ";
            if(lhs_is_integer) cout << "x0, ";
            else cout << "t" << lhs_value << ", ";
            if(rhs_is_integer) cout << "x0, ";
            else cout << "t" << rhs_value << "\n";
            
            cout << "\tsnez t" << now << ", t" << now << "\n";
            cout << "\tandi t" << now << ", t" << now << ", 0xff\n";
            break;
        }
        // lhs == rhs
        case KOOPA_RBO_EQ:{
            // 通过sub后与0判相等, 模拟==操作
            cout << "\tsub t" << now << ", ";
            if(lhs_is_integer) cout << "x0, ";
            else cout << "t" << lhs_value << ", ";
            if(rhs_is_integer) cout << "x0, ";
            else cout << "t" << rhs_value << "\n";
            
            cout << "\tseqz t" << now << ", t" << now << "\n";
            cout << "\tandi t" << now << ", t" << now << ", 0xff\n";
            break;
        }
        // lhs > rhs
        case KOOPA_RBO_GT:{
            cout << "\tsgt t" << now << ", ";
            if(lhs_is_integer) cout << "x0, ";
            else cout << "t" << lhs_value << ", ";
            if(rhs_is_integer) cout << "x0, ";
            else cout << "t" << rhs_value << "\n";
            
            cout << "\tandi t" << now << ", t" << now << ", 0xff\n";
            break;
        }
        // lhs < rhs
        case KOOPA_RBO_LT:{
            cout << "\tslt t" << now << ", ";
            if(lhs_is_integer) cout << "x0, ";
            else cout << "t" << lhs_value << ", ";
            if(rhs_is_integer) cout << "x0, ";
            else cout << "t" << rhs_value << "\n";
            
            cout << "\tandi t" << now << ", t" << now << ", 0xff\n";
            break;
        }
        // lhs >= rhs
        case KOOPA_RBO_GE:{
            cout << "\tslt t" << now << ", ";
            if(lhs_is_integer) cout << "x0, ";
            else cout << "t" << lhs_value << ", ";
            if(rhs_is_integer) cout << "x0, ";
            else cout << "t" << rhs_value << "\n";
            
            cout << "\txori t" << now << ", t" << now << ", 1\n";
            cout << "\tandi t" << now << ", t" << now << ", 0xff\n";
            break;
        }
        // lhs <= rhs
        case KOOPA_RBO_LE:{
            cout << "\tsgt t" << now << ", ";
            if(lhs_is_integer) cout << "x0, ";
            else cout << "t" << lhs_value << ", ";
            if(rhs_is_integer) cout << "x0, ";
            else cout << "t" << rhs_value << "\n";
            
            cout << "\txori t" << now << ", t" << now << ", 1\n";
            cout << "\tandi t" << now << ", t" << now << ", 0xff\n";
            break;
        }


        // lhs + rhs
        case KOOPA_RBO_ADD:{
            cout << "\tadd t" << now << ", ";
            if(lhs_is_integer) cout << "x0, ";
            else cout << "t" << lhs_value << ", ";
            if(rhs_is_integer) cout << "x0, ";
            else cout << "t" << rhs_value << "\n";

            break;
        }
        // lhs - rhs
        case KOOPA_RBO_SUB:{
            cout << "\tsub t" << now << ", ";
            if(lhs_is_integer) cout << "x0, ";
            else cout << "t" << lhs_value << ", ";
            if(rhs_is_integer) cout << "x0, ";
            else cout << "t" << rhs_value << "\n";

            break;
        }
        // lhs * rhs
        case KOOPA_RBO_MUL:{
            cout << "\tmul t" << now << ", ";
            if(lhs_is_integer) cout << "x0, ";
            else cout << "t" << lhs_value << ", ";
            if(rhs_is_integer) cout << "x0, ";
            else cout << "t" << rhs_value << "\n";

            break;
        }
        // lhs / rhs
        case KOOPA_RBO_DIV:{
            cout << "\tdiv t" << now << ", ";
            if(lhs_is_integer) cout << "x0, ";
            else cout << "t" << lhs_value << ", ";
            if(rhs_is_integer) cout << "x0, ";
            else cout << "t" << rhs_value << "\n";

            break;
        }
        // lhs % rhs
        case KOOPA_RBO_MOD:{
            cout << "\trem t" << now << ", ";
            if(lhs_is_integer) cout << "x0, ";
            else cout << "t" << lhs_value << ", ";
            if(rhs_is_integer) cout << "x0, ";
            else cout << "t" << rhs_value << "\n";

            break;
        }
        // lhs & rhs
        case KOOPA_RBO_AND:{
            cout << "\tand t" << now << ", ";
            if(lhs_is_integer) cout << "x0, ";
            else cout << "t" << lhs_value << ", ";
            if(rhs_is_integer) cout << "x0, ";
            else cout << "t" << rhs_value << "\n";

            break;
        }
        // lhs | rhs
        case KOOPA_RBO_OR:{
            cout << "\tor t" << now << ", ";
            if(lhs_is_integer) cout << "x0, ";
            else cout << "t" << lhs_value << ", ";
            if(rhs_is_integer) cout << "x0, ";
            else cout << "t" << rhs_value << "\n";

            break;
        }
        // lhs ^ rhs
        case KOOPA_RBO_XOR:{
            cout << "\txor t" << now << ", ";
            if(lhs_is_integer) cout << "x0, ";
            else cout << "t" << lhs_value << ", ";
            if(rhs_is_integer) cout << "x0, ";
            else cout << "t" << rhs_value << "\n";

            break;
        }
        // lhs << rhs
        case KOOPA_RBO_SHL:{
            cout << "\tsll t" << now << ", ";
            if(lhs_is_integer) cout << "x0, ";
            else cout << "t" << lhs_value << ", ";
            if(rhs_is_integer) cout << "x0, ";
            else cout << "t" << rhs_value << "\n";

            break;
        }
        // lhs >> rhs
        case KOOPA_RBO_SHR:{
            cout << "\tsra t" << now << ", ";
            if(lhs_is_integer) cout << "x0, ";
            else cout << "t" << lhs_value << ", ";
            if(rhs_is_integer) cout << "x0, ";
            else cout << "t" << rhs_value << "\n";

            break;
        }
        // 其他情况
        default:{
            printf("Visit_Inst_Binary binary.op = %d\n", binary.op);
            assert(false);
        }
    }

    return now;
}