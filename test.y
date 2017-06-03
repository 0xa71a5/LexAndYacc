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

%token c d 

%%
S' : S ;
S : C C ;
C : c C ;
C : d ;
%%