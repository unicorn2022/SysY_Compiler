#include <cstring>

#include "front/front_main.hpp"
#include "back/back_main.hpp"

void CopyFile(const char input[], const char output[]){
    // 从input中读取文件内容
    ifstream fin(input);
    std::istreambuf_iterator<char> beg(fin), end;
    std::string input_data(beg, end);
    fin.close();

    // 输出到 output 中
    ofstream fout(output);
    fout << input_data;
    fout.close();
}

int debug_cnt;

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
        // for(int i = 0; i <= 1000000000; i++)
        //     debug_cnt++;
        const char CFile[] = "./test/task.c";
        const char IRFile[] = "./test/task.koopa";

        CopyFile(input, CFile);

        // 前端读入input文件，生成IR树，放到IRFile文件中
        front_main(input, IRFile);
        // 后端读入IRFile文件，解析IR树，生成RISCV，放到output文件中
        back_main(IRFile, output);
    }
    
    return 0;
}