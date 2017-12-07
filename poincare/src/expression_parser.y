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
 * when parsing (a/b) we want to create a new Division), or a string (this will
 * be useful to retrieve the value of Integers for example). */
%union {
  Poincare::Expression * expression;
  Poincare::Symbol * symbol;
  Poincare::ListData * listData;
  Poincare::MatrixData * matrixData;
  Poincare::StaticHierarchy<0> * function;
  /* Caution: all the const char * are NOT guaranteed to be NULL-terminated!
   * While Flex guarantees that yytext is NULL-terminated when building tokens,
   * it does so by temporarily swapping in a NULL terminated in the input text.
   * Of course that hack has vanished by the time the pointer is fed into Bison.
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
%token <string> RADIX_DIGITS
%token <character> SYMBOL
%token <function> FUNCTION
%token <expression> UNDEFINED
%token <expression> EMPTY

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
%token COLON
%token COMMA
%token UNDERSCORE
%token DOT
%token EE
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

/* Note that in bison, precedence of parsing depend on the order they are defined in here, the last
 * one has the highest precedence. */

%nonassoc STO
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
%nonassoc FUNCTION
%left COMMA
%nonassoc UNDERSCORE
%nonassoc DIGITS
%nonassoc RADIX_DIGITS
%nonassoc DOT
%nonassoc EE
%nonassoc UNDEFINED
%nonassoc SYMBOL
%nonassoc EMPTY

/* The "exp" symbol uses the "expression" part of the union. */
%type <expression> final_exp;
%type <expression> term;
%type <expression> bang;
%type <expression> factor;
%type <expression> pow;
%type <expression> exp;
%type <expression> number;
%type <symbol> symb;
%type <listData> lstData;
/* MATRICES_ARE_DEFINED */
%type <matrixData> mtxData;

/* During error recovery, some symbols need to be discarded. We need to tell
 * Bison how to get rid of them. Depending on the type of the symbol, it may
 * have some heap-allocated data that need to be discarded. */

%destructor { delete $$; } FUNCTION
%destructor { delete $$; } UNDEFINED final_exp exp pow factor bang term number EMPTY
%destructor { delete $$; } lstData
/* MATRICES_ARE_DEFINED */
%destructor { delete $$; } mtxData
%destructor { delete $$; } symb

%%

Root:
  final_exp {
    *expressionOutput = $1;
  }
  ;

lstData: exp { $$ = new Poincare::ListData($1); }
       | lstData COMMA exp { $$ = $1; $$->pushExpression($3); }
       ;

/* MATRICES_ARE_DEFINED */
mtxData: LEFT_BRACKET lstData RIGHT_BRACKET { $$ = new Poincare::MatrixData($2, false); $2->detachOperands(); delete $2; }
       | mtxData LEFT_BRACKET lstData RIGHT_BRACKET  { if ($3->numberOfOperands() != $1->numberOfColumns()) { delete $1; delete $3; YYERROR; } ; $$ = $1; $$->pushListData($3, false); $3->detachOperands(); delete $3; }
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
number : DIGITS { $$ = new Poincare::Rational(Poincare::Integer($1.address, false)); }
       | DOT DIGITS { $$ = new Poincare::Decimal(Poincare::Decimal::mantissa(nullptr, 0, $2.address, $2.length, false), Poincare::Decimal::exponent(nullptr, 0, $2.address, $2.length, nullptr, 0, false)); }
       | DIGITS DOT DIGITS { $$ = new Poincare::Decimal(Poincare::Decimal::mantissa($1.address, $1.length, $3.address, $3.length, false), Poincare::Decimal::exponent($1.address, $1.length, $3.address, $3.length, nullptr, 0, false)); }
       | DOT DIGITS EE DIGITS { if ($4.length > 4) { YYERROR; }; int exponent = Poincare::Decimal::exponent(nullptr, 0, $2.address, $2.length, $4.address, $4.length, false); if (exponent > 1000 || exponent < -1000 ) { YYERROR; }; $$ = new Poincare::Decimal(Poincare::Decimal::mantissa(nullptr, 0, $2.address, $2.length, false), exponent); }
       | DIGITS DOT DIGITS EE DIGITS { if ($5.length > 4) { YYERROR; }; int exponent = Poincare::Decimal::exponent($1.address, $1.length, $3.address, $3.length, $5.address, $5.length, false); if (exponent > 1000 || exponent < -1000 ) { YYERROR; }; $$ = new Poincare::Decimal(Poincare::Decimal::mantissa($1.address, $1.length, $3.address, $3.length, false), exponent); }
       | DIGITS EE DIGITS { if ($3.length > 4) { YYERROR; }; int exponent = Poincare::Decimal::exponent($1.address, $1.length, nullptr, 0, $3.address, $3.length, false); if (exponent > 1000 || exponent < -1000 ) { YYERROR; }; $$ = new Poincare::Decimal(Poincare::Decimal::mantissa($1.address, $1.length, nullptr, 0, false), exponent); }
       | DOT DIGITS EE MINUS DIGITS { if ($5.length > 4) { YYERROR; }; int exponent = Poincare::Decimal::exponent(nullptr, 0, $2.address, $2.length, $5.address, $5.length, true);  if (exponent > 1000 || exponent < -1000 ) { YYERROR; }; $$ = new Poincare::Decimal(Poincare::Decimal::mantissa(nullptr, 0, $2.address, $2.length, false), exponent); }
       | DIGITS DOT DIGITS EE MINUS DIGITS { if ($6.length > 4) { YYERROR; }; int exponent = Poincare::Decimal::exponent($1.address, $1.length, $3.address, $3.length, $6.address, $6.length, true); if (exponent > 1000 || exponent < -1000 ) { YYERROR; }; $$ = new Poincare::Decimal(Poincare::Decimal::mantissa($1.address, $1.length, $3.address, $3.length, false), exponent); }
       | DIGITS EE MINUS DIGITS { if ($4.length > 4) { YYERROR; }; int exponent =  Poincare::Decimal::exponent($1.address, $1.length, nullptr, 0, $4.address, $4.length, true); if (exponent > 1000 || exponent < -1000 ) { YYERROR; }; $$ = new Poincare::Decimal(Poincare::Decimal::mantissa($1.address, $1.length, nullptr, 0, false), exponent); }
       | RADIX_DIGITS {
          Poincare::Integer baseDigits(strchr($1.address, ':')+1, false);
          if (baseDigits.isLowerThan(Poincare::Integer(2)) || Poincare::Integer(36).isLowerThan(baseDigits)) {
            YYERROR;
          }
          int base;
          for (base = 2; !baseDigits.isEqualTo(Poincare::Integer(base)); base++);
          for (int i = 0; $1.address[i] != ':'; i++) {
            int digit = ($1.address[i] >= '0' && $1.address[i] <= '9') ? $1.address[i] - '0' : $1.address[i] - 'a' + 10;
            if (digit >= base) {
              YYERROR;
            }
          }
          $$ = new Poincare::Rational(Poincare::Integer($1.address, false, base));
        }
       ;

symb   : SYMBOL         { $$ = new Poincare::Symbol($1); }
       ;

term   : EMPTY          { $$ = $1; }
       | symb           { $$ = $1; }
       | UNDEFINED      { $$ = $1; }
       | number         { $$ = $1; }
       | FUNCTION UNDERSCORE LEFT_BRACE lstData RIGHT_BRACE LEFT_PARENTHESIS lstData RIGHT_PARENTHESIS { $$ = $1; int totalNumberOfArguments = $4->numberOfOperands()+$7->numberOfOperands();
if (!$1->hasValidNumberOfOperands(totalNumberOfArguments)) { delete $1; delete $4; delete $7; YYERROR; };
Poincare::ListData * arguments = new Poincare::ListData();
for (int i = 0; i < $7->numberOfOperands(); i++) { arguments->pushExpression($7->operands()[i]); }
for (int i = 0; i < $4->numberOfOperands(); i++) { arguments->pushExpression($4->operands()[i]); }
$1->setArgument(arguments, totalNumberOfArguments, false);
$4->detachOperands(); delete $4; $7->detachOperands(); delete $7; arguments->detachOperands(); delete arguments;}
       | FUNCTION LEFT_PARENTHESIS lstData RIGHT_PARENTHESIS { $$ = $1; if (!$1->hasValidNumberOfOperands($3->numberOfOperands())) { delete $1; delete $3; YYERROR; } ; $1->setArgument($3, $3->numberOfOperands(), false); $3->detachOperands(); delete $3; }
       | FUNCTION LEFT_PARENTHESIS RIGHT_PARENTHESIS { $$ = $1; if (!$1->hasValidNumberOfOperands(0)) { delete $1; YYERROR; } ; }
       | LEFT_PARENTHESIS exp RIGHT_PARENTHESIS     { Poincare::Expression * terms[1] = {$2}; $$ = new Poincare::Parenthesis(terms, false); }
/* MATRICES_ARE_DEFINED */
       | LEFT_BRACKET mtxData RIGHT_BRACKET { $$ = new Poincare::Matrix($2); delete $2; }
       ;

bang   : term               { $$ = $1; }
       | term BANG          { $$ = new Poincare::Factorial($1, false); }
       ;

factor : bang               { $$ = $1; }
       | bang pow %prec IMPLICIT_MULTIPLY { Poincare::Expression * terms[2] = {$1,$2}; $$ = new Poincare::Multiplication(terms, 2, false);  }
       ;

pow    : factor             { $$ = $1; }
       | bang POW pow       { Poincare::Expression * terms[2] = {$1,$3}; $$ = new Poincare::Power(terms, false); }
       | bang POW MINUS pow { Poincare::Expression * terms1[1] = {$4}; Poincare::Expression * terms[2] = {$1,new Poincare::Opposite(terms1, false)}; $$ = new Poincare::Power(terms, false); }
       ;

exp    : pow                { $$ = $1; }
       | exp DIVIDE exp     { Poincare::Expression * terms[2] = {$1,$3}; $$ = new Poincare::Division(terms, false);  }
       | exp MULTIPLY exp   { Poincare::Expression * terms[2] = {$1,$3}; $$ = new Poincare::Multiplication(terms, 2, false);  }
       | exp MINUS exp      { Poincare::Expression * terms[2] = {$1,$3}; $$ = new Poincare::Subtraction(terms, false); }
       | MINUS exp %prec UNARY_MINUS { Poincare::Expression * terms[1] = {$2}; $$ = new Poincare::Opposite(terms, false); }
       | exp PLUS exp     { Poincare::Expression * terms[2] = {$1,$3}; $$ = new Poincare::Addition(terms, 2, false); }
       ;

final_exp : exp             { $$ = $1; }
          | exp STO symb   { if ($3->name() == Poincare::Symbol::SpecialSymbols::Ans) { delete $1; delete $3; YYERROR; } ; Poincare::Expression * terms[2] = {$1,$3}; $$ = new Poincare::Store(terms, false); }
          | exp STO COLON DIGITS { Poincare::Expression * terms[2] = {$1,new Poincare::Rational(Poincare::Integer($4.address, false))}; $$ = new Poincare::ToBase(terms, false); }
          ;
%%

void poincare_expression_yyerror(Poincare::Expression ** expressionOutput, const char * msg) {
  // Handle the error!
  // TODO: handle explicitely different type of errors (division by 0, missing parenthesis). This should call back the container to display a pop up with a message corresponding to the error?
}
