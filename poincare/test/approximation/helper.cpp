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
      Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                .projectLocalVariables = true},
      Approximation::Context(AngleUnit::Radian, ComplexFormat::Real));
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
void approximates_to(const char* input, T f,
                     const ProjectionContext& projectionContext) {
  Tree* expression = parse(input, projectionContext.m_context);
  T approx = Approximation::To<T>(
      expression,
      Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                .projectLocalVariables = true},
      Approximation::Context(projectionContext.m_angleUnit,
                             projectionContext.m_complexFormat,
                             projectionContext.m_context));
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
                     const ProjectionContext& projectionContext,
                     int nbOfSignificantDigits) {
  // TODO: use same test and log as approximates_to?
  process_tree_and_compare(
      input, output,
      [](Tree* tree, ProjectionContext projectionContext) {
        tree->moveTreeOverTree(Approximation::ToTree<T>(
            tree,
            Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                      .projectLocalVariables = true},
            Approximation::Context(projectionContext.m_angleUnit,
                                   projectionContext.m_complexFormat,
                                   projectionContext.m_context)));
        Beautification::DeepBeautify(tree, projectionContext);
      },
      projectionContext, nbOfSignificantDigits);
}

void approximates_to_float_and_double(
    const char* expression, const char* result,
    Poincare::Internal::ProjectionContext projCtx) {
  /* Reduce significant numbers to 3 to handle platforms discrepancies when
   * computing floats. This allows to expect the same results from both double
   * and float approximations. */
  approximates_to<float>(expression, result, projCtx, 3);
  approximates_to<double>(expression, result, projCtx, 3);
}

void approximates_to_boolean(const Tree* n,
                             Approximation::BooleanOrUndefined expected) {
  Approximation::BooleanOrUndefined approx = Approximation::ToBoolean<float>(
      n,
      Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                .projectLocalVariables = true},
      Approximation::Context(AngleUnit::Radian, ComplexFormat::Real));
  quiz_assert(approx.isUndefined() == expected.isUndefined());
  if (!approx.isUndefined()) {
    quiz_assert(approx.value() == expected.value());
  }
}

void approximates_to_boolean(const char* input,
                             Approximation::BooleanOrUndefined expected,
                             const ProjectionContext& projectionContext) {
  Tree* expression = parse(input, projectionContext.m_context);
  Approximation::BooleanOrUndefined approx = Approximation::ToBoolean<float>(
      expression,
      Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                .projectLocalVariables = true},
      Approximation::Context(projectionContext.m_angleUnit,
                             projectionContext.m_complexFormat,
                             projectionContext.m_context));
  quiz_assert(approx.isUndefined() == expected.isUndefined());
  if (!approx.isUndefined()) {
    quiz_assert(approx.value() == expected.value());
  }
  expression->removeTree();
}

template <typename T>
void assert_float_approximates_to(UserExpression f, const char* result) {
  ProjectionContext projectionContext = Poincare::Internal::ProjectionContext{
      .m_complexFormat = ComplexFormat::Cartesian,
      .m_angleUnit = AngleUnit::Radian};
  int numberOfDigits = PrintFloat::SignificantDecimalDigits<T>();
  char buffer[500];
  f.cloneAndApproximate<T>(projectionContext)
      .serialize(buffer, false, DecimalMode, numberOfDigits);
  quiz_assert_print_if_failure(strcmp(buffer, result) == 0, result);
}

template <typename T>
void simplified_approximates_to(const char* input, const char* output,
                                const ProjectionContext& projectionContext,
                                int nbOfSignificantDigits) {
  process_tree_and_compare(
      input, output,
      [](Tree* tree, ProjectionContext projectionContext) {
        simplify(tree, projectionContext, false);
        tree->moveTreeOverTree(Approximation::ToTree<T>(
            tree,
            Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                      .projectLocalVariables = true},
            Approximation::Context(projectionContext.m_angleUnit,
                                   projectionContext.m_complexFormat,
                                   projectionContext.m_context)));
        Beautification::DeepBeautify(tree, projectionContext);
      },
      projectionContext, nbOfSignificantDigits);
}

template <typename T>
void projected_approximates_to(const char* input, const char* output,
                               const ProjectionContext& projectionContext,
                               int nbOfSignificantDigits) {
  process_tree_and_compare(
      input, output,
      [](Tree* tree, ProjectionContext projectionContext) {
        Simplification::ToSystem(tree, &projectionContext);
        tree->moveTreeOverTree(Approximation::ToTree<T>(
            tree,
            Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                      .prepare = true},
            Approximation::Context(projectionContext.m_angleUnit,
                                   projectionContext.m_complexFormat,
                                   projectionContext.m_context)));
        Beautification::DeepBeautify(tree, projectionContext);
      },
      projectionContext, nbOfSignificantDigits);
}

template <typename T>
void approximates_to_keeping_symbols(const char* expression,
                                     const char* simplifiedExpression,
                                     const ProjectionContext& projectionContext,
                                     int numberOfSignificantDigits) {
  process_tree_and_compare(
      expression, simplifiedExpression,
      [](Tree* tree, ProjectionContext projectionContext) {
        Variables::ProjectLocalVariablesToId(tree);
        Approximation::ApproximateAndReplaceEveryScalar<T>(
            tree, Approximation::Context(projectionContext.m_angleUnit,
                                         projectionContext.m_complexFormat,
                                         projectionContext.m_context));
        Variables::BeautifyToName(tree);
      },
      projectionContext, numberOfSignificantDigits);
}

template void approximates_to<float>(const Tree* n, float f);
template void approximates_to<double>(const Tree* n, double f);

template void approximates_to<float>(
    const char* input, float f, const ProjectionContext& projectionContext);
template void approximates_to<double>(
    const char* input, double f, const ProjectionContext& projectionContext);

template void approximates_to<float>(const char* input, const char* output,
                                     const ProjectionContext& projectionContext,
                                     int nbOfSignificantDigits);
template void approximates_to<double>(
    const char* input, const char* output,
    const ProjectionContext& projectionContext, int nbOfSignificantDigits);

template void assert_float_approximates_to<float>(UserExpression f,
                                                  const char* result);
template void assert_float_approximates_to<double>(UserExpression f,
                                                   const char* result);

template void simplified_approximates_to<float>(
    const char* input, const char* output,
    const ProjectionContext& projectionContext, int nbOfSignificantDigits);
template void simplified_approximates_to<double>(
    const char* input, const char* output,
    const ProjectionContext& projectionContext, int nbOfSignificantDigits);

template void projected_approximates_to<float>(
    const char* input, const char* output,
    const ProjectionContext& projectionContext, int nbOfSignificantDigits);
template void projected_approximates_to<double>(
    const char* input, const char* output,
    const ProjectionContext& projectionContext, int nbOfSignificantDigits);

template void approximates_to_keeping_symbols<float>(
    const char* expression, const char* simplifiedExpression,
    const ProjectionContext& projectionContext, int numberOfSignificantDigits);
template void approximates_to_keeping_symbols<double>(
    const char* expression, const char* simplifiedExpression,
    const ProjectionContext& projectionContext, int numberOfSignificantDigits);
