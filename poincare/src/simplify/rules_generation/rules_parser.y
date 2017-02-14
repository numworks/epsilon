/* This file should be built with Bison 3.0.4. It might work with other Bison
 * version, but those haven't been tested. */

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
  std::vector<Rule *> * rule_list;
  std::vector<Node *> * node_list;
  Rule * rule;
  Node * node;
  std::string * string;
}

/* The IDENTIFIER, CAPITALIZED_IDENTIFIER and VALUE token uses the "string" part
 * of the union to store their value */

%token <string> IDENTIFIER
%token <string> CAPITALIZED_IDENTIFIER
%token <string> VALUE

/* Some tokens don't store any value */

%token SIMPLIFIES_TO
%token LEFT_PARENTHESIS
%token RIGHT_PARENTHESIS
%token LEFT_BRACKET
%token RIGHT_BRACKET
%token COMMA
%token SEMICOLON
%token PERIOD
%token DOLLAR
%token ASTERISK

/* The "rule_list" symbol uses the "rule_list" part of the union, and so on */
%type <rule_list> rule_list;
%type <rule> rule;
%type <node> node;
%type <node_list> node_list;

%%

root:
  rule_list { *rules = $1; }

rule_list:
  rule { $$ = new std::vector<Rule *>(); $$->push_back($1); }
  | rule_list rule { $1->push_back($2); $$ = $1; }

rule:
  node SIMPLIFIES_TO node SEMICOLON { $$ = new Rule($1, $3); }

node:
  CAPITALIZED_IDENTIFIER {
      $$ = new Node(Node::Type::Expression, $1);
    }
  | DOLLAR CAPITALIZED_IDENTIFIER {
      $$ = new Node(Node::Type::Generator, $2);
    }
  | IDENTIFIER {
      $$ = new Node(Node::Type::Any);
      $$->setReference(Node::ReferenceMode::SingleNode, $1);
    }
  | IDENTIFIER ASTERISK {
      $$ = new Node(Node::Type::Any);
      $$->setReference(Node::ReferenceMode::Wildcard, $1);
    }
  | node PERIOD IDENTIFIER {
      $$ = $1;
      $$->setReference(Node::ReferenceMode::SingleNode, $3);
    }
  | node LEFT_BRACKET VALUE RIGHT_BRACKET {
      $$ = $1;
      $$->setValue($3);
    }
  | node LEFT_PARENTHESIS node_list RIGHT_PARENTHESIS {
      $$ = $1;
      $$->setChildren($3);
    }

node_list:
  node { $$ = new std::vector<Node *>(); $$->push_back($1); }
  | node_list COMMA node { $1->push_back($3); $$ = $1; }

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

  std::cout << "#include \"rules.h\"" << std::endl;
  std::cout << "#include \"simplification_generator.h\"" << std::endl;
  std::cout << std::endl;
  std::cout << "using namespace Poincare;" << std::endl;
  std::cout << std::endl;

  int counter = 0;
  for (int i=0; i<rules->size(); i++) {
    std::stringstream name;
    name << "rule" << i;
    rules->at(i)->generate(name.str());
    std::cout << std::endl;
  }
  std::cout << "constexpr Simplification Poincare::simplifications[" << rules->size() << "] = {" << std::endl;
  for (int i=0; i<rules->size(); i++) {
    std::stringstream name;
    name << "rule" << i;
std::cout << "  Simplification((ExpressionSelector *)" << name.str() << "Selector, (ExpressionBuilder *)" << name.str() << "Builder)," << std::endl;
  }
  std::cout << "};" << std::endl;

  std::cout << std::endl;
  std::cout << "constexpr int Poincare::knumberOfSimplifications = " << rules->size() << ";" << std::endl;

  delete rules;
  return 0;
}
