#include "helper.h"
#include <apps/shared/global_context.h>
#include <poincare/src/parsing/parser.h>
#include <poincare/print.h>

using namespace Poincare;

const char * MaxIntegerString() {
  // (2^32)^k_maxNumberOfDigits-1
  static const char * s = "179769313486231590772930519078902473361797697894230657273430081157732675805500963132708477322407536021120113879871393357658789768814416622492847430639474124377767893424865485276302219601246094119453082952085005768838150682342462881473913110540827237163350510684586298239947245938479716304835356329624224137215";
  return s;
}

const char * OverflowedIntegerString() {
  // (2^32)^k_maxNumberOfDigits
  static const char * s = "179769313486231590772930519078902473361797697894230657273430081157732675805500963132708477322407536021120113879871393357658789768814416622492847430639474124377767893424865485276302219601246094119453082952085005768838150682342462881473913110540827237163350510684586298239947245938479716304835356329624224137216";
  return s;
}

const char * BigOverflowedIntegerString() {
  // OverflowedIntegerString() with a 2 on first digit
  static const char * s = "279769313486231590772930519078902473361797697894230657273430081157732675805500963132708477322407536021120113879871393357658789768814416622492847430639474124377767893424865485276302219601246094119453082952085005768838150682342462881473913110540827237163350510684586298239947245938479716304835356329624224137216";
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

void build_failure_infos(char * returnedInformationsBuffer, size_t bufferSize, const char * expression, const char * result, const char * expectedResult) {
  Poincare::Print::SafeCustomPrintf(returnedInformationsBuffer, bufferSize," %s\n processed to\n %s\n instead of\n %s", expression, result, expectedResult);
}

void assert_parsed_expression_process_to(const char * expression, const char * result, ReductionTarget target, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, SymbolicComputation symbolicComputation, UnitConversion unitConversion, ProcessExpression process, int numberOfSignifiantDigits) {
  Shared::GlobalContext globalContext;
  Expression e = parse_expression(expression, &globalContext, false);
  Expression m = process(e, ReductionContext(&globalContext, complexFormat, angleUnit, unitFormat, target, symbolicComputation, unitConversion));
  constexpr int bufferSize = 500;
  char buffer[bufferSize];
  m.serialize(buffer, bufferSize, DecimalMode, numberOfSignifiantDigits);
  const bool test = strcmp(buffer, result) == 0;
  char information[bufferSize] = "";
  if (!test) {
    build_failure_infos(information, bufferSize, expression, buffer, result);
  }
  quiz_assert_print_if_failure(test, information);
}

Poincare::Expression parse_expression(const char * expression, Context * context, bool addParentheses, bool parseForAssignment) {
  Expression result = Expression::Parse(expression, context, addParentheses, parseForAssignment);
  quiz_assert_print_if_failure(!result.isUninitialized(), expression);
  return result;
}


void assert_reduce_and_store(const char * expression, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, Preferences::ComplexFormat complexFormat, ReductionTarget target) {
  Shared::GlobalContext globalContext;
  Expression e = parse_expression(expression, &globalContext, false);
  assert_expression_reduce(e, angleUnit, unitFormat, complexFormat, target, expression);
  assert(e.type() == ExpressionNode::Type::Store);
  static_cast<Store&>(e).storeValueForSymbol(&globalContext);
}

void assert_expression_reduce(Expression e, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, Preferences::ComplexFormat complexFormat, ReductionTarget target, const char * printIfFailure) {
  Shared::GlobalContext globalContext;
  ReductionContext context = ReductionContext(&globalContext, complexFormat, angleUnit, unitFormat, target);
  bool reductionFailure = false;
  e = e.cloneAndDeepReduceWithSystemCheckpoint(&context, &reductionFailure);
  quiz_assert_print_if_failure(!reductionFailure, printIfFailure);
}

void assert_parsed_expression_simplify_to(const char * expression, const char * simplifiedExpression, ReductionTarget target, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, Preferences::ComplexFormat complexFormat, SymbolicComputation symbolicComputation, UnitConversion unitConversion) {
  assert_parsed_expression_process_to(expression, simplifiedExpression, target, complexFormat, angleUnit, unitFormat, symbolicComputation, unitConversion, [](Expression e, ReductionContext reductionContext) {
      Expression simplifiedExpression;
      if (reductionContext.target() == ReductionTarget::User) {
        e.cloneAndSimplifyAndApproximate(&simplifiedExpression, nullptr, reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit(), reductionContext.unitFormat(), reductionContext.symbolicComputation(), reductionContext.unitConversion());
      } else {
        simplifiedExpression = e.cloneAndSimplify(reductionContext);
      }
      return simplifiedExpression;
    });
}

template<typename T>
void assert_expression_approximates_to(const char * expression, const char * approximation, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, Preferences::ComplexFormat complexFormat, int numberOfSignificantDigits) {
  int numberOfDigits = PrintFloat::SignificantDecimalDigits<T>();
  numberOfDigits = numberOfSignificantDigits > 0 ? numberOfSignificantDigits : numberOfDigits;
  assert_parsed_expression_process_to(expression, approximation, SystemForApproximation, complexFormat, angleUnit, unitFormat, ReplaceAllSymbolsWithDefinitionsOrUndefined, DefaultUnitConversion, [](Expression e, ReductionContext reductionContext) {
      return e.approximate<T>(reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit());
    }, numberOfDigits);
}

void assert_expression_simplifies_and_approximates_to(const char * expression, const char * approximation, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, Preferences::ComplexFormat complexFormat, int numberOfSignificantDigits) {
  int numberOfDigits = numberOfSignificantDigits > 0 ? numberOfSignificantDigits : PrintFloat::k_numberOfStoredSignificantDigits;
  assert_parsed_expression_process_to(expression, approximation, SystemForApproximation, complexFormat, angleUnit, unitFormat, ReplaceAllSymbolsWithDefinitionsOrUndefined, DefaultUnitConversion, [](Expression e, ReductionContext reductionContext) {
      Expression reduced;
      Expression approximated;
      e.cloneAndSimplifyAndApproximate(&reduced, &approximated, reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit(), reductionContext.unitFormat(), reductionContext.symbolicComputation());
      return approximated;
    }, numberOfDigits);
}

template<typename T>
void assert_expression_simplifies_approximates_to(const char * expression, const char * approximation, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, Preferences::ComplexFormat complexFormat, int numberOfSignificantDigits) {
  int numberOfDigits = PrintFloat::SignificantDecimalDigits<T>();
  numberOfDigits = numberOfSignificantDigits > 0 ? numberOfSignificantDigits : numberOfDigits;
  assert_parsed_expression_process_to(expression, approximation, SystemForApproximation, complexFormat, angleUnit, unitFormat, ReplaceAllSymbolsWithDefinitionsOrUndefined, DefaultUnitConversion, [](Expression e, ReductionContext reductionContext) {
      e = e.cloneAndSimplify(reductionContext);
      return e.approximate<T>(reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit());
    }, numberOfDigits);
}

void assert_expression_serialize_to(Poincare::Expression expression, const char * serialization, Preferences::PrintFloatMode mode, int numberOfSignificantDigits) {
  constexpr int bufferSize = 500;
  char buffer[bufferSize];
  expression.serialize(buffer, bufferSize, mode, numberOfSignificantDigits);
  bool test = strcmp(serialization, buffer) == 0;
  char information[bufferSize] = "";
  if (!test) {
      build_failure_infos(information, bufferSize, "serialized expression", buffer, serialization);
  }
  quiz_assert_print_if_failure(test, information);
}

void assert_layout_serialize_to(Poincare::Layout layout, const char * serialization) {
  constexpr int bufferSize = 255;
  char buffer[bufferSize];
  layout.serializeForParsing(buffer, bufferSize);
  quiz_assert_print_if_failure(strcmp(serialization, buffer) == 0, serialization);
}

void assert_expression_layouts_as(Poincare::Expression expression, Poincare::Layout layout) {
  Layout l = expression.createLayout(DecimalMode, PrintFloat::k_numberOfStoredSignificantDigits, nullptr);
  quiz_assert(l.isIdenticalTo(layout));
}

template void assert_expression_approximates_to<float>(char const*, char const *, Poincare::Preferences::AngleUnit, Poincare::Preferences::UnitFormat, Poincare::Preferences::ComplexFormat, int);
template void assert_expression_approximates_to<double>(char const*, char const *,  Poincare::Preferences::AngleUnit, Poincare::Preferences::UnitFormat, Poincare::Preferences::ComplexFormat, int);
template void assert_expression_simplifies_approximates_to<float>(char const*, char const *, Poincare::Preferences::AngleUnit, Poincare::Preferences::UnitFormat, Poincare::Preferences::ComplexFormat, int);
template void assert_expression_simplifies_approximates_to<double>(char const*, char const *,  Poincare::Preferences::AngleUnit, Poincare::Preferences::UnitFormat, Poincare::Preferences::ComplexFormat, int);
