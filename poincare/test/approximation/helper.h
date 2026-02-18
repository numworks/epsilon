#pragma once

#include <poincare/user_expression.h>
#include <quiz.h>

#include "../helper.h"

template <typename T>
void approximates_to(const Poincare::Internal::Tree* n, T f);
template <typename T>
void approximates_to(const Poincare::Internal::Tree* n,
                     const Poincare::Internal::Tree* expected);
template <typename T>
void approximates_to(const char* input, T f,
                     const Poincare::SimplificationParameters&
                         simplificationParameters = k_realParams);
template <typename T>
void approximates_to(
    const char* input, const char* output,
    const Poincare::SimplificationParameters& simplificationParameters =
        k_realParams,
    int nbOfSignificantDigits =
        Poincare::PrintFloat::k_undefinedNumberOfSignificantDigits);
void approximates_to_float_and_double(
    const char* expression, const char* result,
    Poincare::SimplificationParameters simplificationParameters = k_realParams);
void approximates_to_boolean(
    const Poincare::Internal::Tree* n,
    Poincare::Internal::Approximation::BooleanOrUndefined expected);
void approximates_to_boolean(
    const char* input,
    Poincare::Internal::Approximation::BooleanOrUndefined expected,
    const Poincare::SimplificationParameters& simplificationParameters);

template <typename T>
void assert_float_approximates_to(Poincare::UserExpression f,
                                  const char* result);

template <typename T>
void simplified_approximates_to(
    const char* input, const char* output,
    const Poincare::SimplificationParameters& simplificationParameters =
        k_realParams,
    int nbOfSignificantDigits =
        Poincare::PrintFloat::k_undefinedNumberOfSignificantDigits);
template <typename T>
void projected_approximates_to(
    const char* input, const char* output,
    const Poincare::SimplificationParameters& simplificationParameters =
        k_realParams,
    int nbOfSignificantDigits =
        Poincare::PrintFloat::k_undefinedNumberOfSignificantDigits);
template <typename T>
void approximates_to_keeping_symbols(
    const char* expression, const char* simplifiedExpression,
    const Poincare::SimplificationParameters& simplificationParameters =
        k_realParams,
    int numberOfSignificantDigits =
        Poincare::PrintFloat::k_undefinedNumberOfSignificantDigits);
