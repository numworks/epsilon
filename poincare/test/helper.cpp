#include <apps/shared/global_context.h>
#include <string.h>
#include <ion.h>
#include <stdlib.h>
#include <assert.h>
#include <cmath>
#include "helper.h"
#if POINCARE_TESTS_PRINT_EXPRESSIONS
#include "../src/expression_debug.h"
#include <iostream>
using namespace std;
#endif

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

void translate_in_special_chars(char * expression) {
  for (char *c = expression; *c; c++) {
    switch (*c) {
      case 'E': *c = Ion::Charset::Exponent; break;
      case 'X': *c = Ion::Charset::Exponential; break;
      case 'I': *c = Ion::Charset::IComplex; break;
      case 'R': *c = Ion::Charset::Root; break;
      case 'P': *c = Ion::Charset::SmallPi; break;
      case '*': *c = Ion::Charset::MultiplicationSign; break;
      case '>': *c = Ion::Charset::Sto; break;
      case '?': *c = Poincare::Symbol::SpecialSymbols::UnknownX; break;
      case '$': *c = Ion::Charset::LeftSuperscript; break;
      case '#': *c = Ion::Charset::RightSuperscript; break;
    }
  }
}

void translate_in_ASCII_chars(char * expression) {
  for (char *c = expression; *c; c++) {
    switch (*c) {
      case Ion::Charset::Exponent: *c = 'E'; break;
      case Ion::Charset::Exponential: *c = 'X'; break;
      case Ion::Charset::IComplex: *c = 'I'; break;
      case Ion::Charset::Root: *c = 'R'; break;
      case Ion::Charset::SmallPi: *c = 'P'; break;
      case Ion::Charset::MultiplicationSign: *c = '*'; break;
      case Ion::Charset::MiddleDot: *c = '*'; break;
      case Ion::Charset::Sto: *c = '>'; break;
      case Poincare::Symbol::SpecialSymbols::UnknownX: *c = '?'; break;
      case Ion::Charset::LeftSuperscript: *c = '$'; break;
      case Ion::Charset::RightSuperscript: *c = '#'; break;
    }
  }
}

Expression parse_expression(const char * expression, bool canBeUnparsable) {
  quiz_print(expression);
  char buffer[500];
  strlcpy(buffer, expression, sizeof(buffer));
  translate_in_special_chars(buffer);
  Expression result = Expression::Parse(buffer);
  if (!canBeUnparsable) {
    quiz_assert(!result.isUninitialized());
  }
  return result;
}

void assert_expression_not_parsable(const char * expression) {
  Expression e = parse_expression(expression, true);
  quiz_assert(e.isUninitialized());
}

void assert_parsed_expression_type(const char * expression, Poincare::ExpressionNode::Type type) {
  Expression e = parse_expression(expression);
  quiz_assert(e.type() == type);
}

void assert_parsed_expression_is(const char * expression, Poincare::Expression r) {
  Expression e = parse_expression(expression);
  bool identical = e.isIdenticalTo(r);
#if POINCARE_TREE_LOG
  if (!identical) {
    std::cout << "Expecting" << std::endl;
    r.log();
    std::cout << "Got" << std::endl;
    e.log();
  }
#endif
  quiz_assert(identical);
}

void assert_parsed_expression_polynomial_degree(const char * expression, int degree, const char * symbolName) {
  Shared::GlobalContext globalContext;
  Expression e = parse_expression(expression);
  Expression result = e.clone().simplify(globalContext, Radian);
  if (result.isUninitialized()) {
    result = e;
  }
  quiz_assert(result.polynomialDegree(globalContext, symbolName) == degree);
}

void assert_simplify(const char * expression) {
  Shared::GlobalContext globalContext;
  Expression e = parse_expression(expression);
  quiz_assert(!e.isUninitialized());
  e = e.simplify(globalContext, Radian);
  quiz_assert(!e.isUninitialized());
}

typedef Expression (*ProcessExpression)(Expression, Context & context, Preferences::AngleUnit angleUnit, Preferences::ComplexFormat complexFormat);

void assert_parsed_expression_process_to(const char * expression, const char * result, Preferences::AngleUnit angleUnit, Preferences::ComplexFormat complexFormat, ProcessExpression process, int numberOfSignifiantDigits = PrintFloat::k_numberOfStoredSignificantDigits) {
  Shared::GlobalContext globalContext;
  Expression e = parse_expression(expression);

#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << " Entry expression: " << expression << "----"  << endl;
  print_expression(e, 0);
#endif
  Expression m = process(e, globalContext, angleUnit, complexFormat);
  char buffer[500];
  m.serialize(buffer, sizeof(buffer), DecimalMode, numberOfSignifiantDigits);
  translate_in_ASCII_chars(buffer);
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << "---- serialize to: " << buffer << " ----"  << endl;
  cout << "----- compared to: " << result << " ----\n"  << endl;
#endif
  quiz_assert(strcmp(buffer, result) == 0);
}

template<typename T>
void assert_parsed_expression_evaluates_to(const char * expression, const char * approximation, Preferences::AngleUnit angleUnit, Preferences::ComplexFormat complexFormat, int numberOfSignificantDigits) {
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << "--------- Approximation ---------" << endl;
#endif
  int numberOfDigits = sizeof(T) == sizeof(double) ? PrintFloat::k_numberOfStoredSignificantDigits : PrintFloat::k_numberOfPrintedSignificantDigits;
  numberOfDigits = numberOfSignificantDigits > 0 ? numberOfSignificantDigits : numberOfDigits;
  assert_parsed_expression_process_to(expression, approximation, angleUnit, complexFormat, [](Expression e, Context & context, Preferences::AngleUnit angleUnit, Preferences::ComplexFormat complexFormat) {
        Expression result = e.clone().simplify(context, angleUnit);
        if (result.isUninitialized()) {
          result = e;
        }
        return result.approximate<T>(context, angleUnit, complexFormat);
      }, numberOfDigits);
}

template<typename T>
void assert_parsed_expression_approximates_with_value_for_symbol(Expression expression, const char * symbol, T value, T approximation, Poincare::Preferences::AngleUnit angleUnit) {
  Shared::GlobalContext globalContext;
  T result = expression.approximateWithValueForSymbol(symbol, value, globalContext, angleUnit);
  quiz_assert(std::fabs(result - approximation) < 10.0*Expression::epsilon<T>());
}

void assert_parsed_expression_simplify_to(const char * expression, const char * simplifiedExpression, Preferences::AngleUnit angleUnit) {
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << "--------- Simplification ---------" << endl;
#endif
  assert_parsed_expression_process_to(expression, simplifiedExpression, angleUnit, Preferences::ComplexFormat::Cartesian, [](Expression e, Context & context, Preferences::AngleUnit angleUnit, Preferences::ComplexFormat complexFormat) {
      Expression result = e.clone().simplify(context, angleUnit);
      if (result.isUninitialized()) {
        return e;
      }
      return result;});
}

void assert_parsed_expression_serialize_to(Expression expression, const char * serializedExpression, Preferences::PrintFloatMode mode, int numberOfSignifiantDigits) {
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << "--------- Serialization ---------" << endl;
#endif
  char buffer[500];
  expression.serialize(buffer, sizeof(buffer), mode, numberOfSignifiantDigits);
  quiz_assert(strcmp(buffer, serializedExpression) == 0);
}

void assert_parsed_expression_layout_serialize_to_self(const char * expressionLayout) {
  Expression e = parse_expression(expressionLayout);
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << "---- Serialize: " << expressionLayout << "----"  << endl;
#endif
  Layout el = e.createLayout(DecimalMode, PrintFloat::k_numberOfStoredSignificantDigits);
  constexpr int bufferSize = 255;
  char buffer[bufferSize];
  el.serializeForParsing(buffer, bufferSize);
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << "---- serialized to: " << buffer << " ----\n"  << endl;
#endif
  quiz_assert(strcmp(expressionLayout, buffer) == 0);
}

void assert_expression_layout_serialize_to(Poincare::Layout layout, const char * serialization) {
  constexpr int bufferSize = 255;
  char buffer[bufferSize];
  layout.serializeForParsing(buffer, bufferSize);
  translate_in_ASCII_chars(buffer);
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << "---- Serialize: " << serialization << "----"  << endl;
  cout << "---- serialized to: " << buffer << " ----"  << endl;
  cout << "----- compared to: " << serialization << " ----\n"  << endl;
#endif
  quiz_assert(strcmp(serialization, buffer) == 0);
}

template void assert_parsed_expression_evaluates_to<float>(char const*, char const *, Poincare::Preferences::AngleUnit, Poincare::Preferences::ComplexFormat, int);
template void assert_parsed_expression_evaluates_to<double>(char const*, char const *, Poincare::Preferences::AngleUnit, Poincare::Preferences::ComplexFormat, int);
template void assert_parsed_expression_approximates_with_value_for_symbol(Poincare::Expression, const char *, float, float, Poincare::Preferences::AngleUnit);
template void assert_parsed_expression_approximates_with_value_for_symbol(Poincare::Expression, const char *, double, double, Poincare::Preferences::AngleUnit);
