#include "helper.h"

#include <apps/shared/global_context.h>
#include <poincare/print.h>
#include <poincare/src/expression/beautification.h>
#include <poincare/src/expression/builtin.h>
#include <poincare/src/expression/float_helper.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/simplification.h>
#include <poincare/src/layout/layouter.h>
#include <poincare/src/layout/parsing/rack_parser.h>
#include <poincare/src/layout/rack_from_text.h>
#include <poincare/src/layout/serialize.h>
#include <poincare/src/memory/tree_stack_checkpoint.h>
#include <poincare/test/helper.h>

using namespace Poincare;

const char *MaxIntegerString() {
  // (2^32)^k_maxNumberOfDigits-1
  static const char *s =
      "179769313486231590772930519078902473361797697894230657273430081157732675"
      "805500963132708477322407536021120113879871393357658789768814416622492847"
      "430639474124377767893424865485276302219601246094119453082952085005768838"
      "150682342462881473913110540827237163350510684586298239947245938479716304"
      "835356329624224137215";
  return s;
}

const char *OverflowedIntegerString() {
  // (2^32)^k_maxNumberOfDigits
  static const char *s =
      "179769313486231590772930519078902473361797697894230657273430081157732675"
      "805500963132708477322407536021120113879871393357658789768814416622492847"
      "430639474124377767893424865485276302219601246094119453082952085005768838"
      "150682342462881473913110540827237163350510684586298239947245938479716304"
      "835356329624224137216";
  return s;
}

const char *BigOverflowedIntegerString() {
  // OverflowedIntegerString() with a 2 on first digit
  static const char *s =
      "279769313486231590772930519078902473361797697894230657273430081157732675"
      "805500963132708477322407536021120113879871393357658789768814416622492847"
      "430639474124377767893424865485276302219601246094119453082952085005768838"
      "150682342462881473913110540827237163350510684586298239947245938479716304"
      "835356329624224137216";
  return s;
}

const char *MaxParsedIntegerString() {
  // 10^k_maxNumberOfParsedDigitsBase10 - 1
  static const char *s = "999999999999999999999999999999";
  return s;
}

const char *ApproximatedParsedIntegerString() {
  // 10^k_maxNumberOfParsedDigitsBase10
  static const char *s = "1000000000000000000000000000000";
  return s;
}

void quiz_assert_log_if_failure(bool test, PoolHandle tree) {
  if (!test) {
#if POINCARE_TREE_LOG
    quiz_print("TEST FAILURE WHILE TESTING:");
    tree.log();
#endif
  }
  quiz_assert(test);
}

void build_failure_infos(char *returnedInformationsBuffer, size_t bufferSize,
                         const char *expression, const char *result,
                         const char *expectedResult) {
  Print::UnsafeCustomPrintf(returnedInformationsBuffer, bufferSize,
                            "FAILURE: %s processed to %s instead of %s",
                            expression, result, expectedResult);
}

void copy_without_system_chars(char *buffer, const char *input) {
  while (char c = *input++) {
    if (c == 0x11) continue;
    if (c == 0x14) continue;
    c = (c == 0x12) ? '(' : (c == 0x13) ? ')' : c;
    *buffer++ = c;
  }
  *buffer = 0;
}

void assert_parsed_expression_process_to(
    const char *expression, const char *oldResult, ReductionTarget target,
    Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit,
    Preferences::UnitFormat unitFormat, SymbolicComputation symbolicComputation,
    UnitConversion unitConversion, ProcessExpression process,
    int numberOfSignificantDigits) {
  Shared::GlobalContext globalContext;
  constexpr int bufferSize = 2048;
  char buffer[bufferSize];
  char result[bufferSize];
  copy_without_system_chars(result, oldResult);
  assert(SharedTreeStack->numberOfTrees() == 0);
  Tree *e = parse_expression(expression, &globalContext);
  Tree *m = process(
      e, ReductionContext(&globalContext, complexFormat, angleUnit, unitFormat,
                          target, symbolicComputation, unitConversion));
  Tree *l =
      Internal::Layouter::LayoutExpression(m, true, numberOfSignificantDigits);
  *Internal::Serialize(l, buffer, buffer + bufferSize) = 0;
  copy_without_system_chars(buffer, buffer);
  l->removeTree();
  bool test = strcmp(buffer, result) == 0;
  assert(SharedTreeStack->numberOfTrees() == 0);
#if POINCARE_STRICT_TESTS
  char information[bufferSize] = "";
  if (!test) {
    build_failure_infos(information, bufferSize, expression, buffer, result);
  }
  quiz_assert_print_if_failure(test, information);
#else
  quiz_tolerate_print_if_failure(test, expression, result, buffer);
#endif
}

Internal::Tree *parse_expression(const char *expression, Context *context,
                                 bool parseForAssignment) {
  Tree *inputLayout = RackFromText(expression);
  RackParser parser(inputLayout, context,
                    parseForAssignment
                        ? Internal::ParsingContext::ParsingMethod::Assignment
                        : Internal::ParsingContext::ParsingMethod::Classic);
  bool success = parser.parse() != nullptr;
  inputLayout->removeTree();
  Tree *result = success ? inputLayout : nullptr;
  quiz_assert_print_if_failure(result != nullptr, expression);
  return result;
}

void assert_parsed_expression_is(const char *expression,
                                 const Poincare::Internal::Tree *expected,
                                 bool parseForAssignment) {
  Shared::GlobalContext context;
  Tree *parsed = parse_expression(expression, &context, parseForAssignment);
  bool test = parsed && parsed->treeIsIdenticalTo(expected);
  if (parsed) {
    parsed->removeTree();
  }
#if POINCARE_STRICT_TESTS
  quiz_assert(test);
#else
  quiz_tolerate_print_if_failure(test, expression, "parsed and identical",
                                 parsed ? "not parsed" : "not identical");
#endif
}

void assert_parse_to_same_expression(const char *expression1,
                                     const char *expression2) {
  Shared::GlobalContext context;
  Tree *e1 = parse_expression(expression1, &context);
  Tree *e2 = parse_expression(expression2, &context);
  quiz_assert(e1);
  quiz_assert(e2);
  quiz_assert(e1->treeIsIdenticalTo(e2));
}

void assert_reduce_and_store(const char *expression,
                             Preferences::AngleUnit angleUnit,
                             Preferences::UnitFormat unitFormat,
                             Preferences::ComplexFormat complexFormat,
                             ReductionTarget target) {
  // TODO_PCJ: reduce expression (to check it stays a store expression)
  if (Poincare::Context::GlobalContext) {
    store(expression, Poincare::Context::GlobalContext);
  } else {
    /* TODO: we should assert a global context exists instead since it doesn't
     * make much sense to store in a temporary context. */
    Shared::GlobalContext globalContext;
    store(expression, &globalContext);
  }
}

void assert_parsed_expression_simplify_to(
    const char *expression, const char *simplifiedExpression,
    ReductionTarget target, Preferences::AngleUnit angleUnit,
    Preferences::UnitFormat unitFormat,
    Preferences::ComplexFormat complexFormat,
    SymbolicComputation symbolicComputation, UnitConversion unitConversion) {
  assert_parsed_expression_process_to(
      expression, simplifiedExpression, target, complexFormat, angleUnit,
      unitFormat, symbolicComputation, unitConversion,
      [](Tree *e, ReductionContext reductionContext) {
        Internal::ProjectionContext context = {
            .m_complexFormat = reductionContext.complexFormat(),
            .m_angleUnit = reductionContext.angleUnit(),
            .m_expansionStrategy =
                (reductionContext.target() ==
                 ReductionTarget::SystemForAnalysis)
                    ? Poincare::Internal::ExpansionStrategy::ExpandAlgebraic
                    : Poincare::Internal::ExpansionStrategy::None,
            .m_unitFormat = reductionContext.unitFormat(),
            .m_symbolic = reductionContext.symbolicComputation(),
            .m_context = reductionContext.context()};
        simplify(e, &context);
        // TODO_PCJ also approximate to see if it crashes
        return e;
      });
}

template <typename T>
void assert_expression_approximates_to(const char *expression,
                                       const char *approximation,
                                       Preferences::AngleUnit angleUnit,
                                       Preferences::UnitFormat unitFormat,
                                       Preferences::ComplexFormat complexFormat,
                                       int numberOfSignificantDigits) {
  assert_parsed_expression_process_to(
      expression, approximation, SystemForApproximation, complexFormat,
      angleUnit, unitFormat, ReplaceAllSymbols, DefaultUnitConversion,
      [](Tree *e, ReductionContext reductionContext) -> Tree * {
        Internal::ProjectionContext context = {
            .m_complexFormat = reductionContext.complexFormat(),
            .m_angleUnit = reductionContext.angleUnit(),
            .m_expansionStrategy =
                (reductionContext.target() ==
                 ReductionTarget::SystemForAnalysis)
                    ? Poincare::Internal::ExpansionStrategy::ExpandAlgebraic
                    : Poincare::Internal::ExpansionStrategy::None,
            .m_unitFormat = reductionContext.unitFormat(),
            .m_symbolic = reductionContext.symbolicComputation(),
            .m_context = reductionContext.context()};
        /* tree is projected beforehand so we can prepare it for approximation,
         * and have better results on integrals for example. */
        Simplification::ToSystem(e, &context);
        TreeRef result = Internal::Approximation::ToTree<T>(
            e,
            Internal::Approximation::Parameters{.isRootAndCanHaveRandom = true,
                                                .prepare = true},
            Internal::Approximation::Context(reductionContext.angleUnit(),
                                             reductionContext.complexFormat(),
                                             reductionContext.context()));
        Beautification::DeepBeautify(result, context);
        e->removeTree();
        return result;
      },
      numberOfSignificantDigits);
}

void assert_expression_approximates_keeping_symbols_to(
    const char *expression, const char *simplifiedExpression,
    Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat,
    Preferences::ComplexFormat complexFormat, int numberOfSignificantDigits) {
  assert_parsed_expression_process_to(
      expression, simplifiedExpression, SystemForApproximation, complexFormat,
      angleUnit, unitFormat, ReplaceDefinedSymbols, DefaultUnitConversion,
      [](Tree *e, ReductionContext reductionContext) {
#if 0
        Tree *simplifiedExpression;
        e.cloneAndSimplifyAndApproximate(
            &simplifiedExpression, nullptr, reductionContext.context(),
            reductionContext.complexFormat(), reductionContext.angleUnit(),
            reductionContext.unitFormat(),
            reductionContext.symbolicComputation(),
            reductionContext.unitConversion(), true);
        return simplifiedExpression;
#endif
        return e;
      },
      numberOfSignificantDigits);
}

template <typename T>
void assert_expression_simplifies_approximates_to(
    const char *expression, const char *approximation,
    Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat,
    Preferences::ComplexFormat complexFormat, int numberOfSignificantDigits) {
#if 0
  assert_parsed_expression_process_to(
      expression, approximation, SystemForApproximation, complexFormat,
      angleUnit, unitFormat, ReplaceAllSymbols,
      DefaultUnitConversion,
      [](Tree *e, ReductionContext reductionContext) {
        e = e.cloneAndSimplify(reductionContext);
       quiz_assert(!e.isUninitialized());
        return e.approximate<T>(ApproximationContext(reductionContext));
      },
      numberOfSignificantDigits);
#endif
}

void assert_expression_serializes_to(const Tree *expression,
                                     const char *serialization,
                                     Preferences::PrintFloatMode mode,
                                     int numberOfSignificantDigits,
                                     OMG::Base base) {
  constexpr int bufferSize = 500;
  char buffer[bufferSize];
  Tree *layout = Internal::Layouter::LayoutExpression(
      expression->cloneTree(), true, numberOfSignificantDigits, mode, base);
  Serialize(layout, buffer, buffer + bufferSize);
  bool test = strcmp(serialization, buffer) == 0;
  layout->removeTree();
#if POINCARE_STRICT_TESTS
  if (!test) {
    build_failure_infos(information, bufferSize, "serialized expression",
                        buffer, serialization);
  }
  quiz_assert_print_if_failure(test, information);
#else
  quiz_tolerate_print_if_failure(test, serialization, serialization, buffer);
#endif
}

void assert_expression_serializes_and_parses_to(
    const Poincare::Internal::Tree *expression,
    const Poincare::Internal::Tree *result) {
  constexpr int bufferSize = 500;
  char buffer[bufferSize];
  Tree *layout =
      Internal::Layouter::LayoutExpression(expression->cloneTree(), true);
  *Internal::Serialize(layout, buffer, buffer + bufferSize) = 0;
  layout->removeTree();
  assert_parsed_expression_is(buffer, result);
}

void assert_expression_serializes_and_parses_to_itself(
    const Poincare::Internal::Tree *expression) {
  return assert_expression_serializes_and_parses_to(expression, expression);
}

void assert_expression_parses_and_serializes_to(
    const char *expression, const char *result,
    Preferences::PrintFloatMode mode, int numberOfSignificantDigits,
    OMG::Base base) {
  Shared::GlobalContext globalContext;
  Tree *e = parse_expression(expression, &globalContext);
  Tree *l = Internal::Layouter::LayoutExpression(
      e, true, numberOfSignificantDigits, mode, base);
  constexpr int bufferSize = 500;
  char buffer[bufferSize];
  Serialize(l, buffer, buffer + bufferSize);
  l->removeTree();
  const bool test = strcmp(buffer, result) == 0;
  char information[bufferSize] = "";
  if (!test) {
    build_failure_infos(information, bufferSize, expression, buffer, result);
  }
  quiz_assert_print_if_failure(test, information);
}

void assert_expression_parses_and_serializes_to_itself(const char *expression) {
  return assert_expression_parses_and_serializes_to(expression, expression);
}

template void assert_expression_approximates_to<float>(
    char const *, char const *, Preferences::AngleUnit, Preferences::UnitFormat,
    Preferences::ComplexFormat, int);
template void assert_expression_approximates_to<double>(
    char const *, char const *, Preferences::AngleUnit, Preferences::UnitFormat,
    Preferences::ComplexFormat, int);
template void assert_expression_simplifies_approximates_to<float>(
    char const *, char const *, Preferences::AngleUnit, Preferences::UnitFormat,
    Preferences::ComplexFormat, int);
template void assert_expression_simplifies_approximates_to<double>(
    char const *, char const *, Preferences::AngleUnit, Preferences::UnitFormat,
    Preferences::ComplexFormat, int);
