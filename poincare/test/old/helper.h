#ifndef POINCARE_TEST_HELPER_H
#define POINCARE_TEST_HELPER_H

#include <poincare/old/expression_node.h>
#include <poincare/old/float.h>
#include <poincare/old/helpers.h>
#include <poincare/print_float.h>
#include <poincare/src/memory/tree.h>
#include <poincare/test/float_helper.h>
#include <quiz.h>

#include <algorithm>
#include <cmath>

namespace Poincare::Internal {
class Tree;
}

const char* MaxIntegerString();         // (2^32)^k_maxNumberOfDigits-1
const char* OverflowedIntegerString();  // (2^32)^k_maxNumberOfDigits
// OverflowedIntegerString with a 2 on first digit
const char* BigOverflowedIntegerString();
const char* MaxParsedIntegerString();
const char* ApproximatedParsedIntegerString();

/* TODO: With C++20, these can be simplified with:
 * using enum Poincare::ReductionTarget;
 * using enum Poincare::SymbolicComputation;
 * ...
 */

constexpr Poincare::ReductionTarget SystemForApproximation =
    Poincare::ReductionTarget::SystemForApproximation;
constexpr Poincare::ReductionTarget SystemForAnalysis =
    Poincare::ReductionTarget::SystemForAnalysis;
constexpr Poincare::ReductionTarget User = Poincare::ReductionTarget::User;
constexpr Poincare::SymbolicComputation ReplaceAllDefinedSymbolsWithDefinition =
    Poincare::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition;
constexpr Poincare::SymbolicComputation
    ReplaceAllSymbolsWithDefinitionsOrUndefined = Poincare::
        SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined;
constexpr Poincare::SymbolicComputation ReplaceDefinedFunctionsWithDefinitions =
    Poincare::SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions;
constexpr Poincare::SymbolicComputation ReplaceAllSymbolsWithUndefined =
    Poincare::SymbolicComputation::ReplaceAllSymbolsWithUndefined;
constexpr Poincare::SymbolicComputation DoNotReplaceAnySymbol =
    Poincare::SymbolicComputation::DoNotReplaceAnySymbol;
constexpr Poincare::UnitConversion NoUnitConversion =
    Poincare::UnitConversion::None;
constexpr Poincare::UnitConversion DefaultUnitConversion =
    Poincare::UnitConversion::Default;
constexpr Poincare::UnitConversion InternationalSystemUnitConversion =
    Poincare::UnitConversion::InternationalSystem;
constexpr Poincare::Preferences::AngleUnit Radian =
    Poincare::Preferences::AngleUnit::Radian;
constexpr Poincare::Preferences::AngleUnit Degree =
    Poincare::Preferences::AngleUnit::Degree;
constexpr Poincare::Preferences::AngleUnit Gradian =
    Poincare::Preferences::AngleUnit::Gradian;
constexpr Poincare::Preferences::UnitFormat MetricUnitFormat =
    Poincare::Preferences::UnitFormat::Metric;
constexpr Poincare::Preferences::UnitFormat Imperial =
    Poincare::Preferences::UnitFormat::Imperial;
constexpr Poincare::Preferences::ComplexFormat Cartesian =
    Poincare::Preferences::ComplexFormat::Cartesian;
constexpr Poincare::Preferences::ComplexFormat Polar =
    Poincare::Preferences::ComplexFormat::Polar;
constexpr Poincare::Preferences::ComplexFormat Real =
    Poincare::Preferences::ComplexFormat::Real;
constexpr Poincare::Preferences::PrintFloatMode DecimalMode =
    Poincare::Preferences::PrintFloatMode::Decimal;
constexpr Poincare::Preferences::PrintFloatMode ScientificMode =
    Poincare::Preferences::PrintFloatMode::Scientific;
constexpr Poincare::Preferences::PrintFloatMode EngineeringMode =
    Poincare::Preferences::PrintFloatMode::Engineering;

void quiz_assert_print_if_failure(bool test, const char* information);
void quiz_assert_log_if_failure(bool test, Poincare::PoolHandle tree);
void quiz_reset_failure_ratio();
void quiz_print_failure_ratio();

typedef Poincare::Internal::Tree* (*ProcessExpression)(
    Poincare::Internal::Tree*, Poincare::ReductionContext reductionContext);

void assert_parsed_expression_process_to(
    const char* expression, const char* result,
    Poincare::ReductionTarget target,
    Poincare::Preferences::ComplexFormat complexFormat,
    Poincare::Preferences::AngleUnit angleUnit,
    Poincare::Preferences::UnitFormat unitFormat,
    Poincare::SymbolicComputation symbolicComputation,
    Poincare::UnitConversion unitConversion, ProcessExpression process,
    int numberOfSignificantDigits =
        Poincare::PrintFloat::k_maxNumberOfSignificantDigits);

// Parsing

Poincare::Internal::Tree* parse_expression(const char* expression,
                                           Poincare::Context* context,
                                           bool addParentheses,
                                           bool parseForAssignment = false);

void assert_parsed_expression_is(
    const char* expression, Poincare::OExpression r,
    bool addParentheses = false, bool parseForAssignment = false,
    Poincare::Preferences::MixedFractions mixedFractionsParameter =
        Poincare::Preferences::MixedFractions::Enabled);
void assert_parse_to_same_expression(const char* expression1,
                                     const char* expression2);

// Simplification

void assert_reduce_and_store(
    const char* expression, Poincare::Preferences::AngleUnit angleUnit = Radian,
    Poincare::Preferences::UnitFormat unitFormat = MetricUnitFormat,
    Poincare::Preferences::ComplexFormat complexFormat = Cartesian,
    Poincare::ReductionTarget target = User);

void assert_expression_reduce(
    Poincare::OExpression expression,
    Poincare::Preferences::AngleUnit angleUnit = Radian,
    Poincare::Preferences::UnitFormat unitFormat = MetricUnitFormat,
    Poincare::Preferences::ComplexFormat complexFormat = Cartesian,
    Poincare::ReductionTarget target = User,
    const char* printIfFailure = "Error");

void assert_parsed_expression_simplify_to(
    const char* expression, const char* simplifiedExpression,
    Poincare::ReductionTarget target = User,
    Poincare::Preferences::AngleUnit angleUnit = Radian,
    Poincare::Preferences::UnitFormat unitFormat = MetricUnitFormat,
    Poincare::Preferences::ComplexFormat complexFormat = Cartesian,
    Poincare::SymbolicComputation symbolicComputation =
        ReplaceAllDefinedSymbolsWithDefinition,
    Poincare::UnitConversion unitConversion = DefaultUnitConversion);

// Approximation

template <typename T>
void assert_expression_approximates_to(
    const char* expression, const char* approximation,
    Poincare::Preferences::AngleUnit angleUnit = Degree,
    Poincare::Preferences::UnitFormat unitFormat = MetricUnitFormat,
    Poincare::Preferences::ComplexFormat complexFormat = Cartesian,
    int numberOfSignificantDigits =
        Poincare::PrintFloat::SignificantDecimalDigits<T>());

void assert_expression_approximates_keeping_symbols_to(
    const char* expression, const char* simplifiedExpression,
    Poincare::Preferences::AngleUnit angleUnit = Degree,
    Poincare::Preferences::UnitFormat unitFormat = MetricUnitFormat,
    Poincare::Preferences::ComplexFormat complexFormat = Cartesian,
    int numberOfSignificantDigits = 10);

template <typename T>
void assert_expression_simplifies_approximates_to(
    const char* expression, const char* approximation,
    Poincare::Preferences::AngleUnit angleUnit = Degree,
    Poincare::Preferences::UnitFormat unitFormat = MetricUnitFormat,
    Poincare::Preferences::ComplexFormat complexFormat = Cartesian,
    int numberOfSignificantDigits =
        Poincare::PrintFloat::SignificantDecimalDigits<T>());

// OExpression serializing

void assert_expression_serializes_to(
    Poincare::Internal::Tree* expression, const char* serialization,
    Poincare::Preferences::PrintFloatMode mode = ScientificMode,
    int numberOfSignificantDigits = 7);

void assert_expression_serializes_and_parses_to_itself(
    Poincare::OExpression expression);
void assert_expression_parses_and_serializes_to(const char* expression,
                                                const char* result);
void assert_expression_parses_and_serializes_to_itself(const char* expression);

// Layout serializing

void assert_layout_serializes_to(Poincare::Internal::Tree* layout,
                                 const char* serialization);

// OExpression layouting

void assert_expression_layouts_as(Poincare::Internal::Tree* expression,
                                  Poincare::Internal::Tree* layout);

#endif
