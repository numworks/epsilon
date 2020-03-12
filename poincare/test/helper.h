#include <poincare_nodes.h>
#include <quiz.h>

const char * MaxIntegerString(); // (2^32)^k_maxNumberOfDigits-1
const char * OverflowedIntegerString(); // (2^32)^k_maxNumberOfDigits
const char * BigOverflowedIntegerString(); // OverflowedIntegerString with a 2 on first digit

constexpr Poincare::ExpressionNode::ReductionTarget SystemForApproximation = Poincare::ExpressionNode::ReductionTarget::SystemForApproximation;
constexpr Poincare::ExpressionNode::ReductionTarget SystemForAnalysis = Poincare::ExpressionNode::ReductionTarget::SystemForAnalysis;
constexpr Poincare::ExpressionNode::ReductionTarget User = Poincare::ExpressionNode::ReductionTarget::User;
constexpr Poincare::ExpressionNode::SymbolicComputation ReplaceAllDefinedSymbolsWithDefinition = Poincare::ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition;
constexpr Poincare::ExpressionNode::SymbolicComputation ReplaceAllSymbolsWithDefinitionsOrUndefined = Poincare::ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined;
constexpr Poincare::ExpressionNode::SymbolicComputation ReplaceDefinedFunctionsWithDefinitions = Poincare::ExpressionNode::SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions;
constexpr Poincare::ExpressionNode::SymbolicComputation ReplaceAllSymbolsWithUndefinedAndDoNotReplaceUnits = Poincare::ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithUndefinedAndDoNotReplaceUnits;
constexpr Poincare::Preferences::AngleUnit Degree = Poincare::Preferences::AngleUnit::Degree;
constexpr Poincare::Preferences::AngleUnit Radian = Poincare::Preferences::AngleUnit::Radian;
constexpr Poincare::Preferences::AngleUnit Gradian = Poincare::Preferences::AngleUnit::Gradian;
constexpr Poincare::Preferences::ComplexFormat Cartesian = Poincare::Preferences::ComplexFormat::Cartesian;
constexpr Poincare::Preferences::ComplexFormat Polar = Poincare::Preferences::ComplexFormat::Polar;
constexpr Poincare::Preferences::ComplexFormat Real = Poincare::Preferences::ComplexFormat::Real;
constexpr Poincare::Preferences::PrintFloatMode DecimalMode = Poincare::Preferences::PrintFloatMode::Decimal;
constexpr Poincare::Preferences::PrintFloatMode ScientificMode = Poincare::Preferences::PrintFloatMode::Scientific;
constexpr Poincare::Preferences::PrintFloatMode EngineeringMode = Poincare::Preferences::PrintFloatMode::Engineering;

void quiz_assert_print_if_failure(bool test, const char * information);
void quiz_assert_log_if_failure(bool test, Poincare::TreeHandle tree);

typedef Poincare::Expression (*ProcessExpression)(Poincare::Expression, Poincare::ExpressionNode::ReductionContext reductionContext);

void assert_parsed_expression_process_to(const char * expression, const char * result, Poincare::ExpressionNode::ReductionTarget target, Poincare::Preferences::ComplexFormat complexFormat, Poincare::Preferences::AngleUnit angleUnit, Poincare::ExpressionNode::SymbolicComputation symbolicComputation, ProcessExpression process, int numberOfSignifiantDigits = Poincare::PrintFloat::k_numberOfStoredSignificantDigits);

// Parsing

Poincare::Expression parse_expression(const char * expression, Poincare::Context * context, bool addParentheses);

// Simplification

void assert_simplify(const char * expression, Poincare::Preferences::AngleUnit angleUnit = Radian, Poincare::Preferences::ComplexFormat complexFormat = Cartesian, Poincare::ExpressionNode::ReductionTarget target = User);

void assert_parsed_expression_simplify_to(const char * expression, const char * simplifiedExpression, Poincare::ExpressionNode::ReductionTarget target = User, Poincare::Preferences::AngleUnit angleUnit = Radian, Poincare::Preferences::ComplexFormat complexFormat = Cartesian, Poincare::ExpressionNode::SymbolicComputation symbolicComputation = ReplaceAllDefinedSymbolsWithDefinition);

// Approximation

template<typename T>
void assert_expression_approximates_to(const char * expression, const char * approximation, Poincare::Preferences::AngleUnit angleUnit = Degree, Poincare::Preferences::ComplexFormat complexFormat = Cartesian, int numberOfSignificantDigits = -1);
void assert_expression_simplifies_and_approximates_to(const char * expression, const char * approximation, Poincare::Preferences::AngleUnit angleUnit = Degree, Poincare::Preferences::ComplexFormat complexFormat = Cartesian, int numberOfSignificantDigits = -1);
template<typename T>
void assert_expression_simplifies_approximates_to(const char * expression, const char * approximation, Poincare::Preferences::AngleUnit angleUnit = Degree, Poincare::Preferences::ComplexFormat complexFormat = Cartesian, int numberOfSignificantDigits = -1);


// Layout serializing

void assert_layout_serialize_to(Poincare::Layout layout, const char * serialization);

// Expression layouting

void assert_expression_layouts_as(Poincare::Expression expression, Poincare::Layout layout);
