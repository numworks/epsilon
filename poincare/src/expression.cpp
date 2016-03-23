#include <poincare/expression.h>
#include "expression_parser.hpp"
#include "expression_lexer.hpp"
#include "simplify/simplifier.h"
#include "simplify/simplifier_zero.h"

static expression_simplifier_t kSimplifiers[] = {
  &SimplifierZero,
  nullptr
};

int poincare_expression_yyparse(yyscan_t scanner, Expression ** expressionOutput);

Expression::~Expression() {
}

Expression * Expression::parse(char * string) {
  void * scanner;
  poincare_expression_yylex_init(&scanner);
  YY_BUFFER_STATE buf = poincare_expression_yy_scan_string(string, scanner);
  Expression * expression = 0;
  poincare_expression_yyparse(scanner, &expression);
  poincare_expression_yy_delete_buffer(buf, scanner);
  poincare_expression_yylex_destroy(scanner);

  return expression;
}

Expression * Expression::simplify() {
  Expression * result = this;
  expression_simplifier_t * simplifier_pointer = kSimplifiers;
  while (expression_simplifier_t simplifier = *simplifier_pointer) {
    Expression * simplification = simplifier(result);
    if (simplification != nullptr) {
      if (result != this) {
        delete result;
      }
      result = simplification;
    }
    simplifier_pointer++;
  }

  return result;
}

