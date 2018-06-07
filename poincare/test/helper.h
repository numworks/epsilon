#include <poincare.h>

// Expressions

constexpr Poincare::Expression::AngleUnit Degree = Poincare::Expression::AngleUnit::Degree;
constexpr Poincare::Expression::AngleUnit Radian = Poincare::Expression::AngleUnit::Radian;
constexpr Poincare::Expression::ComplexFormat Cartesian = Poincare::Expression::ComplexFormat::Cartesian;
constexpr Poincare::Expression::ComplexFormat Polar = Poincare::Expression::ComplexFormat::Polar;
constexpr Poincare::PrintFloat::Mode DecimalMode = Poincare::PrintFloat::Mode::Decimal;
constexpr Poincare::PrintFloat::Mode ScientificMode = Poincare::PrintFloat::Mode::Scientific;

void translate_in_special_chars(char * expression);
void translate_in_ASCII_chars(char * expression);
Poincare::Expression * parse_expression(const char * expression);
void assert_parsed_expression_type(const char * expression, Poincare::Expression::Type type);
void assert_parsed_expression_polynomial_degree(const char * expression, int degree, char symbolName = 'x');
template<typename T>
void assert_parsed_expression_evaluates_to(const char * expression, const char * approximation, Poincare::Expression::AngleUnit angleUnit = Degree, Poincare::Expression::ComplexFormat complexFormat = Cartesian, int numberOfSignificantDigits = -1);
void assert_parsed_expression_simplify_to(const char * expression, const char * simplifiedExpression, Poincare::Expression::AngleUnit angleUnit = Poincare::Expression::AngleUnit::Radian);

// Layouts
void assert_parsed_expression_layout_serialize_to_self(const char * expressionLayout);
void assert_expression_layout_serialize_to(Poincare::ExpressionLayout * layout, const char * serialization);
