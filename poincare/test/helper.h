#include <poincare.h>

constexpr Poincare::Expression::AngleUnit Degree = Poincare::Expression::AngleUnit::Degree;
constexpr Poincare::Expression::AngleUnit Radian = Poincare::Expression::AngleUnit::Radian;

Poincare::Expression * parse_expression(const char * expression);
void assert_parsed_expression_type(const char * expression, Poincare::Expression::Type type);
template<typename T>
void assert_parsed_expression_evaluates_to(const char * expression, Poincare::Complex<T> * results, int numberOfRows, int numberOfColumns, Poincare::Expression::AngleUnit angleUnit = Degree);
template<typename T>
void assert_parsed_expression_evaluates_to(const char * expression, Poincare::Complex<T> * results, Poincare::Expression::AngleUnit angleUnit = Degree) {
  assert_parsed_expression_evaluates_to(expression, results, 0, 0, angleUnit);
}
