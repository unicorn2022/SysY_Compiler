%code requires {
    #include <memory>
    #include <string>
    #include "AST.hpp"
}

%{

#include <iostream>
#include <memory>
#include <string>
#include "AST.hpp"

// 声明 lexer 函数和错误处理函数
int yylex();
void yyerror(std::unique_ptr<BaseAST> &ast, const char *s);

using namespace std;

%}

// 定义 parser 函数和错误处理函数的附加参数, 类型为unique_ptr<BaseAST>
%parse-param { std::unique_ptr<BaseAST> &ast }

// yylval 的定义, 我们把它定义成了一个联合体 (union)
// 因为 token 的值有的是 string*, 有的是 int, 有的是 BastAST*
// lexer 中用到的 str_val 和 int_val 就是在这里被定义的
%union {
    std::string *str_val;
    int int_val;
    BaseAST *ast_val;
}

// lexer 返回的所有 token 种类的声明
// 注意 IDENT 和 INT_CONST 会返回 token 的值, 分别对应 str_val 和 int_val
%token INT RETURN
%token <str_val> IDENT
%token <int_val> INT_CONST

// 非终结符的类型定义, 分别对应 ast_val 和 int_val
%type <ast_val> FuncDef FuncType Block Stmt Exp PrimaryExp UnaryExp
%type <int_val> Number  

%%

// CompUnit ::= FuncDef
// 之前我们定义了 FuncDef 会返回一个 ast_val
// 而 parser 一旦解析完 CompUnit, 就说明所有的 token 都被解析了, 即解析结束了
// 此时我们应该把 FuncDef 返回的结果收集起来, 作为 AST 传给调用 parser 的函数
// $1 指代规则里第一个符号的返回值, 也就是 FuncDef 的返回值
CompUnit
    : FuncDef {
        auto comp_unit = make_unique<CompUnitAST>();
        comp_unit->func_def = unique_ptr<BaseAST>($1);
        ast = move(comp_unit);
    }
    ;

// FuncDef ::= FuncType IDENT '(' ')' Block;
// 我们这里可以直接写 '(' 和 ')', 因为之前在 lexer 里已经处理了单个字符的情况
// 解析完成后, 把这些符号的结果收集起来, 然后拼成一个 FuncDefAST, 作为结果返回
// $$ 表示非终结符的返回值, 我们可以通过给这个符号赋值的方法来返回结果
FuncDef
    : FuncType IDENT '(' ')' Block {
        auto ast = new FuncDefAST();
        ast->func_type = unique_ptr<BaseAST>($1);
        ast->ident = *unique_ptr<string>($2);
        ast->block = unique_ptr<BaseAST>($5);
        $$ = ast;
    }
    ;

// FuncType ::= INT
FuncType
    : INT {
        auto ast = new FuncTypeAST();
        ast->type = "int";
        $$ = ast;
    }
    ;

// Block :== '{' Stmt '}'
Block
    : '{' Stmt '}' {
        auto ast = new BlockAST();
        ast->stmt = unique_ptr<BaseAST>($2);
        $$ = ast;
    }
    ;

// Stmt ::= 'return' Exp ';';
Stmt
    : RETURN Exp ';' {
        auto ast = new StmtAST();
        ast->exp = unique_ptr<BaseAST>($2);
        $$ = ast;
    }
    ;

// Exp ::= UnaryExp;
Exp
    : UnaryExp {
        auto ast = new ExpAST();
        ast->unaryExp = unique_ptr<BaseAST>($1);
        $$ = ast;
    }
    ;

// PrimaryExp ::= "(" Exp ")" | Number;
PrimaryExp
    : '(' Exp ')' {
        auto ast = new PrimaryExpAST();
        ast->kind = 1;
        ast->exp = unique_ptr<BaseAST>($2);
        $$ = ast;
    }
    | Number {
        auto ast = new PrimaryExpAST();
        ast->kind = 2;
        ast->number = $1;
        $$ = ast;
    }
    ;

// Number ::= INT_CONST;
Number
    : INT_CONST{
        $$ = $1;
    }

// UnaryExp ::= PrimaryExp | UnaryOp UnaryExp;
UnaryExp
    : PrimaryExp{
        auto ast = new UnaryExpAST();
        ast->kind = 1;
        ast->primaryExp = unique_ptr<BaseAST>($1);
        ast->GetNumber();
        $$ = ast;
    }
    | '+' UnaryExp{
        auto ast = new UnaryExpAST();
        ast->kind = 2;
        ast->unaryExp = unique_ptr<BaseAST>($2);
        ast->GetNumber();
        $$ = ast;
    }
    | '-' UnaryExp{
        auto ast = new UnaryExpAST();
        ast->kind = 3;
        ast->unaryExp = unique_ptr<BaseAST>($2);
        ast->GetNumber();
        $$ = ast;
    }
    | '!' UnaryExp{
        auto ast = new UnaryExpAST();
        ast->kind = 4;
        ast->unaryExp = unique_ptr<BaseAST>($2);
        ast->GetNumber();
        $$ = ast;
    }
    ;

%%

// 定义错误处理函数, 其中第二个参数是错误信息
// parser 如果发生错误 (例如输入的程序出现了语法错误), 就会调用这个函数
void yyerror(unique_ptr<BaseAST> &ast, const char *s) {
    cerr << "error: " << s << endl;
}
