%option noyywrap

%{

#include "rule.h"
#include "rules_tokens.h"

%}

%%

[A-Z][a-z]+ { yylval.string = new std::string(yytext); return(EXPRESSION_TYPE); }
\$[a-zA-Z]+ { yylval.string = new std::string(yytext); return(EXPRESSION_GENERATOR); }
[a-z]+\* { yylval.string = new std::string(yytext); return(WILDCARD); }
[a-z]+ { yylval.string = new std::string(yytext); return(VARIABLE); }
\-\> { return(SIMPLIFIES_TO); }
\( { return(LEFT_PARENTHESIS); }
\) { return(RIGHT_PARENTHESIS); }
\, { return(COMMA); }
\; { return(SEMICOLON); }

%%
