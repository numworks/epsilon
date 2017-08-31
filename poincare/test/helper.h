#include <poincare.h>

constexpr Poincare::Expression::AngleUnit Degree = Poincare::Expression::AngleUnit::Degree;
constexpr Poincare::Expression::AngleUnit Radian = Poincare::Expression::AngleUnit::Radian;

template<typename T>
void assert_parsed_expression_evaluate_to(const char * expression, Poincare::Complex<T> * results, int numberOfEntries, Poincare::Expression::AngleUnit angleUnit = Degree);
template<typename T>
void assert_parsed_expression_evaluate_to(const char * expression, Poincare::Complex<T> * results, int numberOfRows, int numberOfColumns, Poincare::Expression::AngleUnit angleUnit = Degree);
