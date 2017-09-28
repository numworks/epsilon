%option noyywrap

%{

#include "rule.h"
#include "rules_tokens.h"

%}

%%

[a-z]+ { yylval.string = new std::string(yytext); return (IDENTIFIER); }
[A-Za-z]+ { yylval.string = new std::string(yytext); return (CAPITALIZED_IDENTIFIER); }
-?[0-9]+ { yylval.string = new std::string(yytext); return (VALUE); }
\$ { return(DOLLAR); }
\* { return (ASTERISK); }
\-\> { return(SIMPLIFIES_TO); }
\( { return(LEFT_PARENTHESIS); }
\) { return(RIGHT_PARENTHESIS); }
\[ { return(LEFT_BRACKET); }
\] { return(RIGHT_BRACKET); }
\, { return(COMMA); }
\; { return(SEMICOLON); }
\. { return(PERIOD); }
\! { return BANG; }
"//".* {} /* ignore single line comments */

[ \t\n] {} /* ignore line jump and whitespaces */

%%
