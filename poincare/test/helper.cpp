#include "helper.h"
#include <apps/shared/global_context.h>
#include <poincare/src/parsing/parser.h>

using namespace Poincare;

const char * MaxIntegerString() {
  static const char * s = "179769313486231590772930519078902473361797697894230657273430081157732675805500963132708477322407536021120113879871393357658789768814416622492847430639474124377767893424865485276302219601246094119453082952085005768838150682342462881473913110540827237163350510684586298239947245938479716304835356329624224137215"; // (2^32)^k_maxNumberOfDigits-1
  return s;
}

const char * OverflowedIntegerString() {
  static const char * s = "179769313486231590772930519078902473361797697894230657273430081157732675805500963132708477322407536021120113879871393357658789768814416622492847430639474124377767893424865485276302219601246094119453082952085005768838150682342462881473913110540827237163350510684586298239947245938479716304835356329624224137216"; // (2^32)^k_maxNumberOfDigits
  return s;
}

const char * BigOverflowedIntegerString() {
  static const char * s = "279769313486231590772930519078902473361797697894230657273430081157732675805500963132708477322407536021120113879871393357658789768814416622492847430639474124377767893424865485276302219601246094119453082952085005768838150682342462881473913110540827237163350510684586298239947245938479716304835356329624224137216"; // OverflowedIntegerString() with a 2 on first digit
  return s;
}

void quiz_assert_print_if_failure(bool test, const char * information) {
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

void assert_parsed_expression_process_to(const char * expression, const char * result, ExpressionNode::ReductionTarget target, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::SymbolicComputation symbolicComputation, ProcessExpression process, int numberOfSignifiantDigits) {
  Shared::GlobalContext globalContext;
  Expression e = parse_expression(expression, &globalContext, false);
  Expression m = process(e, ExpressionNode::ReductionContext(&globalContext, complexFormat, angleUnit, target, symbolicComputation));
  constexpr int bufferSize = 500;
  char buffer[bufferSize];
  m.serialize(buffer, bufferSize, DecimalMode, numberOfSignifiantDigits);
  const bool test = strcmp(buffer, result) == 0;
  char information[bufferSize] = "";
  if (!test) {
    char * position = information;
    size_t remainingLength = bufferSize;
    static constexpr size_t numberOfPieces = 6;
    const char * piecesOfInformation[numberOfPieces] = {
      "  ",
      expression,
      "\n  processed to\n  ",
      buffer,
      "\n  instead of\n  ",
      result,
    };
    for (size_t piece = 0; piece < numberOfPieces; piece++) {
      const size_t length = strlcpy(position, piecesOfInformation[piece], remainingLength);
      if (length > remainingLength) {
        break;
      }
      remainingLength -= length;
      position += length;
    }
  }
  quiz_assert_print_if_failure(test, information);
}

Poincare::Expression parse_expression(const char * expression, Context * context, bool addParentheses) {
  Expression result = Expression::Parse(expression, context, addParentheses);
  quiz_assert(!result.isUninitialized());
  return result;
}

void assert_simplify(const char * expression, Preferences::AngleUnit angleUnit, Preferences::ComplexFormat complexFormat, ExpressionNode::ReductionTarget target) {
  Shared::GlobalContext globalContext;
  Expression e = parse_expression(expression, &globalContext, false);
  quiz_assert_print_if_failure(!e.isUninitialized(), expression);
  e = e.simplify(ExpressionNode::ReductionContext(&globalContext, complexFormat, angleUnit, target));
  quiz_assert_print_if_failure(!(e.isUninitialized()), expression);
}

void assert_parsed_expression_simplify_to(const char * expression, const char * simplifiedExpression, ExpressionNode::ReductionTarget target, Preferences::AngleUnit angleUnit, Preferences::ComplexFormat complexFormat, ExpressionNode::SymbolicComputation symbolicComputation) {
  assert_parsed_expression_process_to(expression, simplifiedExpression, target, complexFormat, angleUnit, symbolicComputation, [](Expression e, ExpressionNode::ReductionContext reductionContext) {
      Expression copy = e.clone();
      if (reductionContext.target() == ExpressionNode::ReductionTarget::User) {
        copy.simplifyAndApproximate(&copy, nullptr, reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit(), reductionContext.symbolicComputation());
      } else {
        copy = copy.simplify(reductionContext);
      }
      if (copy.isUninitialized()) {
        return e;
      }
      return copy;
    });
}

template<typename T>
void assert_expression_approximates_to(const char * expression, const char * approximation, Preferences::AngleUnit angleUnit, Preferences::ComplexFormat complexFormat, int numberOfSignificantDigits) {
  int numberOfDigits = sizeof(T) == sizeof(double) ? PrintFloat::k_numberOfStoredSignificantDigits : PrintFloat::k_numberOfPrintedSignificantDigits;
  numberOfDigits = numberOfSignificantDigits > 0 ? numberOfSignificantDigits : numberOfDigits;
  assert_parsed_expression_process_to(expression, approximation, SystemForApproximation, complexFormat, angleUnit, ReplaceAllSymbolsWithDefinitionsOrUndefined, [](Expression e, ExpressionNode::ReductionContext reductionContext) {
      return e.approximate<T>(reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit());
    }, numberOfDigits);
}

void assert_expression_simplifies_and_approximates_to(const char * expression, const char * approximation, Preferences::AngleUnit angleUnit, Preferences::ComplexFormat complexFormat, int numberOfSignificantDigits) {
  int numberOfDigits = numberOfSignificantDigits > 0 ? numberOfSignificantDigits : PrintFloat::k_numberOfStoredSignificantDigits;
  assert_parsed_expression_process_to(expression, approximation, SystemForApproximation, complexFormat, angleUnit, ReplaceAllSymbolsWithDefinitionsOrUndefined, [](Expression e, ExpressionNode::ReductionContext reductionContext) {
      Expression reduced;
      Expression approximated;
      e.simplifyAndApproximate(&reduced, &approximated, reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit(), reductionContext.symbolicComputation());
      return approximated;
    }, numberOfDigits);
}

template<typename T>
void assert_expression_simplifies_approximates_to(const char * expression, const char * approximation, Preferences::AngleUnit angleUnit, Preferences::ComplexFormat complexFormat, int numberOfSignificantDigits) {
  int numberOfDigits = sizeof(T) == sizeof(double) ? PrintFloat::k_numberOfStoredSignificantDigits : PrintFloat::k_numberOfPrintedSignificantDigits;
  numberOfDigits = numberOfSignificantDigits > 0 ? numberOfSignificantDigits : numberOfDigits;
  assert_parsed_expression_process_to(expression, approximation, SystemForApproximation, complexFormat, angleUnit, ReplaceAllSymbolsWithDefinitionsOrUndefined, [](Expression e, ExpressionNode::ReductionContext reductionContext) {
      e = e.simplify(reductionContext);
      return e.approximate<T>(reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit());
    }, numberOfDigits);
}

void assert_layout_serialize_to(Poincare::Layout layout, const char * serialization) {
  constexpr int bufferSize = 255;
  char buffer[bufferSize];
  layout.serializeForParsing(buffer, bufferSize);
  quiz_assert_print_if_failure(strcmp(serialization, buffer) == 0, serialization);
}

void assert_expression_layouts_as(Poincare::Expression expression, Poincare::Layout layout) {
  Layout l = expression.createLayout(DecimalMode, PrintFloat::k_numberOfStoredSignificantDigits);
  quiz_assert(l.isIdenticalTo(layout));
}

template void assert_expression_approximates_to<float>(char const*, char const *, Poincare::Preferences::AngleUnit, Poincare::Preferences::ComplexFormat, int);
template void assert_expression_approximates_to<double>(char const*, char const *,  Poincare::Preferences::AngleUnit, Poincare::Preferences::ComplexFormat, int);
template void assert_expression_simplifies_approximates_to<float>(char const*, char const *, Poincare::Preferences::AngleUnit, Poincare::Preferences::ComplexFormat, int);
template void assert_expression_simplifies_approximates_to<double>(char const*, char const *,  Poincare::Preferences::AngleUnit, Poincare::Preferences::ComplexFormat, int);
