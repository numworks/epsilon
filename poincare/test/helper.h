#include <poincare.h>

// Expressions

static const char * MaxIntegerString = "179769313486231590772930519078902473361797697894230657273430081157732675805500963132708477322407536021120113879871393357658789768814416622492847430639474124377767893424865485276302219601246094119453082952085005768838150682342462881473913110540827237163350510684586298239947245938479716304835356329624224137215"; // (2^32)^k_maxNumberOfDigits-1
static const char * OverflowedIntegerString = "179769313486231590772930519078902473361797697894230657273430081157732675805500963132708477322407536021120113879871393357658789768814416622492847430639474124377767893424865485276302219601246094119453082952085005768838150682342462881473913110540827237163350510684586298239947245938479716304835356329624224137216"; // (2^32)^k_maxNumberOfDigits

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
void assert_parsed_expression_is(const char * expression, Poincare::Expression r);
void assert_parsed_expression_polynomial_degree(const char * expression, int degree, char symbolName = 'x');

template<typename T>
void assert_parsed_expression_evaluates_to(const char * expression, const char * approximation, Poincare::Preferences::AngleUnit angleUnit = Degree, Poincare::Preferences::ComplexFormat complexFormat = Cartesian, int numberOfSignificantDigits = -1);
void assert_parsed_expression_simplify_to(const char * expression, const char * simplifiedExpression, Poincare::Preferences::AngleUnit angleUnit = Poincare::Preferences::AngleUnit::Radian);
void assert_parsed_expression_serialize_to(Poincare::Expression expression, const char * serializedExpression, Poincare::Preferences::PrintFloatMode mode = DecimalMode, int numberOfSignifiantDigits = -1);


// Layouts
void assert_parsed_expression_layout_serialize_to_self(const char * expressionLayout);
void assert_expression_layout_serialize_to(Poincare::LayoutRef layout, const char * serialization);
