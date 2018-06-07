#include <quiz.h>
#include <poincare.h>
#include <string.h>
#include <ion.h>
#include <stdlib.h>
#include <assert.h>
#include <cmath>
#include "helper.h"
#if POINCARE_TESTS_PRINT_EXPRESSIONS
#include "../src/expression_debug.h"
#include <iostream>
using namespace std;
#endif

using namespace Poincare;

void translate_in_special_chars(char * expression) {
  for (char *c = expression; *c; c++) {
    switch (*c) {
      case 'E': *c = Ion::Charset::Exponent; break;
      case 'X': *c = Ion::Charset::Exponential; break;
      case 'I': *c = Ion::Charset::IComplex; break;
      case 'R': *c = Ion::Charset::Root; break;
      case 'P': *c = Ion::Charset::SmallPi; break;
      case '*': *c = Ion::Charset::MultiplicationSign; break;
      case '>': *c = Ion::Charset::Sto; break;
    }
  }
}

void translate_in_ASCII_chars(char * expression) {
  for (char *c = expression; *c; c++) {
    switch (*c) {
      case Ion::Charset::Exponent: *c = 'E'; break;
      case Ion::Charset::Exponential: *c = 'X'; break;
      case Ion::Charset::IComplex: *c = 'I'; break;
      case Ion::Charset::Root: *c = 'R'; break;
      case Ion::Charset::SmallPi: *c = 'P'; break;
      case Ion::Charset::MultiplicationSign: *c = '*'; break;
      case Ion::Charset::Sto: *c = '>'; break;
    }
  }
}

Expression * parse_expression(const char * expression) {
  quiz_print(expression);
  char buffer[200];
  strlcpy(buffer, expression, sizeof(buffer));
  translate_in_special_chars(buffer);
  Expression * result = Expression::parse(buffer);
  assert(result);
  return result;
}

void assert_parsed_expression_type(const char * expression, Poincare::Expression::Type type) {
  Expression * e = parse_expression(expression);
  assert(e->type() == type);
  delete e;
}

void assert_parsed_expression_polynomial_degree(const char * expression, int degree, char symbolName) {
  GlobalContext globalContext;
  Expression * e = parse_expression(expression);
  Expression::Simplify(&e, globalContext, Radian);
  assert(e->polynomialDegree(symbolName) == degree);
  delete e;
}

typedef Expression * (*ProcessExpression)(Expression *, Context & context, Expression::AngleUnit angleUnit, Expression::ComplexFormat complexFormat);

void assert_parsed_expression_process_to(const char * expression, const char * result, Expression::AngleUnit angleUnit, Expression::ComplexFormat complexFormat, ProcessExpression process, int numberOfSignifiantDigits = PrintFloat::k_numberOfStoredSignificantDigits) {
  GlobalContext globalContext;
  Expression * e = parse_expression(expression);
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << " Entry expression: " << expression << "----"  << endl;
#endif
  Expression::Simplify(&e, globalContext, angleUnit);
  Expression * m = process(e, globalContext, angleUnit, complexFormat);
  char buffer[500];
  m->writeTextInBuffer(buffer, sizeof(buffer), DecimalMode, numberOfSignifiantDigits);
  translate_in_ASCII_chars(buffer);
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  print_expression(e, 0);
  cout << "---- serialize to: " << buffer << " ----"  << endl;
  cout << "----- compared to: " << result << " ----\n"  << endl;
#endif
  assert(strcmp(buffer, result) == 0);
  delete e;
  if (e != m) {
    delete m;
  }
}

template<typename T>
void assert_parsed_expression_evaluates_to(const char * expression, const char * approximation, Expression::AngleUnit angleUnit, Expression::ComplexFormat complexFormat, int numberOfSignificantDigits) {
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << "--------- Approximation ---------" << endl;
#endif
  int numberOfDigits = sizeof(T) == sizeof(double) ? PrintFloat::k_numberOfStoredSignificantDigits : PrintFloat::k_numberOfPrintedSignificantDigits;
  numberOfDigits = numberOfSignificantDigits > 0 ? numberOfSignificantDigits : numberOfDigits;
  assert_parsed_expression_process_to(expression, approximation, angleUnit, complexFormat, [](Expression * e, Context & context, Expression::AngleUnit angleUnit, Expression::ComplexFormat complexFormat) {
        return e->approximate<T>(context, angleUnit, complexFormat);
      }, numberOfDigits);
}

void assert_parsed_expression_simplify_to(const char * expression, const char * simplifiedExpression, Expression::AngleUnit angleUnit) {
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << "--------- Simplification ---------" << endl;
#endif
  assert_parsed_expression_process_to(expression, simplifiedExpression, angleUnit, Expression::ComplexFormat::Cartesian, [](Expression * e, Context & context, Expression::AngleUnit angleUnit, Expression::ComplexFormat complexFormat) { return e; });
}

void assert_parsed_expression_layout_serialize_to_self(const char * expressionLayout) {
  Expression * e = parse_expression(expressionLayout);
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << "---- Serialize: " << expressionLayout << "----"  << endl;
#endif
  ExpressionLayout * el = e->createLayout(DecimalMode, PrintFloat::k_numberOfStoredSignificantDigits);
  int bufferSize = 255;
  char buffer[bufferSize];
  el->writeTextInBuffer(buffer, bufferSize);
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << "---- serialized to: " << buffer << " ----\n"  << endl;
#endif
  assert(strcmp(expressionLayout, buffer) == 0);
  delete e;
  delete el;
}

void assert_expression_layout_serialize_to(Poincare::ExpressionLayout * layout, const char * serialization) {
  int bufferSize = 255;
  char buffer[bufferSize];
  layout->writeTextInBuffer(buffer, bufferSize);
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << "---- Serialize: " << serialization << "----"  << endl;
  cout << "---- serialized to: " << buffer << " ----"  << endl;
  cout << "----- compared to: " << serialization << " ----\n"  << endl;
#endif
  assert(strcmp(serialization, buffer) == 0);
}

template void assert_parsed_expression_evaluates_to<float>(char const*, char const *, Poincare::Expression::AngleUnit, Poincare::Expression::ComplexFormat, int);
template void assert_parsed_expression_evaluates_to<double>(char const*, char const *, Poincare::Expression::AngleUnit, Poincare::Expression::ComplexFormat, int);
