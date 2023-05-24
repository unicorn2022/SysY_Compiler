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
NestedSymbolTable symbolTable;

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
}

// lexer 返回的所有 token 种类的声明（终结符）
// 注意 IDENT 和 INT_CONST 会返回 token 的值, 分别对应 str_val 和 int_val
%token INT CONST IF ELSE WHILE BREAK CONTINUE RETURN LT GT LE GE EQ NE AND OR
%token <str_val> IDENT
%token <int_val> INT_CONST

// 非终结符的类型定义, 分别对应 ast_val 和 int_val
%type <ast_val> FuncDef FuncType Block BlockItem Decl ConstDecl ConstDef VarDecl VarDef Stmt MatchStmt UnmatchStmt OtherStmt
%type <expAst_val> ConstInitVal ConstExp InitVal Exp PrimaryExp UnaryExp MulExp AddExp RelExp EqExp LAndExp LOrExp
%type <ast_list> BlockItems ConstDefs VarDefs
%type <str_val> LVal
%type <int_val> BType Number If While

// 无返回类型的终结符
// BlockBeg BlockEnd

%%

// CompUnit ::= FuncDef;
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

// FuncType ::= INT;
FuncType
    : INT {
        auto ast = new FuncTypeAST();
        ast->type = "int";
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
    : CONST BType ConstDefs ';' {
        auto ast = new ConstDeclAST();
        ast->bType = $2;
        ast->constDefs = unique_ptr<vector<unique_ptr<BaseAST> > >($3);
        $$ = ast;
    }
    ;

// BType ::= INT
BType
    : INT {
        $$ = 0; // FIXME 这里应该是一个类型
    }
    ;

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
        symbolTable.AddConstSymbol(ast->ident, ast->constInitVal->CalcConstExp());
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
    : BType VarDefs ';' {
        auto ast = new VarDeclAST();
        ast->bType = $1;
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
        ast->kind = VarDefAST::kUnInit;
        ast->ident = *unique_ptr<string>($1);
        // 将变量定义插入符号表
        symbolTable.AddVarSymbol(ast->ident);

        // 变量名后面加上序号
        int id = symbolTable.GetVarSymbolValue(ast->ident);
        ast->ident += id == 0 ? "" : "_" + to_string(id); // 优化：如果序号为0，不加在变量名后面
        
        $$ = ast;
    }
    | IDENT '=' InitVal {
        auto ast = new VarDefAST();
        ast->kind = VarDefAST::kInit;
        ast->ident = *unique_ptr<string>($1);
        ast->initVal = unique_ptr<BaseExpAST>($3);
        // 将变量定义插入符号表
        symbolTable.AddVarSymbol(ast->ident);
        
        // 变量名后面加上序号
        int id = symbolTable.GetVarSymbolValue(ast->ident);
        ast->ident += id == 0 ? "" : "_" + to_string(id); // 优化：如果序号为0，不加在变量名后面

        $$ = ast;
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
        ast->lVal = *unique_ptr<string>($1);

        // 变量名后面加上序号
        int id = symbolTable.GetVarSymbolValue(ast->lVal);
        ast->lVal += id == 0 ? "" : "_" + to_string(id); // 优化：如果序号为0，不加在变量名后面

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
        ast->lVal = *unique_ptr<string>($1);

        // LVal 是常量标识符，直接替换为常量值
        if (symbolTable.GetSymbolType(ast->lVal) == SymbolTable::Symbol::kConst) {
            ast->kind = PrimaryExpAST::kNumber;
            ast->number = symbolTable.GetConstSymbolValue(ast->lVal);
        } else
        // LVal 是变量标识符，替换为变量名
        if (symbolTable.GetSymbolType(ast->lVal) == SymbolTable::Symbol::kVar) {
            ast->kind = PrimaryExpAST::kLVal;

            // 变量名后面加上序号
            int id = symbolTable.GetVarSymbolValue(ast->lVal);
            ast->lVal += id == 0 ? "" : "_" + to_string(id); // 优化：如果序号为0，不加在变量名后面
        } else {
            std::cerr << "error: " << ast->lVal << " is not a variable or constant" << std::endl;
        }

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
        $$ = $1;
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
