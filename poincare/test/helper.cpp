#include <quiz.h>
#include <poincare.h>
#include <string.h>
#include <ion.h>
#include <stdlib.h>
#include <assert.h>
#include <cmath>

using namespace Poincare;

Expression * parse_expression(const char * expression) {
  quiz_print(expression);
  char buffer[200];
  strlcpy(buffer, expression, sizeof(buffer));
  for (char *c = buffer; *c; c++) {
    switch (*c) {
      case 'E': *c = Ion::Charset::Exponent; break;
      case 'X': *c = Ion::Charset::Exponential; break;
      case 'I': *c = Ion::Charset::IComplex; break;
      case 'R': *c = Ion::Charset::Root; break;
      case 'P': *c = Ion::Charset::SmallPi; break;
    }
  }
  Expression * result = Expression::parse(buffer);
  assert(result);
  return result;
}

void assert_parsed_expression_type(const char * expression, Poincare::Expression::Type type) {
  Expression * e = parse_expression(expression);
  assert(e->type() == type);
  delete e;
}

template<typename T>
void assert_parsed_expression_evaluates_to(const char * expression, Complex<T> * results, int numberOfRows, int numberOfColumns, Expression::AngleUnit angleUnit) {
  GlobalContext globalContext;
  Expression * a = parse_expression(expression);
  Expression * m = a->evaluate<T>(globalContext, angleUnit);
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

template void assert_parsed_expression_evaluates_to<float>(char const*, Poincare::Complex<float>*, int, int, Poincare::Expression::AngleUnit);
template void assert_parsed_expression_evaluates_to<double>(char const*, Poincare::Complex<double>*, int, int, Poincare::Expression::AngleUnit);
