#pragma once
#include <iostream>
// 所有 AST 的基类
class BaseAST {
public:
    virtual ~BaseAST() = default;
    virtual std::string PrintAST(std::string tab) const = 0;
    virtual std::string PrintIR(std::string tab) const = 0;
};

// CompUnit 是 BaseAST
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

    std::string PrintIR(std::string tab) const override{
        return func_def->PrintIR(tab);
    }
};

// FuncDef 也是 BaseAST
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

    std::string PrintIR(std::string tab) const override{
        std::string ans = "";
        // 函数的定义
        ans += tab + "fun @" + ident + "():";
        ans += func_type->PrintIR(tab);
        ans += tab + "{\n";

        // 函数的代码块
        ans += block->PrintIR(tab);

        // 函数结尾
        ans += tab + "}\n";
        return ans;
    }
};

// FuncType 也是 BaseAST
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

    std::string PrintIR(std::string tab) const override{
        if(type == "int") {
            return tab + " i32 ";
        } else{
            return tab + " error-type ";
        }
    }
};

// Block 也是 BaseAST
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

    std::string PrintIR(std::string tab) const override{
        std::string ans = "";
        ans += tab + "\%entry:\n";
        ans += stmt->PrintIR(tab + "\t");
        return ans;
    }
};

// Stmt 也是 BaseAST
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

    std::string PrintIR(std::string tab) const override{
        return tab + "ret " + exp->PrintIR(tab + "\t");
    }
};

// Exp 也是 BaseAST
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

    std::string PrintIR(std::string tab) const override{
        return tab + unaryExp->PrintIR(tab + "\t");
    }
};

// PrimaryExpAST 也是 BaseAST
class PrimaryExpAST : public BaseAST {
public:
    int kind;
    std::unique_ptr<BaseAST> exp;
    int number;

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "PrimaryExpAST {\n";
        if(kind == 1) ans += tab + "\texp: " + exp->PrintAST(tab + "\t");
        else ans += tab + "\tnumber: " + std::to_string(number);
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab) const override{
        if(kind == 1) return tab + exp->PrintIR(tab + '\t');
        else return tab + std::to_string(number) + "\n";
    }
};

// UnaryExpAST 也是 BaseAST
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
            ans += tab + "\t + number: " + unaryExp->PrintAST(tab + "\t");
        else if(kind == 3) 
            ans += tab + "\t - number: " + unaryExp->PrintAST(tab + "\t");
        else if(kind == 4) 
            ans += tab + "\t ! number: " + unaryExp->PrintAST(tab + "\t");
        
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab) const override{
        if(kind == 1) return tab + primaryExp->PrintIR(tab + '\t');
        else return tab + unaryExp->PrintIR(tab + '\t');
    }
};