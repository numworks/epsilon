#include <poincare/expression.h>
#include "expression_parser.hpp"
#include "expression_lexer.hpp"
extern "C" {
#include <assert.h>
}

#include "simplify/simplification_rules.h"

int poincare_expression_yyparse(yyscan_t scanner, Expression ** expressionOutput);

Expression::~Expression() {
}

Expression * Expression::parse(char const * string) {
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
  bool simplification_pass_was_useful = true;
  while (simplification_pass_was_useful) {
    simplification_pass_was_useful = false;
    for (int i=0; i<knumberOfSimplifications; i++) {
      const Simplification * simplification = (simplifications + i); // Pointer arithmetics
      Expression * simplified = simplification->simplify(result);
      if (simplified != nullptr) {
        simplification_pass_was_useful = true;
        if (result != this) {
          delete result;
        }
        result = simplified;
      }
    }
  }
  return result;
}

bool Expression::isIdenticalTo(Expression * e) {
  if (e->type() != this->type() || e->numberOfOperands() != this->numberOfOperands()) {
    return false;
  }
  for (int i=0; i<this->numberOfOperands(); i++) {
    if (!e->operand(i)->isIdenticalTo(this->operand(i))) {
      return false;
    }
  }
  return e->valueEquals(this);
}

bool Expression::valueEquals(Expression * e) {
  assert(this->type() == e->type());
  /* This behavior makes sense for value-less nodes (addition, product, fraction
   * power, etc… For nodes with a value (Integer, Float), this must be over-
   * -riden. */
  return true;
}
