/* This file should be built with Bison 3.0.4. It might work with other Bison
 * version, but those haven't been tested. */

/* Our lexer and parser are reentrant. That means that their generated functions
 * (such as yylex) will expect a context parameter, so let's tell Bison about
 * it. Note that the context is an opaque pointer. */

/* When calling the parser, we will provide yyparse with an extra parameter : a
 * backpointer to the resulting expression. */
%parse-param { std::vector<Rule *> ** rules }

%{
#include "rule.h"

int yylex();
int yyparse(std::vector<Rule *> ** rules);
int yyerror(std::vector<Rule *> ** rules, const char *s);
%}


/* All symbols (both terminals and non-terminals) may have a value associated
 * with them. In our case, it's going to be either an Expression (for example,
 * when parsing (a/b) we want to create a new Fraction), or a string (this will
 * be useful to retrieve the value of Integers for example). */
%union {
  std::vector<Builder *> * builder_list;
  std::vector<Selector *> * selector_list;
  std::vector<Rule *> * rule_list;
  Rule * rule;
  Selector * selector;
  Builder * builder;
  std::string * string;
}

/* The INTEGER token uses the "string" part of the union to store its value */
%token <string> INTEGER
%token <string> SYMBOL

%token <string> EXPRESSION_TYPE
%token <string> EXPRESSION_GENERATOR
%token <string> WILDCARD
%token <string> VARIABLE

%token SIMPLIFIES_TO
%token LEFT_PARENTHESIS
%token RIGHT_PARENTHESIS
%token COMMA
%token SEMICOLON

%type <rule_list> rule_list;
%type <rule> rule;
%type <selector> selector;
%type <selector_list> selector_list;
%type <builder> builder;
%type <builder_list> builder_list;

/* The "exp" symbol uses the "expression" part of the union. */
/*%type <expression> exp;*/

%%

root:
  rule_list { *rules = $1; }

rule_list:
  rule { $$ = new std::vector<Rule *>(); $$->push_back($1); }
  | rule_list rule { $1->push_back($2); $$ = $1; }

rule:
  selector SIMPLIFIES_TO builder SEMICOLON { $$ = new Rule($1, $3); }

selector:
  VARIABLE { $$ = new Selector(Selector::Type::Variable, $1); }
  | WILDCARD { $$ = new Selector(Selector::Type::Wildcard, $1); }
  | EXPRESSION_TYPE LEFT_PARENTHESIS selector_list RIGHT_PARENTHESIS {
      $$ = new Selector(Selector::Type::ExpressionType, $1, $3);
    }

selector_list:
  selector { $$ = new std::vector<Selector *>(); $$->push_back($1); }
  | selector_list COMMA selector { $1->push_back($3); $$ = $1; }

builder:
  VARIABLE { $$ = new Builder(Builder::Type::Variable, $1); }
  | WILDCARD { $$ = new Builder(Builder::Type::Wildcard, $1); }
  | EXPRESSION_GENERATOR LEFT_PARENTHESIS builder_list RIGHT_PARENTHESIS {
      $$ = new Builder(Builder::Type::ExpressionGenerator, $1, $3);
    }
  | EXPRESSION_TYPE LEFT_PARENTHESIS builder_list RIGHT_PARENTHESIS {
      $$ = new Builder(Builder::Type::ExpressionType, $1, $3);
    }

builder_list:
  builder { $$ = new std::vector<Builder *>(); $$->push_back($1); }
  | builder_list COMMA builder { $1->push_back($3); $$ = $1; }

%%

#include <sstream>
#include <iostream>

int yyerror(std::vector<Rule *> ** rules, const char *s) {
  printf("Error: %s\n",s);
  return 0;
}

int main(void) {
  std::vector<Rule *> * rules = new std::vector<Rule *>();

  yyparse(&rules);

  std::cout << "#include \"simplification_rules.h\"" << std::endl;
  std::cout << "#include \"simplification_generator.h\"" << std::endl;
  std::cout << std::endl;

  int counter = 0;
  for (int i=0; i<rules->size(); i++) {
    std::stringstream name;
    name << "rule" << i;
    rules->at(i)->generate(name.str());
    std::cout << std::endl;
  }
  std::cout << "const Simplification simplifications[" << rules->size() << "] = {" << std::endl;
  for (int i=0; i<rules->size(); i++) {
    std::stringstream name;
    name << "rule" << i;
    std::cout << "  {" << std::endl;
    std::cout << "    .m_selector = (ExpressionSelector *)" << name.str() << "Selector," << std::endl;
    std::cout << "    .m_builder = (ExpressionBuilder *)" << name.str() << "Builder," << std::endl;
    std::cout << "  }," << std::endl;
  }
  std::cout << "};" << std::endl;

  std::cout << std::endl;
  std::cout << "const int knumberOfSimplifications = " << rules->size() << ";" << std::endl;


  delete rules;
  return 0;
}
