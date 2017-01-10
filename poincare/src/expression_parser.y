/* This file should be built with Bison 3.0.4. It might work with other Bison
 * version, but those haven't been tested. */

/* When calling the parser, we will provide yyparse with an extra parameter : a
 * backpointer to the resulting expression. */
%parse-param { Expression ** expressionOutput }

%{
#include <poincare.h>
/* The lexer manipulates tokens defined by the parser, so we need the following
 * inclusion order. */
#include "expression_parser.hpp"
#include "expression_lexer.hpp"

/* Declare our error-handling function. Since we're making a re-entrant parser,
 * it takes a context parameter as its first input. */
void poincare_expression_yyerror(Expression ** expressionOutput, char const *msg);

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
  ListData * listData;
  MatrixData * matrixData;
  Function * function;
  /* Caution: all the const char * are NOT guaranteed to be NULL-terminated!
   * While Flex guarantees that yytext is NULL-terminated when building tokens,
   * it does so by temporarily swapping in a NULL terminated in the input text.
   * Of course that hack has vanished when the pointer is fed into Bison.
   * We thus record the length of the char fed into Flex in a structure and give
   * it to the object constructor called by Bison along with the char *. */
  struct {
     char * address;
     int length;
  } string;
  char character;
}

/* The INTEGER token uses the "string" part of the union to store its value */
%token <string> DIGITS
%token <string> FLOAT
%token <character> SYMBOL
%token <function> FUNCTION

/* Operator tokens */
%token PLUS
%token MINUS
%token MULTIPLY
%token DIVIDE
%token POW
%token LEFT_PARENTHESIS
%token RIGHT_PARENTHESIS
%token LEFT_BRACE
%token RIGHT_BRACE
%token LEFT_BRACKET
%token RIGHT_BRACKET
%token COMMA
%token DOT
%token EE

/* Make the operators left associative.
 * This makes 1 - 2 - 5’  be ‘(1 - 2) - 5’ instead of ‘1 - (2 - 5)’.
 * This makes 1 / 2 / 5’  be ‘(1 / 2) / 5’ instead of ‘1 / (2 / 5)’.
 *
 * This also puts the precedence of the operators, here DIVIDE has a bigger
 * precedence than PLUS for example.
 *
 * Note that specifying the precedence of reduces is usually a very bad practice
 * expect in the case of operator precedence and of IF/THE/ELSE structure which
 * are the only two exceptions.
 * If you need to define precedence in order to avoid shift/redice conflicts for
 * other situations your grammar is most likely ambiguous.
 */
%left PLUS
%left MINUS
%left MULTIPLY
%left DIVIDE
%left POW

/* The "exp" symbol uses the "expression" part of the union. */
%type <expression> exp;
%type <expression> number;
%type <listData> lstData;
%type <matrixData> mtxData;

%%

Root:
  exp {
    *expressionOutput = $1;
  }

/* Note that in bison, precedence of parsing depend on the order they are defined in here, the last
 * one has the highest precedence. */

lstData:
  exp { $$ = new ListData($1); }
  | lstData COMMA exp { $$ = $1; $$->pushExpression($3); }

mtxData:
  LEFT_BRACKET lstData RIGHT_BRACKET { $$ = new MatrixData($2); }
  | mtxData LEFT_BRACKET lstData RIGHT_BRACKET  { $$ = $1; $$->pushListData($3); }

number:
  DIGITS { $$ = new Integer($1.address, false); }
  | MINUS DIGITS { $$ = new Integer($2.address, true); }
  | DIGITS DOT DIGITS { $$ = new Float($1.address, $1.length, false, $3.address, $3.length, nullptr, 0, false); }
  | MINUS DIGITS DOT DIGITS { $$ = new Float($2.address, $2.length, true, $4.address, $4.length, nullptr, 0, false); }
  | DIGITS DOT DIGITS EE DIGITS { $$ = new Float($1.address, $1.length, false, $3.address, $3.length, $5.address, $5.length, false); }
  | MINUS DIGITS DOT DIGITS EE DIGITS { $$ = new Float($2.address, $2.length, true, $4.address, $4.length, $6.address, $6.length, false); }
  | DIGITS DOT DIGITS EE MINUS DIGITS { $$ = new Float($1.address, $1.length, false, $3.address, $3.length, $6.address, $6.length, true); }
  | MINUS DIGITS DOT DIGITS EE MINUS DIGITS { $$ = new Float($2.address, $2.length, true, $4.address, $4.length, $7.address, $7.length, true); }

exp:
  number             { $$ = $1; }
  | SYMBOL           { $$ = new Symbol($1); }
  | exp PLUS exp     { Expression * terms[2] = {$1,$3}; $$ = new Addition(terms, 2, false); }
  | exp MINUS exp    { Expression * terms[2] = {$1,$3}; $$ = new Subtraction(terms, false); }
  | exp MULTIPLY exp { Expression * terms[2] = {$1,$3}; $$ = new Product(terms, false);  }
  | exp DIVIDE exp   { Expression * terms[2] = {$1,$3}; $$ = new Fraction(terms, false); }
  | exp POW exp      { Expression * terms[2] = {$1,$3}; $$ = new Power(terms, false); }
  | LEFT_PARENTHESIS exp RIGHT_PARENTHESIS     { $$ = new Parenthesis($2, false); }
  | LEFT_BRACKET mtxData RIGHT_BRACKET { $$ = new Matrix($2); }
  | FUNCTION LEFT_PARENTHESIS exp RIGHT_PARENTHESIS { $$ = $1; Expression * terms[1] = {$3}; $1->setArgument(terms, 1, false); }
  | FUNCTION LEFT_PARENTHESIS exp COMMA exp RIGHT_PARENTHESIS { $$ = $1; Expression * terms[2] = {$3, $5}; $1->setArgument(terms, 2, false); }
;

%%

void poincare_expression_yyerror(Expression ** expressionOutput, char const *msg) {
  // Handle the error!
}
