%option noyywrap
%option nounput
%option noinput

%{

#include <cstdlib>
#include <string>

// 因为 Flex 会用到 Bison 中关于 token 的定义
// 所以需要 include Bison 生成的头文件
#include "SysY.tab.hpp"

using namespace std;

%}

/* 空白符和注释 */
WhiteSpace    [ \t\n\r]*
LineComment   "//".*

/* 标识符 */
Identifier    [a-zA-Z_][a-zA-Z0-9_]*

/* 整数字面量 */
Decimal       [1-9][0-9]*
Octal         0[0-7]*
Hexadecimal   0[xX][0-9a-fA-F]+

/* 普通字符 */
Char          '[^'\\\n]'

/* 转义字符 */
Escape        '"\\"[nrt0'\\]'

%%

{WhiteSpace}    { /* 忽略, 不做任何操作 */ }
{LineComment}   { /* 忽略, 不做任何操作 */ }

"void"          { return VOID; }
"int"           { return INT; }
"const"         { return CONST; }
"if"            { return IF; }
"else"          { return ELSE; }
"while"         { return WHILE; }
"break"         { return BREAK; }
"continue"      { return CONTINUE; }
"return"        { return RETURN; }

"<"             { return LT; }
">"             { return GT; }
"<="            { return LE; }
">="            { return GE; }
"=="            { return EQ; }
"!="            { return NE; }
"&&"            { return AND; }
"||"            { return OR; }

{Identifier}    { yylval.str_val = new string(yytext); return IDENT; }

{Decimal}       { yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST; }
{Octal}         { yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST; }
{Hexadecimal}   { yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST; }

{Char}          { yylval.int_val = yytext[1]; return INT_CONST; }
{Escape}        {
                    switch (yytext[2]) {
                        case 'n': yylval.int_val = '\n'; break;
                        case 'r': yylval.int_val = '\r'; break;
                        case 't': yylval.int_val = '\t'; break;
                        case '0': yylval.int_val = '\0'; break;
                        case '\'': yylval.int_val = '\''; break;
                        case '\\': yylval.int_val = '\\'; break;
                    }
                    return INT_CONST;
                }

.               { return yytext[0]; }

%%
