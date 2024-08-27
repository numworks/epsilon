#include "helper.h"

#include <apps/shared/global_context.h>
#include <poincare/expression.h>
#include <poincare/helpers/store.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/layout/layouter.h>
#include <poincare/src/layout/parsing/rack_parser.h>
#include <poincare/src/layout/rack_from_text.h>
#include <poincare/src/layout/serialize.h>
#include <poincare/src/memory/tree_stack_checkpoint.h>

Tree* parse(const char* input, Poincare::Context* context,
            bool parseForAssignment) {
  Tree* inputLayout = RackFromText(input);
  RackParser parser(inputLayout, context, -1,
                    parseForAssignment
                        ? ParsingContext::ParsingMethod::Assignment
                        : ParsingContext::ParsingMethod::Classic);
  bool success = parser.parse() != nullptr;
  // quiz_assert(expression);
  inputLayout->removeTree();
  return success ? inputLayout : nullptr;
}

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

const char * MaxParsedIntegerString() {
  static const char * s = "999999999999999999999999999999"; // 10^k_maxNumberOfParsedDigitsBase10 - 1
  return s;
}

const char * ApproximatedParsedIntegerString() {
  static const char * s = "1000000000000000000000000000000"; // 10^k_maxNumberOfParsedDigitsBase10
  return s;
}
#endif

Tree* TextToTree(const char* input, Poincare::Context* context) {
  Tree* expression = RackFromText(input);
  Tree* parsed = RackParser(expression, context).parse();
  if (!parsed) {
    parsed = KUndef->cloneTree();
  }
  expression->moveTreeOverTree(parsed);
  return expression;
}

void quiz_assert_print_if_failure(bool test, const char* information) {
  if (!test) {
#if 0  // TODO_PCJ
    quiz_print("TEST FAILURE WHILE TESTING:");
    quiz_print(information);
#else
    TreeStackCheckpoint::Raise(ExceptionType::Other);
#endif
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

void process_tree_and_compare(const char* input, const char* output,
                              ProcessTree process,
                              ProjectionContext projectionContext) {
  Tree* expected = TextToTree(output, projectionContext.m_context);
  Tree* expression = TextToTree(input, projectionContext.m_context);
  process(expression, projectionContext);
  quiz_assert(expression);
  quiz_assert(expected);
  bool ok = expression->treeIsIdenticalTo(expected);
  if (!ok) {
    Tree* outputLayout =
        Layouter::LayoutExpression(expression->cloneTree(), true);
    quiz_assert(outputLayout);
    constexpr size_t bufferSize = 256;
    char buffer[bufferSize];
    *Serialize(outputLayout, buffer, buffer + bufferSize) = 0;
    remove_system_codepoints(buffer);
    bool visuallyOk = strcmp(output, buffer) == 0;
    if (visuallyOk) {
      ok = true;
    } else {
#ifndef PLATFORM_DEVICE
      std::cout << input << " processed to " << buffer << " instead of "
                << output << std::endl;
#endif
    }
    quiz_assert(ok);
    outputLayout->removeTree();
  }
  expression->removeTree();
  expected->removeTree();
  assert(SharedTreeStack->numberOfTrees() == 0);
}

void store(const char* storeExpression, Poincare::Context* ctx) {
  Poincare::Expression s = Poincare::Expression::Parse(storeExpression, ctx);
  Poincare::StoreHelper::Store(ctx, s);
}
