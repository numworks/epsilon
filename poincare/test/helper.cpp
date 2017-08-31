#include <quiz.h>
#include <poincare.h>
#include <string.h>
#include <ion.h>
#include <stdlib.h>
#include <assert.h>
#include <cmath>

using namespace Poincare;

template<typename T>
static Evaluation<T> * parse_expression(const char * expression, Expression::AngleUnit angleUnit) {
  char buffer[200];
  strlcpy(buffer, expression, 200);
  for (size_t i=0; i<strlen(buffer); i++) {
    if (buffer[i] == 'E') {
      buffer[i] = Ion::Charset::Exponent;
    }
    if (buffer[i] == 'X') {
      buffer[i] = Ion::Charset::Exponential;
    }
    if (buffer[i] == 'I') {
      buffer[i] = Ion::Charset::IComplex;
    }
    if (buffer[i] == 'R') {
      buffer[i] = Ion::Charset::Root;
    }
    if (buffer[i] == 'P') {
      buffer[i] = Ion::Charset::SmallPi;
    }
  }
  GlobalContext globalContext;
  Expression * a = Expression::parse(buffer);
  Evaluation<T> * m = a->evaluate<T>(globalContext, angleUnit);
  delete a;
  return m;
}

template<typename T>
static void check_results(Evaluation<T> * actual, Complex<T> * expected) {
  for (int i = 0; i < actual->numberOfOperands(); i++) {
    assert(std::fabs(actual->complexOperand(i)->a() - expected[i].a()) < 0.0001f);
    assert(std::fabs(actual->complexOperand(i)->b() - expected[i].b()) < 0.0001f);
  }
}

template<typename T>
void assert_parsed_expression_evaluate_to(const char * expression, Complex<T> * results, int numberOfEntries, Expression::AngleUnit angleUnit) {
  Evaluation<T> * m = parse_expression<T>(expression, angleUnit);
  assert(m->numberOfOperands() == numberOfEntries);
  check_results(m, results);
  delete m;
}

template<typename T>
void assert_parsed_expression_evaluate_to(const char * expression, Complex<T> * results, int numberOfRows, int numberOfColumns, Expression::AngleUnit angleUnit) {
  Evaluation<T> * m = parse_expression<T>(expression, angleUnit);
  assert(m->numberOfRows() == numberOfRows);
  assert(m->numberOfColumns() == numberOfColumns);
  check_results(m, results);
  delete m;
}

template void assert_parsed_expression_evaluate_to<float>(char const*, Poincare::Complex<float>*, int, Poincare::Expression::AngleUnit);
template void assert_parsed_expression_evaluate_to<double>(char const*, Poincare::Complex<double>*, int, Poincare::Expression::AngleUnit);
template void assert_parsed_expression_evaluate_to<float>(char const*, Poincare::Complex<float>*, int, int, Poincare::Expression::AngleUnit);
template void assert_parsed_expression_evaluate_to<double>(char const*, Poincare::Complex<double>*, int, int, Poincare::Expression::AngleUnit);
