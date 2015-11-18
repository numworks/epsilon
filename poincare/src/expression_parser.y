/* This file has been tested with Bison 2.3. It should work with newer versions
 * but it hasn't been tested. */
%{
/* We will be generating all kind of Expressions, so we need their definitions
 * here. */
#include <poincare.h>

/* expression_lexer.hpp expects YYSTYPE to be defined. It is defined in the
 * expression_parser.hpp file, which must be included before. */
#include "expression_parser.hpp"
#include "expression_lexer.hpp"


/* Declare our error-handling function. Since we're making a re-entrant parser,
 * it takes a "context" parameter as its first input. */
void poincare_expression_yyerror(yyscan_t scanner, Expression ** expressionOutput, char const *msg);

/* Bison expects to use __builtin_memcpy. We don't want to provide this, but
 * instead we do provide regular memcpy. Let's instruct Bison to use it. */
#define YYCOPY(To, From, Count) memcpy(To, From, (Count)*sizeof(*(From)))

/*
  #include <private/css_selector.h>
  using namespace ui::Private;
  #include "css_selector_parser.hpp"
  #include "css_selector_lexer.hpp"
  CSSSelector::Combinator awe_css_combinator_from_string(std::string string);
  void awe_css_yyerror(yyscan_t scanner, CSSSelector ** selector, char const *msg);
*/
%}

/* We want a reentrant parser. Otherwise Bison makes use of global variables,
 * which we want to avoid. */
%pure-parser

/* Our lexer and parser are reentrant. That means that their generated functions
 * (such as *yylex) take a "context" as an input parameter. We're telling Bison
 * That this input will be a "yyscan_t" named "scanner". */
%lex-param   { yyscan_t scanner }
%parse-param { yyscan_t scanner }

/* When calling the parser, we will provide yyparse with a backpointer to the
 * resulting expression. */
%parse-param { Expression ** expressionOutput }


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

/* The DIVIDE and POW tokens use no value */
%token DIVIDE
%token MULTIPLY
%token POW
%token PLUS

/* The "exp" symbol uses the "expression" part of the union. */
%type <expression> exp;

/*
  //Expression * expression;
  //CSSSelector * selector;
%union {
  int value;
  char * string;
}

%token

%right <string> COMBINATOR
%token <string> IDENT
%token HASH
%token DOT

%type <selector> Expression;
*/


%%

Root:
  exp {
    *expressionOutput = $1;
  }

exp:
  INTEGER            { $$ = new Integer($1);     }
  | SYMBOL           { $$ = new Symbol($1);    }
  | exp DIVIDE exp   { $$ = new Fraction($1,$3); }
  | exp MULTIPLY exp { $$ = new Product($1,$3);  }
  | exp PLUS exp     { $$ = new Addition($1,$3); }
/*  | exp POW exp    { $$ = new Power($1,$3); } */
;

/*

Root:
  Selector {
    *selector = $1;
    //printf("ROOT! : %p\n", $1);
  }

Selector:
     {
      //$$ = new CSSSelector();
      //printf("New selector at %p\n", $$);
    }
  | IDENT {
      //$$ = new CSSSelector();
      //$$->setNameRequirement(*$1);
      //delete $1;
      //printf("Create NodeSpecWithName %s at %p\n", $1, $$);
    }
  | Selector DOT IDENT {
      //$1->addClassRequirement(*$3);
      //delete $3;
      //printf("AddClassToNodeSpec %s\n", $3);
    }
  | Selector HASH IDENT {
      //$1->setIdentifierRequirement(*$3);
      //delete $3;
      //printf("AddIdToNodeSpec : %s\n", $3);
    }
  | Selector COMBINATOR Selector {
      //$3->setParentSelectorWithCombinator($1, awe_css_combinator_from_string(*$2));
      //printf("Combine with \"%s\"\n", $2->c_str());
      //delete $2;
    }
  ;

*/
%%

void poincare_expression_yyerror(yyscan_t scanner, Expression ** expressionOutput, char const *msg) {
  // Handle the error!
}

/*

CSSSelector::Combinator awe_css_combinator_from_string(std::string string) {
  for (char& c : string) {
    switch (c) {
      case '>':
        return CSSSelector::Combinator::DIRECT;
    }
  }
  return CSSSelector::Combinator::ANY;
}

void awe_css_yyerror(yyscan_t scanner, CSSSelector ** selector, char const *msg) {
  printf("ERROR %s\n",msg);
}
*/

