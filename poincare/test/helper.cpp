#include "helper.h"

#include <poincare/helpers/store.h>
#include <poincare/print.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/metric.h>
#include <poincare/src/layout/layout_serializer.h>
#include <poincare/src/layout/layouter.h>
#include <poincare/src/layout/parser.h>
#include <poincare/src/layout/rack_from_text.h>
#include <poincare/src/memory/tree_stack_checkpoint.h>
#include <poincare/symbol_context.h>
#include <poincare/user_expression.h>
#include <poincare/variable_store.h>

using namespace Poincare::Internal;

const char* AlmostMaxIntegerString() {
  static const char* s =
      "179769313486231590772930519078902473361797697894230657273430081157732675"
      "805500963132708477322407536021120113879871393357658789768814416622492847"
      "430639474124377767893424865485276302219601246094119453082952085005768838"
      "150682342462881473913110540827237163350510684586298239947245938479716304"
      "835356329624224137214";  // (2^8)^k_maxNumberOfDigits-2
  return s;
}

#if 0
const char * MaxIntegerString() {
  static const char * s = "179769313486231590772930519078902473361797697894230657273430081157732675805500963132708477322407536021120113879871393357658789768814416622492847430639474124377767893424865485276302219601246094119453082952085005768838150682342462881473913110540827237163350510684586298239947245938479716304835356329624224137215"; // (2^8)^k_maxNumberOfDigits-1
  return s;
}

const char * OverflowedIntegerString() {
  static const char * s = "179769313486231590772930519078902473361797697894230657273430081157732675805500963132708477322407536021120113879871393357658789768814416622492847430639474124377767893424865485276302219601246094119453082952085005768838150682342462881473913110540827237163350510684586298239947245938479716304835356329624224137216"; // (2^8)^k_maxNumberOfDigits
  return s;
}

const char * BigOverflowedIntegerString() {
  static const char * s = "279769313486231590772930519078902473361797697894230657273430081157732675805500963132708477322407536021120113879871393357658789768814416622492847430639474124377767893424865485276302219601246094119453082952085005768838150682342462881473913110540827237163350510684586298239947245938479716304835356329624224137216"; // OverflowedIntegerString() with a 2 on first digit
  return s;
}

const char * ApproximatedParsedIntegerString() {
  static const char * s = "1000000000000000000000000000000"; // 10^k_maxNumberOfParsedDigitsBase10
  return s;
}
#endif

/* TODO_PCJ: Use quiz_tolerate_print_if_failure instead of quiz_assert in tests
 * commented as TODO_PCJ */
void quiz_tolerate_print_if_failure(bool test, const char* input,
                                    const char* expected,
                                    const char* observed) {
  constexpr int bufferSize = 500;
  char buffer[bufferSize];
  int i = Poincare::Print::UnsafeCustomPrintf(
      buffer, bufferSize, "%s\t%s\t%s", test ? "OK" : "BAD", input, expected);
  if (!test) {
    Poincare::Print::UnsafeCustomPrintf(buffer + i, bufferSize - i, "\t%s",
                                        observed);
  }
  quiz_print(buffer);
}

void quiz_assert_print_if_failure(bool test, const char* information,
                                  const char* expected, const char* observed) {
  if (!test) {
    quiz_print("TEST FAILURE WHILE TESTING:");
    quiz_print(information);
    if (expected && observed) {
      constexpr int bufferSize = 500;
      char buffer[bufferSize];
      Poincare::Print::UnsafeCustomPrintf(
          buffer, bufferSize, "Expected: %s\nObserved: %s", expected, observed);
      quiz_print(buffer);
    }
  }
  quiz_assert(test);
}

void remove_system_codepoints(char* buffer) {
  // TODO serialization should not add system codepoints instead
  const char* source = buffer;
  char* dest = buffer;
  while (*source) {
    if (*source == '\x11') {
      source++;
      continue;
    }
    *dest++ = *source++;
  }
  *dest = 0;
}

void simplify(Tree* e, const ProjectionContext& ctx, bool beautify) {
  if (!Simplification::Simplify(e, ctx, beautify)) {
    // Return undef to spot tests overflowing the TreeStack
    e->cloneNodeOverTree(KFailedSimplification);
  }
}

void process_tree_and_compare(const char* input, const char* output,
                              ProcessTree process,
                              ProjectionContext projectionContext,
                              int nbOfSignificantDigits, bool preserveInput) {
  Tree* expected = parse(output, projectionContext.m_context,
                         {.preserveInput = preserveInput});
  Tree* expression = parse(input, projectionContext.m_context,
                           {.preserveInput = preserveInput});
  ;
  if (!expression || !expected) {
    // Parsing failed
    quiz_assert(false);
    if (expression) {
      expression->removeTree();
    } else if (expected) {
      expected->removeTree();
    }
    return;
  }
  process(expression, projectionContext);
  if (!expression) {
    // process failed
    quiz_assert(false);
    expected->removeTree();
    return;
  }
  bool ok = expression->treeIsIdenticalTo(expected);
  if (!ok) {
    constexpr size_t bufferSize = 1024;
    char buffer[bufferSize];
    serialize_expression(expression, buffer, nbOfSignificantDigits);
    bool visuallyOk = strcmp(output, buffer) == 0;
    if (visuallyOk) {
      ok = true;
    }
#ifndef PLATFORM_DEVICE
    else {
      float expectedMetric =
          Metric::GetMetric(expected, projectionContext.m_reductionTarget);
      float expressionMetric =
          Metric::GetMetric(expression, projectionContext.m_reductionTarget);
      const char* metricText =
          expectedMetric > expressionMetric
              ? " (better "
              : (expectedMetric == expressionMetric ? " (same " : " (worse ");
      std::cout << input << " processed to " << buffer << " instead of "
                << output << metricText << expressionMetric << " vs "
                << expectedMetric << ")" << std::endl;
    }
#endif
    quiz_assert(ok);
  }
  expression->removeTree();
  expected->removeTree();
  assert(SharedTreeStack->numberOfTrees() == 0);
}

Tree* private_parse(const char* input,
                    const Poincare::SymbolContext& symbolContext,
                    ParsingParameters params = {},
                    bool assertNotParsable = false) {
  Tree* layout = RackFromText(input);
  Tree* expression = Parser::Parse(layout, symbolContext, params);
  if (assertNotParsable || !expression) {
    quiz_assert(assertNotParsable == !expression);
    layout->removeTree();
    return nullptr;
  }
  layout->moveTreeOverTree(expression);
  return layout;
}

Tree* parse(const char* input, const Poincare::SymbolContext& symbolContext,
            ParsingParameters params) {
  return private_parse(input, symbolContext, params);
}

Tree* parse_and_reduce(const char* input, bool beautify) {
  Tree* e = parse(input);
  assert(e);
  ProjectionContext ctx = {};
  simplify(e, ctx, beautify);
  return e;
}

void assert_parsed_expression_is(const char* expression,
                                 const Poincare::Internal::Tree* expected,
                                 ParsingParameters params) {
  assert_parsed_expression_is(expression, expected,
                              Poincare::EmptySymbolContext{}, params);
}

void assert_parsed_expression_is(const char* expression,
                                 const Poincare::Internal::Tree* expected,
                                 const Poincare::SymbolContext& symbolContext,
                                 ParsingParameters params) {
  Tree* parsed = parse(expression, symbolContext, params);
  bool test = parsed && parsed->treeIsIdenticalTo(expected);
  quiz_assert_print_if_failure(test, expression, "parsed and identical",
                               parsed ? "not identical" : "not parsed");
  if (parsed) {
    parsed->removeTree();
  }
}

void assert_parse_to_same_expression(
    const char* expression1, const char* expression2,
    const Poincare::SymbolContext& symbolContext) {
  Tree* e1 = parse(expression1, symbolContext);
  Tree* e2 = parse(expression2, symbolContext);
  quiz_assert(e1);
  quiz_assert(e2);
  quiz_assert(e1->treeIsIdenticalTo(e2));
}

void assert_expression_serializes_and_parses_to(
    const Poincare::Internal::Tree* expression,
    const Poincare::Internal::Tree* result) {
  constexpr int bufferSize = 500;
  char buffer[bufferSize];
  Tree* layout = Layouter::LayoutExpression(expression->cloneTree(), true);
  LayoutSerializer::Serialize(layout, buffer);
  layout->removeTree();
  assert_parsed_expression_is(buffer, result);
}

void assert_text_not_parsable(const char* input,
                              const Poincare::SymbolContext& symbolContext,
                              ParsingParameters params) {
  bool parsed = private_parse(input, symbolContext, params, true);
  quiz_assert(!parsed);
}

void assert_parse_to_integer_overflow(
    const char* input, const Poincare::SymbolContext& symbolContext) {
  ExceptionTry {
    private_parse(input, symbolContext);
    quiz_assert(false);
  }
  ExceptionCatch(type) { quiz_assert(type == ExceptionType::IntegerOverflow); }
  SharedTreeStack->flush();
}

void serialize_expression(const Tree* expression, std::span<char> buffer,
                          int numberOfSignificantDigits) {
  Tree* layout = Layouter::LayoutExpression(expression->cloneTree(), true,
                                            false, numberOfSignificantDigits);
  quiz_assert(layout);
  LayoutSerializer::Serialize(layout, buffer);
  remove_system_codepoints(buffer.data());
  layout->removeTree();
}

void assert_expression_serializes_to(const Tree* expression,
                                     const char* serialization,
                                     Preferences::PrintFloatMode mode,
                                     int numberOfSignificantDigits,
                                     OMG::Base base) {
  constexpr int bufferSize = 500;
  char buffer[bufferSize];
  Tree* layout =
      Layouter::LayoutExpression(expression->cloneTree(), true, false,
                                 numberOfSignificantDigits, mode, base);
  LayoutSerializer::Serialize(layout, buffer);
  bool test = strcmp(serialization, buffer) == 0;
  layout->removeTree();
  quiz_assert_print_if_failure(test, serialization, serialization, buffer);
}

void assert_layout_serializes_to(const Tree* layout,
                                 const char* serialization) {
  assert(layout);
  assert(serialization);
  constexpr int bufferSize = 255;
  char buffer[bufferSize];
  LayoutSerializer::Serialize(layout, buffer);
  remove_system_codepoints(buffer);
  bool success = strcmp(buffer, serialization) == 0;
  quiz_assert_print_if_failure(success, serialization, serialization, buffer);
}

void assert_expression_parses_and_serializes_to(
    const char* expression, const char* result,
    const Poincare::SymbolContext& symbolContext,
    Preferences::PrintFloatMode mode, int numberOfSignificantDigits,
    OMG::Base base) {
  Tree* e = parse(expression, symbolContext);
  Tree* l = Layouter::LayoutExpression(e, true, false,
                                       numberOfSignificantDigits, mode, base);
  constexpr int bufferSize = 500;
  char buffer[bufferSize];
  LayoutSerializer::Serialize(l, buffer);
  l->removeTree();
  const bool test = strcmp(buffer, result) == 0;
  quiz_assert_print_if_failure(test, expression, result, buffer);
}

void assert_expression_parses_and_serializes_to_itself(
    const char* expression, const Poincare::SymbolContext& symbolContext) {
  return assert_expression_parses_and_serializes_to(expression, expression,
                                                    symbolContext);
}
