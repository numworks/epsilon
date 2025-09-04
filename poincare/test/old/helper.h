#pragma once

#include <omg/enums.h>
#include <omg/float.h>
#include <poincare/pool_handle.h>
#include <poincare/print_float.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/memory/tree.h>
#include <poincare/symbol_context.h>
#include <poincare/symbol_store_interface.h>
#include <poincare/test/float_helper.h>
#include <quiz.h>

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
constexpr Poincare::SymbolicComputation ReplaceDefinedSymbols =
    Poincare::SymbolicComputation::ReplaceDefinedSymbols;
constexpr Poincare::SymbolicComputation ReplaceAllSymbols =
    Poincare::SymbolicComputation::ReplaceAllSymbols;
constexpr Poincare::SymbolicComputation ReplaceDefinedFunctions =
    Poincare::SymbolicComputation::ReplaceDefinedFunctions;
constexpr Poincare::SymbolicComputation ReplaceAllSymbolsWithUndefined =
    Poincare::SymbolicComputation::ReplaceAllSymbolsWithUndefined;
constexpr Poincare::SymbolicComputation KeepAllSymbols =
    Poincare::SymbolicComputation::KeepAllSymbols;
constexpr Poincare::AngleUnit Radian = Poincare::AngleUnit::Radian;
constexpr Poincare::AngleUnit Degree = Poincare::AngleUnit::Degree;
constexpr Poincare::AngleUnit Gradian = Poincare::AngleUnit::Gradian;
constexpr Poincare::Preferences::UnitFormat MetricUnitFormat =
    Poincare::Preferences::UnitFormat::Metric;
constexpr Poincare::Preferences::UnitFormat Imperial =
    Poincare::Preferences::UnitFormat::Imperial;
constexpr Poincare::ComplexFormat Cartesian =
    Poincare::ComplexFormat::Cartesian;
constexpr Poincare::ComplexFormat Polar = Poincare::ComplexFormat::Polar;
constexpr Poincare::ComplexFormat Real = Poincare::ComplexFormat::Real;

void quiz_assert_log_if_failure(bool test, Poincare::PoolHandle tree);

void copy_without_system_chars(char* buffer, const char* input);

typedef Poincare::Internal::Tree* (*ProcessExpression)(
    Poincare::Internal::Tree*,
    Poincare::ProjectionContext& projectionContext);

void assert_parsed_expression_process_to(
    const char* expression, const char* result,
    const Poincare::SymbolContext& symbolContext,
    Poincare::ReductionTarget target, Poincare::ComplexFormat complexFormat,
    Poincare::AngleUnit angleUnit, Poincare::Preferences::UnitFormat unitFormat,
    Poincare::SymbolicComputation symbolicComputation,
    ProcessExpression process,
    int numberOfSignificantDigits =
        Poincare::PrintFloat::k_maxNumberOfSignificantDigits);

// Simplification

/* The two signatures (with and without a context parameter) are there for
 * retro-compatibilty with the old tests */
void assert_parsed_expression_simplify_to(
    const char* expression, const char* simplifiedExpression,
    const Poincare::SymbolContext& symbolContext,
    Poincare::ReductionTarget target = User,
    Poincare::AngleUnit angleUnit = Radian,
    Poincare::Preferences::UnitFormat unitFormat = MetricUnitFormat,
    Poincare::ComplexFormat complexFormat = Cartesian,
    Poincare::SymbolicComputation symbolicComputation = ReplaceDefinedSymbols,
    bool beautify = true);
void assert_parsed_expression_simplify_to(
    const char* expression, const char* simplifiedExpression,
    Poincare::ReductionTarget target = User,
    Poincare::AngleUnit angleUnit = Radian,
    Poincare::Preferences::UnitFormat unitFormat = MetricUnitFormat,
    Poincare::ComplexFormat complexFormat = Cartesian,
    Poincare::SymbolicComputation symbolicComputation = ReplaceDefinedSymbols,
    bool beautify = true);

// Approximation

/* The two signatures (with and without a context parameter) are there for
 * retro-compatibilty with the old tests */
template <typename T>
void assert_expression_approximates_to(
    const char* expression, const char* approximation,
    Poincare::SymbolContext& context, Poincare::AngleUnit angleUnit = Degree,
    Poincare::Preferences::UnitFormat unitFormat = MetricUnitFormat,
    Poincare::ComplexFormat complexFormat = Cartesian,
    int numberOfSignificantDigits =
        Poincare::PrintFloat::SignificantDecimalDigits<T>());
