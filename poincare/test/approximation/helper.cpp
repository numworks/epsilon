#include "helper.h"

#include <omg/float.h>
#include <poincare/src/expression/beautification.h>
#include <poincare/src/expression/variables.h>

using namespace Poincare;
using namespace Poincare::Internal;

template <typename T>
void approximates_to(const Tree* n, T f) {
  T approx = Approximation::To<T>(
      n,
      Approximation::Parameters{.policy = {.projectLocalVariables = true},
                                .user = {.complexFormat = ComplexFormat::Real,
                                         .angleUnit = AngleUnit::Radian}});
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
      n, Precision<T>::value,
      Approximation::Parameters{.policy = {.projectLocalVariables = true},
                                .user = {.complexFormat = ComplexFormat::Real,
                                         .angleUnit = AngleUnit::Radian}});
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
                     const Poincare::UserSettings& userSettings) {
  Tree* expression = parse(input, userSettings.context);
  T approx = Approximation::To<T>(
      expression,
      Approximation::Parameters{.policy = {.projectLocalVariables = true},
                                .user = userSettings});
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
                     const Poincare::UserSettings& userSettings,
                     int nbOfSignificantDigits) {
  // TODO: use same test and log as approximates_to?
  process_tree_and_compare(
      input, output,
      [](Tree* tree,
         const Poincare::SimplificationParameters& simplificationParameters) {
        tree->moveTreeOverTree(Approximation::To<Tree*>(
            tree, Precision<T>::value,
            Approximation::Parameters{.policy = {.projectLocalVariables = true},
                                      .user = simplificationParameters.user}));
        Beautification::DeepBeautify(
            tree, ToBeautificationParameters(simplificationParameters));
      },
      SimplificationParameters{.user = ToUserSettingsWithUnit(userSettings)},
      nbOfSignificantDigits);
}

void approximates_to_float_and_double(
    const char* expression, const char* result,
    const Poincare::UserSettings& userSettings) {
  /* Reduce significant numbers to 3 to handle platforms discrepancies when
   * computing floats. This allows to expect the same results from both double
   * and float approximations. */
  approximates_to<float>(expression, result, userSettings, 3);
  approximates_to<double>(expression, result, userSettings, 3);
}

void approximates_to_boolean(const Tree* n,
                             Approximation::BooleanOrUndefined expected) {
  Approximation::BooleanOrUndefined approx =
      Approximation::To<Approximation::BooleanOrUndefined>(
          n, FloatPrecision::Single,
          Approximation::Parameters{
              .policy = {.projectLocalVariables = true},
              .user = {.complexFormat = ComplexFormat::Real,
                       .angleUnit = AngleUnit::Radian}});
  quiz_assert(approx.isUndefined() == expected.isUndefined());
  if (!approx.isUndefined()) {
    quiz_assert(approx.value() == expected.value());
  }
}

void approximates_to_boolean(const char* input,
                             Approximation::BooleanOrUndefined expected,
                             const Poincare::UserSettings& userSettings) {
  Tree* expression = parse(input, userSettings.context);
  Approximation::BooleanOrUndefined approx =
      Approximation::To<Approximation::BooleanOrUndefined>(
          expression, FloatPrecision::Single,
          Approximation::Parameters{.policy = {.projectLocalVariables = true},
                                    .user = userSettings});
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
  f.cloneAndApproximate(
       Poincare::UserSettingsWithUnit{.complexFormat = ComplexFormat::Cartesian,
                                      .angleUnit = AngleUnit::Radian},
       false, Poincare::Precision<T>::value)
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
            tree, Precision<T>::value,
            Approximation::Parameters{
                .policy = {.projectLocalVariables = true, .optimize = true},
                .user = simplificationParameters.user}));
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
            tree, Precision<T>::value,
            Approximation::Parameters{
                .policy = {.prepare = true, .optimize = true},
                .user = simplificationParameters.user}));
        Beautification::DeepBeautify(
            tree, ToBeautificationParameters(simplificationParameters));
      },
      simplificationParameters, nbOfSignificantDigits);
}

template <typename T>
void approximates_to_keeping_symbols(const char* expression,
                                     const char* simplifiedExpression,
                                     const Poincare::UserSettings& userSettings,
                                     int numberOfSignificantDigits) {
  process_tree_and_compare(
      expression, simplifiedExpression,
      [](Tree* tree, const SimplificationParameters& simplificationParameters) {
        Variables::ProjectLocalVariablesToId(tree);
        Approximation::Approximate(
            tree, Precision<T>::value,
            Approximation::Parameters{.policy = {.onlyReplaceNumerical = true},
                                      .user = simplificationParameters.user});
        Variables::BeautifyToName(tree);
      },
      SimplificationParameters{.user = ToUserSettingsWithUnit(userSettings)},
      numberOfSignificantDigits);
}

template void approximates_to<float>(const Tree* n, float f);
template void approximates_to<double>(const Tree* n, double f);

template void approximates_to<float>(const Tree* n, const Tree* expected);
template void approximates_to<double>(const Tree* n, const Tree* expected);

template void approximates_to<float>(
    const char* input, float f, const Poincare::UserSettings& userSettings);
template void approximates_to<double>(
    const char* input, double f, const Poincare::UserSettings& userSettings);

template void approximates_to<float>(const char* input, const char* output,
                                     const Poincare::UserSettings& userSettings,
                                     int nbOfSignificantDigits);
template void approximates_to<double>(
    const char* input, const char* output,
    const Poincare::UserSettings& userSettings, int nbOfSignificantDigits);

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
    const Poincare::UserSettings& userSettings, int numberOfSignificantDigits);
template void approximates_to_keeping_symbols<double>(
    const char* expression, const char* simplifiedExpression,
    const Poincare::UserSettings& userSettings, int numberOfSignificantDigits);
