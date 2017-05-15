/* This file should be built with Bison 3.0.4. It might work with other Bison
 * version, but those haven't been tested. */

/* When calling the parser, we will provide yyparse with an extra parameter : a
 * backpointer to the resulting expression. */
%parse-param { Poincare::Expression ** expressionOutput }

%{
#include <poincare.h>
/* The lexer manipulates tokens defined by the parser, so we need the following
 * inclusion order. */
#include "expression_parser.hpp"
#include "expression_lexer.hpp"

/* Declare our error-handling function. Since we're making a re-entrant parser,
 * it takes a context parameter as its first input. */
void poincare_expression_yyerror(Poincare::Expression ** expressionOutput, char const *msg);

/* Bison expects to use __builtin_memcpy. We don't want to provide this, but
 * instead we do provide regular memcpy. Let's instruct Bison to use it. */
#define YYCOPY(To, From, Count) memcpy(To, From, (Count)*sizeof(*(From)))

%}


/* All symbols (both terminals and non-terminals) may have a value associated
 * with them. In our case, it's going to be either an Expression (for example,
 * when parsing (a/b) we want to create a new Fraction), or a string (this will
 * be useful to retrieve the value of Integers for example). */
%union {
  Poincare::Expression * expression;
  Poincare::Symbol * symbol;
  Poincare::ListData * listData;
  Poincare::MatrixData * matrixData;
  Poincare::Function * function;
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
%token <character> SYMBOL
%token <function> FUNCTION
%token <expression> UNDEFINED

/* Operator tokens */
%token PLUS
%token MINUS
%token MULTIPLY
%token DIVIDE
%token POW
%token BANG
%token LEFT_PARENTHESIS
%token RIGHT_PARENTHESIS
%token LEFT_BRACE
%token RIGHT_BRACE
%token LEFT_BRACKET
%token RIGHT_BRACKET
%token COMMA
%token DOT
%token EE
%token ICOMPLEX
%token STO
%token UNDEFINED_SYMBOL

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
%nonassoc STO
%left PLUS
%left MINUS
%left IMPLICIT_MULTIPLY
%left UNARY_MINUS
%left MULTIPLY
%left DIVIDE
%left POW
%left BANG
%left LEFT_PARENTHESIS
%left RIGHT_PARENTHESIS
%left LEFT_BRACKET
%left RIGHT_BRACKET
%left FUNCTION
%left COMMA
%left DIGITS
%left DOT
%left EE
%left ICOMPLEX
%left UNDEFINED_SYMBOL
%left SYMBOL

/* The "exp" symbol uses the "expression" part of the union. */
%type <expression> final_exp;
%type <expression> exp;
%type <expression> number;
%type <symbol> symb;
%type <listData> lstData;
/* MATRICES_ARE_DEFINED */
/*%type <matrixData> mtxData;*/

/* During error recovery, some symbols need to be discarded. We need to tell
 * Bison how to get rid of them. Depending on the type of the symbol, it may
 * have some heap-allocated data that need to be discarded. */

%destructor { delete $$; } FUNCTION
%destructor { delete $$; } UNDEFINED final_exp exp number
%destructor { delete $$; } lstData
/* MATRICES_ARE_DEFINED */
/*%destructor { delete $$; } mtxData*/
%destructor { delete $$; } symb

%%

Root:
  final_exp {
    *expressionOutput = $1;
  }

/* Note that in bison, precedence of parsing depend on the order they are defined in here, the last
 * one has the highest precedence. */

lstData:
  exp { $$ = new Poincare::ListData($1); }
  | lstData COMMA exp { $$ = $1; $$->pushExpression($3); }
/* MATRICES_ARE_DEFINED */
/* mtxData:
  LEFT_BRACKET lstData RIGHT_BRACKET { $$ = new Poincare::MatrixData($2, true); delete $2; }
  | mtxData LEFT_BRACKET lstData RIGHT_BRACKET  { $$ = $1; $$->pushListData($3, true); delete $3; }*/

number:
  DIGITS { $$ = new Poincare::Integer($1.address, false); }
  | DOT DIGITS { $$ = new Poincare::Complex(nullptr, 0, false, $2.address, $2.length, nullptr, 0, false); }
  | DIGITS DOT DIGITS { $$ = new Poincare::Complex($1.address, $1.length, false, $3.address, $3.length, nullptr, 0, false); }
  | DOT DIGITS EE DIGITS { $$ = new Poincare::Complex(nullptr, 0, false, $2.address, $2.length, $4.address, $4.length, false); }
  | DIGITS DOT DIGITS EE DIGITS { $$ = new Poincare::Complex($1.address, $1.length, false, $3.address, $3.length, $5.address, $5.length, false); }
  | DIGITS EE DIGITS { $$ = new Poincare::Complex($1.address, $1.length, false, nullptr, 0, $3.address, $3.length, false); }
  | DOT DIGITS EE MINUS DIGITS { $$ = new Poincare::Complex(nullptr, 0, false, $2.address, $2.length, $5.address, $5.length, true); }
  | DIGITS DOT DIGITS EE MINUS DIGITS { $$ = new Poincare::Complex($1.address, $1.length, false, $3.address, $3.length, $6.address, $6.length, true); }
  | DIGITS EE MINUS DIGITS { $$ = new Poincare::Complex($1.address, $1.length, false, nullptr, 0, $4.address, $4.length, true); }

symb:
  SYMBOL           { $$ = new Poincare::Symbol($1); }

exp:
  UNDEFINED        { $$ = $1; }
  | exp BANG       { $$ = new Poincare::Factorial($1, false); }
  | number             { $$ = $1; }
  | ICOMPLEX         { $$ = new Poincare::Complex(Poincare::Complex::Cartesian(0.0f, 1.0f)); }
  | symb           { $$ = $1; }
  | exp PLUS exp     { Poincare::Expression * terms[2] = {$1,$3}; $$ = new Poincare::Addition(terms, false); }
  | exp MINUS exp    { Poincare::Expression * terms[2] = {$1,$3}; $$ = new Poincare::Subtraction(terms, false); }
  | exp MULTIPLY exp { Poincare::Expression * terms[2] = {$1,$3}; $$ = new Poincare::Multiplication(terms, false);  }
  | exp exp %prec IMPLICIT_MULTIPLY  { Poincare::Expression * terms[2] = {$1,$2}; $$ = new Poincare::Multiplication(terms, false);  }
  | exp DIVIDE exp   { Poincare::Expression * terms[2] = {$1,$3}; $$ = new Poincare::Fraction(terms, false); }
  | exp POW exp      { Poincare::Expression * terms[2] = {$1,$3}; $$ = new Poincare::Power(terms, false); }
  | MINUS exp %prec UNARY_MINUS           { $$ = new Poincare::Opposite($2, false); }
  | LEFT_PARENTHESIS exp RIGHT_PARENTHESIS     { $$ = new Poincare::Parenthesis($2, false); }
/* MATRICES_ARE_DEFINED */
/*  | LEFT_BRACKET mtxData RIGHT_BRACKET { $$ = new Poincare::Matrix($2); } */
  | FUNCTION LEFT_PARENTHESIS lstData RIGHT_PARENTHESIS { $$ = $1; $1->setArgument($3, true); delete $3; }

final_exp:
  exp      { $$ = $1; }
  | exp STO symb   { $$ = new Poincare::Store($3, $1, false); };
%%

void poincare_expression_yyerror(Poincare::Expression ** expressionOutput, char const *msg) {
  // Handle the error!
}
