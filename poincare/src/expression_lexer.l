%{

/* Flex's job is to generate tokens that will be fed to the parser. Each token
 * can be attached a value. The type of this value is specified in the Bison
 * file. In our case, it can be a Poincare Expression. Therefore we need the
 * definition of these types, hence the include.
 * Note that those have to come before including the parser header, because the
 * parser also needs those types to be defined. */
#include <poincare.h>

/* Flex generate a lexer, which outputs tokens. Strangely enough, tokens end up
 * being defined by Bison in the parser's header file. Since we'll emit tokens
 * in our C file, we'll definitely need their definition, hence this include. */
#include "expression_parser.hpp"


/* Flex has provision for reading files. We'll never use this, so we're defining
 * YY_INPUT which effectively disable taking input from a file. */
#define YY_INPUT

/* Flex can print to stdout what token it matches by calling the ECHO function.
 * We're not using this feature, and we don't even provide printf anyway. */
#define ECHO

#define fprintf(...) ((void)0)
#define exit(...) abort()

%}

/* We're using a reentrant lexer. Otherwise Flex makes use of global variables
 * for historical reasons (compatibility with Lex) which we don't care about. */
%option reentrant

/* Normally, Flex will call *yywrap when reaching an end-of-file. We don't want
 * to deal with files anyway, so let's just disable this. */
%option noyywrap

%option bison-bridge

/* Normally, on each new input file the scanner calls isatty() in an attempt to
 * determine whether the scanner's input source is interactive and thus should
 * be read a character at a time.
 * We obviously do *not* provide isatty(), and we know we're never going to use
 * an interactive input source.*/
%option never-interactive

/* Definitions */

%%

[0-9]+ { yylval->string = yytext; return(INTEGER); }
\/ { return(DIVIDE); }
\* { return(MULTIPLY); }
\^ { return(POW); }
\+ { return(PLUS); }
[A-Za-z]+ { yylval->string = yytext; return(SYMBOL); }

%%

