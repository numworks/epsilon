/* This file should be built with Bison 3.0.4. It might work with other Bison
 * version, but those haven't been tested. */

/* Ask for a reentrant parser. Otherwise Bison uses global variables, and we
 * want to avoid this behavior. */
%pure-parser

/* Our lexer and parser are reentrant. That means that their generated functions
 * (such as yylex) will expect a context parameter, so let's tell Bison about
 * it. Note that the context is an opaque pointer. */
%lex-param   { void * scanner }
%parse-param { void * scanner }

/* When calling the parser, we will provide yyparse with an extra parameter : a
 * backpointer to the resulting expression. */
%parse-param { Expression ** expressionOutput }

%{
// We will be generating all kind of Expressions, so we need their definitions
#include <poincare.h>

/* The lexer manipulates tokens defined by the parser, so we need the following
 * inclusion order. */
#include "expression_parser.hpp"
#include "expression_lexer.hpp"

/* Declare our error-handling function. Since we're making a re-entrant parser,
 * it takes a context parameter as its first input. */
void poincare_expression_yyerror(void * scanner, Expression ** expressionOutput, char const *msg);

/* Bison expects to use __builtin_memcpy. We don't want to provide this, but
 * instead we do provide regular memcpy. Let's instruct Bison to use it. */
#define YYCOPY(To, From, Count) memcpy(To, From, (Count)*sizeof(*(From)))

%}


/* All symbols (both terminals and non-terminals) may have a value associated
 * with them. In our case, it's going to be either an Expression (for example,
 * when parsing (a/b) we want to create a new Fraction), or a string (this will
 * be useful to retrieve the value of Integers for example). */
%union {
  Expression * expression;
  char * string;
}

/* The INTEGER token uses the "string" part of the union to store its value */
%token <string> INTEGER
%token <string> SYMBOL

/* Operator tokens */
%token PLUS
%token MINUS
%token MULTIPLY
%token DIVIDE
%token POW
%token LEFT_PARENTHESIS
%token RIGHT_PARENTHESIS

/* Make the operators left associative.
 * This makes 1 - 2 - 5’  be ‘(1 - 2) - 5’ instead of ‘1 - (2 - 5)’.
 * This makes 1 / 2 / 5’  be ‘(1 / 2) / 5’ instead of ‘1 / (2 / 5)’.
 */
%left '-' '+'
%left '*' '/'

/* The "exp" symbol uses the "expression" part of the union. */
%type <expression> exp;

%%

Root:
  exp {
    *expressionOutput = $1;
  }

/* Note that in bison, precedence of parsing depend on the order they are defined in here, the last
 * one has the highest precedence. */
exp:
  INTEGER            { $$ = new Integer($1);     }
  | SYMBOL           { $$ = new Symbol($1);    }
  | exp PLUS exp     { $$ = new Addition($1,$3); }
  | exp MINUS exp    { $$ = new Substraction($1,$3); }
  | exp MULTIPLY exp { $$ = new Product($1,$3);  }
  | exp DIVIDE exp   { $$ = new Fraction($1,$3); }
  | exp POW exp      { $$ = new Power($1,$3); }
  | LEFT_PARENTHESIS exp RIGHT_PARENTHESIS     { $$ = $2; }
;

%%

void poincare_expression_yyerror(void * scanner, Expression ** expressionOutput, char const *msg) {
  // Handle the error!
}
