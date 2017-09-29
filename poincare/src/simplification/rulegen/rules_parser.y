/* This file should be built with Bison 3.0.4. It might work with other Bison
 * version, but those haven't been tested. */

/* When calling the parser, we will provide yyparse with an extra parameter : a
 * backpointer to the resulting expression. */
%parse-param { ParserResult * result }

%{
#include "rule.h"

struct ParserResult {
  std::string * name;
  std::vector<Rule *> * rules;
};

int yylex();
int yyparse(ParserResult * result);
int yyerror(ParserResult * result, const char * s);
%}

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
%token BANG
%token DOLLAR
%token ASTERISK

/* The "rule_list" symbol uses the "rule_list" part of the union, and so on */
%type <rule_list> rule_list;
%type <rule> rule;
%type <node> node;
%type <node_list> node_list;

%%

root:
  CAPITALIZED_IDENTIFIER rule_list { result->name = $1; result->rules = $2; }

rule_list:
  rule { $$ = new std::vector<Rule *>(); $$->push_back($1); }
  | rule_list rule { $1->push_back($2); $$ = $1; }

rule:
  node SIMPLIFIES_TO node SEMICOLON { $$ = new Rule($1, $3); }

node:
  CAPITALIZED_IDENTIFIER {
      $$ = new Node($1, nullptr, nullptr);
    }
  | CAPITALIZED_IDENTIFIER PERIOD IDENTIFIER {
      $$ = new Node($1, $3, nullptr);
    }
  | CAPITALIZED_IDENTIFIER LEFT_BRACKET VALUE RIGHT_BRACKET {
      $$ = new Node($1, nullptr, $3);
    }
  | CAPITALIZED_IDENTIFIER PERIOD IDENTIFIER LEFT_BRACKET VALUE RIGHT_BRACKET {
      $$ = new Node($1, $3, $5);
    }
  | CAPITALIZED_IDENTIFIER BANG {
      $$ = new Node($1, nullptr, nullptr, false);
    }
  | CAPITALIZED_IDENTIFIER BANG PERIOD IDENTIFIER {
      $$ = new Node($1, $4, nullptr, false);
    }
  | CAPITALIZED_IDENTIFIER BANG LEFT_BRACKET VALUE RIGHT_BRACKET {
      $$ = new Node($1, nullptr, $4, false);
    }
  | IDENTIFIER {
      $$ = new Node(nullptr, $1, nullptr);
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

int yyerror(ParserResult * result, const char *s) {
  printf("Error: %s\n",s);
  return 0;
}

int main(void) {
  ParserResult result;
  //std::vector<Rule *> * rules = new std::vector<Rule *>();

  yyparse(&result);

  std::cout << "#include \"ruleset.h\"" << std::endl << std::endl;
  std::cout << "#include \"transform/transform.h\"" << std::endl << std::endl;

  std::cout << "namespace Poincare {" << std::endl;
  std::cout << "namespace Simplification {" << std::endl << std::endl;

  std::cout << "namespace " << *(result.name) << "Rules {" << std::endl << std::endl;

  for (int i=0; i<result.rules->size(); i++) {
    result.rules->at(i)->generate(i);
    std::cout << std::endl;
  }

  std::cout << "constexpr Rule rules[" << result.rules->size() << "] = {";
  for (int i=0; i<result.rules->size(); i++) {
    std::cout << "Rule" << i << "::rule";
    if (i+1 != result.rules->size()) {
      std::cout << ", ";
    }
  }
  std::cout << "};" << std::endl;
  std::cout << "};" << std::endl << std::endl;
  std::cout << "constexpr Ruleset " << *(result.name) << "(" << *(result.name) << "Rules::rules, " << result.rules->size() << ");" << std::endl;
  std::cout << std::endl << "}" << std::endl << "}" << std::endl;

#if 0

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

#endif
  return 0;
}
