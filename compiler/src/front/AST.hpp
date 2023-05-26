#pragma once
#include <iostream>
#include <map>
#include <vector>

/**************** 符号表 ****************/

class SymbolTable {
public:
    struct Symbol {
        enum Type { kConst, kVar, kFunc } type;
        union { // XXX 每种类型的符号要存储的信息不同，且一个简单变量可能存不下，建议改为 struct
            int const_val;
            int var_val;
            int func_val; // 0 表示 void，1 表示 int
        } val;
    };

    SymbolTable() = default;
    ~SymbolTable() = default;

    void Clear() {
        table.clear();
    }

    // 插入常量符号定义
    void AddConstSymbol(std::string name, int const_val) {
        if (table.find(name) != table.end()) {
            std::cerr << "SymbolTable::AddSymbol: symbol " << name << " already exists" << std::endl;
            return;
        }
        Symbol symbol;
        symbol.type = Symbol::kConst;
        symbol.val.const_val = const_val;
        table[name] = symbol;
    }

    // 插入变量符号定义
    void AddVarSymbol(std::string name, int var_val) {
        if (table.find(name) != table.end()) {
            std::cerr << "SymbolTable::AddSymbol: symbol " << name << " already exists" << std::endl;
            return;
        }
        Symbol symbol;
        symbol.type = Symbol::kVar;
        symbol.val.var_val = var_val;
        table[name] = symbol;
    }

    // 插入函数符号定义
    void AddFuncSymbol(std::string name, int func_val) {
        if (table.find(name) != table.end()) {
            std::cerr << "SymbolTable::AddSymbol: symbol " << name << " already exists" << std::endl;
            return;
        }
        Symbol symbol;
        symbol.type = Symbol::kFunc;
        symbol.val.func_val = func_val;
        table[name] = symbol;
    }

    // 确认符号定义是否存在
    bool HasSymbol(std::string name) {
        return table.find(name) != table.end();
    }

    Symbol::Type GetSymbolType(std::string name) {
        if (table.find(name) == table.end()) {
            std::cerr << "SymbolTable::GetSymbolType: symbol " << name << " not found" << std::endl;
            return Symbol::kVar; // XXX 没有意义，只是为了不报错
        }
        return table[name].type;
    }

    int GetConstSymbolValue(std::string name) {
        if (table.find(name) == table.end()) {
            std::cerr << "SymbolTable::GetSymbolType: symbol " << name << " not found" << std::endl;
            return 0;
        }
        if (table[name].type != Symbol::kConst) {
            std::cerr << "SymbolTable::GetSymbolType: symbol " << name << " is not a const" << std::endl;
            return 0;
        }
        return table[name].val.const_val;
    }

    int GetVarSymbolValue(std::string name) {
        if (table.find(name) == table.end()) {
            std::cerr << "SymbolTable::GetSymbolType: symbol " << name << " not found" << std::endl;
            return 0;
        }
        if (table[name].type != Symbol::kVar) {
            std::cerr << "SymbolTable::GetSymbolType: symbol " << name << " is not a var" << std::endl;
            return 0;
        }
        return table[name].val.var_val;
    }

    int GetFuncSymbolValue(std::string name) {
        if (table.find(name) == table.end()) {
            std::cerr << "SymbolTable::GetSymbolType: symbol " << name << " not found" << std::endl;
            return 0;
        }
        if (table[name].type != Symbol::kFunc) {
            std::cerr << "SymbolTable::GetSymbolType: symbol " << name << " is not a func" << std::endl;
            return 0;
        }
        return table[name].val.func_val;
    }
private:
    std::map<std::string, Symbol> table;
};

class NestedSymbolTable {
public:
    NestedSymbolTable() = default;
    ~NestedSymbolTable() = default;

    void AddTable() {
        tables.emplace_back();
    }
    void DropTable() {
        if (tables.size() == 0) {
            std::cerr << "NestedSymbolTable::DropTable: no table to drop" << std::endl;
            return;
        }
        tables.pop_back();
    }

    void AddConstSymbol(std::string name, int const_val) {
        if (tables.size() == 0) {
            std::cerr << "NestedSymbolTable::AddConstSymbol: no table to add" << std::endl;
            return;
        }
        if (tables.back().HasSymbol(name)) { // 当前作用域不允许重复定义
            std::cerr << "NestedSymbolTable::AddConstSymbol: symbol " << name << " already exists" << std::endl;
            return;
        }
        tables.back().AddConstSymbol(name, const_val);
    }

    void AddVarSymbol(std::string name) {
        if (tables.size() == 0) {
            std::cerr << "NestedSymbolTable::AddVarSymbol: no table to add" << std::endl;
            return;
        }
        if (tables.back().HasSymbol(name)) { // 当前作用域不允许重复定义
            std::cerr << "NestedSymbolTable::AddVarSymbol: symbol " << name << " already exists" << std::endl;
            return;
        }
        tables.back().AddVarSymbol(name, varSymbolCount[name] + 1); // 从 1 开始，把 0（没有标号）留给全局变量
        varSymbolCount[name]++;
    }

    void AddFParamSymbol(std::string name) {
        if (fParamTable.HasSymbol(name)) { // 当前作用域不允许重复定义
            std::cerr << "NestedSymbolTable::AddFParamSymbol: symbol " << name << " already exists" << std::endl;
            return;
        }
        fParamTable.AddVarSymbol(name, -1); // var_val 为 -1，表示是函数参数
    }

    // 清空函数参数符号表，在结束函数定义时调用
    void ClearFParamTable() {
        fParamTable.Clear();
    }

    bool HasSymbol(std::string name) {
        if (fParamTable.HasSymbol(name)) {
            return true;
        }
        for (auto it = tables.rbegin(); it != tables.rend(); it++) {
            if (it->HasSymbol(name)) {
                return true;
            }
        }
        return false;
    }

    SymbolTable::Symbol::Type GetSymbolType(std::string name) {
        if (fParamTable.HasSymbol(name)) {
            return fParamTable.GetSymbolType(name);
        }
        for (auto it = tables.rbegin(); it != tables.rend(); it++) {
            if (it->HasSymbol(name)) {
                return it->GetSymbolType(name);
            }
        }
        std::cerr << "NestedSymbolTable::GetSymbolType: symbol " << name << " not found" << std::endl;
        return SymbolTable::Symbol::kVar; // XXX 没有意义（因为此时说明不存在该 Symbol），只是为了不报错
    }

    // 获取常量符号的值，需要提前保证符号存在且类型为常量
    int GetConstSymbolValue(std::string name) {
        for (auto it = tables.rbegin(); it != tables.rend(); it++) {
            if (it->HasSymbol(name)) {
                return it->GetConstSymbolValue(name);
            }
        }
        std::cerr << "NestedSymbolTable::GetConstSymbolValue: symbol " << name << " not found" << std::endl;
        return 0;
    }

    // 获取变量符号的值，需要提前保证符号存在且类型为变量
    int GetVarSymbolValue(std::string name) {
        if (fParamTable.HasSymbol(name)) {
            return fParamTable.GetVarSymbolValue(name);
        }
        for (auto it = tables.rbegin(); it != tables.rend(); it++) {
            if (it->HasSymbol(name)) {
                return it->GetVarSymbolValue(name);
            }
        }
        std::cerr << "NestedSymbolTable::GetVarSymbolValue: symbol " << name << " not found" << std::endl;
        return 0;
    }
private:
    std::vector<SymbolTable> tables;
    // 函数形参符号表，用于特殊处理函数形参
    SymbolTable fParamTable;
    // 变量符号的计数器，用于生成唯一的变量名
    // 第一个变量名应该生成为 %ident_0，第二个应该为 %ident_1，以此类推
    std::map<std::string, int> varSymbolCount;
};

// extern NestedSymbolTable symbolTable;

/**************** AST ****************/

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

class BaseExpAST : public BaseAST {
public:
    virtual int CalcConstExp() const = 0;
};

// CompUnit: 起始字符, 表示整个文件
// CompUnit ::= GlobalDefs
class CompUnitAST : public BaseAST {
public:
    // 用智能指针管理对象
    std::unique_ptr<std::vector<std::unique_ptr<BaseAST> > > globalDefs;

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += tab + "CompUnitAST {\n";
        for (auto &globalDef : *globalDefs) {
            ans += tab + "\t" + globalDef->PrintAST(tab + "\t");
        }
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab, std::string &buffer) const override{
        for (auto &globalDef : *globalDefs) {
            globalDef->PrintIR(tab, buffer);
        }
        return "";
    }
};

class GlobalDefAST : public BaseAST {
public:
    enum Kind {
        kFuncDef,
        kDecl
    };
    
    Kind kind;

    std::unique_ptr<BaseAST> funcDef;
    std::unique_ptr<BaseAST> decl;

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += tab + "GlobalDefAST {\n";
        if (kind == kFuncDef) {
            ans += tab + "\tfuncDef: " + funcDef->PrintAST(tab + "\t");
        } else
        if (kind == kDecl) {
            ans += tab + "\tdecl: " + decl->PrintAST(tab + "\t");
        } else {
            std::cerr << "GlobalDefAST::PrintAST: unknown kind" << std::endl;
        }
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab, std::string &buffer) const override{
        if (kind == kFuncDef) {
            funcDef->PrintIR(tab, buffer);
        } else
        if (kind == kDecl) {
            decl->PrintIR(tab, buffer);
        } else {
            std::cerr << "GlobalDefAST::PrintIR: unknown kind" << std::endl;
        }
        return "";
    }
};

// FuncDef: 函数定义
// FuncDef ::= FuncType IDENT '(' [FuncFParams] ')' Block;
/**
*   翻译为:
*   fun @IDENT (): [FuncType]{
*       [Block]
*   }
*/
class FuncDefAST : public BaseAST {
public:
    int funcType; // 0 表示 void, 1 表示 int
    std::string ident;
    // 函数参数列表
    // XXX 没有在符号表中存参数列表，语义分析没有检查函数调用时参数是否匹配
    std::unique_ptr<std::vector<std::unique_ptr<BaseAST> > > funcFParams;
    std::unique_ptr<BaseAST> block;

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "FuncDefAST {\n";
        ans += tab + "\tfuncType: " + std::to_string(funcType) + "\n";
        ans += tab + "\tident: " + ident + "\n";
        ans += tab + "\tfuncFParams: {";
        if (funcFParams == nullptr) {
            ans += "null";
        } else {
            for (auto &funcFParam : *funcFParams) {
                ans += tab + "\t\t" + funcFParam->PrintAST(tab + "\t\t");
            }
        }
        ans += tab + "\t}\n";
        ans += tab + "\tblock: " + block->PrintAST(tab + "\t");
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab, std::string &buffer) const override{
        std::string funcEntryLabel = "\%entry_" + ident;
        std::vector<std::string> funcFParamNames; // 存储 funcFParam 的 ident
        
        // 函数的头部（函数名）
        buffer += tab + "fun @" + ident + "(";

        // 函数的参数
        if (funcFParams == nullptr) {
            buffer += "";
        } else {
            for (auto &funcFParam : *funcFParams) {
                funcFParamNames.push_back(funcFParam->PrintIR(tab, buffer));
                if (funcFParam != funcFParams->back()) {
                    buffer += ", ";
                }
            }
        }
        
        // 函数的类型
        buffer += ")";
        if (funcType == 0) {
            buffer += "";
        } else
        if (funcType == 1) {
            buffer += ": i32";
        } else {
            std::cerr << "FuncDefAST::PrintIR: unknown funcType" << std::endl;
        }
        buffer += " {\n";

        // 函数的入口
        buffer += funcEntryLabel + ":\n";

        // 特殊处理形参
        if (funcType == 1) {
            for (auto &funcFParamName : funcFParamNames) {
                buffer += tab + "\t\%fParam_" + funcFParamName + " = alloc i32\n";
                buffer += tab + "\tstore @" + funcFParamName + ", \%fParam_" + funcFParamName + "\n";
            }
        }

        // 函数的代码块
        std::string blockRet = block->PrintIR(tab + '\t', buffer);
        // void 函数没有 return 语句，需要手动加上 ret
        // int 函数如果没有 return 语句，不会自动加 ret 0
        if (funcType == 0 && blockRet != "return") {
            buffer += tab + "\tret\n";
        }

        // 函数结尾
        buffer += tab + "}\n";
        return "";
    }
};

// FuncFParam: 函数参数
// FuncFParam ::= Btype IDENT
class FuncFParamAST : public BaseAST {
public:
    int bType; // XXX bType 用数字实现，不太好，应该改成枚举
    std::string ident;

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "FuncFParamAST {\n";
        ans += tab + "\tbType: " + std::to_string(bType) + "\n";
        ans += tab + "\tident: " + ident + "\n";
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab, std::string &buffer) const override{
        buffer += "@" + ident + ": " + "i32"; // XXX bType 目前只有 int
        return ident;
    }
};

// Block: 函数的结构体
// Block :== '{' {BlockItem} '}'
class BlockAST : public BaseAST {
public:
    std::unique_ptr<std::vector<std::unique_ptr<BaseAST> > > blockItems;

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "BlockAST {\n";
        ans += tab + "\tblockItems: [\n";
        if (blockItems == nullptr) {
            ans += tab + "\t\tNULL\n";
        } else {
            for (auto &blockItem : *blockItems) {
                ans += tab + "\t\t" + blockItem->PrintAST(tab + "\t\t");
            }
        }
        ans += tab + "\t]\n";
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab, std::string &buffer) const override{
        if (blockItems == nullptr) {
            return "";
        }
        std::string ret;
        for (auto &blockItem : *blockItems) {
            ret = blockItem->PrintIR(tab, buffer);
            // 如果是 return、break、continue 语句，提前返回
            if (ret == "return" || ret == "break" || ret == "continue") {
                break;
            }
        }
        return ret; // 返回最后一个语句的返回值，可能是 return、break、continue
    }
};

class BlockItemAST : public BaseAST {
public:
    enum Kind {
        kDecl,
        kStmt
    };

    Kind kind;

    std::unique_ptr<BaseAST> decl;
    std::unique_ptr<BaseAST> stmt;

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "BlockItemAST {\n";
        switch (kind) { // XXX 把switch改成if else，switch中不能定义变量
        case kDecl:
            ans += tab + "\tdecl: " + decl->PrintAST(tab + "\t");
            break;
        case kStmt:
            ans += tab + "\tstmt: " + stmt->PrintAST(tab + "\t");
            break;
        default:
            std::cerr << "BlockItemAST::PrintAST: unknown kind" << std::endl;
            break;
        }
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab, std::string &buffer) const override{
        if (kind == kDecl) {
            return decl->PrintIR(tab, buffer);
        } else
        if (kind == kStmt) {
            return stmt->PrintIR(tab, buffer); // 可能会返回 "return"
        } else {
            std::cerr << "BlockItemAST::PrintIR: unknown kind" << std::endl;
        }
        return "";
    }
};

class DeclAST : public BaseAST {
public:
    enum Kind {
        kConstDecl,
        kVarDecl
    };
    
    Kind kind;
    
    std::unique_ptr<BaseAST> constDecl;
    std::unique_ptr<BaseAST> varDecl;

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "DeclAST {\n";
        switch (kind) { // XXX 把switch改成if else，switch中不能定义变量
        case kConstDecl:
            ans += tab + "\tconstDecl: " + constDecl->PrintAST(tab + "\t");
            break;
        case kVarDecl:
            ans += tab + "\tvarDecl: " + varDecl->PrintAST(tab + "\t");
            break;
        default:
            std::cerr << "DeclAST::PrintAST: unknown kind" << std::endl;
            break;
        }
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab, std::string &buffer) const override{
        if (kind == kConstDecl) {
            // 常量的定义，不需要产生IR
        } else
        if (kind == kVarDecl) {
            varDecl->PrintIR(tab, buffer);
        } else {
            std::cerr << "DeclAST::PrintIR: unknown kind" << std::endl;
        }
        
        return "";
    }
};

class ConstDeclAST : public BaseAST {
public:
    /* enum BType {
        kInt,
    }; */

    int bType;
    std::unique_ptr<std::vector<std::unique_ptr<BaseAST> > > constDefs; // XXX 也许不需要存储指针

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "ConstDeclAST {\n";
        switch (bType) { // XXX 把switch改成if else，switch中不能定义变量
        case 0:
            ans += tab + "\tbType: int\n";
            break;
        default:
            std::cerr << "ConstDeclAST::PrintAST: unknown bType" << std::endl;
            break;
        }
        ans += tab + "\tconstDefs: [\n";
        for (auto &constDef : *constDefs) {
            ans += tab + "\t\t" + constDef->PrintAST(tab + "\t\t");
        }
        ans += tab + "\t]\n";
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab, std::string &buffer) const override{
        // 常量的定义，不需要产生IR
        return "";
    }
};

class ConstDefAST : public BaseAST {
public:
    std::string ident;
    std::unique_ptr<BaseExpAST> constInitVal;

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "ConstDefAST {\n";
        ans += tab + "\tident: " + ident + "\n";
        ans += tab + "\tconstInitVal: " + constInitVal->PrintAST(tab + "\t");
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab, std::string &buffer) const override{
        // 常量的定义，不需要产生IR
        return "";
    }
};

class ConstInitValAST : public BaseExpAST {
public:
    std::unique_ptr<BaseExpAST> constExp;

    int CalcConstExp() const override {
        return constExp->CalcConstExp();
    }

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "ConstInitValAST {\n";
        ans += tab + "\tconstExp: " + constExp->PrintAST(tab + "\t");
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab, std::string &buffer) const override{
        std::string var = constExp->PrintIR(tab, buffer);
        return var;
    }
};

class VarDeclAST : public BaseAST {
public:
    int bType;
    std::unique_ptr<std::vector<std::unique_ptr<BaseAST> > > varDefs;

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "VarDeclAST {\n";
        switch (bType) { // XXX 把switch改成if else，switch中不能定义变量
        case 0:
            ans += tab + "\tbType: int\n";
            break;
        default:
            std::cerr << "VarDeclAST::PrintAST: unknown bType" << std::endl;
            break;
        }
        ans += tab + "\tvarDefs: [\n";
        for (auto &varDef : *varDefs) {
            ans += tab + "\t\t" + varDef->PrintAST(tab + "\t\t");
        }
        ans += tab + "\t]\n";
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab, std::string &buffer) const override{
        for (auto &varDef : *varDefs) {
            varDef->PrintIR(tab, buffer);
        }
        return "";
    }
};

class VarDefAST : public BaseAST {
public:
    enum Kind {
        kUnInit,
        kArray, // 只实现了没有手动初始化的数组
        kInit
    };

    Kind kind;

    bool isGlobal;
    std::string ident;
    std::unique_ptr<std::vector<std::unique_ptr<BaseExpAST> > > constArrayDims; 
    std::unique_ptr<BaseExpAST> initVal;

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "VarDefAST {\n";
        ans += tab + "\tident: " + ident + "\n";
        switch (kind) { // XXX 把switch改成if else，switch中不能定义变量
        case kUnInit:
            ans += tab + "\tkind: unInit\n";
            break;
        case kArray:
            ans += tab + "\tkind: array\n";
            ans += tab + "\tconstArrayDims: [\n";
            for (auto &constArrayDim : *constArrayDims) {
                ans += tab + "\t\t" + constArrayDim->PrintAST(tab + "\t\t");
            }
            ans += tab + "\t]\n";
            break;
        case kInit:
            ans += tab + "\tkind: init\n";
            ans += tab + "\tinitVal: " + initVal->PrintAST(tab + "\t");
            break;
        default:
            std::cerr << "VarDefAST::PrintAST: unknown kind" << std::endl;
            break;
        }
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab, std::string &buffer) const override{
        if (kind == kUnInit) {
            if (isGlobal) {
                buffer += tab + "global @" + ident + " = alloc i32, zeroinit\n";
            } else {
                buffer += tab + "@" + ident + " = alloc i32\n";
            }
        } else
        if (kind == kArray) {
            if (isGlobal) {
                buffer += tab + "global @" + ident + " = alloc ";
                // buffer 加上 constArrayDims 个数个 '['
                buffer.append(constArrayDims->size(), '[');
                buffer += "i32";
                for (auto it = constArrayDims->rbegin(); it != constArrayDims->rend(); it++) {
                    buffer += ", " + (*it)->PrintIR(tab, buffer) + "]";
                }
                buffer += ", zeroinit\n";
            } else {
                buffer += tab + "@" + ident + " = alloc ";
                // buffer 加上 constArrayDims 个数个 '['
                buffer.append(constArrayDims->size(), '[');
                buffer += "i32";
                for (auto it = constArrayDims->rbegin(); it != constArrayDims->rend(); it++) {
                    buffer += ", " + (*it)->PrintIR(tab, buffer) + "]";
                }
                buffer += "\n"; // 局部变量不初始化
            }
        } else
        if (kind == kInit) {
            if (isGlobal) {
                // XXX 没有检查initVal是否是常量
                std::string var = initVal->PrintIR(tab, buffer);
                buffer += tab + "global @" + ident + " = alloc i32, " + var + "\n";
            } else {
                buffer += tab + "@" + ident + " = alloc i32\n";
                std::string var = initVal->PrintIR(tab, buffer);
                buffer += tab + "store " + var + ", @" + ident + "\n";
            }
        } else {
            std::cerr << "VarDefAST::PrintIR: unknown kind" << std::endl;
        }
        return "";
    }
};

class InitValAST : public BaseExpAST {
public:
    std::unique_ptr<BaseExpAST> exp;

    int CalcConstExp() const override {
        return exp->CalcConstExp();
    }

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "InitValAST {\n";
        ans += tab + "\texp: " + exp->PrintAST(tab + "\t");
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab, std::string &buffer) const override{
        std::string var = exp->PrintIR(tab, buffer); // XXX 目前没有检测是否是常量表达式（可以优化）
        return var;
    }
};

class StmtAST : public BaseAST {
public:
    enum Kind {
        kMatch,
        kUnmatch,
    };

    Kind kind;

    std::unique_ptr<BaseAST> matchStmt;
    std::unique_ptr<BaseAST> unmatchStmt;

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "StmtAST {\n";
        switch (kind) { // XXX 把switch改成if else，switch中不能定义变量
        case kMatch:
            ans += tab + "\tkind: match\n";
            ans += tab + "\tmatchStmt: " + matchStmt->PrintAST(tab + "\t");
            break;
        case kUnmatch:
            ans += tab + "\tkind: unmatch\n";
            ans += tab + "\tunmatchStmt: " + unmatchStmt->PrintAST(tab + "\t");
            break;
        default:
            std::cerr << "StmtAST::PrintAST: unknown kind" << std::endl;
            break;
        }
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab, std::string &buffer) const override{
        if (kind == kMatch) {
            return matchStmt->PrintIR(tab, buffer);
        } else
        if (kind == kUnmatch) {
            return unmatchStmt->PrintIR(tab, buffer);
        } else {
            std::cerr << "StmtAST::PrintIR: unknown kind" << std::endl;
        }
        return "";
    }
};

class MatchStmtAST : public BaseAST {
public:
    enum Kind {
        kIf,
        kOther
    };

    Kind kind;
    
    std::unique_ptr<BaseExpAST> exp;
    std::unique_ptr<BaseAST> matchStmt1, matchStmt2;
    std::unique_ptr<BaseAST> otherStmt;
    int ifLabelIndex;

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "MatchStmtAST {\n";
        switch (kind) { // XXX 把switch改成if else，switch中不能定义变量
        case kIf:
            ans += tab + "\tkind: if\n";
            ans += tab + "\texp: " + exp->PrintAST(tab + "\t");
            ans += tab + "\tmatchStmt1: " + matchStmt1->PrintAST(tab + "\t");
            ans += tab + "\tmatchStmt2: " + matchStmt2->PrintAST(tab + "\t");
            break;
        case kOther:
            ans += tab + "\tkind: other\n";
            ans += tab + "\totherStmt: " + otherStmt->PrintAST(tab + "\t");
            break;
        default:
            std::cerr << "MatchStmtAST::PrintAST: unknown kind" << std::endl;
            break;
        }
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab, std::string &buffer) const override{
        if (kind == kIf) {
            std::string thenLabel = "\%then" + (ifLabelIndex == 0 ? "" : "_" + std::to_string(ifLabelIndex));
            std::string elseLabel = "\%else" + (ifLabelIndex == 0 ? "" : "_" + std::to_string(ifLabelIndex));
            std::string ifEndLabel = "\%if_end" + (ifLabelIndex == 0 ? "" : "_" + std::to_string(ifLabelIndex));
            std::string stmtRet;

            // if 的条件判断部分
            std::string var = exp->PrintIR(tab, buffer);
            buffer += tab + "br " + var + ", " + thenLabel + ", " + elseLabel + "\n";
            // if 语句的 if 分支
            buffer += thenLabel + ":\n";
            stmtRet = matchStmt1->PrintIR(tab, buffer);
            if (stmtRet != "return" && stmtRet != "break" && stmtRet != "continue") {
                buffer += tab + "jump " + ifEndLabel + "\n";
            }
            // if 语句的 else 分支
            buffer += elseLabel + ":\n";
            stmtRet = matchStmt2->PrintIR(tab, buffer);
            if (stmtRet != "return" && stmtRet != "break" && stmtRet != "continue") {
                buffer += tab + "jump " + ifEndLabel + "\n";
            }
            // if 语句之后的内容（的标号）
            buffer += ifEndLabel + ":\n";
        } else
        if (kind == kOther) {
            return otherStmt->PrintIR(tab, buffer);
        } else {
            std::cerr << "MatchStmtAST::PrintIR: unknown kind" << std::endl;
        }
        return "";
    }
};

class UnmatchStmtAST : public BaseAST {
public:
    enum Kind {
        kNoElse,
        kElse
    };

    Kind kind;

    std::unique_ptr<BaseExpAST> exp;
    std::unique_ptr<BaseAST> stmt;
    std::unique_ptr<BaseAST> matchStmt;
    std::unique_ptr<BaseAST> unmatchStmt;
    int ifLabelIndex;

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "UnmatchStmtAST {\n";
        switch (kind) { // XXX 把switch改成if else，switch中不能定义变量
        case kNoElse:
            ans += tab + "\tkind: noElse\n";
            ans += tab + "\texp: " + exp->PrintAST(tab + "\t");
            ans += tab + "\tstmt: " + stmt->PrintAST(tab + "\t");
            break;
        case kElse:
            ans += tab + "\tkind: else\n";
            ans += tab + "\texp: " + exp->PrintAST(tab + "\t");
            ans += tab + "\tmatchStmt: " + matchStmt->PrintAST(tab + "\t");
            ans += tab + "\tunmatchStmt: " + unmatchStmt->PrintAST(tab + "\t");
            break;
        default:
            std::cerr << "UnmatchStmtAST::PrintAST: unknown kind" << std::endl;
            break;
        }
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab, std::string &buffer) const override {
        if (kind == kNoElse) {
            std::string thenLabel = "\%then" + (ifLabelIndex == 0 ? "" : "_" + std::to_string(ifLabelIndex));
            std::string ifEndLabel = "\%if_end" + (ifLabelIndex == 0 ? "" : "_" + std::to_string(ifLabelIndex));
            std::string stmtRet;

            // if 的条件判断部分
            std::string var = exp->PrintIR(tab, buffer);
            buffer += tab + "br " + var + ", " + thenLabel + ", " + ifEndLabel + "\n";
            // if 语句的 if 分支
            buffer += thenLabel + ":\n";
            stmtRet = stmt->PrintIR(tab, buffer);
            if (stmtRet != "return" && stmtRet != "break" && stmtRet != "continue") {
                buffer += tab + "jump " + ifEndLabel + "\n";
            }
            // if 语句之后的内容（的标号）
            buffer += ifEndLabel + ":\n";
        } else
        if (kind == kElse) {
            std::string thenLabel = "\%then" + (ifLabelIndex == 0 ? "" : "_" + std::to_string(ifLabelIndex));
            std::string elseLabel = "\%else" + (ifLabelIndex == 0 ? "" : "_" + std::to_string(ifLabelIndex));
            std::string ifEndLabel = "\%if_end" + (ifLabelIndex == 0 ? "" : "_" + std::to_string(ifLabelIndex));
            std::string stmtRet;

            // if 的条件判断部分
            std::string var = exp->PrintIR(tab, buffer);
            buffer += tab + "br " + var + ", " + thenLabel + ", " + elseLabel + "\n";
            // if 语句的 if 分支
            buffer += thenLabel + ":\n";
            stmtRet = matchStmt->PrintIR(tab, buffer);
            if (stmtRet != "return" && stmtRet != "break" && stmtRet != "continue") {
                buffer += tab + "jump " + ifEndLabel + "\n";
            }
            // if 语句的 else 分支
            buffer += elseLabel + ":\n";
            unmatchStmt->PrintIR(tab, buffer); // UnmatchStmt 不会以 return 结尾，一定会跳转，所以不需要判断返回值
            buffer += tab + "jump " + ifEndLabel + "\n";
            // if 语句之后的内容（的标号）
            buffer += ifEndLabel + ":\n";
        } else {
            std::cerr << "UnmatchStmtAST::PrintIR: unknown kind" << std::endl;
        }
        return "";
    }
};

// OtherStmt: 不是条件分支的 SysY 语句
// OtherStme ::= 
// 翻译为: ret var, var是Exp对应的变量
/**
*   翻译为:
*   Exp
*   ret var
*/
class OtherStmtAST : public BaseAST {
public:
    enum Kind {
        kExp,
        kAssign,
        kReturn,
        kWhile,
        kBreak,
        kContinue,
        kBlock
    };
    
    Kind kind;

    std::unique_ptr<BaseExpAST> lVal;
    std::unique_ptr<BaseExpAST> exp; // 在 kExp 和 kReturn 类型中，可以为 nullptr
    int whileIndex; // kWhile、kBreak、kContinue 类型中使用
    std::unique_ptr<BaseAST> stmt;
    std::unique_ptr<BaseAST> block;

    std::string PrintAST(std::string tab) const override {
        if (kind == kExp) {
            std::string ans = "";
            ans += "OtherStmtAST {\n";
            ans += tab + "\tkind: " + "exp\n";
            ans += tab + "\texp: " + (exp == nullptr ? "NULL\n" : exp->PrintAST(tab + "\t"));
            ans += tab + "}\n";
            return ans;
        } else
        if (kind == kAssign) {
            std::string ans = "";
            ans += "OtherStmtAST {\n";
            ans += tab + "\tkind: " + "assign\n";
            ans += tab + "\tlVal: " + lVal->PrintAST(tab) + "\n";
            ans += tab + "\texp: " + exp->PrintAST(tab + "\t");
            ans += tab + "}\n";
            return ans;
        } else
        if (kind == kWhile) {
            std::string ans = "";
            ans += "OtherStmtAST {\n";
            ans += tab + "\tkind: " + "while\n";
            ans += tab + "\twhileIndex: " + std::to_string(whileIndex) + "\n";
            ans += tab + "\texp: " + exp->PrintAST(tab + "\t");
            ans += tab + "\tstmt: " + stmt->PrintAST(tab + "\t");
            ans += tab + "}\n";
            return ans;
        } else
        if (kind == kBreak) {
            std::string ans = "";
            ans += "OtherStmtAST {\n";
            ans += tab + "\tkind: " + "break\n";
            ans += tab + "\twhileIndex: " + std::to_string(whileIndex) + "\n";
            ans += tab + "}\n";
            return ans;
        } else
        if (kind == kContinue) {
            std::string ans = "";
            ans += "OtherStmtAST {\n";
            ans += tab + "\tkind: " + "continue\n";
            ans += tab + "\twhileIndex: " + std::to_string(whileIndex) + "\n";
            ans += tab + "}\n";
            return ans;
        } else
        if (kind == kReturn) {
            std::string ans = "";
            ans += "OtherStmtAST {\n";
            ans += tab + "\tkind: " + "return\n";
            ans += tab + "\texp: " + (exp == nullptr ? "NULL\n" : exp->PrintAST(tab + "\t"));
            ans += tab + "}\n";
            return ans;
        } else
        if (kind == kBlock) {
            std::string ans = "";
            ans += "OtherStmtAST {\n";
            ans += tab + "\tkind: " + "block\n";
            ans += tab + "\tblock: " + block->PrintAST(tab + "\t");
            ans += tab + "}\n";
            return ans;
        } else {
            std::cerr << "OtherStmtAST::PrintAST: unknown kind" << std::endl;
            return "";
        }
    }

    std::string PrintIR(std::string tab, std::string &buffer) const override{
        // Exp
        if (kind == kExp) {
            if (exp != nullptr) exp->PrintIR(tab, buffer);
        } else
        // Exp
        // store var, @lVal
        if (kind == kAssign) {
            std::string var = exp->PrintIR(tab, buffer);
            std::string lValName = lVal->PrintIR(tab, buffer);
            buffer += tab + "store " + var + ", " + lValName + "\n";
        } else
        if (kind == kWhile) {
            std::string whileEntryLabel = "\%while_entry" + (whileIndex == 0 ? "" : "_" + std::to_string(whileIndex));
            std::string whileBodyLabel = "\%while_body" + (whileIndex == 0 ? "" : "_" + std::to_string(whileIndex));
            std::string whileEndLabel = "\%while_end" + (whileIndex == 0 ? "" : "_" + std::to_string(whileIndex));
            std::string stmtRet;

            // while 循环的入口
            buffer += tab + "jump " + whileEntryLabel + "\n";
            buffer += whileEntryLabel + ":\n";
            buffer += tab + "br " + exp->PrintIR(tab, buffer) + ", " + whileBodyLabel + ", " + whileEndLabel + "\n";
            // while 循环的循环体
            buffer += whileBodyLabel + ":\n";
            stmtRet = stmt->PrintIR(tab, buffer);
            if (stmtRet != "return" && stmtRet != "break" && stmtRet != "continue") {
                buffer += tab + "jump " + whileEntryLabel + "\n";
            }
            // while 循环的结尾
            buffer += whileEndLabel + ":\n";
        } else
        if (kind == kBreak) {
            std::string whileEndLabel = "\%while_end" + (whileIndex == 0 ? "" : "_" + std::to_string(whileIndex));
            buffer += tab + "jump " + whileEndLabel + "\n";
            return "break"; // 返回 break，说明该基本块以 break 语句结尾，语句块结尾不能加 br、jump 等
        } else
        if (kind == kContinue) {
            std::string whileEntryLabel = "\%while_entry" + (whileIndex == 0 ? "" : "_" + std::to_string(whileIndex));
            buffer += tab + "jump " + whileEntryLabel + "\n";
            return "continue"; // 返回 continue，说明该基本块以 continue 语句结尾，语句块结尾不能加 br、jump 等
        } else
        // Exp
        // ret var
        if (kind == kReturn) {
            std::string retVar = exp == nullptr ? "" : " " + exp->PrintIR(tab, buffer);
            buffer += tab + "ret" + retVar + "\n";
            return "return"; // 返回 return，说明该基本块以 return 语句结尾，语句块结尾不能加 br、jump 等
        } else
        // Block
        if (kind == kBlock) {
            return block->PrintIR(tab, buffer);
        } else {
            std::cerr << "OtherStmtAST::PrintIR: unknown kind" << std::endl;
        }
        return "";
    }
};

class ConstExpAST : public BaseExpAST {
public:
    std::unique_ptr<BaseExpAST> exp;

    bool isCalcuated = false;
    int value;

    int CalcConstExp() const override {
        if (isCalcuated) {
            return value;
        } else {
            return exp->CalcConstExp();
        }
    }

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "ConstExpAST {\n";
        ans += tab + "\texp: " + exp->PrintAST(tab + "\t");
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab, std::string &buffer) const override{
        // PrintIR 前已经计算了常量表达式的值（在语法分析的时候）
        return std::to_string(value);
    }
};

// Exp: 一个 SysY 表达式
// Exp ::= AddExp;
/**
*   翻译为:
*   [addExp]
*/
class ExpAST : public BaseExpAST {
public:
    std::unique_ptr<BaseExpAST> lOrExp;

    int CalcConstExp() const override {
        return lOrExp->CalcConstExp();
    }

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

// PrimaryExpAST: 表达式中优先计算的部分, 即被'()'包裹的表达式/标识符/单个数字
// PrimaryExp ::= "(" Exp ")" | LVal | Number;
// LVal 就是变量标识符，常量会被直接替换为 Number
class PrimaryExpAST : public BaseExpAST {
public:
    enum Kind {
        kExp,
        kLVal,
        kNumber
    };

    Kind kind;
    std::unique_ptr<BaseExpAST> exp;
    std::unique_ptr<BaseExpAST> lVal;
    int number;

    int CalcConstExp() const override {
        if (kind == kExp) {
            return exp->CalcConstExp();
        } else
        if (kind == kLVal) {
            return lVal->CalcConstExp();
            return 0;
        } else
        if (kind == kNumber) {
            return number;
        } else {
            std::cerr << "PrimaryExpAST::CalcConstExp: unknown kind" << std::endl;
            return 0;
        }
    }

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "PrimaryExpAST {\n";
        switch (kind) { // XXX 把switch改成if else，switch中不能定义变量
        case kExp:
            ans += tab + "\texp: " + exp->PrintAST(tab + "\t");
            break;
        case kLVal:
            ans += tab + "\tLVal: " + lVal->PrintAST(tab + "\t") + "\n";
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
        if (kind == kExp) {
            return exp->PrintIR(tab, buffer);
        } else
        if (kind == kLVal) {
            std::string lVarName = lVal->PrintIR(tab, buffer);
            if (lVarName[0] != '@' && lVarName[0] != '%')  { // 不是需要 load 的变量
                return lVarName;
            } else { // 需要 load 的变量
                std::string now = NewTempSymbol();
                buffer += tab + now + " = load " + lVarName + "\n";
                return now;
            }
        } else
        if (kind == kNumber) {
            return std::to_string(number);
        } else {
            std::cerr << "PrimaryExpAST::PrintIR: unknown kind" << std::endl;
        }
        return "";
    }
};

class LValAST: public BaseExpAST {
public:
    enum Kind {
        kConst,
        kVar,
        kArray
    };

    Kind kind;

    int identVal; // 标识符在符号表中的值
    std::string ident;
    std::unique_ptr<std::vector<std::unique_ptr<BaseExpAST> > > arrayDims;

    int CalcConstExp() const override {
        if (kind == kConst) {
            return identVal;
        } else {
            std::cerr << "LValAST::CalcConstExp: lVal is not a const" << std::endl;
        }
        return 0;
    }

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "LValAST {\n";
        switch (kind) {
        case kConst:
            ans += tab + "\tconst: " + std::to_string(identVal) + "\n";
            break;
        case kVar:
            ans += tab + "\tvar: " + ident + "\n";
            break;
        case kArray:
            ans += tab + "\tarray: " + ident + "\n";
            ans += tab + "\tarrayDims: [\n";
            for (auto &dim : *arrayDims) {
                ans += tab + "\t\t" + dim->PrintAST(tab + "\t\t") + "\n";
            }
            ans += tab + "\t]\n";
            break;
        default:
            std::cerr << "LValAST::PrintAST: unknown kind" << std::endl;
            break;
        }
        ans += tab + "}\n";
        return ans;
    }

    std::string PrintIR(std::string tab, std::string &buffer) const override{
        if (kind == kConst) {
            return std::to_string(identVal);
        } else
        if (kind == kVar) {
            if (identVal == -1) {
                return "\%fParam_" + ident;
            } else {
                return "@" + ident + (identVal == 0 ? "" : "_" + std::to_string(identVal));
            }
        } else
        if (kind == kArray) {
            std::string pre = "@" + ident + (identVal == 0 ? "" : "_" + std::to_string(identVal));

            for (auto &dim : *arrayDims) {
                std::string var = dim->PrintIR(tab, buffer); // 数组下标
                std::string now = NewTempSymbol();
                buffer += tab + now + " = getelemptr " + pre + ", " + var + "\n";
                pre = now;
            }

            return pre;
        } else {
            std::cerr << "LValAST::PrintIR: unknown kind" << std::endl;
        }
        return "";
    }
};

// UnaryExpAST: 单目运算表达式
// UnaryExp ::= PrimaryExp | '+' UnaryExp | '-' UnaryExp | '!' UnaryExp
/**
*   翻译为:
*   1. [PrimaryExp]
*   2. [UnaryExp]
*/
class UnaryExpAST : public BaseExpAST {
public:
    enum Kind {
        kPrimaryExp,
        kCall,
        kPositive,
        kNegative,
        kNot
    };
    
    Kind kind;
    std::unique_ptr<BaseExpAST> primaryExp;
    std::string ident;
    int funcType;
    std::unique_ptr<std::vector<std::unique_ptr<BaseExpAST> > > funcRParams;
    std::unique_ptr<BaseExpAST> unaryExp;

    int CalcConstExp() const override {
        if (kind == kPrimaryExp) {
            return primaryExp->CalcConstExp();
        } else
        if (kind == kCall) {
            std::cerr << "UnaryExpAST::CalcConstExp: call is not a const" << std::endl;
            return 0;
        } else
        if (kind == kPositive) {
            return unaryExp->CalcConstExp();
        } else
        if (kind == kNegative) {
            return -unaryExp->CalcConstExp();
        } else
        if (kind == kNot) {
            return !unaryExp->CalcConstExp();
        } else {
            std::cerr << "UnaryExpAST::CalcConstExp: unknown kind" << std::endl;
            return 0;
        }
    }

    std::string PrintAST(std::string tab) const override {
        std::string ans = "";
        ans += "UnaryExpAST {\n";
        switch (kind) { // XXX 把switch改成if else，switch中不能定义变量
        case kPrimaryExp:
            ans += tab + "\tprimaryExp: " + primaryExp->PrintAST(tab + "\t");
            break;
        case kCall:
            ans += tab + "\tident: " + ident + "\n";
            ans += tab + "\tfuncRParams: {\n";
            for (auto &param : *funcRParams) {
                ans += tab + "\t\t" + param->PrintAST(tab + "\t\t");
            }
            ans += tab + "\t}\n";
            break;
        case kPositive:
            ans += tab + "\tunaryOp: " + "+" + "\n";
            ans += tab + "\tunaryExp: " + unaryExp->PrintAST(tab + "\t");
            break;
        case kNegative:
            ans += tab + "\tunaryOp: " + "-" + "\n";
            ans += tab + "\tunaryExp: " + unaryExp->PrintAST(tab + "\t");
            break;
        case kNot:
            ans += tab + "\tunaryOp: " + "!" + "\n";
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
        } else
        if (kind == kCall) {
            if (funcType == 0) { // void
                // 先算出实参列表
                std::vector<std::string> params;
                if (funcRParams != nullptr) {
                    for (auto &paramAST : *funcRParams) {
                        params.push_back(paramAST->PrintIR(tab, buffer));
                    }
                }
                buffer += tab + "call @" + ident + "(";
                if (funcRParams != nullptr) {
                    for (auto &param : params) {
                        buffer += param;
                        if (param != params.back()) {
                            buffer += ", ";
                        }
                    }
                }
                buffer += ")\n";
                return "";
            } else
            if (funcType == 1) { // int
                // 先算出实参列表
                std::vector<std::string> params;
                if (funcRParams != nullptr) {
                    for (auto &paramAST : *funcRParams) {
                        params.push_back(paramAST->PrintIR(tab, buffer));
                    }
                }
                std::string now = NewTempSymbol();
                buffer += tab + now + " = call @" + ident + "(";
                if (funcRParams != nullptr) {
                    for (auto &param : params) {
                        buffer += param;
                        if (param != params.back()) {
                            buffer += ", ";
                        }
                    }
                }
                buffer += ")\n";
                return now;
            } else {
                std::cerr << "UnaryExpAST::PrintIR: unknown funcType" << std::endl;
            }
        } else
        // +, 不产生IR
        if (kind == kPositive) {
            std::string var = unaryExp->PrintIR(tab, buffer);
            return var;
        } else
        // -, IR格式为: now = sub 0, var
        if (kind == kNegative) {
            std::string var = unaryExp->PrintIR(tab, buffer);
            std::string now = NewTempSymbol();
            buffer += tab + now + " = " + "sub" + " 0, " + var + "\n";
            return now;
        } else
        // !, IR格式为: now = eq 0, var
        if (kind == kNot) {
            std::string var = unaryExp->PrintIR(tab, buffer);
            std::string now = NewTempSymbol();
            buffer += tab + now + " = " + "eq" + " 0, " + var + "\n";
            return now;
        } else {
            std::cerr << "UnaryExpAST::PrintIR: unknown kind" << std::endl;
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
class MulExpAST : public BaseExpAST {
public:
    enum Kind {
        kUnaryExp,
        kMul,
        kDiv,
        kMod
    };

    Kind kind;

    std::unique_ptr<BaseExpAST> unaryExp;
    std::unique_ptr<BaseExpAST> mulExp;

    int CalcConstExp() const override {
        if (kind == kUnaryExp) {
            return unaryExp->CalcConstExp();
        } else
        if (kind == kMul) {
            return mulExp->CalcConstExp() * unaryExp->CalcConstExp();
        } else
        if (kind == kDiv) {
            return mulExp->CalcConstExp() / unaryExp->CalcConstExp();
        } else
        if (kind == kMod) {
            return mulExp->CalcConstExp() % unaryExp->CalcConstExp();
        } else {
            std::cerr << "MulExpAST::CalcConstExp: unknown kind" << std::endl;
            return 0;
        }
    }

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
class AddExpAST : public BaseExpAST {
public:
    enum Kind {
        kMulExp,
        kAdd,
        kSub
    };

    Kind kind;

    std::unique_ptr<BaseExpAST> mulExp;
    std::unique_ptr<BaseExpAST> addExp;

    int CalcConstExp() const override {
        if (kind == kMulExp) {
            return mulExp->CalcConstExp();
        } else
        if (kind == kAdd) {
            return addExp->CalcConstExp() + mulExp->CalcConstExp();
        } else
        if (kind == kSub) {
            return addExp->CalcConstExp() - mulExp->CalcConstExp();
        } else {
            std::cerr << "AddExpAST::CalcConstExp: unknown kind" << std::endl;
            return 0;
        }
    }

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
class RelExpAST : public BaseExpAST {
public:
    enum Kind {
        kAddExp,
        kLT,
        kGT,
        kLE,
        kGE,
    };

    Kind kind;

    std::unique_ptr<BaseExpAST> addExp;
    std::unique_ptr<BaseExpAST> relExp;

    int CalcConstExp() const override {
        if (kind == kAddExp) {
            return addExp->CalcConstExp();
        } else
        if (kind == kLT) {
            return relExp->CalcConstExp() < addExp->CalcConstExp();
        } else
        if (kind == kGT) {
            return relExp->CalcConstExp() > addExp->CalcConstExp();
        } else
        if (kind == kLE) {
            return relExp->CalcConstExp() <= addExp->CalcConstExp();
        } else
        if (kind == kGE) {
            return relExp->CalcConstExp() >= addExp->CalcConstExp();
        } else {
            std::cerr << "RelExpAST::CalcConstExp: unknown kind" << std::endl;
            return 0;
        }
    }

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
class EqExpAST : public BaseExpAST {
public:
    enum Kind {
        kRelExp,
        kEQ,
        kNE,
    };
    
    Kind kind;

    std::unique_ptr<BaseExpAST> relExp;
    std::unique_ptr<BaseExpAST> eqExp;

    int CalcConstExp() const override {
        if (kind == kRelExp) {
            return relExp->CalcConstExp();
        } else
        if (kind == kEQ) {
            return eqExp->CalcConstExp() == relExp->CalcConstExp();
        } else
        if (kind == kNE) {
            return eqExp->CalcConstExp() != relExp->CalcConstExp();
        } else {
            std::cerr << "EqExpAST::CalcConstExp: unknown kind" << std::endl;
            return 0;
        }
    }

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
class LAndExpAST : public BaseExpAST {
public:
    enum Kind {
        kEqExp,
        kAnd,
    };

    Kind kind;

    std::unique_ptr<BaseExpAST> eqExp;
    std::unique_ptr<BaseExpAST> lAndExp;

    int CalcConstExp() const override {
        if (kind == kEqExp) {
            return eqExp->CalcConstExp();
        } else
        if (kind == kAnd) {
            return lAndExp->CalcConstExp() && eqExp->CalcConstExp();
        } else {
            std::cerr << "LAndExpAST::CalcConstExp: unknown kind" << std::endl;
            return 0;
        }
    }

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
class LOrExpAST : public BaseExpAST {
public:
    enum Kind {
        kLAndExp,
        kOr,
    };

    Kind kind;

    std::unique_ptr<BaseExpAST> lAndExp;
    std::unique_ptr<BaseExpAST> lOrExp;

    int CalcConstExp() const override {
        if (kind == kLAndExp) {
            return lAndExp->CalcConstExp();
        } else
        if (kind == kOr) {
            return lOrExp->CalcConstExp() || lAndExp->CalcConstExp();
        } else {
            std::cerr << "LOrExpAST::CalcConstExp: unknown kind" << std::endl;
        }
        return 0;
    }

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
