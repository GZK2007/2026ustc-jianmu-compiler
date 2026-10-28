%option noyywrap
%{
/*****************声明和选项设置  begin*****************/
#include <stdio.h>
#include <stdlib.h>

#include "syntax_tree.h"
#include "syntax_analyzer.h"

int lines=1;
int pos_start=1;
int pos_end=1;

void pass_node(char *text){
     yylval.node = new_syntax_tree_node(text);
}

/*****************声明和选项设置  end*****************/

%}

%x COMMENT

%%
 /* to do for students */

 /* 空白字符：空格、制表符、回车，跳过但更新列号 */
[ \t\r\f\v]+	{ pos_start = pos_end; pos_end += yyleng; }
 /* 换行：行号累加，列号重置 */
\n+	{ lines += yyleng; pos_start = pos_end = 1; }

 /* 注释开始记号，进入注释状态 */
"/*"	{ pos_start = pos_end; pos_end += 2; BEGIN(COMMENT); }
 /* 注释结束记号，返回正常状态 */
<COMMENT>"*/"	{ pos_start = pos_end; pos_end += 2; BEGIN(INITIAL); }
 /* 注释内部的换行：同样要维护行号与列号 */
<COMMENT>\n	{ lines++; pos_start = pos_end = 1; }
 /* 注释内部的其他字符：全部丢弃 */
<COMMENT>.	{ pos_start = pos_end; pos_end++; }

"else"	{ pos_start = pos_end; pos_end += yyleng; pass_node(yytext); return ELSE; }
"if"	     { pos_start = pos_end; pos_end += yyleng; pass_node(yytext); return IF; }
"int"	{ pos_start = pos_end; pos_end += yyleng; pass_node(yytext); return INT; }
"return"	{ pos_start = pos_end; pos_end += yyleng; pass_node(yytext); return RETURN; }
"void"	{ pos_start = pos_end; pos_end += yyleng; pass_node(yytext); return VOID; }
"while"	{ pos_start = pos_end; pos_end += yyleng; pass_node(yytext); return WHILE; }
"float"	{ pos_start = pos_end; pos_end += yyleng; pass_node(yytext); return FLOAT; }

[A-Za-z]+	               { pos_start = pos_end; pos_end += yyleng; pass_node(yytext); return IDENTIFIER; }
[0-9]+\.[0-9]*|\.[0-9]+	{ pos_start = pos_end; pos_end += yyleng; pass_node(yytext); return FLOATPOINT; }
[0-9]+	               { pos_start = pos_end; pos_end += yyleng; pass_node(yytext); return INTEGER; }
 /* two cases for you, pass_node will send flex's token to bison */
\+ 	{pos_start = pos_end; pos_end += 1; pass_node(yytext); return ADD;}
\-	{pos_start = pos_end; pos_end += 1; pass_node(yytext); return SUB;}
\*	{pos_start = pos_end; pos_end += 1; pass_node(yytext); return MUL;}
\/	{pos_start = pos_end; pos_end += 1; pass_node(yytext); return DIV;}
\<	{pos_start = pos_end; pos_end += 1; pass_node(yytext); return LT;}
\<=	{pos_start = pos_end; pos_end += 2; pass_node(yytext); return LTE;}
\>	{pos_start = pos_end; pos_end += 1; pass_node(yytext); return GT;}
\>=	{pos_start = pos_end; pos_end += 2; pass_node(yytext); return GTE;}
==	{pos_start = pos_end; pos_end += 2; pass_node(yytext); return EQ;}
!=	{pos_start = pos_end; pos_end += 2; pass_node(yytext); return NEQ;}
=	{pos_start = pos_end; pos_end += 1; pass_node(yytext); return ASSIGN;}
;	{pos_start = pos_end; pos_end += 1; pass_node(yytext); return SEMICOLON;}
,	{pos_start = pos_end; pos_end += 1; pass_node(yytext); return COMMA;}
 /* TODO: phase1. 请在这里补充其他的词法规则 */
\(	{ pos_start = pos_end; pos_end += yyleng; pass_node(yytext); return LPARENTHESE; }
\)	{ pos_start = pos_end; pos_end += yyleng; pass_node(yytext); return RPARENTHESE; }
\[	{ pos_start = pos_end; pos_end += yyleng; pass_node(yytext); return LBRACKET; }
\]	{ pos_start = pos_end; pos_end += yyleng; pass_node(yytext); return RBRACKET; }
\{	{ pos_start = pos_end; pos_end += yyleng; pass_node(yytext); return LBRACE; }
\}	{ pos_start = pos_end; pos_end += yyleng; pass_node(yytext); return RBRACE; }


. { pos_start = pos_end; pos_end++; return ERROR; }

 /****请在此补全所有flex的模式与动作  end******/
%%
