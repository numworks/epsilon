#include "helper.h"

#include <apps/shared/global_context.h>
#include <poincare/print.h>
#include <poincare/src/expression/beautification.h>
#include <poincare/src/expression/conversion.h>
#include <poincare/src/expression/float_helper.h>
#include <poincare/src/expression/simplification.h>
#include <poincare/src/layout/layouter.h>
#include <poincare/src/layout/serialize.h>
#include <poincare/src/memory/tree_stack_checkpoint.h>
#include <poincare/src/old/parsing/parser.h>
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
#if 0  // TODO_PCJ
#if POINCARE_TREE_LOG
    quiz_print("TEST FAILURE WHILE TESTING:");
    tree.log();
#endif
#else
    TreeStackCheckpoint::Raise(ExceptionType::Other);
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

static int k_crash;
static int k_bad;
static int k_total;

void quiz_reset_failure_ratio() {
  k_crash = 0;
  k_bad = 0;
  k_total = 0;
}

void quiz_print_failure_ratio() {
  if (!k_total) {
    return;
  }
  constexpr int bufferSize = 100;
  char buffer[bufferSize];
  int success = k_total - k_bad - k_crash;
  Poincare::Print::CustomPrintf(
      buffer, bufferSize, "  %i ok   %i bad   %i crash  / %i", success, k_bad,
      k_crash, k_total,
      // 100. * static_cast<float>(success) / static_cast<float>(k_total),
      Preferences::PrintFloatMode::Decimal, 5);
  quiz_print(buffer);
}

static void copy_without_system_chars(char *buffer, const char *input) {
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
  k_total++;
  Shared::GlobalContext globalContext;
  constexpr int bufferSize = 2048;
  char buffer[bufferSize];
  char result[bufferSize];
  copy_without_system_chars(result, oldResult);
  bool bad = false;
  bool crash = false;
  ExceptionTry {
    assert(SharedTreeStack->numberOfTrees() == 0);
    Tree *e = parse_expression(expression, &globalContext);
    Tree *m = process(e, ReductionContext(&globalContext, complexFormat,
                                          angleUnit, unitFormat, target,
                                          symbolicComputation, unitConversion));
    Tree *l = Internal::Layouter::LayoutExpression(m, true,
                                                   numberOfSignificantDigits);
    *Internal::Serialize(l, buffer, buffer + bufferSize) = 0;
    copy_without_system_chars(buffer, buffer);
    l->removeTree();
    bad = strcmp(buffer, result) != 0;
  }
  ExceptionCatch(type) {
    SharedTreeStack->flush();
    crash = true;
  }
  assert(SharedTreeStack->numberOfTrees() == 0);
  k_bad += bad;
  k_crash += crash;

  char information[bufferSize] = "";
  int i = Poincare::Print::UnsafeCustomPrintf(
      information, bufferSize, "%s\t%s\t%s",
      crash ? "CRASH" : (bad ? "BAD" : "OK"), expression, result);
  if (bad) {
    Poincare::Print::UnsafeCustomPrintf(information + i, bufferSize - i, "\t%s",
                                        buffer);
  }
  quiz_print(information);
}

Internal::Tree *parse_expression(const char *expression, Context *context,
                                 bool addParentheses, bool parseForAssignment) {
  Tree *result = parse(expression, context);
  quiz_assert_print_if_failure(result != nullptr, expression);
  // TODO: handle parameters addParentheses and parseForAssignment (cf
  // OExpression::addMissingParentheses)
  assert(!addParentheses && !parseForAssignment);
  return result;
}

void assert_parsed_expression_is(const char *expression,
                                 Poincare::OExpression r, bool addParentheses,
                                 bool parseForAssignment) {
  k_total++;
  Shared::GlobalContext context;
  bool bad = false;
  bool crash = false;
  ExceptionTry {
    assert(SharedTreeStack->numberOfTrees() == 0);
    Tree *parsed = parse_expression(expression, &context, addParentheses,
                                    parseForAssignment);
    Tree *expected = Internal::FromPoincareExpression(r);
    bad = !parsed || !expected || !parsed->treeIsIdenticalTo(expected);
    expected->removeTree();
    parsed->removeTree();
  }
  ExceptionCatch(type) {
    SharedTreeStack->flush();
    crash = true;
  }
  assert(SharedTreeStack->numberOfTrees() == 0);
  k_bad += bad;
  k_crash += crash;

  constexpr int bufferSize = 2048;
  char information[bufferSize] = "";
  Poincare::Print::UnsafeCustomPrintf(information, bufferSize, "%s\t%s",
                                      crash ? "CRASH" : (bad ? "BAD" : "OK"),
                                      expression);
  quiz_print(information);
}

void assert_parse_to_same_expression(const char *expression1,
                                     const char *expression2) {
#if 0
  Shared::GlobalContext context;
  OExpression e1 = parse_expression(expression1, &context);
  OExpression e2 = parse_expression(expression2, &context);
  quiz_assert(e1.isIdenticalTo(e2));
#endif
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
            .m_unitFormat = reductionContext.unitFormat(),
            .m_symbolic = reductionContext.symbolicComputation(),
            .m_context = reductionContext.context()};
        Internal::Simplification::SimplifyWithAdaptiveStrategy(e, &context);
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
      angleUnit, unitFormat, ReplaceAllSymbolsWithDefinitionsOrUndefined,
      DefaultUnitConversion,
      [](Tree *e, ReductionContext reductionContext) -> Tree * {
        Internal::ProjectionContext context = {
            .m_complexFormat = reductionContext.complexFormat(),
            .m_angleUnit = reductionContext.angleUnit(),
            .m_unitFormat = reductionContext.unitFormat(),
            .m_symbolic = reductionContext.symbolicComputation(),
            .m_context = reductionContext.context()};
        /* TODO_PCJ: replacing symbols should be done in RootTreeToTree or
         * RootTreeToTree should be called on simplified trees. */
        /* TODO_PCJ: use ToSystem instead of PrepareForProjection and see the
         * tests that fail */
        // Replace user symbols, seed randoms and check dimensions
        Simplification::ToSystem(e, &context);
        if (Internal::Dimension::Get(e).isUnit()) {
          // no unit approximation yet
          e->removeTree();
          return KUndef->cloneTree();
        }
        if (!Internal::Dimension::Get(e).isScalar() ||
            Internal::Dimension::ListLength(e) !=
                Internal::Dimension::k_nonListListLength) {
          TreeRef result = Internal::Approximation::RootTreeToTree<T>(
              e, Internal::AngleUnit(reductionContext.angleUnit()),
              Internal::ComplexFormat(reductionContext.complexFormat()));
          e->removeTree();
          return result;
        }
        Approximation::PrepareExpressionForApproximation(
            e, context.m_complexFormat);
        std::complex<T> value = Approximation::RootPreparedToComplex<T>(e);
        e->moveTreeOverTree(Beautification::PushBeautifiedComplex(
            value, context.m_complexFormat));
        return e;
      },
      numberOfSignificantDigits);
}

void assert_expression_approximates_keeping_symbols_to(
    const char *expression, const char *simplifiedExpression,
    Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat,
    Preferences::ComplexFormat complexFormat, int numberOfSignificantDigits) {
  assert_parsed_expression_process_to(
      expression, simplifiedExpression, SystemForApproximation, complexFormat,
      angleUnit, unitFormat, ReplaceAllDefinedSymbolsWithDefinition,
      DefaultUnitConversion,
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
      angleUnit, unitFormat, ReplaceAllSymbolsWithDefinitionsOrUndefined,
      DefaultUnitConversion,
      [](Tree *e, ReductionContext reductionContext) {
        e = e.cloneAndSimplify(reductionContext);
        return e.approximate<T>(ApproximationContext(reductionContext));
      },
      numberOfSignificantDigits);
#endif
}

void assert_expression_serializes_to(Tree *expression,
                                     const char *serialization,
                                     Preferences::PrintFloatMode mode,
                                     int numberOfSignificantDigits) {
#if 0
  constexpr int bufferSize = 500;
  char buffer[bufferSize];
  expression.serialize(buffer, bufferSize, mode, numberOfSignificantDigits);
  bool test = strcmp(serialization, buffer) == 0;
  char information[bufferSize] = "";
  if (!test) {
    build_failure_infos(information, bufferSize, "serialized expression",
                        buffer, serialization);
  }
  quiz_assert_print_if_failure(test, information);
#endif
}

void assert_expression_serializes_and_parses_to_itself(
    Poincare::OExpression expression) {
  constexpr int bufferSize = 500;
  char buffer[bufferSize];
  expression.serialize(buffer, bufferSize);
  assert_parsed_expression_is(buffer, expression);
}

void assert_expression_parses_and_serializes_to(const char *expression,
                                                const char *result) {
#if 0
  Shared::GlobalContext globalContext;
  OExpression e = parse_expression(expression, &globalContext);
  constexpr int bufferSize = 500;
  char buffer[bufferSize];
  e.serialize(buffer, bufferSize);
  const bool test = strcmp(buffer, result) == 0;
  char information[bufferSize] = "";
  if (!test) {
    build_failure_infos(information, bufferSize, expression, buffer, result);
  }
  quiz_assert_print_if_failure(test, information);
#endif
}

void assert_expression_parses_and_serializes_to_itself(const char *expression) {
  return assert_expression_parses_and_serializes_to(expression, expression);
}

void assert_layout_serializes_to(Tree *layout, const char *serialization) {
#if 0
  constexpr int bufferSize = 255;
  char buffer[bufferSize];
  layout.serializeForParsing(buffer, bufferSize);
  quiz_assert_print_if_failure(strcmp(serialization, buffer) == 0,
                               serialization);
#endif
}

void assert_expression_layouts_as(Tree *expression, Tree *layout) {
  Tree *l = Internal::Layouter::LayoutExpression(expression);
  quiz_assert(l->treeIsIdenticalTo(layout));
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
