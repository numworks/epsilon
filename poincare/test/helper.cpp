#include "helper.h"

#include <apps/shared/global_context.h>
#include <poincare/print.h>
#include <poincare/src/parsing/parser.h>
#include <poincare_junior/include/expression.h>
#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/expression/constant.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/layout/layoutter.h>
#include <poincare_junior/src/memory/exception_checkpoint.h>
#include <poincare_junior/test/helper.h>

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

void quiz_assert_print_if_failure(bool test, const char *information) {
  if (!test) {
    quiz_print("TEST FAILURE WHILE TESTING:");
    quiz_print(information);
  }
  quiz_assert(test);
}

void quiz_assert_log_if_failure(bool test, TreeHandle tree) {
#if POINCARE_TREE_LOG
  if (!test) {
    quiz_print("TEST FAILURE WHILE TESTING:");
    tree.log();
  }
#endif
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
  constexpr int bufferSize = 500;
  char buffer[bufferSize];
  char result[bufferSize];
  copy_without_system_chars(result, oldResult);
  bool bad = false;
  bool crash = false;
  ExceptionTry {
    assert(SharedEditionPool->numberOfTrees() == 0);
    assert(CachePool::SharedCachePool->numberOfTrees() == 0);
    Tree *e = parse_expression(expression, &globalContext, false);
    Tree *m = process(e, ReductionContext(&globalContext, complexFormat,
                                          angleUnit, unitFormat, target,
                                          symbolicComputation, unitConversion));
    Tree *l = PoincareJ::Layoutter::LayoutExpression(m, true,
                                                     numberOfSignificantDigits);
    *PoincareJ::Layout::Serialize(l, buffer, buffer + bufferSize) = 0;
    l->removeTree();
    bad = strcmp(buffer, result) != 0;
  }
  ExceptionCatch(type) {
    CachePool::SharedCachePool->reset();
    crash = true;
  }
  assert(SharedEditionPool->numberOfTrees() == 0);
  assert(CachePool::SharedCachePool->numberOfTrees() == 0);
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

PoincareJ::Tree *parse_expression(const char *expression, Context *context,
                                  bool addParentheses,
                                  bool parseForAssignment) {
  Tree *result = parse(expression);
  quiz_assert_print_if_failure(result != nullptr, expression);
  return result;
}

void assert_parsed_expression_is(
    const char *expression, Poincare::Expression r, bool addParentheses,
    bool parseForAssignment,
    Preferences::MixedFractions mixedFractionsParameter) {
#if 0
  Shared::GlobalContext context;
  Preferences::SharedPreferences()->enableMixedFractions(
      mixedFractionsParameter);
  Expression e = parse_expression(expression, &context, addParentheses,
                                  parseForAssignment);
  quiz_assert_print_if_failure(e.isIdenticalTo(r), expression);
#endif
}

void assert_parse_to_same_expression(const char *expression1,
                                     const char *expression2) {
#if 0
  Shared::GlobalContext context;
  Expression e1 = parse_expression(expression1, &context, false);
  Expression e2 = parse_expression(expression2, &context, false);
  quiz_assert(e1.isIdenticalTo(e2));
#endif
}

void assert_reduce_and_store(const char *expression,
                             Preferences::AngleUnit angleUnit,
                             Preferences::UnitFormat unitFormat,
                             Preferences::ComplexFormat complexFormat,
                             ReductionTarget target) {
#if 0
  Shared::GlobalContext globalContext;
  Tree *e = parse_expression(expression, &globalContext, false);
  assert_expression_reduce(e, angleUnit, unitFormat, complexFormat, target,
  expression);
  assert(e.type() == ExpressionNode::Type::Store);
  static_cast<Store &>(e).storeValueForSymbol(&globalContext);
#endif
}

void assert_expression_reduce(Tree *e, Preferences::AngleUnit angleUnit,
                              Preferences::UnitFormat unitFormat,
                              Preferences::ComplexFormat complexFormat,
                              ReductionTarget target,
                              const char *printIfFailure) {
  Shared::GlobalContext globalContext;
  ReductionContext context = ReductionContext(&globalContext, complexFormat,
                                              angleUnit, unitFormat, target);
  bool reductionFailure = false;
  PoincareJ::Simplification::Simplify(e);
  quiz_assert_print_if_failure(!reductionFailure, printIfFailure);
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
        PoincareJ::ProjectionContext context = {
            .m_complexFormat =
                PoincareJ::ComplexFormat(reductionContext.complexFormat()),
            .m_angleUnit = PoincareJ::AngleUnit(reductionContext.angleUnit()),
            .m_unitFormat =
                PoincareJ::UnitFormat(reductionContext.unitFormat())};
        PoincareJ::Simplification::Simplify(e, context);
        // TODO PCJ also approximate to see if it crashes
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
        EditionReference result = PoincareJ::Approximation::RootTreeToTree<T>(
            e, PoincareJ::AngleUnit(reductionContext.angleUnit()),
            PoincareJ::ComplexFormat(reductionContext.complexFormat()));
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
    Poincare::Expression expression) {
  constexpr int bufferSize = 500;
  char buffer[bufferSize];
  expression.serialize(buffer, bufferSize);
  assert_parsed_expression_is(buffer, expression);
}

void assert_expression_parses_and_serializes_to(const char *expression,
                                                const char *result) {
#if 0
  Shared::GlobalContext globalContext;
  Expression e = parse_expression(expression, &globalContext, false);
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
  Tree *l = PoincareJ::Layoutter::LayoutExpression(expression);
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
