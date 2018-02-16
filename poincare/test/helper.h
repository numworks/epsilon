#include <poincare.h>

constexpr Poincare::Expression::AngleUnit Degree = Poincare::Expression::AngleUnit::Degree;
constexpr Poincare::Expression::AngleUnit Radian = Poincare::Expression::AngleUnit::Radian;

void translate_in_special_chars(char * expression);
void translate_in_ASCII_chars(char * expression);
Poincare::Expression * parse_expression(const char * expression);
void assert_parsed_expression_type(const char * expression, Poincare::Expression::Type type);
void assert_parsed_expression_polynomial_degree(const char * expression, int degree, char symbolName = 'x');
template<typename T>
void assert_parsed_expression_evaluates_to(const char * expression, Poincare::Complex<T> * results, int numberOfRows, int numberOfColumns, Poincare::Expression::AngleUnit angleUnit = Degree);
template<typename T>
void assert_parsed_expression_evaluates_to(const char * expression, Poincare::Complex<T> * results, Poincare::Expression::AngleUnit angleUnit = Degree) {
  assert_parsed_expression_evaluates_to(expression, results, 0, 0, angleUnit);
}
void assert_parsed_expression_simplify_to(const char * expression, const char * simplifiedExpression, Poincare::Expression::AngleUnit angleUnit = Poincare::Expression::AngleUnit::Radian);
