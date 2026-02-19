#include "helper.h"

#include <omg/float.h>
#include <poincare/src/expression/beautification.h>
#include <poincare/src/expression/variables.h>

using namespace Poincare;
using namespace Poincare::Internal;

template <typename T>
void approximates_to(const Tree* n, T f) {
  T approx = Approximation::To<T>(
      n, Approximation::Parameters{.projectLocalVariables = true,
                                   .angleUnit = AngleUnit::Radian,
                                   .complexFormat = ComplexFormat::Real});
  bool result =
      OMG::Float::RoughlyEqual<T>(approx, f, OMG::Float::EpsilonLax<T>(), true);
#if POINCARE_TREE_LOG
  if (!result) {
    std::cout << "Approximation test failure with: \n";
    n->log();
    std::cout << "Approximated to " << approx << " instead of " << f << "\n";
    std::cout << "Absolute difference is : " << std::fabs(approx - f) << "\n";
    std::cout << "Relative difference is : " << std::fabs((approx - f) / f)
              << "\n";
  }
#endif
  quiz_assert(result);
}

template <typename T>
void approximates_to(const Tree* n, const Tree* expected) {
  Tree* approx = Approximation::To<Tree*>(
      n, Approximation::Precision<T>::value,
      Approximation::Parameters{.projectLocalVariables = true,
                                .angleUnit = AngleUnit::Radian,
                                .complexFormat = ComplexFormat::Real});
  bool result = approx->treeIsIdenticalTo(expected);
#if POINCARE_TREE_LOG
  if (!result) {
    std::cout << "Approximation test failure with: ";
    n->logSerializeWithoutLineReturn();
    std::cout << " approximated to ";
    approx->logSerializeWithoutLineReturn();
    std::cout << " instead of ";
    expected->logSerialize();
  }
#endif
  quiz_assert(result);
  approx->removeTree();
}

template <typename T>
void approximates_to(const char* input, T f,
                     const SimplificationParameters& simplificationParameters) {
  Tree* expression = parse(input, simplificationParameters.context);
  T approx = Approximation::To<T>(
      expression, Approximation::Parameters{
                      .projectLocalVariables = true,
                      .angleUnit = simplificationParameters.angleUnit,
                      .complexFormat = simplificationParameters.complexFormat,
                      .symbolContext = simplificationParameters.context});
  bool result =
      OMG::Float::RoughlyEqual<T>(approx, f, OMG::Float::EpsilonLax<T>(), true);
#if POINCARE_TREE_LOG
  if (!result) {
    std::cout << "Approximation test failure with: " << input << "\n";
    std::cout << "Approximated to " << approx << " instead of " << f << "\n";
    std::cout << "Absolute difference is : " << std::fabs(approx - f) << "\n";
    std::cout << "Relative difference is : " << std::fabs((approx - f) / f)
              << "\n";
  }
#endif
  quiz_assert(result);
  expression->removeTree();
}

template <typename T>
void approximates_to(const char* input, const char* output,
                     const SimplificationParameters& simplificationParameters,
                     int nbOfSignificantDigits) {
  // TODO: use same test and log as approximates_to?
  process_tree_and_compare(
      input, output,
      [](Tree* tree, const SimplificationParameters& simplificationParameters) {
        tree->moveTreeOverTree(Approximation::To<Tree*>(
            tree, Approximation::Precision<T>::value,
            Approximation::Parameters{
                .projectLocalVariables = true,
                .angleUnit = simplificationParameters.angleUnit,
                .complexFormat = simplificationParameters.complexFormat,
                .symbolContext = simplificationParameters.context}));
        Beautification::DeepBeautify(
            tree, ToBeautificationParameters(simplificationParameters));
      },
      simplificationParameters, nbOfSignificantDigits);
}

void approximates_to_float_and_double(
    const char* expression, const char* result,
    Poincare::SimplificationParameters simplificationParameters) {
  /* Reduce significant numbers to 3 to handle platforms discrepancies when
   * computing floats. This allows to expect the same results from both double
   * and float approximations. */
  approximates_to<float>(expression, result, simplificationParameters, 3);
  approximates_to<double>(expression, result, simplificationParameters, 3);
}

void approximates_to_boolean(const Tree* n,
                             Approximation::BooleanOrUndefined expected) {
  Approximation::BooleanOrUndefined approx =
      Approximation::To<Approximation::BooleanOrUndefined>(
          n, Approximation::FloatPrecision::Single,
          Approximation::Parameters{.projectLocalVariables = true,
                                    .angleUnit = AngleUnit::Radian,
                                    .complexFormat = ComplexFormat::Real});
  quiz_assert(approx.isUndefined() == expected.isUndefined());
  if (!approx.isUndefined()) {
    quiz_assert(approx.value() == expected.value());
  }
}

void approximates_to_boolean(
    const char* input, Approximation::BooleanOrUndefined expected,
    const SimplificationParameters& simplificationParameters) {
  Tree* expression = parse(input, simplificationParameters.context);
  Approximation::BooleanOrUndefined approx =
      Approximation::To<Approximation::BooleanOrUndefined>(
          expression, Approximation::FloatPrecision::Single,
          Approximation::Parameters{
              .projectLocalVariables = true,
              .angleUnit = simplificationParameters.angleUnit,
              .complexFormat = simplificationParameters.complexFormat,
              .symbolContext = simplificationParameters.context});
  quiz_assert(approx.isUndefined() == expected.isUndefined());
  if (!approx.isUndefined()) {
    quiz_assert(approx.value() == expected.value());
  }
  expression->removeTree();
}

template <typename T>
void assert_float_approximates_to(UserExpression f, const char* result) {
  int numberOfDigits = PrintFloat::SignificantDecimalDigits<T>();
  char buffer[500];
  f.cloneAndApproximate<T>(Poincare::SimplificationParameters{
                               .complexFormat = ComplexFormat::Cartesian,
                               .angleUnit = AngleUnit::Radian})
      .serialize(buffer, false, DecimalMode, numberOfDigits);
  quiz_assert_print_if_failure(strcmp(buffer, result) == 0, result);
}

template <typename T>
void simplified_approximates_to(
    const char* input, const char* output,
    const SimplificationParameters& simplificationParameters,
    int nbOfSignificantDigits) {
  process_tree_and_compare(
      input, output,
      [](Tree* tree, const SimplificationParameters& simplificationParameters) {
        simplify(tree, simplificationParameters, false);
        tree->moveTreeOverTree(Approximation::To<Tree*>(
            tree, Approximation::Precision<T>::value,
            Approximation::Parameters{
                .projectLocalVariables = true,
                .optimize = true,
                .angleUnit = simplificationParameters.angleUnit,
                .complexFormat = simplificationParameters.complexFormat,
                .symbolContext = simplificationParameters.context}));
        Beautification::DeepBeautify(
            tree, ToBeautificationParameters(simplificationParameters));
      },
      simplificationParameters, nbOfSignificantDigits);
}

template <typename T>
void projected_approximates_to(
    const char* input, const char* output,
    const SimplificationParameters& simplificationParameters,
    int nbOfSignificantDigits) {
  process_tree_and_compare(
      input, output,
      [](Tree* tree, const SimplificationParameters& simplificationParameters) {
        Simplification::ToSystem(
            tree, ToProjectionParameters(simplificationParameters));
        tree->moveTreeOverTree(Approximation::To<Tree*>(
            tree, Approximation::Precision<T>::value,
            Approximation::Parameters{
                .prepare = true,
                .optimize = true,
                .angleUnit = simplificationParameters.angleUnit,
                .complexFormat = simplificationParameters.complexFormat,
                .symbolContext = simplificationParameters.context}));
        Beautification::DeepBeautify(
            tree, ToBeautificationParameters(simplificationParameters));
      },
      simplificationParameters, nbOfSignificantDigits);
}

template <typename T>
void approximates_to_keeping_symbols(
    const char* expression, const char* simplifiedExpression,
    const SimplificationParameters& simplificationParameters,
    int numberOfSignificantDigits) {
  process_tree_and_compare(
      expression, simplifiedExpression,
      [](Tree* tree, const SimplificationParameters& simplificationParameters) {
        Variables::ProjectLocalVariablesToId(tree);
        Approximation::Approximate(
            tree, Approximation::Precision<T>::value,
            Approximation::Parameters{
                .onlyReplaceNumerical = true,
                .angleUnit = simplificationParameters.angleUnit,
                .complexFormat = simplificationParameters.complexFormat,
                .symbolContext = simplificationParameters.context});
        Variables::BeautifyToName(tree);
      },
      simplificationParameters, numberOfSignificantDigits);
}

template void approximates_to<float>(const Tree* n, float f);
template void approximates_to<double>(const Tree* n, double f);

template void approximates_to<float>(const Tree* n, const Tree* expected);
template void approximates_to<double>(const Tree* n, const Tree* expected);

template void approximates_to<float>(
    const char* input, float f,
    const SimplificationParameters& simplificationParameters);
template void approximates_to<double>(
    const char* input, double f,
    const SimplificationParameters& simplificationParameters);

template void approximates_to<float>(
    const char* input, const char* output,
    const SimplificationParameters& simplificationParameters,
    int nbOfSignificantDigits);
template void approximates_to<double>(
    const char* input, const char* output,
    const SimplificationParameters& simplificationParameters,
    int nbOfSignificantDigits);

template void assert_float_approximates_to<float>(UserExpression f,
                                                  const char* result);
template void assert_float_approximates_to<double>(UserExpression f,
                                                   const char* result);

template void simplified_approximates_to<float>(
    const char* input, const char* output,
    const SimplificationParameters& simplificationParameters,
    int nbOfSignificantDigits);
template void simplified_approximates_to<double>(
    const char* input, const char* output,
    const SimplificationParameters& simplificationParameters,
    int nbOfSignificantDigits);

template void projected_approximates_to<float>(
    const char* input, const char* output,
    const SimplificationParameters& simplificationParameters,
    int nbOfSignificantDigits);
template void projected_approximates_to<double>(
    const char* input, const char* output,
    const SimplificationParameters& simplificationParameters,
    int nbOfSignificantDigits);

template void approximates_to_keeping_symbols<float>(
    const char* expression, const char* simplifiedExpression,
    const SimplificationParameters& simplificationParameters,
    int numberOfSignificantDigits);
template void approximates_to_keeping_symbols<double>(
    const char* expression, const char* simplifiedExpression,
    const SimplificationParameters& simplificationParameters,
    int numberOfSignificantDigits);
