#include "helper.h"

#include <poincare/print.h>
#include <poincare/src/expression/beautification.h>
#include <poincare/src/expression/builtin.h>
#include <poincare/src/expression/float_helper.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/simplification.h>
#include <poincare/src/layout/layout_serializer.h>
#include <poincare/src/layout/layouter.h>
#include <poincare/src/layout/parser.h>
#include <poincare/src/layout/rack_from_text.h>
#include <poincare/src/memory/tree_stack_checkpoint.h>
#include <poincare/symbol_store_interface.h>
#include <poincare/test/helper.h>
#include <poincare/test/helpers/symbol_store.h>

using namespace Poincare;
using namespace Poincare::Internal;

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
    const char *expression, const char *oldResult,
    const SymbolContext &symbolContext, ReductionTarget target,
    ComplexFormat complexFormat, AngleUnit angleUnit,
    Preferences::UnitFormat unitFormat, SymbolicComputation symbolicComputation,
    ProcessExpression process, int numberOfSignificantDigits) {
  constexpr int bufferSize = 2048;
  char buffer[bufferSize];
  char result[bufferSize];
  copy_without_system_chars(result, oldResult);
  assert(SharedTreeStack->numberOfTrees() == 0);
  Tree *inputLayout = RackFromText(expression);
  TreeRef e = Parser::Parse(inputLayout, symbolContext);
  inputLayout->removeTree();
  quiz_assert_print_if_failure(e != nullptr, expression);
  Poincare::ProjectionContext projCtx = {.m_complexFormat = complexFormat,
                                         .m_angleUnit = angleUnit,
                                         .m_reductionTarget = target,
                                         .m_unitFormat = unitFormat,
                                         .m_symbolic = symbolicComputation,
                                         .m_context = symbolContext};
  Tree *m = process(e, projCtx);
  Tree *l = Internal::Layouter::LayoutExpression(
      m, {.symbolContext = symbolContext,
          .layouterMode = LayouterMode::Linear,
          .numberOfSignificantDigits = numberOfSignificantDigits});
  Internal::LayoutSerializer::Serialize(l, buffer);
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

void assert_parsed_expression_simplify_to(
    const char *expression, const char *simplifiedExpression,
    ReductionTarget target, AngleUnit angleUnit,
    Preferences::UnitFormat unitFormat, ComplexFormat complexFormat,
    SymbolicComputation symbolicComputation, bool beautify) {
  PoincareTest::SymbolStore symbolStore;
  // TODO_PCJ also approximate to see if it crashes
  assert_parsed_expression_simplify_to(
      expression, simplifiedExpression, symbolStore, target, angleUnit,
      unitFormat, complexFormat, symbolicComputation, beautify);
}

void assert_parsed_expression_simplify_to(
    const char *expression, const char *simplifiedExpression,
    const SymbolContext &symbolContext, ReductionTarget target,
    AngleUnit angleUnit, Preferences::UnitFormat unitFormat,
    ComplexFormat complexFormat, SymbolicComputation symbolicComputation,
    bool beautify) {
  // TODO_PCJ also approximate to see if it crashes
  assert_parsed_expression_process_to(
      expression, simplifiedExpression, symbolContext, target, complexFormat,
      angleUnit, unitFormat, symbolicComputation,
      beautify ?
      [](Tree *e, Poincare::ProjectionContext &projCtx) {
        simplify(e, projCtx, true);
        return e;
      } :[](Tree *e, Poincare::ProjectionContext &projCtx) {
        simplify(e, projCtx, false);
        /* Beautify the expression with a different target, to allow layouting and output comparison. */
        ReductionTarget previousReductionTarget = projCtx.m_reductionTarget;
        projCtx.m_reductionTarget = User;
        Beautification::DeepBeautify(e, projCtx);
        projCtx.m_reductionTarget = previousReductionTarget;
        return e;
      });
}
