%{
/*  myyacc.txt
*/
#include <vector>
#include <stack>
#include <string>
#include "symTable.h"
using namespace std;

int yylval;

%}

%token IDENTIFIER NUM
%token INT VOID IF WHILE RETURN ELSE
%token ';'	 '(' 	')' 	',' 	EPLSION		'EQ'	'{'		'}'		'='

%left '+' '-'	'%'
%left '*' '/'
%%
program : decl_list
	;
decl_list : decl_list decl
	| decl
	;
decl : var_decl
	| fun_decl
	;
var_decl : type_spec IDENTIFIER ';'
	;
type_spec : VOID
	| INT
	;
fun_decl : type_spec IDENTIFIER '(' params ')' compound_stmt
	;
params : param_list
	| VOID
	;
param_list : param_list ',' param
	| param
	;
param : type_spec IDENTIFIER 
	;
stmt_list : stmt_list stmt
	|	EPLSION
	|	stmt
	;
stmt : expr_stmt
	| compound_stmt
	| if_stmt
	| while_stmt
	| return_stmt
	;
expr_stmt : IDENTIFIER '=' expr ';'	{#sym_table.setValue(s[0],s[2]);}
	| ';'
	;
while_stmt : WHILE '(' expr ')' stmt
	;
compound_stmt : '{' local_decls stmt_list '}'
	|	'{' local_decls  '}'
	|	'{'	 '}'
	|	'{'  stmt_list '}'
	;
local_decls : local_decls local_decl
	|	EPLSION
	|	local_decl
	;
local_decl : type_spec IDENTIFIER ';'
	;
if_stmt : IF '(' expr ')' stmt
	| IF '(' expr ')' stmt ELSE stmt
	;
return_stmt : RETURN ';'
	| RETURN expr ';'
	;
expr : expr 'EQ' expr	{#$$=(s[0]==s[2]);}
	| expr '+' expr	{#$$=s[0]+s[2];}
	| expr '-' expr	{#$$=s[0]-s[2];}
	| expr '*' expr	{#$$=s[0]*s[2];}
	| expr '/' expr {#$$=s[0]/s[2];}
	| expr '%' expr {#$$=s[0]%s[2];}
	| '(' expr ')'	{#$$=s[1];}
	| '(' '-' expr ')'	{#$$=0-s[2];}
	| IDENTIFIER	{#$$=sym_table.getValue(s[0]);}
	| NUM		{#$$=yylval;}
	;
args : arg_list
	|	EPLSION
	;
arg_list : arg_list ',' expr
	| expr
	; 
%%