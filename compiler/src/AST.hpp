#pragma once
#include <iostream>
// 所有 AST 的基类
class BaseAST {
public:
    virtual ~BaseAST() = default;
    virtual std::string PrintAST(std::string tab) const = 0;
    // 输出到buffer中, 返回值为需要的信息
    virtual std::string PrintIR(std::string tab, std::string &buffer) const = 0;
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
// Exp ::= UnaryExp;
/**
*   翻译为: 
*   return [unaryExp]
*/
class ExpAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> unaryExp;

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "ExpAST {\n";
        ans += tab + "\tunaryExp: " + unaryExp->PrintAST(tab + "\t");
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab, std::string &buffer) const override{
        std::string var = unaryExp->PrintIR(tab, buffer);
        return var;
    }
};

// PrimaryExpAST: 表达式中优先计算的部分, 即被'()'包裹的表达式/单个数字
// PrimaryExp ::= "(" Exp ")" | Number;
/**
*   翻译为: 
*   1. return [Exp]
*   2. return Number
*/
class PrimaryExpAST : public BaseAST {
public:
    int kind;
    std::unique_ptr<BaseAST> exp;
    int number;

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "PrimaryExpAST {\n";
        if(kind == 1) ans += tab + "\texp: " + exp->PrintAST(tab + "\t");
        else ans += tab + "\tnumber: " + std::to_string(number) + "\n";
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab, std::string &buffer) const override{
        if(kind == 1) {
            std::string var = exp->PrintIR(tab, buffer);
            return var;
        }
        else{
            return std::to_string(number);
        }
    }
};

// UnaryExpAST: 单目运算表达式
// UnaryExp ::= PrimaryExp | '+' UnaryExp | '-' UnaryExp | '!' UnaryExp
/**
*   翻译为: 
*   1. return [PrimaryExp]
*   2. return [UnaryExp]
*   3. 
*   4. 
*/
class UnaryExpAST : public BaseAST {
public:
    // 1: PrimaryExp
    // 2: + unaryExp
    // 3: - unaryExp
    // 4: ! unaryExp 
    int kind;
    std::unique_ptr<BaseAST> primaryExp;
    std::unique_ptr<BaseAST> unaryExp;

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "UnaryExpAST {\n";
        if(kind == 1) 
            ans += tab + "\texp: " + primaryExp->PrintAST(tab + "\t");
        else if(kind == 2) 
            ans += tab + "\t + unaryExp: " + unaryExp->PrintAST(tab + "\t");
        else if(kind == 3) 
            ans += tab + "\t - unaryExp: " + unaryExp->PrintAST(tab + "\t");
        else if(kind == 4) 
            ans += tab + "\t ! unaryExp: " + unaryExp->PrintAST(tab + "\t");
        
        ans += tab + "}\n";
        return ans;
    }
    int GetNowVar(std::string var) const{
        static int count_var = 0;
        if(var[0] == '%') count_var++;
        return count_var;
    }
    std::string PrintIR(std::string tab, std::string &buffer) const override{
        if(kind == 1) {
            std::string var = primaryExp->PrintIR(tab, buffer);
            return var;
        }
        // +, 不产生IR
        else if(kind == 2){
            std::string var = unaryExp->PrintIR(tab, buffer);
            return var;
        }
        // -, IR格式为: %now = sub 0, var
        else if(kind == 3){
            std::string var = unaryExp->PrintIR(tab, buffer);
            int now = GetNowVar(var);
            buffer += tab + "%" + std::to_string(now) + " = sub 0, " + var + "\n";
            return "%" + std::to_string(now); 
        }
        // !, IR格式为: %now = eq 0, var
        else{
            std::string var = unaryExp->PrintIR(tab, buffer);
            int now = GetNowVar(var);
            buffer += tab + "%" + std::to_string(now) + " = eq 0, " + var + "\n";
            return "%" + std::to_string(now); 
        }
    }
};