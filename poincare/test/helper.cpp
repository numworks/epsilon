#include "helper.h"

#include <apps/shared/global_context.h>
#include <poincare/expression.h>
#include <poincare/old/store.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/layout/parsing/rack_parser.h>
#include <poincare/src/layout/rack_from_text.h>
#include <poincare/src/memory/exception_checkpoint.h>

Tree* parse(const char* input) {
  Tree* inputLayout = RackFromText(input);
  bool success = RackParser(inputLayout, nullptr).parse() != nullptr;
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
    parsed = KUndef->clone();
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
    ExceptionCheckpoint::Raise(ExceptionType::Other);
#endif
  }
  quiz_assert(test);
}

void store(const char* storeExpression, Poincare::Context* ctx) {
  Poincare::Expression s = Poincare::Expression::Parse(storeExpression, ctx);
  static_cast<const Poincare::Store&>(s).storeValueForSymbol(ctx);
}
