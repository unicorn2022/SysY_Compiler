#include <cstring>

#include "front/front_main.hpp"
#include "back/back_main.hpp"

int main(int argc, const char *argv[]) {
    // 解析命令行参数. 测试脚本/评测平台要求你的编译器能接收如下参数:
    // compiler 模式 输入文件 -o 输出文件
    assert(argc == 5);
    auto mode = argv[1];
    auto input = argv[2];
    auto output = argv[4];
    
    if (strcmp(mode, "-koopa") == 0) {
        front_main(input, output);
    } 
    else if (strcmp(mode, "-riscv") == 0) {
        const char IRFile[] = "task.koopa";

        // 前端读入input文件，生成IR树，放到IRFile文件中
        front_main(input, IRFile);
        // 后端读入IRFile文件，解析IR树，生成RISCV，放到output文件中
        back_main(IRFile, output);
    }
    
    return 0;
}