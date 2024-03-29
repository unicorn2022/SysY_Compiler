#include "front_main.hpp"

std::string AddLibraryFunction() {
    std::string decls;

    bool *isFParamArray;

    globalSymbolTable.AddFuncSymbol("getint", 1, 0, NULL); // decl @getint(): i32
    decls += "decl @getint(): i32\n";
    
    globalSymbolTable.AddFuncSymbol("getch", 1, 0, NULL); // decl @getch(): i32
    decls += "decl @getch(): i32\n";
    
    isFParamArray = new bool[1];
    isFParamArray[0] = true;
    globalSymbolTable.AddFuncSymbol("getarray", 1, 1, isFParamArray); // decl @getarray(*i32): i32
    decls += "decl @getarray(*i32): i32\n";
    
    isFParamArray = new bool[1];
    isFParamArray[0] = false;
    globalSymbolTable.AddFuncSymbol("putint", 0, 1, isFParamArray); // decl @putint(i32)
    decls += "decl @putint(i32)\n";
    
    isFParamArray = new bool[1];
    isFParamArray[0] = false;
    globalSymbolTable.AddFuncSymbol("putch", 0, 1, isFParamArray); // decl @putch(i32)
    decls += "decl @putch(i32)\n";
    
    isFParamArray = new bool[2];
    isFParamArray[0] = false;
    isFParamArray[1] = true;
    globalSymbolTable.AddFuncSymbol("putarray", 0, 1, isFParamArray); // decl @putarray(i32, *i32)
    decls += "decl @putarray(i32, *i32)\n";
    
    globalSymbolTable.AddFuncSymbol("starttime", 0, 0, NULL); // decl @starttime()
    decls += "decl @starttime()\n";
    
    globalSymbolTable.AddFuncSymbol("stoptime", 0, 0, NULL); // decl @stoptime()
    decls += "decl @stoptime()\n";

    return decls;
}

void front_main(const char input[], const char output[]){
    // freopen(output, "w", stdout);

    // 打开输入文件, 并且指定 lexer 在解析的时候读取这个文件
    yyin = fopen(input, "r");
    assert(yyin);

    // 在 parse 之前, 添加库函数
    std::string libraryFunctionDecls = AddLibraryFunction();

    // 调用 parser 函数, parser 函数会进一步调用 lexer 解析输入文件的
    unique_ptr<BaseAST> ast;
    auto ret = yyparse(ast);
    assert(!ret);

    // AST树
    // cout << "front:\n" << ast->PrintAST("");

    // IR树
    std::string IRTree;
    // cout << "front:\n";
    ast->PrintIR("", IRTree);

    ofstream fout(output);
    fout << libraryFunctionDecls;
    fout << IRTree;
}
