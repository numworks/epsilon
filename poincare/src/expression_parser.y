/* This file should be built with Bison 3.0.4. It might work with other Bison
 * version, but those haven't been tested. */

/* When calling the parser, we will provide yyparse with an extra parameter : a
 * backpointer to the resulting expression. */
%parse-param { Poincare::Expression * expressionOutput }

/* The value stored in each token is an Expresssion, which is a complex C++
 * object. If we use a global variable to keep track of yylval, this object will
 * be long-lived AND will need to be initialized at startup.
 * Both those behaviors are annoying: the first one because you need to delete
 * the object if an exception happens, the second one because of the static init
 * order fiasco.
 * Using a pure parser (i.e. one without any global variable) fixes this. */
%pure-parser

%{
#include <poincare_nodes.h>

/* YYSTYPE has to be defined in expression_lexer.y, expression.cpp and here
 * because the genereated code is included in expression_parser.cpp, not .hpp
 * so we use an additional header which only define YYSTYPE. This should be
 * discarded with Bison 3. */
#include "expression_lexer_parser.h"

/* The lexer manipulates tokens defined by the parser, so we need the following
 * inclusion order. */
#include "expression_parser.hpp"
#include "expression_lexer.hpp"

/* Declare our error-handling function. Since we're making a re-entrant parser,
 * it takes a context parameter as its first input. */
void poincare_expression_yyerror(Poincare::Expression * expressionOutput, char const *msg);

/* Bison expects to use __builtin_memcpy. We don't want to provide this, but
 * instead we do provide regular memcpy. Let's instruct Bison to use it. */
#define YYCOPY(To, From, Count) memcpy(To, From, (Count)*sizeof(*(From)))

using namespace Poincare;

%}

/* The INTEGER token uses the "string" part of the union to store its value */


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

/* Note that in bison, precedence of parsing depend on the order they are defined in here, the last
 * one has the highest precedence. */

%nonassoc EQUAL STO
%left PLUS
%left MINUS
%left MULTIPLY
%left DIVIDE
%left IMPLICIT_MULTIPLY
%nonassoc UNARY_MINUS
%right POW
%left BANG
%nonassoc LEFT_PARENTHESIS
%nonassoc RIGHT_PARENTHESIS
%nonassoc LEFT_BRACKET
%nonassoc RIGHT_BRACKET
%nonassoc LEFT_BRACE
%nonassoc RIGHT_BRACE
%nonassoc FUNCTION LOGFUNCTION
%left COMMA
%nonassoc UNDERSCORE
%nonassoc DIGITS
%nonassoc TERM
%nonassoc SYMBOL
%nonassoc UNDEFINED_SYMBOL

/* During error recovery, some symbols need to be discarded. No destructor need
 * to be specified to Bison because all symbols are Poincare::Expression that
 * are garbage collected. */

%%

Root: final_exp { *expressionOutput = $1; }
    ;

lstData: exp               { $$ = Matrix($1); }
       | lstData COMMA exp { $$ = $1; static_cast<Matrix &>($$).addChildAtIndexInPlace($3, $$.numberOfChildren(), $$.numberOfChildren()); }
       ;

/* MATRICES_ARE_DEFINED */
mtxData: LEFT_BRACKET lstData RIGHT_BRACKET { $$ = Matrix::EmptyMatrix(); static_cast<Matrix &>($$).addChildrenAsRowInPlace($2, 0); }
       | mtxData LEFT_BRACKET lstData RIGHT_BRACKET  { if ($3.numberOfChildren() != static_cast<Matrix &>($1).numberOfColumns()) { YYERROR; } ; $$ = $1; static_cast<Matrix &>($$).addChildrenAsRowInPlace($3, $$.numberOfChildren()); }
       ;

/* When approximating expressions to double, results are bounded by 1E308 (and
 * 1E-308 for small numbers). We thus accept decimals whose exponents are in
 * {-1000, 1000}. However, we have to compute the exponent first to decide
 * whether to accept the decimal. The exponent of a Decimal is stored as an
 * int32_t. We thus have to throw an error when the exponent computation might
 * overflow. Finally, we escape computation by throwing an error when the length
 * of the exponent digits is above 4 (0.00...-256 times-...01E1256=1E1000 is
 * accepted and 1000-...256times...-0E10000 = 1E10256, 10256 does not overflow
 * an int32_t). */
number : DIGITS { $$ = $1; }
       | DIGITS DIGITS { YYERROR; }
       ;

symb   : SYMBOL { $$ = $1; }
       ;

term   : TERM   { $$ = $1; }
       | symb   { $$ = $1; }
       | number { $$ = $1; }
       | LOGFUNCTION UNDERSCORE LEFT_BRACE exp RIGHT_BRACE LEFT_PARENTHESIS exp RIGHT_PARENTHESIS { $$ = Logarithm($7, $4); }
       | FUNCTION LEFT_PARENTHESIS lstData RIGHT_PARENTHESIS { $$ = $1; if ($$.numberOfChildren() != ($3.numberOfChildren())) { YYERROR; } ; $$.setChildrenInPlace($3); }
/* Special case for logarithm, as we do not at first if it needs 1 or 2 children */
       | LOGFUNCTION LEFT_PARENTHESIS lstData RIGHT_PARENTHESIS { if ($3.numberOfChildren() == 1) { $$ = Logarithm($3.childAtIndex(0)); } else if ($3.numberOfChildren() == 2) { $$ = Logarithm($3.childAtIndex(0), $3.childAtIndex(1));} else { YYERROR; } ; }
       | FUNCTION LEFT_PARENTHESIS RIGHT_PARENTHESIS { if ($1.numberOfChildren() != 0) { YYERROR; } $$ = $1; }
       | LEFT_PARENTHESIS exp RIGHT_PARENTHESIS { $$ = Parenthesis($2); }
/* MATRICES_ARE_DEFINED */
       | LEFT_BRACKET mtxData RIGHT_BRACKET { $$ = $2; }
       ;

bang   : term      { $$ = $1; }
       | term BANG { $$ = Factorial($1); }
       ;

factor : bang { $$ = $1; }
       | bang pow %prec IMPLICIT_MULTIPLY { $$ = Multiplication($1, $2); }
       ;

pow    : factor             { $$ = $1; }
       | bang POW pow       { $$ = Power($1,$3); }
       | bang POW MINUS pow { $$ = Power($1,Opposite($4)); }
       ;

exp    : pow              { $$ = $1; }
       | exp DIVIDE exp   { $$ = Division($1,$3); }
       | exp MULTIPLY exp { $$ = Multiplication($1,$3); }
       | exp MINUS exp    { $$ = Subtraction($1,$3); }
       | MINUS exp %prec UNARY_MINUS { $$ = Opposite($2); }
       | exp PLUS exp     { $$ = Addition($1,$3); }
       ;

final_exp : exp           { $$ = $1; }
          | exp STO symb  { if (static_cast<Symbol&>($3).name() == Symbol::SpecialSymbols::Ans) { YYERROR; } ; $$ = Store($1, static_cast<Symbol &>($3)); }
          | exp EQUAL exp { $$ = Equal($1, $3); }
          ;
%%

void poincare_expression_yyerror(Expression * expressionOutput, const char * msg) {
  // Handle the error!
  // TODO: handle explicitely different type of errors (division by 0, missing parenthesis). This should call back the container to display a pop up with a message corresponding to the error?
}
