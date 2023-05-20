#pragma once
#include <iostream>
// 所有 AST 的基类
class BaseAST {
public:
    virtual ~BaseAST() = default;
    virtual std::string PrintAST(std::string tab) const = 0;
    // 输出到buffer中, 返回值为需要的信息
    virtual std::string PrintIR(std::string tab, std::string &buffer) const = 0;

    std::string NewTempSymbol() const{
        static int count_var = 0;
        std::string ret = "%" + std::to_string(count_var);
        count_var++;
        return ret;
    }
};

// CompUnit: 起始字符, 表示整个文件
// CompUnit ::= FuncDef
class CompUnitAST : public BaseAST {
public:
    // 用智能指针管理对象
    std::unique_ptr<BaseAST> func_def;

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "CompUnitAST {\n";
        ans += tab + "\tfunc_def: " + func_def->PrintAST(tab + "\t");
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab, std::string &buffer) const override{
        func_def->PrintIR(tab, buffer);
        return "";
    }
};

// FuncDef: 函数定义
// FuncDef ::= FuncType IDENT '(' ')' Block;
/**
*   翻译为:
*   fun @IDENT (): [FuncType]{
*       [Block]
*   }
*/
class FuncDefAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> func_type;
    std::string ident;
    std::unique_ptr<BaseAST> block;

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "FuncDefAST {\n";
        ans += tab + "\tfunc_type: " + func_type->PrintAST(tab + "\t");
        ans += tab + "\tident: " + ident + "\n";
        ans += tab + "\tblock: " + block->PrintAST(tab + "\t");
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab, std::string &buffer) const override{
        // 函数的定义
        buffer += tab + "fun @" + ident + "():";
        func_type->PrintIR(tab, buffer);
        buffer += tab + "{\n";

        // 函数的代码块
        block->PrintIR(tab, buffer);

        // 函数结尾
        buffer += tab + "}\n";
        return "";
    }
};

// FuncType: 函数返回值类型(int)
// FuncType ::= INT
/**
* 翻译为: i32
*/
class FuncTypeAST : public BaseAST {
public:
    std::string type;

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "FuncTypeAST {\n";
        ans += tab + "\ttype: " + type + "\n";
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab, std::string &buffer) const override{
        if(type == "int") {
            buffer += tab + " i32 ";
        } else{
            buffer += tab + " error-type ";
        }
        return "";
    }
};

// Block: 函数的结构体
// Block :== '{' Stmt '}'
/**
*   翻译为:
*   %entry:
*       [Stmt]
*/
class BlockAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> stmt;

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "BlockAST {\n";
        ans += tab + "\tstmt: " + stmt->PrintAST(tab + "\t");
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab, std::string &buffer) const override{
        buffer += tab + "\%entry:\n";
        stmt->PrintIR(tab + "\t", buffer);
        return "";
    }
};

// Stmt: 一条 SysY 语句
// Stmt ::= 'return' Exp ';';
// 翻译为: ret var, var是Exp对应的变量
/**
*   翻译为:
*   var = [Exp]
*   ret var
*/
class StmtAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> exp;

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "StmtAST {\n";
        ans += tab + "\texp: " + exp->PrintAST(tab + "\t");
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab, std::string &buffer) const override{
        // 根据后面计算出来的变量名称
        std::string var = exp->PrintIR(tab, buffer);
        buffer += tab + "ret " + var + "\n";
        return "";
    }
};

// Exp: 一个 SysY 表达式
// Exp ::= AddExp;
/**
*   翻译为:
*   [addExp]
*/
class ExpAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> lOrExp;

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "ExpAST {\n";
        ans += tab + "\taddExp: " + lOrExp->PrintAST(tab + "\t");
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab, std::string &buffer) const override{
        std::string var = lOrExp->PrintIR(tab, buffer);
        return var;
    }
};

// PrimaryExpAST: 表达式中优先计算的部分, 即被'()'包裹的表达式/单个数字
// PrimaryExp ::= "(" Exp ")" | Number;
/**
*   翻译为:
*   1. [Exp]
*   2. Number
*/
class PrimaryExpAST : public BaseAST {
public:
    enum Kind {
        kExp,
        kNumber
    };

    Kind kind;
    std::unique_ptr<BaseAST> exp;
    int number;

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "PrimaryExpAST {\n";
        switch (kind) { // XXX 把switch改成if else，switch中不能定义变量
        case kExp:
            ans += tab + "\texp: " + exp->PrintAST(tab + "\t");
            break;
        case kNumber:
            ans += tab + "\tnumber: " + std::to_string(number) + "\n";
            break;
        default:
            std::cerr << "PrimaryExpAST::PrintAST: unknown kind" << std::endl;
            break;
        }
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab, std::string &buffer) const override{
        switch (kind) { // XXX 把switch改成if else，switch中不能定义变量
        case kExp:
            return exp->PrintIR(tab, buffer);
            break;
        case kNumber:
            return std::to_string(number);
            break;
        default:
            std::cerr << "PrimaryExpAST::PrintIR: unknown kind" << std::endl;
            break;
        }
        return "";
    }
};

// UnaryExpAST: 单目运算表达式
// UnaryExp ::= PrimaryExp | UnaryOp UnaryExp
/**
*   翻译为:
*   1. [PrimaryExp]
*   2. [UnaryExp]
*/
class UnaryExpAST : public BaseAST {
public:
    enum Kind {
        kPrimaryExp,
        kUnaryExp
    };
    
    Kind kind;
    std::unique_ptr<BaseAST> primaryExp;
    std::unique_ptr<BaseAST> unaryOp;
    std::unique_ptr<BaseAST> unaryExp;

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "UnaryExpAST {\n";
        switch (kind) { // XXX 把switch改成if else，switch中不能定义变量
        case kPrimaryExp:
            ans += tab + "\tprimaryExp: " + primaryExp->PrintAST(tab + "\t");
            break;
        case kUnaryExp:
            ans += tab + "\tunaryOp: " + unaryOp->PrintAST(tab + "\t");
            ans += tab + "\tunaryExp: " + unaryExp->PrintAST(tab + "\t");
            break;
        default:
            std::cerr << "UnaryExpAST::PrintAST: unknown kind" << std::endl;
            break;
        }

        ans += tab + "}\n";
        return ans;
    }
    std::string PrintIR(std::string tab, std::string &buffer) const override{
        if(kind == kPrimaryExp) {
            std::string var = primaryExp->PrintIR(tab, buffer);
            return var;
        } else if (kind == kUnaryExp) {
            std::string op = unaryOp->PrintIR(tab, buffer);
            // +, 不产生IR
            if(op == "add"){
                std::string var = unaryExp->PrintIR(tab, buffer);
                return var;
            }
            // -, IR格式为: now = sub 0, var
            // !, IR格式为: now = eq 0, var
            // now 为 %0, %1, %2, ...
            else if(op == "sub" || op == "eq"){
                std::string var = unaryExp->PrintIR(tab, buffer);
                std::string now = NewTempSymbol();
                buffer += tab + now + " = " + op + " 0, " + var + "\n";
                return now;
            }
            else {
                std::cerr << "UnaryExpAST::PrintIR: unknown UnaryOp kind" << std::endl;
            }
        } else {
            std::cerr << "UnaryExpAST::PrintIR: unknown kind" << std::endl;
        }
        return "";
    }
};

// UnaryOpAST: 单目运算符
// UnaryOp ::= '+' | '-' | '!'
class UnaryOpAST : public BaseAST {
public:
    enum Kind {
        kPlus,
        kMinus,
        kNot
    };

    Kind kind;

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "UnaryOpAst {\n";
        switch (kind) { // XXX 把switch改成if else，switch中不能定义变量
        case kPlus:
            ans += tab + "\tkind: +\n";
            break;
        case kMinus:
            ans += tab + "\tkind: -\n";
            break;
        case kNot:
            ans += tab + "\tkind: !\n";
            break;
        default:
            std::cerr << "UnaryOpAst::PrintAST: unknown kind" << std::endl;
            break;
        }
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab, std::string &buffer) const override{
        if (kind == kPlus) {
            return "add";
        } else
        if (kind == kMinus) {
            return "sub";
        } else
        if (kind == kNot) {
            return "eq";
        } else {
            std::cerr << "UnaryOpAst::PrintIR: unknown kind" << std::endl;
        }
        return "";
    }
};

// MulExpAST: 乘法表达式，包括乘法、除法、取模
// MulExp ::= UnaryExp | MulExp '*' UnaryExp | MulExp '/' UnaryExp | MulExp '%' UnaryExp
/**
 * 翻译为:
 * 1. [UnaryExp]
 * 2. var = mul [MulExp], [UnaryExp]
 * 3.
 * 4.
 */
class MulExpAST : public BaseAST {
public:
    enum Kind {
        kUnaryExp,
        kMul,
        kDiv,
        kMod
    };

    Kind kind;

    std::unique_ptr<BaseAST> unaryExp;
    std::unique_ptr<BaseAST> mulExp;

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "MulExpAST {\n";
        switch (kind) { // XXX 把switch改成if else，switch中不能定义变量
        case kUnaryExp:
            ans += tab + "\tunaryExp: " + unaryExp->PrintAST(tab + "\t");
            break;
        case kMul:
            ans += tab + "\tmulExp: " + mulExp->PrintAST(tab + "\t");
            ans += tab + "\tkind: *\n";
            ans += tab + "\tunaryExp: " + unaryExp->PrintAST(tab + "\t");
            break;
        case kDiv:
            ans += tab + "\tmulExp: " + mulExp->PrintAST(tab + "\t");
            ans += tab + "\tkind: /\n";
            ans += tab + "\tunaryExp: " + unaryExp->PrintAST(tab + "\t");
            break;
        case kMod:
            ans += tab + "\tmulExp: " + mulExp->PrintAST(tab + "\t");
            ans += tab + "\tkind: %\n";
            ans += tab + "\tunaryExp: " + unaryExp->PrintAST(tab + "\t");
            break;
        default:
            std::cerr << "MulExpAST::PrintAST: unknown kind" << std::endl;
            break;
        }
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab, std::string &buffer) const override {
        if (kind == kUnaryExp) {
            std::string var = unaryExp->PrintIR(tab, buffer);
            return var;
        } else
        if (kind == kMul) {
            std::string var1 = mulExp->PrintIR(tab, buffer);
            std::string var2 = unaryExp->PrintIR(tab, buffer);
            std::string now = NewTempSymbol();
            buffer += tab + now + " = mul " + var1 + ", " + var2 + "\n";
            return now;
        } else
        if (kind == kDiv) {
            std::string var1 = mulExp->PrintIR(tab, buffer);
            std::string var2 = unaryExp->PrintIR(tab, buffer);
            std::string now = NewTempSymbol();
            buffer += tab + now + " = div " + var1 + ", " + var2 + "\n";
            return now;
        } else
        if (kind == kMod) {
            std::string var1 = mulExp->PrintIR(tab, buffer);
            std::string var2 = unaryExp->PrintIR(tab, buffer);
            std::string now = NewTempSymbol();
            buffer += tab + now + " = mod " + var1 + ", " + var2 + "\n";
            return now;
        } else {
            std::cerr << "MulExpAST::PrintIR: unknown kind" << std::endl;
        }
        return "";
    }
};

// AddExpAST: 加法表达式，包括加法、减法
// AddExp ::= MulExp | AddExp '+' MulExp | AddExp '-' MulExp
class AddExpAST : public BaseAST {
public:
    enum Kind {
        kMulExp,
        kAdd,
        kSub
    };

    Kind kind;

    std::unique_ptr<BaseAST> mulExp;
    std::unique_ptr<BaseAST> addExp;

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "AddExpAST {\n";
        switch (kind) { // XXX 把switch改成if else，switch中不能定义变量
        case kMulExp:
            ans += tab + "\tmulExp: " + mulExp->PrintAST(tab + "\t");
            break;
        case kAdd:
            ans += tab + "\taddExp: " + addExp->PrintAST(tab + "\t");
            ans += tab + "\tkind: +\n";
            ans += tab + "\tmulExp: " + mulExp->PrintAST(tab + "\t");
            break;
        case kSub:
            ans += tab + "\taddExp: " + addExp->PrintAST(tab + "\t");
            ans += tab + "\tkind: -\n";
            ans += tab + "\tmulExp: " + mulExp->PrintAST(tab + "\t");
            break;
        default:
            std::cerr << "AddExpAST::PrintAST: unknown kind" << std::endl;
            break;
        }
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab, std::string &buffer) const override {
        if (kind == kMulExp) {
            std::string var = mulExp->PrintIR(tab, buffer);
            return var;
        } else
        if (kind == kAdd) {
            std::string var1 = addExp->PrintIR(tab, buffer);
            std::string var2 = mulExp->PrintIR(tab, buffer);
            std::string now = NewTempSymbol();
            buffer += tab + now + " = add " + var1 + ", " + var2 + "\n";
            return now;
        } else
        if (kind == kSub) {
            std::string var1 = addExp->PrintIR(tab, buffer);
            std::string var2 = mulExp->PrintIR(tab, buffer);
            std::string now = NewTempSymbol();
            buffer += tab + now + " = sub " + var1 + ", " + var2 + "\n";
            return now;
        } else {
            std::cerr << "AddExpAST::PrintIR: unknown kind" << std::endl;
        }
        return "";
    }
};

// RelExpAST: 关系表达式，包括小于、大于、小于等于、大于等于
// RelExp ::= AddExp | RelExp '<' AddExp | RelExp '>' AddExp | RelExp '<=' AddExp | RelExp '>=' AddExp
class RelExpAST : public BaseAST {
public:
    enum Kind {
        kAddExp,
        kLT,
        kGT,
        kLE,
        kGE,
    };

    Kind kind;

    std::unique_ptr<BaseAST> addExp;
    std::unique_ptr<BaseAST> relExp;

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "RelExpAST {\n";
        switch (kind) { // XXX 把switch改成if else，switch中不能定义变量
        case kAddExp:
            ans += tab + "\taddExp: " + addExp->PrintAST(tab + "\t");
            break;
        case kLT:
            ans += tab + "\trelExp: " + relExp->PrintAST(tab + "\t");
            ans += tab + "\tkind: <\n";
            ans += tab + "\taddExp: " + addExp->PrintAST(tab + "\t");
            break;
        case kGT:
            ans += tab + "\trelExp: " + relExp->PrintAST(tab + "\t");
            ans += tab + "\tkind: >\n";
            ans += tab + "\taddExp: " + addExp->PrintAST(tab + "\t");
            break;
        case kLE:
            ans += tab + "\trelExp: " + relExp->PrintAST(tab + "\t");
            ans += tab + "\tkind: <=\n";
            ans += tab + "\taddExp: " + addExp->PrintAST(tab + "\t");
            break;
        case kGE:
            ans += tab + "\trelExp: " + relExp->PrintAST(tab + "\t");
            ans += tab + "\tkind: >=\n";
            ans += tab + "\taddExp: " + addExp->PrintAST(tab + "\t");
            break;
        default:
            std::cerr << "RelExpAST::PrintAST: unknown kind" << std::endl;
            break;
        }
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab, std::string &buffer) const override {
        if (kind == kAddExp) {
            std::string var = addExp->PrintIR(tab, buffer);
            return var;
        } else
        if (kind == kLT) {
            std::string var1 = relExp->PrintIR(tab, buffer);
            std::string var2 = addExp->PrintIR(tab, buffer);
            std::string now = NewTempSymbol();
            buffer += tab + now + " = lt " + var1 + ", " + var2 + "\n";
            return now;
        } else
        if (kind == kGT) {
            std::string var1 = relExp->PrintIR(tab, buffer);
            std::string var2 = addExp->PrintIR(tab, buffer);
            std::string now = NewTempSymbol();
            buffer += tab + now + " = gt " + var1 + ", " + var2 + "\n";
            return now;
        } else
        if (kind == kLE) {
            std::string var1 = relExp->PrintIR(tab, buffer);
            std::string var2 = addExp->PrintIR(tab, buffer);
            std::string now = NewTempSymbol();
            buffer += tab + now + " = le " + var1 + ", " + var2 + "\n";
            return now;
        } else
        if (kind == kGE) {
            std::string var1 = relExp->PrintIR(tab, buffer);
            std::string var2 = addExp->PrintIR(tab, buffer);
            std::string now = NewTempSymbol();
            buffer += tab + now + " = ge " + var1 + ", " + var2 + "\n";
            return now;
        } else {
            std::cerr << "RelExpAST::PrintIR: unknown kind" << std::endl;
        }
        return "";
    }
};

// EqExpAST: 相等表达式，包括等于、不等于
// EqExp ::= RelExp | EqExp '==' RelExp | EqExp '!=' RelExp
class EqExpAST : public BaseAST {
public:
    enum Kind {
        kRelExp,
        kEQ,
        kNE,
    };
    
    Kind kind;

    std::unique_ptr<BaseAST> relExp;
    std::unique_ptr<BaseAST> eqExp;

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "EqExpAST {\n";
        switch (kind) { // XXX 把switch改成if else，switch中不能定义变量
        case kRelExp:
            ans += tab + "\trelExp: " + relExp->PrintAST(tab + "\t");
            break;
        case kEQ:
            ans += tab + "\teqExp: " + eqExp->PrintAST(tab + "\t");
            ans += tab + "\tkind: ==\n";
            ans += tab + "\trelExp: " + relExp->PrintAST(tab + "\t");
            break;
        case kNE:
            ans += tab + "\teqExp: " + eqExp->PrintAST(tab + "\t");
            ans += tab + "\tkind: !=\n";
            ans += tab + "\trelExp: " + relExp->PrintAST(tab + "\t");
            break;
        default:
            std::cerr << "EqExpAST::PrintAST: unknown kind" << std::endl;
            break;
        }
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab, std::string &buffer) const override {
        if (kind == kRelExp) {
            std::string var = relExp->PrintIR(tab, buffer);
            return var;
        } else
        if (kind == kEQ) {
            std::string var1 = eqExp->PrintIR(tab, buffer);
            std::string var2 = relExp->PrintIR(tab, buffer);
            std::string now = NewTempSymbol();
            buffer += tab + now + " = eq " + var1 + ", " + var2 + "\n";
            return now;
        } else
        if (kind == kNE) {
            std::string var1 = eqExp->PrintIR(tab, buffer);
            std::string var2 = relExp->PrintIR(tab, buffer);
            std::string now = NewTempSymbol();
            buffer += tab + now + " = ne " + var1 + ", " + var2 + "\n";
            return now;
        } else {
            std::cerr << "EqExpAST::PrintIR: unknown kind" << std::endl;
        }
        return "";
    }
};

// LANDExpAST: 逻辑与表达式
// LAndExp ::= EqExp | LAndExp '&&' EqExp
class LAndExpAST : public BaseAST {
public:
    enum Kind {
        kEqExp,
        kAnd,
    };

    Kind kind;

    std::unique_ptr<BaseAST> eqExp;
    std::unique_ptr<BaseAST> lAndExp;

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "LAndExpAST {\n";
        switch (kind) { // XXX 把switch改成if else，switch中不能定义变量
        case kEqExp:
            ans += tab + "\teqExp: " + eqExp->PrintAST(tab + "\t");
            break;
        case kAnd:
            ans += tab + "\tlAndExp: " + lAndExp->PrintAST(tab + "\t");
            ans += tab + "\tkind: &&\n";
            ans += tab + "\teqExp: " + eqExp->PrintAST(tab + "\t");
            break;
        default:
            std::cerr << "LAndExpAST::PrintAST: unknown kind" << std::endl;
            break;
        }
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab, std::string &buffer) const override {
        if (kind == kEqExp) {
            std::string var = eqExp->PrintIR(tab, buffer);
            return var;
        } else
        if (kind == kAnd) {
            std::string var1 = lAndExp->PrintIR(tab, buffer);
            std::string var2 = eqExp->PrintIR(tab, buffer);
            // 用其他运算实现逻辑与
            // now1 = ne 0, var1
            // now2 = ne 0, var2
            // now3 = and now1, now2
            std::string now1 = NewTempSymbol();
            std::string now2 = NewTempSymbol();
            std::string now3 = NewTempSymbol();
            buffer += tab + now1 + " = ne 0, " + var1 + "\n";
            buffer += tab + now2 + " = ne 0, " + var2 + "\n";
            buffer += tab + now3 + " = and " + now1 + ", " + now2 + "\n";
            return now3;
        } else {
            std::cerr << "LAndExpAST::PrintIR: unknown kind" << std::endl;
        }
        return "";
    }
};

// LORExpAST: 逻辑或表达式
// LOrExp ::= LAndExp | LOrExp '||' LAndExp
class LOrExpAST : public BaseAST {
public:
    enum Kind {
        kLAndExp,
        kOr,
    };

    Kind kind;

    std::unique_ptr<BaseAST> lAndExp;
    std::unique_ptr<BaseAST> lOrExp;

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "LOrExpAST {\n";
        switch (kind) { // XXX 把switch改成if else，switch中不能定义变量
        case kLAndExp:
            ans += tab + "\tlAndExp: " + lAndExp->PrintAST(tab + "\t");
            break;
        case kOr:
            ans += tab + "\tlOrExp: " + lOrExp->PrintAST(tab + "\t");
            ans += tab + "\tkind: ||\n";
            ans += tab + "\tlAndExp: " + lAndExp->PrintAST(tab + "\t");
            break;
        default:
            std::cerr << "LOrExpAST::PrintAST: unknown kind" << std::endl;
            break;
        }
        ans += tab + "}\n";
        return ans;
    }
        
    std::string PrintIR(std::string tab, std::string &buffer) const override {
        if (kind == kLAndExp) {
            std::string var = lAndExp->PrintIR(tab, buffer);
            return var;
        } else
        if (kind == kOr) {
            std::string var1 = lOrExp->PrintIR(tab, buffer);
            std::string var2 = lAndExp->PrintIR(tab, buffer);
            // 用其他运算实现逻辑或
            // now1 = or var1, var2
            // now2 = ne 0, now1
            std::string now1 = NewTempSymbol();
            std::string now2 = NewTempSymbol();
            buffer += tab + now1 + " = or " + var1 + ", " + var2 + "\n";
            buffer += tab + now2 + " = ne 0, " + now1 + "\n";
            return now2;
        } else {
            std::cerr << "LOrExpAST::PrintIR: unknown kind" << std::endl;
        }
        return "";
    }
};