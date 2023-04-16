#pragma once
#include <iostream>
// 所有 AST 的基类
class BaseAST {
public:
    virtual ~BaseAST() = default;
    virtual void PrintAST(std::string tab) const = 0;
    virtual void PrintIR(std::string tab) const = 0;
};

// CompUnit 是 BaseAST
class CompUnitAST : public BaseAST {
public:
    // 用智能指针管理对象
    std::unique_ptr<BaseAST> func_def;

    void PrintAST(std::string tab) const override {
        std::cout << "CompUnitAST {\n";
        std::cout << tab << "\tfunc_def: "; func_def->PrintAST(tab + "\t");
        std::cout << tab << "}\n";
    }

    void PrintIR(std::string tab) const override{
        func_def->PrintIR(tab);
    }
};

// FuncDef 也是 BaseAST
class FuncDefAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> func_type;
    std::string ident;
    std::unique_ptr<BaseAST> block;

    void PrintAST(std::string tab) const override {
        std::cout << "FuncDefAST {\n";
        std::cout << tab << "\tfunc_type: "; func_type->PrintAST(tab + "\t");
        std::cout << tab << "\tident: " << ident << "\n";
        std::cout << tab << "\tblock: "; block->PrintAST(tab + "\t");
        std::cout << tab << "}\n";
    }

    void PrintIR(std::string tab) const override{
        // 函数的定义
        std::cout << tab << "fun @" << ident << "():";
        func_type->PrintIR(tab);
        std::cout << tab << "{\n";

        // 函数的代码块
        block->PrintIR(tab);

        // 函数结尾
        std::cout << tab << "}\n";
    }
};

// FuncType 也是 BaseAST
class FuncTypeAST : public BaseAST {
public:
    std::string type;

    void PrintAST(std::string tab) const override {
        std::cout << "FuncTypeAST {\n";
        std::cout << tab << "\ttype: " << type << "\n";
        std::cout << tab << "}\n";
    }

    void PrintIR(std::string tab) const override{
        if(type == "int") {
            std::cout << tab << " i32 ";
        } else{
            std::cout << tab << " error-type ";
        }
    }
};

// Block 也是 BaseAST
class BlockAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> stmt;

    void PrintAST(std::string tab) const override {
        std::cout << "BlockAST {\n";
        std::cout << tab << "\tstmt: "; stmt->PrintAST(tab + "\t");
        std::cout << tab << "}\n";
    }

    void PrintIR(std::string tab) const override{
        std::cout << tab << "\%entry:\n";
        stmt->PrintIR(tab + "\t");
    }
};

// Stmt 也是 BaseAST
class StmtAST : public BaseAST {
public:
    int number;

    void PrintAST(std::string tab) const override {
        std::cout << "StmtAST {\n";
        std::cout << tab << "\tnumber: " << number << "\n";
        std::cout << tab << "}\n";
    }

    void PrintIR(std::string tab) const override{
        std::cout << tab << "ret " << number << "\n";
    }
};