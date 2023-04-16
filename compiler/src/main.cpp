#include "main.hpp"

int main(int argc, const char *argv[]) {
    // 解析命令行参数. 测试脚本/评测平台要求你的编译器能接收如下参数:
    // compiler 模式 输入文件 -o 输出文件
    assert(argc == 5);
    auto mode = argv[1];
    auto input = argv[2];
    auto output = argv[4];

    Work(mode, input, output);
    return 0;
}

void Work(const char mode[], const char input[], const char output[]){
    // freopen(output, "w", stdout);

    // 打开输入文件, 并且指定 lexer 在解析的时候读取这个文件
    yyin = fopen(input, "r");
    assert(yyin);

    // 调用 parser 函数, parser 函数会进一步调用 lexer 解析输入文件的
    unique_ptr<BaseAST> ast;
    auto ret = yyparse(ast);
    assert(!ret);

    // AST树
    cout << ast->PrintAST("");

    // IR树
    std::string IRTree;
    ast->PrintIR("", IRTree);
    cout << IRTree;
    //GetKoopaIR(IRTree.c_str());
}

// 从文本IR中解析KoopaIR
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
    Visit(raw);
    

    // 处理完成, 释放 raw program builder 占用的内存
    // 注意, raw program 中所有的指针指向的内存均为 raw program builder 的内存
    // 所以不要在 raw program 处理完毕之前释放 builder
    koopa_delete_raw_program_builder(builder);
}


// 访问 raw program
void Visit(const koopa_raw_program_t &program) {
    // printf("---------------program:\n");

    // 访问所有全局变量
    Visit(program.values);

    // 执行一些其他的必要操作
    cout << "\t.text\n";        // 声明之后的数据需要被放入代码段中
    cout << "\t.globl main\n";  // 声明全局符号 main, 以便链接器处理
    // 访问所有函数
    Visit(program.funcs);
}

// 访问 raw slice
void Visit(const koopa_raw_slice_t &slice) {
    // printf("---------------slice:\n");

    for (size_t i = 0; i < slice.len; ++i) {
        auto ptr = slice.buffer[i];
        // 根据 slice 的 kind 决定将 ptr 视作何种元素
        switch (slice.kind) {
        case KOOPA_RSIK_FUNCTION:
            // 访问函数
            Visit(reinterpret_cast<koopa_raw_function_t>(ptr));
            break;
        case KOOPA_RSIK_BASIC_BLOCK:
            // 访问基本块
            Visit(reinterpret_cast<koopa_raw_basic_block_t>(ptr));
            break;
        case KOOPA_RSIK_VALUE:
            // 访问指令
            Visit(reinterpret_cast<koopa_raw_value_t>(ptr));
            break;
        default:
            // 我们暂时不会遇到其他内容, 于是不对其做任何处理
            assert(false);
        }
    }
}

// 访问函数
void Visit(const koopa_raw_function_t &func) {
    // printf("---------------func: name = %s\n", func->name);

    // 执行一些其他的必要操作
    cout << func->name+1 << ":\n";  // 标记 main 的入口点
    // 访问所有基本块
    Visit(func->bbs);
}

// 访问基本块
void Visit(const koopa_raw_basic_block_t &bb) {
    // printf("---------------bb: name = %s\n", bb->name);

    // 执行一些其他的必要操作
    // cout << bb->name+1 << ":\n";  // 标记 基本块 的入口点
    // 访问所有指令
    Visit(bb->insts);
}

// 访问指令
void Visit(const koopa_raw_value_t &value) {
    // printf("---------------bb: name = %s\n", value->name);

    // 根据指令类型判断后续需要如何访问
    const auto &kind = value->kind;
    switch (kind.tag) {
        case KOOPA_RVT_RETURN:
        // 访问 return 指令
        Visit(kind.data.ret);
        break;
        case KOOPA_RVT_INTEGER:
        // 访问 integer 指令
        Visit(kind.data.integer);
        break;
        default:
        // 其他类型暂时遇不到
        assert(false);
    }
}

// 访问 return 指令
void Visit(const koopa_raw_return_t &ret){
    // printf("---------------return: name=%s\n", ret.value->name);
    
    // return 指令中, value 代表返回值
    koopa_raw_value_t ret_value = ret.value;

    // 示例程序中, ret_value 一定是一个 integer
    assert(ret_value->kind.tag == KOOPA_RVT_INTEGER);

    // 于是我们可以按照处理 integer 的方式处理 ret_value
    // integer 中, value 代表整数的数值
    // 示例程序中, 这个数值一定是 0
    int32_t int_val = ret_value->kind.data.integer.value;

    cout << "\tli a0, " << int_val << "\n";
    cout << "\tret\n";
}

// 访问 integer 指令
void Visit(const koopa_raw_integer_t &integer){
    // printf("---------------Integer\n");
}