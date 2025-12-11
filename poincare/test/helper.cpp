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
#include <poincare/symbol_store_interface.h>
#include <poincare/user_expression.h>

using namespace Poincare::Internal;

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
  if (sPrintToleratedTests) {
    quiz_print(buffer);
  }
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

void simplify(Tree* e, const Poincare::ProjectionContext& ctx, bool beautify) {
  if (!Simplification::Simplify(e, ctx, beautify)) {
    // Return undef to spot tests overflowing the TreeStack
    e->cloneTreeOverTree(KFailedSimplification);
  }
}

void process_tree_and_compare(const char* input, const char* output,
                              ProcessTree process,
                              Poincare::ProjectionContext projectionContext,
                              int nbOfSignificantDigits, bool preserveInput) {
  Tree* expression = parse(input, projectionContext.m_context,
                           {.preserveInput = preserveInput});
  Tree* expected = parse(output, projectionContext.m_context,
                         {.preserveInput = preserveInput});
  process_tree_and_compare(expression, expected, process, projectionContext,
                           nbOfSignificantDigits, input, output);
  expected->removeTree();
  expression->removeTree();
}

void process_tree_and_compare(const Tree* expression, const Tree* expected,
                              ProcessTree process,
                              Poincare::ProjectionContext projectionContext,
                              int nbOfSignificantDigits, const char* input,
                              const char* output) {
  /* If expected output has been given has serialized tree, we can't expect a
   * perfect tree match through parsing and serialization. */
  /* TODO: use (output != nullptr) instead, but it would require uniform float
   * numbers approximation across all platforms for tests to pass this way. */
  bool fallbackOnSerialization = true;
  int previousNumberOfTrees = SharedTreeStack->numberOfTrees();
  if (!expression || !expected) {
    // Parsing failed
    quiz_assert(false);
    return;
  }
  Tree* expressionClone = expression->cloneTree();
  process(expressionClone, projectionContext);
  if (!expressionClone) {
    // process failed
    quiz_assert(false);
    return;
  }
  bool ok = expressionClone->treeIsIdenticalTo(expected);
  if (!ok) {
    constexpr size_t bufferSize = 1024;
    char processedBuffer[bufferSize];
    char outputBuffer[bufferSize];
    if (!output) {
      serialize_expression(expected, outputBuffer, nbOfSignificantDigits);
      output = outputBuffer;
    }
    serialize_expression(expressionClone, processedBuffer,
                         nbOfSignificantDigits);
    if (fallbackOnSerialization) {
      ok = strcmp(output, processedBuffer) == 0;
    }
#ifndef PLATFORM_DEVICE
    if (!ok) {
      float expectedMetric =
          Metric::GetMetric(expected, projectionContext.m_reductionTarget);
      float expressionMetric = Metric::GetMetric(
          expressionClone, projectionContext.m_reductionTarget);
      const char* metricText =
          expectedMetric > expressionMetric
              ? "better"
              : (expectedMetric == expressionMetric ? "same" : "worse");
      if (input) {
        std::cout << input;
      } else {
#if POINCARE_TREE_LOG
        expression->logSerializeWithoutLineReturn();
#else
        char inputBuffer[bufferSize];
        serialize_expression(expression, inputBuffer, nbOfSignificantDigits);
        std::cout << inputBuffer;
#endif
      }
      std::cout << " processed to " << processedBuffer << " instead of "
                << output << " (" << metricText << " " << expressionMetric
                << " vs " << expectedMetric << ")" << std::endl;
    }
#endif
    quiz_assert(ok);
  }
  expressionClone->removeTree();
  assert(SharedTreeStack->numberOfTrees() == previousNumberOfTrees);
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
  Poincare::ProjectionContext ctx = {};
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
  Tree* layout = Layouter::LayoutExpression(
      expression, {.layouterMode = LayouterMode::Linear});
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
  Tree* layout = Layouter::LayoutExpression(
      expression, {.layouterMode = LayouterMode::Linear,
                   .numberOfSignificantDigits = numberOfSignificantDigits});
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
  Tree* layout = Layouter::LayoutExpression(
      expression, {.layouterMode = LayouterMode::Linear,
                   .numberOfSignificantDigits = numberOfSignificantDigits,
                   .floatMode = mode,
                   .base = base});
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
  Tree* l = Layouter::LayoutExpression(
      e, {.symbolContext = symbolContext,
          .layouterMode = LayouterMode::Linear,
          .numberOfSignificantDigits = numberOfSignificantDigits,
          .floatMode = mode,
          .base = base});
  constexpr int bufferSize = 500;
  char buffer[bufferSize];
  LayoutSerializer::Serialize(l, buffer);
  l->removeTree();
  e->removeTree();
  const bool test = strcmp(buffer, result) == 0;
  quiz_assert_print_if_failure(test, expression, result, buffer);
}

void assert_expression_parses_and_serializes_to_itself(
    const char* expression, const Poincare::SymbolContext& symbolContext) {
  return assert_expression_parses_and_serializes_to(expression, expression,
                                                    symbolContext);
}
