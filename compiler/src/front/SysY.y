%code requires {
    #include <memory>
    #include <string>
    #include <vector>
    #include "AST.hpp"
}

%{

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "AST.hpp"

// 用于存储符号表
// 符号表的作用域为函数内，随着编译过程动态增加或删除
NestedSymbolTable symbolTable;

// 用于存储全局符号表
SymbolTable globalSymbolTable;

// 用于记录当前是否在全局作用域
bool isGlobal = true;

// 用于记录 if 语句的出现词序，以生成唯一的标签
int ifCount = 0;

// 用于记录 while 语句的出现词序，以生成唯一的标签
int whileCount = 0;

// 用于记录 while 嵌套中各层的 while 标号，以生成 break/continue 的跳转语句
std::vector<int> nestedWhileIndex;

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
    BaseExpAST *expAst_val;
    std::vector<std::unique_ptr<BaseAST> > *ast_list;
    std::vector<std::unique_ptr<BaseExpAST> > *expAst_list;
}

// lexer 返回的所有 token 种类的声明（终结符）
// 注意 IDENT 和 INT_CONST 会返回 token 的值, 分别对应 str_val 和 int_val
%token VOID INT CONST IF ELSE WHILE BREAK CONTINUE RETURN LT GT LE GE EQ NE AND OR
%token <str_val> IDENT
%token <int_val> INT_CONST

// 非终结符的类型定义, 分别对应 ast_val 和 int_val
%type <ast_val> GlobalDef FuncDef FuncFParam Block BlockItem Decl ConstDecl ConstDef VarDecl VarDef Stmt MatchStmt UnmatchStmt OtherStmt
%type <expAst_val> ConstInitVal ConstExp InitVal Exp PrimaryExp LVal UnaryExp MulExp AddExp RelExp EqExp LAndExp LOrExp
%type <ast_list> GlobalDefs FuncFParams BlockItems ConstDefs VarDefs
%type <expAst_list> FuncRParams ConstArrayDims ArrayDims
%type <int_val> Number If While

// 无返回类型的终结符
// BlockBeg BlockEnd

%%

// CompUnit ::= [CompUnit] FuncDef;
// 之前我们定义了 FuncDef 会返回一个 ast_val
// 而 parser 一旦解析完 CompUnit, 就说明所有的 token 都被解析了, 即解析结束了
// 此时我们应该把 FuncDef 返回的结果收集起来, 作为 AST 传给调用 parser 的函数
// $1 指代规则里第一个符号的返回值, 也就是 FuncDef 的返回值
CompUnit
    : GlobalDefs {
        auto compUnit = make_unique<CompUnitAST>();
        compUnit->globalDefs = unique_ptr<vector<unique_ptr<BaseAST> > >($1);
        ast = move(compUnit);
    }
    ;

GlobalDefs
    : GlobalDef {
        auto ast_list = new vector<unique_ptr<BaseAST> >();
        ast_list->push_back(unique_ptr<BaseAST>($1));
        $$ = ast_list;
    }
    | GlobalDefs GlobalDef {
        auto ast_list = $1; // XXX 这里没用 unique_ptr
        ast_list->push_back(unique_ptr<BaseAST>($2));
        $$ = ast_list;
    }
    ;

GlobalDef
    : Decl {
        auto ast = new GlobalDefAST();
        ast->kind = GlobalDefAST::kDecl;
        ast->decl = unique_ptr<BaseAST>($1);
        $$ = ast;
    }
    | FuncDef {
        auto ast = new GlobalDefAST();
        ast->kind = GlobalDefAST::kFuncDef;
        ast->funcDef = unique_ptr<BaseAST>($1);
        $$ = ast;
    }
    ;

// FuncDef ::= FuncType IDENT '(' [FuncFParams] ')' Block;
// 我们这里可以直接写 '(' 和 ')', 因为之前在 lexer 里已经处理了单个字符的情况
// 解析完成后, 把这些符号的结果收集起来, 然后拼成一个 FuncDefAST, 作为结果返回
// $$ 表示非终结符的返回值, 我们可以通过给这个符号赋值的方法来返回结果
FuncDef
    : VOID IDENT '(' ')' {
        globalSymbolTable.AddFuncSymbol(*$2, 0, 0, NULL);
    } Block {
        auto ast = new FuncDefAST();
        ast->funcType = 0; // 0 表示 void
        ast->ident = *unique_ptr<string>($2);
        ast->funcFParams = nullptr;
        ast->block = unique_ptr<BaseAST>($6);

        isGlobal = true; // 函数定义结束，全局状态设为 true
        
        $$ = ast;
    }
    | INT IDENT '(' ')' {
        globalSymbolTable.AddFuncSymbol(*$2, 1, 0, NULL);
    } Block {
        auto ast = new FuncDefAST();
        ast->funcType = 1; // 1 表示 int
        ast->ident = *unique_ptr<string>($2);
        ast->funcFParams = nullptr;
        ast->block = unique_ptr<BaseAST>($6);

        isGlobal = true; // 函数定义结束，全局状态设为 true
        
        $$ = ast;
    }
    | VOID IDENT '(' FuncFParams ')' {
        vector<unique_ptr<BaseAST> > *fParams = $4;

        int fParamNum = fParams->size();
        bool *fParamIsArray = new bool[fParamNum];
        for (int i = 0; i < fParamNum; i++) {
            auto fParam = static_cast<FuncFParamAST*>((*fParams)[i].get());
            fParamIsArray[i] = fParam->kind == FuncFParamAST::kIntArray;
        }
        globalSymbolTable.AddFuncSymbol(*$2, 0, fParamNum, fParamIsArray);
    } Block {
        auto ast = new FuncDefAST();
        ast->funcType = 0; // 0 表示 void
        ast->ident = *unique_ptr<string>($2);
        ast->funcFParams = unique_ptr<vector<unique_ptr<BaseAST> > >($4);
        ast->block = unique_ptr<BaseAST>($7);

        symbolTable.ClearFParamTable();
        isGlobal = true; // 函数定义结束，全局状态设为 true

        $$ = ast;
    }
    | INT IDENT '(' FuncFParams ')' {
        vector<unique_ptr<BaseAST> > *fParams = $4;
        
        int fParamNum = fParams->size();
        bool *fParamIsArray = new bool[fParamNum];
        for (int i = 0; i < fParamNum; i++) {
            auto fParam = static_cast<FuncFParamAST*>((*fParams)[i].get());
            fParamIsArray[i] = fParam->kind == FuncFParamAST::kIntArray;
        }
        globalSymbolTable.AddFuncSymbol(*$2, 1, fParamNum, fParamIsArray);
    } Block {
        auto ast = new FuncDefAST();
        ast->funcType = 1; // 1 表示 int
        ast->ident = *unique_ptr<string>($2);
        ast->funcFParams = unique_ptr<vector<unique_ptr<BaseAST> > >($4);
        ast->block = unique_ptr<BaseAST>($7);

        symbolTable.ClearFParamTable();
        isGlobal = true; // 函数定义结束，全局状态设为 true

        $$ = ast;
    }
    ;

FuncFParams
    : FuncFParam {
        auto ast_list = new vector<unique_ptr<BaseAST> >();
        ast_list->push_back(unique_ptr<BaseAST>($1));
        $$ = ast_list;
    }
    | FuncFParams ',' FuncFParam {
        auto ast_list = $1; // XXX 这里没用 unique_ptr
        ast_list->push_back(unique_ptr<BaseAST>($3));
        $$ = ast_list;
    }
    ;

FuncFParam
    : INT IDENT { // XXX BType 确定为 int
        auto ast = new FuncFParamAST();
        ast->kind = FuncFParamAST::kInt;
        ast->bType = 0;
        ast->ident = *unique_ptr<string>($2);

        symbolTable.AddFParamSymbol(ast->ident, -1, 0); // 添加函数形参符号，-1 表示是函数参数
        ast->ident += "_"; // 函数形参的标识符加上下划线，以区分全局变量
        
        $$ = ast;
    }
    | INT IDENT '[' ']' {
        auto ast = new FuncFParamAST();
        ast->kind = FuncFParamAST::kIntArray;
        ast->bType = 0;
        ast->ident = *unique_ptr<string>($2);
        ast->constArrayDims = nullptr;

        symbolTable.AddFParamSymbol(ast->ident, -2, 1); // 添加函数形参符号，-2 表示是数组类型
        ast->ident += "_"; // 函数形参的标识符加上下划线，以区分全局变量

        $$ = ast;
    }
    | INT IDENT '[' ']' ConstArrayDims {
        auto ast = new FuncFParamAST();
        ast->kind = FuncFParamAST::kIntArray;
        ast->bType = 0;
        ast->ident = *unique_ptr<string>($2);
        ast->constArrayDims = unique_ptr<vector<unique_ptr<BaseExpAST> > >($5);

        symbolTable.AddFParamSymbol(ast->ident, -2, 1 + ast->constArrayDims->size()); // 添加函数形参符号，-2 表示是数组类型
        ast->ident += "_"; // 函数形参的标识符加上下划线，以区分全局变量

        $$ = ast;
    }
    ;

// Block :== '{' BlockItems '}';
Block
    : '{' '}' {
        auto ast = new BlockAST();
        ast->blockItems = nullptr;
        $$ = ast;
    }
    | BlockBeg BlockItems BlockEnd {
        auto ast = new BlockAST();
        ast->blockItems = unique_ptr<vector<unique_ptr<BaseAST> > >($2);
        $$ = ast;
    }
    ;

// BlockBeg ::= '{';
// 进入代码块，创建新的符号表
BlockBeg
    : '{' {
        if (isGlobal == true) { // 是全局状态，说明代码块为进入函数体的代码块，将全局状态设为 false
            isGlobal = false;
        }
        symbolTable.AddTable();
    }
    ;

// BlockEnd ::= '}';
// 退出代码块，删除当前符号表
BlockEnd
    : '}' {
        symbolTable.DropTable();
    }
    ;

// BlockItems ::= %empty | BlockItems BlockItem;
BlockItems
    : BlockItem {
        auto ast_list = new vector<unique_ptr<BaseAST> >();
        ast_list->push_back(unique_ptr<BaseAST>($1));
        $$ = ast_list;
    }
    | BlockItems BlockItem {
        auto ast_list = $1; // XXX 这里没用 unique_ptr
        ast_list->push_back(unique_ptr<BaseAST>($2));
        $$ = ast_list;
    }
    ;

// BlockItem ::= Decl | Stmt;
BlockItem
    : Decl {
        auto ast = new BlockItemAST();
        ast->kind = BlockItemAST::kDecl;
        ast->decl = unique_ptr<BaseAST>($1);
        $$ = ast;
    }
    | Stmt {
        auto ast = new BlockItemAST();
        ast->kind = BlockItemAST::kStmt;
        ast->stmt = unique_ptr<BaseAST>($1);
        $$ = ast;
    }
    ;

// Decl ::= ConstDecl | VarDecl;
Decl
    : ConstDecl {
        auto ast = new DeclAST();
        ast->kind = DeclAST::kConstDecl;
        ast->constDecl = unique_ptr<BaseAST>($1);
        $$ = ast;
    }
    | VarDecl {
        auto ast = new DeclAST();
        ast->kind = DeclAST::kVarDecl;
        ast->varDecl = unique_ptr<BaseAST>($1);
        $$ = ast;
    }
    ;

// ConstDecl ::= "const" BType ConstDef {"," ConstDef} ";";
// (ConstDecl ::= "const" BType ConstDefs ';';)
ConstDecl
    : CONST INT ConstDefs ';' {
        auto ast = new ConstDeclAST();
        ast->bType = 0;
        ast->constDefs = unique_ptr<vector<unique_ptr<BaseAST> > >($3);
        $$ = ast;
    }
    ;

// BType ::= INT
// BType
//     : INT {
//         $$ = 0; // XXX 这里应该是一个类型
//     }
//     ;

// (ConstDefs ::= ConstDef | ConstDefs ',';)
ConstDefs
    : ConstDef {
        auto ast_list = new vector<unique_ptr<BaseAST> >();
        ast_list->push_back(unique_ptr<BaseAST>($1));
        $$ = ast_list;
    }
    | ConstDefs ',' ConstDef {
        auto ast_list = $1; // XXX 这里没用 unique_ptr
        ast_list->push_back(unique_ptr<BaseAST>($3));
        $$ = ast_list;
    }
    ;

// ConstDef ::= IDENT '=' ConstInitVal;
ConstDef
    : IDENT '=' ConstInitVal {
        auto ast = new ConstDefAST();
        ast->ident = *unique_ptr<string>($1);
        ast->constInitVal = unique_ptr<BaseExpAST>($3);
        // 将常量定义插入符号表
        if (isGlobal) {
            globalSymbolTable.AddConstSymbol(ast->ident, ast->constInitVal->CalcConstExp());
        } else {
        symbolTable.AddConstSymbol(ast->ident, ast->constInitVal->CalcConstExp());
        }
        $$ = ast;
    }
    ;

// ConstInitVal ::= ConstExp;
ConstInitVal
    : ConstExp {
        auto ast = new ConstInitValAST();
        ast->constExp = unique_ptr<BaseExpAST>($1);
        $$ = ast;
    }
    ;

// VarDecl ::= BType VarDef {"," VarDef} ";";
// (VarDecl ::= BType VarDefs ';';)
VarDecl
    : INT VarDefs ';' {
        auto ast = new VarDeclAST();
        ast->bType = 0;
        ast->varDefs = unique_ptr<vector<unique_ptr<BaseAST> > >($2);
        $$ = ast;
    }
    ;

// VarDefs ::= VarDef | VarDefs ',' VarDef;
VarDefs
    : VarDef {
        auto ast_list = new vector<unique_ptr<BaseAST> >();
        ast_list->push_back(unique_ptr<BaseAST>($1));
        $$ = ast_list;
    }
    | VarDefs ',' VarDef {
        auto ast_list = $1; // XXX 这里没用 unique_ptr
        ast_list->push_back(unique_ptr<BaseAST>($3));
        $$ = ast_list;
    }
    ;

// VarDef ::= IDENT | IDENT '=' InitVal;
VarDef
    : IDENT {
        auto ast = new VarDefAST();
        ast->isGlobal = isGlobal;
        ast->kind = VarDefAST::kUnInit;
        ast->ident = *unique_ptr<string>($1);

        if (isGlobal) {
            // 将变量定义插入符号表
            globalSymbolTable.AddVarSymbol(ast->ident, 0, 0);
            // 全局变量名不加序号
        } else {
            // 将变量定义插入符号表
            symbolTable.AddVarSymbol(ast->ident, 0);

            // 变量名后面加上序号
            int id = symbolTable.GetVarSymbolId(ast->ident);
            ast->ident += id == 0 ? "" : "_" + to_string(id); // XXX 不需要 0 了，因为全局变量不加序号，局部变量默认都加序号
                                                            // 原优化：如果序号为0，不加在变量名后面
        }
        
        $$ = ast;
    }
    | IDENT ConstArrayDims {
        auto ast = new VarDefAST();
        ast->isGlobal = isGlobal;
        ast->kind = VarDefAST::kArray;
        ast->ident = *unique_ptr<string>($1);
        ast->constArrayDims = unique_ptr<vector<unique_ptr<BaseExpAST> > >($2);

        // TODO 没有在符号表中区分数组和变量
        if (isGlobal) {
            // 将变量定义插入符号表
            globalSymbolTable.AddVarSymbol(ast->ident, 0, ast->constArrayDims->size());
            // 全局变量名不加序号
        } else {
            // 将变量定义插入符号表
            symbolTable.AddVarSymbol(ast->ident, ast->constArrayDims->size());

            // 变量名后面加上序号
            int id = symbolTable.GetVarSymbolId(ast->ident);
            ast->ident += id == 0 ? "" : "_" + to_string(id); // XXX 不需要 0 了，因为全局变量不加序号，局部变量默认都加序号
                                                              // 原优化：如果序号为0，不加在变量名后面
        }

        $$ = ast;
    }
    | IDENT '=' InitVal {
        auto ast = new VarDefAST();
        ast->isGlobal = isGlobal;
        ast->kind = VarDefAST::kInit;
        ast->ident = *unique_ptr<string>($1);
        ast->initVal = unique_ptr<BaseExpAST>($3);

        if (isGlobal) {
            // 将变量定义插入符号表
            globalSymbolTable.AddVarSymbol(ast->ident, 0, 0);
            // 全局变量名不加序号
        } else {
            // 将变量定义插入符号表
            symbolTable.AddVarSymbol(ast->ident, 0);
            
            // 变量名后面加上序号
            int id = symbolTable.GetVarSymbolId(ast->ident);
            ast->ident += id == 0 ? "" : "_" + to_string(id); // XXX 不需要 0 了，因为全局变量不加序号，局部变量默认都加序号
                                                              // 原优化：如果序号为0，不加在变量名后面
        }

        $$ = ast;
    }
    ;

ConstArrayDims
    : '[' ConstExp ']' {
        auto expAst_list = new vector<unique_ptr<BaseExpAST> >();
        expAst_list->push_back(unique_ptr<BaseExpAST>($2));
        $$ = expAst_list;
    }
    | ConstArrayDims '[' ConstExp ']' {
        auto expAst_list = $1; // XXX 这里没用 unique_ptr
        expAst_list->push_back(unique_ptr<BaseExpAST>($3));
        $$ = expAst_list;
    }
    ;

// InitVal ::= Exp;
InitVal
    : Exp {
        auto ast = new InitValAST();
        ast->exp = unique_ptr<BaseExpAST>($1);
        $$ = ast;
    }
    ;

// Stmt ::= MatchStmt | UnmatchStmt;
Stmt
    : MatchStmt {
        auto ast = new StmtAST();
        ast->kind = StmtAST::kMatch;
        ast->matchStmt = unique_ptr<BaseAST>($1);
        $$ = ast;
    }
    | UnmatchStmt {
        auto ast = new StmtAST();
        ast->kind = StmtAST::kUnmatch;
        ast->unmatchStmt = unique_ptr<BaseAST>($1);
        $$ = ast;
    }
    ;

// MatchStmt ::= "if" '(' Exp ')' MatchStmt "else" MatchStmt | OtherStmt;
MatchStmt
    : If '(' Exp ')' MatchStmt ELSE MatchStmt {
        auto ast = new MatchStmtAST();
        ast->kind = MatchStmtAST::kIf;
        ast->ifLabelIndex = $1;
        ast->exp = unique_ptr<BaseExpAST>($3);
        ast->matchStmt1 = unique_ptr<BaseAST>($5);
        ast->matchStmt2 = unique_ptr<BaseAST>($7);
        $$ = ast;
    }
    | OtherStmt {
        auto ast = new MatchStmtAST();
        ast->kind = MatchStmtAST::kOther;
        ast->otherStmt = unique_ptr<BaseAST>($1);
        $$ = ast;
    }
    ;

// UnmatchStmt ::= "if" '(' Exp ')' Stmt | "if" '(' Exp ')' MatchStmt "else" UnmatchStmt;
UnmatchStmt
    : If '(' Exp ')' Stmt {
        auto ast = new UnmatchStmtAST();
        ast->kind = UnmatchStmtAST::kNoElse;
        ast->ifLabelIndex = $1;
        ast->exp = unique_ptr<BaseExpAST>($3);
        ast->stmt = unique_ptr<BaseAST>($5);
        $$ = ast;
    }
    | If '(' Exp ')' MatchStmt ELSE UnmatchStmt {
        auto ast = new UnmatchStmtAST();
        ast->kind = UnmatchStmtAST::kElse;
        ast->ifLabelIndex = $1;
        ast->exp = unique_ptr<BaseExpAST>($3);
        ast->matchStmt = unique_ptr<BaseAST>($5);
        ast->unmatchStmt = unique_ptr<BaseAST>($7);
        $$ = ast;
    }
    ;

// 用于返回 if 语句的唯一标签（以 int 形式返回 if 的出现次序）
If
    : IF {
        $$ = ifCount++;
    }
    ;

// OtherStmt ::= LVal '=' Exp ';' | [Exp] ';' | Block | 'return' Exp ';';
// (OtherStmt ::= LVal '=' Exp ';' | ';' | Exp ';' | Block | RETURN Exp ';';)
OtherStmt
    : ';' {
        auto ast = new OtherStmtAST();
        ast->kind = OtherStmtAST::kExp;
        ast->exp = nullptr;
        $$ = ast;
    }
    | Exp ';' {
        auto ast = new OtherStmtAST();
        ast->kind = OtherStmtAST::kExp;
        ast->exp = unique_ptr<BaseExpAST>($1);
        $$ = ast;
    }
    | LVal '=' Exp ';' {
        auto ast = new OtherStmtAST();
        ast->kind = OtherStmtAST::kAssign;
        ast->lVal = unique_ptr<BaseExpAST>($1);
        ast->exp = unique_ptr<BaseExpAST>($3);
        $$ = ast;
    }
    | While '(' Exp ')' Stmt {
        auto ast = new OtherStmtAST();
        ast->kind = OtherStmtAST::kWhile;
        ast->whileIndex = $1;
        ast->exp = unique_ptr<BaseExpAST>($3);
        ast->stmt = unique_ptr<BaseAST>($5);
        nestedWhileIndex.pop_back(); // 退出 while 循环，删除当前 while 的序号
        $$ = ast;
    }
    | BREAK ';' {
        auto ast = new OtherStmtAST();
        ast->kind = OtherStmtAST::kBreak;
        ast->whileIndex = nestedWhileIndex.back(); // 跳出 while 循环，跳转到最近的 while 循环结尾，所以需要获取最近 while 的标号
        $$ = ast;
    }
    | CONTINUE ';' {
        auto ast = new OtherStmtAST();
        ast->kind = OtherStmtAST::kContinue;
        ast->whileIndex = nestedWhileIndex.back(); // 跳出 while 循环，跳转到最近的 while 循环判断，所以需要获取最近 while 的标号
        $$ = ast;
    }
    | RETURN ';' {
        auto ast = new OtherStmtAST();
        ast->kind = OtherStmtAST::kReturn;
        ast->exp = nullptr;
        $$ = ast;
    }
    | RETURN Exp ';' {
        auto ast = new OtherStmtAST();
        ast->kind = OtherStmtAST::kReturn;
        ast->exp = unique_ptr<BaseExpAST>($2);
        $$ = ast;
    }
    | Block {
        auto ast = new OtherStmtAST();
        ast->kind = OtherStmtAST::kBlock;
        ast->block = unique_ptr<BaseAST>($1);
        $$ = ast;
    }
    ;

While
    : WHILE {
        nestedWhileIndex.push_back(whileCount); // 进入 while 循环，记录当前 while 的序号
        $$ = whileCount++;
    }

// ConstExp ::= Exp;
ConstExp
    : Exp {
        auto ast = new ConstExpAST();
        ast->exp = unique_ptr<BaseExpAST>($1);
        ast->value = ast->CalcConstExp(); // 遍历 AST，计算常量表达式的值
        ast->isCalcuated = true;
        $$ = ast;
    }
    ;

// Exp ::= LOrExp;
Exp
    : LOrExp {
        auto ast = new ExpAST();
        ast->lOrExp = unique_ptr<BaseExpAST>($1);
        $$ = ast;
    }
    ;

// PrimaryExp ::= '(' Exp ')' | LVal | Number;
PrimaryExp
    : '(' Exp ')' {
        auto ast = new PrimaryExpAST();
        ast->kind = PrimaryExpAST::kExp;
        ast->exp = unique_ptr<BaseExpAST>($2);
        $$ = ast;
    }
    | LVal {
        auto ast = new PrimaryExpAST();
        ast->kind = PrimaryExpAST::kLVal;
        ast->lVal = unique_ptr<BaseExpAST>($1);
        $$ = ast;
    }
    | Number {
        auto ast = new PrimaryExpAST();
        ast->kind = PrimaryExpAST::kNumber;
        ast->number = $1;
        $$ = ast;
    }
    ;

// LVal ::= IDENT;
// 不对应 AST, 直接返回 IDENT 的值
LVal
    : IDENT {
        auto ast = new LValAST();
        ast->ident = *unique_ptr<string>($1);

        // 先查找局部标识符
        if (symbolTable.HasSymbol(ast->ident)) {
            if (symbolTable.GetSymbolType(ast->ident) == SymbolTable::Symbol::kConst) {
                ast->kind = LValAST::kConst;
                ast->identVal = symbolTable.GetConstSymbolValue(ast->ident);
                ast->isArrayPtr = false;
            } else
            if (symbolTable.GetSymbolType(ast->ident) == SymbolTable::Symbol::kVar) {
                ast->kind = LValAST::kVar;
                ast->identVal = symbolTable.GetVarSymbolId(ast->ident);
                ast->isArrayPtr = symbolTable.GetVarSymbolDim(ast->ident) > 0;
            } else {
                std::cerr << "LVal: unknown symbol type" << std::endl;
            }
        } else
        // 再查找全局标识符
        if (globalSymbolTable.HasSymbol(ast->ident)) {
            if (globalSymbolTable.GetSymbolType(ast->ident) == SymbolTable::Symbol::kConst) {
                ast->kind = LValAST::kConst;
                ast->identVal = globalSymbolTable.GetConstSymbolValue(ast->ident);
                ast->isArrayPtr = false;
            } else
            if (globalSymbolTable.GetSymbolType(ast->ident) == SymbolTable::Symbol::kVar) {
                ast->kind = LValAST::kVar;
                ast->identVal = globalSymbolTable.GetVarSymbolId(ast->ident);
                ast->isArrayPtr = globalSymbolTable.GetVarSymbolDim(ast->ident) > 0;
            } else {
                std::cerr << "LVal: unknown symbol type" << std::endl;
            }
        } else {
            std::cerr << "error: " << ast->ident << " is not defined" << std::endl;
        }
        
        $$ = ast;
    }
    | IDENT ArrayDims {
        auto ast = new LValAST();
        ast->kind = LValAST::kArray;
        ast->ident = *unique_ptr<string>($1);
        ast->arrayDims = unique_ptr<vector<unique_ptr<BaseExpAST> > >($2);
        ast->isArrayPtr = false;

        // XXX 目前数组只有变量
        // 先查找局部标识符
        if (symbolTable.HasSymbol(ast->ident)) {
            if (symbolTable.GetSymbolType(ast->ident) == SymbolTable::Symbol::kConst) {
                ast->identVal = symbolTable.GetConstSymbolValue(ast->ident);
            } else
            if (symbolTable.GetSymbolType(ast->ident) == SymbolTable::Symbol::kVar) {
                ast->identVal = symbolTable.GetVarSymbolId(ast->ident);
                ast->isArrayPtr = symbolTable.GetVarSymbolDim(ast->ident) > ast->arrayDims->size();
            } else {
                std::cerr << "LVal: unknown symbol type" << std::endl;
            }
        } else
        // 再查找全局标识符
        if (globalSymbolTable.HasSymbol(ast->ident)) {
            if (globalSymbolTable.GetSymbolType(ast->ident) == SymbolTable::Symbol::kConst) {
                ast->identVal = globalSymbolTable.GetConstSymbolValue(ast->ident);
            } else
            if (globalSymbolTable.GetSymbolType(ast->ident) == SymbolTable::Symbol::kVar) {
                ast->identVal = globalSymbolTable.GetVarSymbolId(ast->ident);
                ast->isArrayPtr = globalSymbolTable.GetVarSymbolDim(ast->ident) > ast->arrayDims->size();
            } else {
                std::cerr << "LVal: unknown symbol type" << std::endl;
            }
        } else {
            std::cerr << "error: " << ast->ident << " is not defined" << std::endl;
        }

        $$ = ast;
    }
    ;

ArrayDims
    : '[' Exp ']' {
        auto expAst_list = new vector<unique_ptr<BaseExpAST> >();
        expAst_list->push_back(unique_ptr<BaseExpAST>($2));
        $$ = expAst_list;
    }
    | ArrayDims '[' Exp ']' {
        auto expAst_list = $1; // XXX 这里没用 unique_ptr
        expAst_list->push_back(unique_ptr<BaseExpAST>($3));
        $$ = expAst_list;
    }
    ;

// Number ::= INT_CONST;
// 不对应 AST, 直接返回 INT_CONST 的值
Number
    : INT_CONST{
        $$ = $1;
    }
    ;

// UnaryExp ::= PrimaryExp | '+' UnaryExp | '-' UnaryExp | '!' UnaryExp;
UnaryExp
    : PrimaryExp{
        auto ast = new UnaryExpAST();
        ast->kind = UnaryExpAST::kPrimaryExp;
        ast->primaryExp = unique_ptr<BaseExpAST>($1);
        $$ = ast;
    }
    | IDENT '(' ')' {
        auto ast = new UnaryExpAST();
        ast->kind = UnaryExpAST::kCall;
        ast->ident = *unique_ptr<string>($1);
        ast->funcType = globalSymbolTable.GetFuncSymbolType(ast->ident);
        ast->funcFParamNum = 0;
        ast->funcFParamIsArray = NULL;
        ast->funcRParams = nullptr;
        $$ = ast;
    }
    | IDENT '(' FuncRParams ')' {
        auto ast = new UnaryExpAST();
        ast->kind = UnaryExpAST::kCall;
        ast->ident = *unique_ptr<string>($1);
        ast->funcType = globalSymbolTable.GetFuncSymbolType(ast->ident);
        ast->funcFParamNum = globalSymbolTable.GetFuncSymbolFParamNum(ast->ident);
        ast->funcFParamIsArray = globalSymbolTable.GetFuncSymbolFParamIsArray(ast->ident);
        ast->funcRParams = unique_ptr<vector<unique_ptr<BaseExpAST> > >($3);
        $$ = ast;
    }
    | '+' UnaryExp{
        auto ast = new UnaryExpAST();
        ast->kind = UnaryExpAST::kPositive;
        ast->unaryExp = unique_ptr<BaseExpAST>($2);
        $$ = ast;
    }
    | '-' UnaryExp{
        auto ast = new UnaryExpAST();
        ast->kind = UnaryExpAST::kNegative;
        ast->unaryExp = unique_ptr<BaseExpAST>($2);
        $$ = ast;
    }
    | '!' UnaryExp{
        auto ast = new UnaryExpAST();
        ast->kind = UnaryExpAST::kNot;
        ast->unaryExp = unique_ptr<BaseExpAST>($2);
        $$ = ast;
    }
    ;

FuncRParams
    : Exp {
        auto expAst_list = new vector<unique_ptr<BaseExpAST> >();
        expAst_list->push_back(unique_ptr<BaseExpAST>($1));
        $$ = expAst_list;
    }
    | FuncRParams ',' Exp {
        auto expAst_list = $1; // XXX 这里没用 unique_ptr
        expAst_list->push_back(unique_ptr<BaseExpAST>($3));
        $$ = expAst_list;
    }
    ;

// MulExp ::= UnaryExp | MulExp ('*' | '/' | '%') UnaryExp;
MulExp
    : UnaryExp {
        auto ast = new MulExpAST();
        ast->kind = MulExpAST::kUnaryExp;
        ast->unaryExp = unique_ptr<BaseExpAST>($1);
        $$ = ast;
    }
    | MulExp '*' UnaryExp {
        auto ast = new MulExpAST();
        ast->kind = MulExpAST::kMul;
        ast->mulExp = unique_ptr<BaseExpAST>($1);
        ast->unaryExp = unique_ptr<BaseExpAST>($3);
        $$ = ast;
    }
    | MulExp '/' UnaryExp {
        auto ast = new MulExpAST();
        ast->kind = MulExpAST::kDiv;
        ast->mulExp = unique_ptr<BaseExpAST>($1);
        ast->unaryExp = unique_ptr<BaseExpAST>($3);
        $$ = ast;
    }
    | MulExp '%' UnaryExp {
        auto ast = new MulExpAST();
        ast->kind = MulExpAST::kMod;
        ast->mulExp = unique_ptr<BaseExpAST>($1);
        ast->unaryExp = unique_ptr<BaseExpAST>($3);
        $$ = ast;
    }
    ;

// AddExp ::= MulExp | AddExp ('+' | '-') MulExp;
AddExp
    : MulExp {
        auto ast = new AddExpAST();
        ast->kind = AddExpAST::kMulExp;
        ast->mulExp = unique_ptr<BaseExpAST>($1);
        $$ = ast;
    }
    | AddExp '+' MulExp {
        auto ast = new AddExpAST();
        ast->kind = AddExpAST::kAdd;
        ast->addExp = unique_ptr<BaseExpAST>($1);
        ast->mulExp = unique_ptr<BaseExpAST>($3);
        $$ = ast;
    }
    | AddExp '-' MulExp {
        auto ast = new AddExpAST();
        ast->kind = AddExpAST::kSub;
        ast->addExp = unique_ptr<BaseExpAST>($1);
        ast->mulExp = unique_ptr<BaseExpAST>($3);
        $$ = ast;
    }
    ;

// RelExp ::= AddExp | RelExp ('<' | '>' | '<=' | '>=') AddExp;
RelExp
    : AddExp {
        auto ast = new RelExpAST();
        ast->kind = RelExpAST::kAddExp;
        ast->addExp = unique_ptr<BaseExpAST>($1);
        $$ = ast;
    }
    | RelExp LT AddExp {
        auto ast = new RelExpAST();
        ast->kind = RelExpAST::kLT;
        ast->relExp = unique_ptr<BaseExpAST>($1);
        ast->addExp = unique_ptr<BaseExpAST>($3);
        $$ = ast;
    }
    | RelExp GT AddExp {
        auto ast = new RelExpAST();
        ast->kind = RelExpAST::kGT;
        ast->relExp = unique_ptr<BaseExpAST>($1);
        ast->addExp = unique_ptr<BaseExpAST>($3);
        $$ = ast;
    }
    | RelExp LE AddExp {
        auto ast = new RelExpAST();
        ast->kind = RelExpAST::kLE;
        ast->relExp = unique_ptr<BaseExpAST>($1);
        ast->addExp = unique_ptr<BaseExpAST>($3);
        $$ = ast;
    }
    | RelExp GE AddExp {
        auto ast = new RelExpAST();
        ast->kind = RelExpAST::kGE;
        ast->relExp = unique_ptr<BaseExpAST>($1);
        ast->addExp = unique_ptr<BaseExpAST>($3);
        $$ = ast;
    }
    ;

// EqExp ::= RelExp | EqExp ('==' | '!=') Rel;
EqExp
    : RelExp {
        auto ast = new EqExpAST();
        ast->kind = EqExpAST::kRelExp;
        ast->relExp = unique_ptr<BaseExpAST>($1);
        $$ = ast;
    }
    | EqExp EQ RelExp {
        auto ast = new EqExpAST();
        ast->kind = EqExpAST::kEQ;
        ast->eqExp = unique_ptr<BaseExpAST>($1);
        ast->relExp = unique_ptr<BaseExpAST>($3);
        $$ = ast;
    }
    | EqExp NE RelExp {
        auto ast = new EqExpAST();
        ast->kind = EqExpAST::kNE;
        ast->eqExp = unique_ptr<BaseExpAST>($1);
        ast->relExp = unique_ptr<BaseExpAST>($3);
        $$ = ast;
    }
    ;

// LAndExp ::= EqExp | LAndExp '&&' EqExp;
LAndExp
    : EqExp {
        auto ast = new LAndExpAST();
        ast->kind = LAndExpAST::kEqExp;
        ast->eqExp = unique_ptr<BaseExpAST>($1);
        $$ = ast;
    }
    | LAndExp AND EqExp {
        auto ast = new LAndExpAST();
        ast->kind = LAndExpAST::kAnd;
        ast->lAndExp = unique_ptr<BaseExpAST>($1);
        ast->eqExp = unique_ptr<BaseExpAST>($3);
        $$ = ast;
    }
    ;

// LOrExp ::= LAndExp | LOrExp '||' LAndExp;
LOrExp
    : LAndExp {
        auto ast = new LOrExpAST();
        ast->kind = LOrExpAST::kLAndExp;
        ast->lAndExp = unique_ptr<BaseExpAST>($1);
        $$ = ast;
    }
    | LOrExp OR LAndExp {
        auto ast = new LOrExpAST();
        ast->kind = LOrExpAST::kOr;
        ast->lOrExp = unique_ptr<BaseExpAST>($1);
        ast->lAndExp = unique_ptr<BaseExpAST>($3);
        $$ = ast;
    }
    ;
%%

// 定义错误处理函数, 其中第二个参数是错误信息
// parser 如果发生错误 (例如输入的程序出现了语法错误), 就会调用这个函数
void yyerror(unique_ptr<BaseAST> &ast, const char *s) {
    cerr << "error: " << s << endl;
}
