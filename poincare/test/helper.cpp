#include <quiz.h>
#include <poincare.h>
#include <string.h>
#include <ion.h>
#include <stdlib.h>
#include <assert.h>
#include <cmath>
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
  Expression::Simplify(&e, globalContext);
  assert(e->polynomialDegree(symbolName) == degree);
  delete e;
}

template<typename T>
void assert_parsed_expression_evaluates_to(const char * expression, Complex<T> * results, int numberOfRows, int numberOfColumns, Expression::AngleUnit angleUnit) {
  GlobalContext globalContext;
  Expression * a = parse_expression(expression);
  Expression * m = a->approximate<T>(globalContext, angleUnit);
  assert(m);
  assert(m->numberOfOperands() == 0 || m->numberOfOperands() == numberOfRows*numberOfColumns);
  if (m->type() == Expression::Type::Matrix) {
    assert(static_cast<Matrix *>(m)->numberOfRows() == numberOfRows);
    assert(static_cast<Matrix *>(m)->numberOfColumns() == numberOfColumns);
    for (int i = 0; i < m->numberOfOperands(); i++) {
      assert(std::fabs(static_cast<const Complex<T> *>(m->operand(i))->a() - results[i].a()) < 0.0001f);
      assert(std::fabs(static_cast<const Complex<T> *>(m->operand(i))->b() - results[i].b()) < 0.0001f);
    }
  } else {
    assert(std::fabs(static_cast<const Complex<T> *>(m)->a() - results[0].a()) < 0.0001f);
    assert(std::fabs(static_cast<const Complex<T> *>(m)->b() - results[0].b()) < 0.0001f);
  }
  delete a;
  delete m;
}

void assert_parsed_expression_simplify_to(const char * expression, const char * simplifiedExpression, Expression::AngleUnit angleUnit) {
  GlobalContext globalContext;
  Expression * e = parse_expression(expression);
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << "---- Simplify: " << expression << "----"  << endl;
#endif
  Expression::Simplify(&e, globalContext, angleUnit);
  char buffer[500];
  e->writeTextInBuffer(buffer, sizeof(buffer));
  translate_in_ASCII_chars(buffer);
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  print_expression(e, 0);
  cout << "---- serialize to: " << buffer << " ----"  << endl;
  cout << "----- compared to: " << simplifiedExpression << " ----\n"  << endl;
#endif
  assert(strcmp(buffer, simplifiedExpression) == 0);
  delete e;
}

template void assert_parsed_expression_evaluates_to<float>(char const*, Poincare::Complex<float>*, int, int, Poincare::Expression::AngleUnit);
template void assert_parsed_expression_evaluates_to<double>(char const*, Poincare::Complex<double>*, int, int, Poincare::Expression::AngleUnit);
