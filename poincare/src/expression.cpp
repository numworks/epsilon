#include <poincare/expression.h>
#include "expression_parser.hpp"
#include "expression_lexer.hpp"

int poincare_expression_yyparse(yyscan_t scanner, Expression ** expressionOutput);

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

Expression::~Expression() {
}
