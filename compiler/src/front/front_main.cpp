#include "front_main.hpp"

void front_main(const char input[], const char output[]){
    // freopen(output, "w", stdout);

    // 打开输入文件, 并且指定 lexer 在解析的时候读取这个文件
    yyin = fopen(input, "r");
    assert(yyin);

    // 调用 parser 函数, parser 函数会进一步调用 lexer 解析输入文件的
    unique_ptr<BaseAST> ast;
    auto ret = yyparse(ast);
    assert(!ret);

    // AST树
    cout << "front:\n" << ast->PrintAST("");

    // IR树
    std::string IRTree;
    // cout << "front:\n";
    ast->PrintIR("", IRTree);

    ofstream fout(output);
    fout << IRTree;
}