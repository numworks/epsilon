#include <poincare_nodes.h>
#include <quiz.h>
#include "poincare/src/parsing/parser.h"

// Expressions

const char * MaxIntegerString(); // (2^32)^k_maxNumberOfDigits-1
const char * OverflowedIntegerString(); // (2^32)^k_maxNumberOfDigits
const char * BigOverflowedIntegerString(); // OverflowedIntegerString with a 2 on first digit

constexpr Poincare::Preferences::AngleUnit Degree = Poincare::Preferences::AngleUnit::Degree;
constexpr Poincare::Preferences::AngleUnit Radian = Poincare::Preferences::AngleUnit::Radian;
constexpr Poincare::Preferences::ComplexFormat Cartesian = Poincare::Preferences::ComplexFormat::Cartesian;
constexpr Poincare::Preferences::ComplexFormat Polar = Poincare::Preferences::ComplexFormat::Polar;
constexpr Poincare::Preferences::PrintFloatMode DecimalMode = Poincare::Preferences::PrintFloatMode::Decimal;
constexpr Poincare::Preferences::PrintFloatMode ScientificMode = Poincare::Preferences::PrintFloatMode::Scientific;

void translate_in_special_chars(char * expression);
void translate_in_ASCII_chars(char * expression);
Poincare::Expression parse_expression(const char * expression, bool canBeUnparsable = false);
void assert_expression_not_parsable(const char * expression);
void assert_parsed_expression_type(const char * expression, Poincare::ExpressionNode::Type type);
void assert_parsed_expression_is(const char * expression, Poincare::Expression r);
void assert_parsed_expression_polynomial_degree(const char * expression, int degree, const char * symbolName = "x");
void assert_simplify(const char * expression);

template<typename T>
void assert_parsed_expression_evaluates_to(const char * expression, const char * approximation, Poincare::Preferences::AngleUnit angleUnit = Degree, Poincare::Preferences::ComplexFormat complexFormat = Cartesian, int numberOfSignificantDigits = -1);

template<typename T>
void assert_parsed_expression_approximates_with_value_for_symbol(Poincare::Expression expression, const char * symbol, T value, T approximation, Poincare::Preferences::AngleUnit angleUnit = Degree);

void assert_parsed_expression_simplify_to(const char * expression, const char * simplifiedExpression, Poincare::Preferences::AngleUnit angleUnit = Poincare::Preferences::AngleUnit::Radian);
void assert_parsed_expression_serialize_to(Poincare::Expression expression, const char * serializedExpression, Poincare::Preferences::PrintFloatMode mode = DecimalMode, int numberOfSignifiantDigits = -1);


// Layouts
void assert_parsed_expression_layout_serialize_to_self(const char * expressionLayout);
void assert_expression_layout_serialize_to(Poincare::Layout layout, const char * serialization);
