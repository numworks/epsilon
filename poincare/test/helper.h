#include <poincare.h>

// Expressions

constexpr Poincare::Preferences::AngleUnit Degree = Poincare::Preferences::AngleUnit::Degree;
constexpr Poincare::Preferences::AngleUnit Radian = Poincare::Preferences::AngleUnit::Radian;
constexpr Poincare::Preferences::ComplexFormat Cartesian = Poincare::Preferences::ComplexFormat::Cartesian;
constexpr Poincare::Preferences::ComplexFormat Polar = Poincare::Preferences::ComplexFormat::Polar;
constexpr Poincare::Preferences::PrintFloatMode DecimalMode = Poincare::Preferences::PrintFloatMode::Decimal;
constexpr Poincare::Preferences::PrintFloatMode ScientificMode = Poincare::Preferences::PrintFloatMode::Scientific;

void translate_in_special_chars(char * expression);
void translate_in_ASCII_chars(char * expression);
Poincare::Expression parse_expression(const char * expression);
void assert_parsed_expression_type(const char * expression, Poincare::ExpressionNode::Type type);
void assert_parsed_expression_polynomial_degree(const char * expression, int degree, char symbolName = 'x');
template<typename T>
void assert_parsed_expression_evaluates_to(const char * expression, const char * approximation, Poincare::Preferences::AngleUnit angleUnit = Degree, Poincare::Preferences::ComplexFormat complexFormat = Cartesian, int numberOfSignificantDigits = -1);
void assert_parsed_expression_simplify_to(const char * expression, const char * simplifiedExpression, Poincare::Preferences::AngleUnit angleUnit = Poincare::Preferences::AngleUnit::Radian);

// Layouts
/*void assert_parsed_expression_layout_serialize_to_self(const char * expressionLayout);
void assert_expression_layout_serialize_to(Poincare::ExpressionLayout * layout, const char * serialization);*/
