#include <apps/shared/global_context.h>
#include <apps/shared/record_delegate.h>
#include <poincare/addition.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/subtraction.h>
#include <poincare/undefined.h>
#include "helper.h"

using namespace Poincare;

template<typename T>
void assert_parsed_expression_approximates_with_value_for_symbol(Expression expression, const char * symbol, T value, T approximation, Poincare::Preferences::ComplexFormat complexFormat = Cartesian, Poincare::Preferences::AngleUnit angleUnit = Radian) {
  Shared::GlobalContext globalContext;
  T result = expression.approximateWithValueForSymbol(symbol, value, &globalContext, complexFormat, angleUnit);
  assert_roughly_equal(result, approximation, Poincare::Float<T>::Epsilon(), true);
}

QUIZ_CASE(poincare_context_store_overwrite) {
  Shared::RecordDelegate recordDelegate;
  Ion::Storage::sharedStorage()->setRecordDelegate(&recordDelegate);
  assert_parsed_expression_simplify_to("1→g", "1");
  assert_parsed_expression_simplify_to("2→g", "2");
  assert_expression_approximates_to<double>("g", "2");
  assert_parsed_expression_simplify_to("-1→g(x)", "-1");
  assert_expression_approximates_to<double>("g", "undef");
  assert_expression_approximates_to<double>("g(4)", "-1");

  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("g.func").destroy();
  Ion::Storage::sharedStorage()->setRecordDelegate(nullptr);
}

QUIZ_CASE(poincare_context_store_do_not_overwrite) {
  Shared::RecordDelegate recordDelegate;
  Ion::Storage::sharedStorage()->setRecordDelegate(&recordDelegate);

  assert_parsed_expression_simplify_to("-1→g(x)", "-1");
  assert_parsed_expression_simplify_to("1+g(x)→f(x)", "1+g(x)");
  assert_expression_approximates_to<double>("f(1)", "0");
  assert_parsed_expression_simplify_to("2→g", Undefined::Name());
  assert_expression_approximates_to<double>("g(4)", "-1");
  assert_expression_approximates_to<double>("f(4)", "0");

  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();
  Ion::Storage::sharedStorage()->recordNamed("g.func").destroy();
  Ion::Storage::sharedStorage()->setRecordDelegate(nullptr);
}

QUIZ_CASE(poincare_context_user_variable_simple) {
  Shared::RecordDelegate recordDelegate;
  Ion::Storage::sharedStorage()->setRecordDelegate(&recordDelegate);

  // Fill variable
  assert_parsed_expression_simplify_to("1+2→Adadas", "3");
  assert_parsed_expression_simplify_to("Adadas", "3");

  // Fill f1
  assert_parsed_expression_simplify_to("1+x→f1(x)", "1+x");
  assert_parsed_expression_simplify_to("f1(4)", "5");
  assert_parsed_expression_simplify_to("f1(Adadas)", "4");

  // Fill f2
  assert_parsed_expression_simplify_to("x-1→f2(x)", "x-1");
  assert_parsed_expression_simplify_to("f2(4)", "3");
  assert_parsed_expression_simplify_to("f2(Adadas)", "2");

  // Define fBoth with f1 and f2
  assert_parsed_expression_simplify_to("f1(x)+f2(x)→fBoth(x)", "f1(x)+f2(x)");
  assert_parsed_expression_simplify_to("fBoth(4)", "8");
  assert_parsed_expression_simplify_to("fBoth(Adadas)", "6");

  // Change f2
  assert_parsed_expression_simplify_to("x→f2(x)", "x");
  assert_parsed_expression_simplify_to("f2(4)", "4");
  assert_parsed_expression_simplify_to("f2(Adadas)", "3");

  // Make sure fBoth has changed
  assert_parsed_expression_simplify_to("fBoth(4)", "9");
  assert_parsed_expression_simplify_to("fBoth(Adadas)", "7");

  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("Adadas.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("f1.func").destroy();
  Ion::Storage::sharedStorage()->recordNamed("f2.func").destroy();
  Ion::Storage::sharedStorage()->recordNamed("fBoth.func").destroy();
  Ion::Storage::sharedStorage()->setRecordDelegate(nullptr);
}

QUIZ_CASE(poincare_context_user_variable_2_circular_variables) {

  assert_reduce("a→b");
  assert_reduce("b→a");
  assert_expression_approximates_to<double>("a", Undefined::Name());
  assert_expression_approximates_to<double>("b", Undefined::Name());

  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("b.exp").destroy();
}

QUIZ_CASE(poincare_context_user_variable_3_circular_variables) {
  assert_reduce("a→b");
  assert_reduce("b→c");
  assert_reduce("c→a");
  assert_expression_approximates_to<double>("a", Undefined::Name());
  assert_expression_approximates_to<double>("b", Undefined::Name());
  assert_expression_approximates_to<double>("c", Undefined::Name());

  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("b.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("c.exp").destroy();
}

QUIZ_CASE(poincare_context_user_variable_1_circular_function) {
  // h: x → h(x)
  assert_reduce("h(x)→h(x)");
  assert_expression_approximates_to<double>("h(1)", Undefined::Name());

  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("h.func").destroy();
}

QUIZ_CASE(poincare_context_user_variable_2_circular_functions) {
  assert_reduce("1→f(x)");
  assert_reduce("f(x)→g(x)");
  assert_reduce("g(x)→f(x)");
  assert_expression_approximates_to<double>("f(1)", Undefined::Name());
  assert_expression_approximates_to<double>("g(1)", Undefined::Name());

  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();
  Ion::Storage::sharedStorage()->recordNamed("g.func").destroy();
}

QUIZ_CASE(poincare_context_user_variable_3_circular_functions) {
  assert_reduce("1→f(x)");
  assert_reduce("f(x)→g(x)");
  assert_reduce("g(x)→h(x)");
  assert_reduce("h(x)→f(x)");
  assert_expression_approximates_to<double>("f(1)", Undefined::Name());
  assert_expression_approximates_to<double>("g(1)", Undefined::Name());
  assert_expression_approximates_to<double>("h(1)", Undefined::Name());

  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();
  Ion::Storage::sharedStorage()->recordNamed("g.func").destroy();
  Ion::Storage::sharedStorage()->recordNamed("h.func").destroy();
}

QUIZ_CASE(poincare_context_user_variable_circular_variables_and_functions) {
  assert_reduce("a→b");
  assert_reduce("b→a");
  assert_reduce("a→f(x)");
  assert_expression_approximates_to<double>("f(1)", Undefined::Name());
  assert_expression_approximates_to<double>("a", Undefined::Name());
  assert_expression_approximates_to<double>("b", Undefined::Name());

  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("b.exp").destroy();
}

QUIZ_CASE(poincare_context_user_variable_composed_functions) {
  // f: x→x^2
  assert_reduce("x^2→f(x)");
  // g: x→f(x-2)
  assert_reduce("f(x-2)→g(x)");
  assert_expression_approximates_to<double>("f(2)", "4");
  assert_expression_approximates_to<double>("g(3)", "1");
  assert_expression_approximates_to<double>("g(5)", "9");

  // g: x→f(x-2)+f(x+1)
  assert_reduce("f(x-2)+f(x+1)→g(x)");
  // Add a sum to bypass simplification
  assert_expression_approximates_to<double>("g(3)+sum(1, n, 2, 4)", "20");
  assert_expression_approximates_to<double>("g(5)", "45");

  // g: x→x+1
  assert_reduce("x+1→g(x)");
  assert_expression_approximates_to<double>("f(g(4))", "25");
  // Add a sum to bypass simplification
  assert_expression_approximates_to<double>("f(g(4))+sum(1, n, 2, 4)", "28");

  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();
  Ion::Storage::sharedStorage()->recordNamed("g.func").destroy();
}

QUIZ_CASE(poincare_context_user_variable_functions_approximation_with_value_for_symbol) {
  // f : x→ x^2
  assert_reduce("x^2→f(x)");
  // Approximate f(?-2) with ? = 5

  constexpr int bufferSize = CodePoint::MaxCodePointCharLength + 1;
  char x[bufferSize];
  Poincare::SerializationHelper::CodePoint(x, bufferSize, UCodePointUnknown);

  assert_parsed_expression_approximates_with_value_for_symbol(Function::Builder("f", 1, Subtraction::Builder(Symbol::Builder(UCodePointUnknown), Rational::Builder(2))), x, 5.0, 9.0);
  // Approximate f(?-1)+f(?+1) with ? = 3
  assert_parsed_expression_approximates_with_value_for_symbol(Addition::Builder(Function::Builder("f", 1, Subtraction::Builder(Symbol::Builder(UCodePointUnknown), Rational::Builder(1))), Function::Builder("f", 1, Addition::Builder(Symbol::Builder(UCodePointUnknown), Rational::Builder(1)))), x, 3.0, 20.0);

  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();

  // f : x → √(-1)
  assert_reduce("√(-1)×√(-1)→f(x)");
  // Approximate f(?) with ? = 5
  // Cartesian
  assert_parsed_expression_approximates_with_value_for_symbol(Function::Builder("f", 1, Symbol::Builder(UCodePointUnknown)), x, 1.0, -1.0);
  // Real
  assert_parsed_expression_approximates_with_value_for_symbol(Function::Builder("f", 1, Symbol::Builder(UCodePointUnknown)), x, 1.0, (double)NAN, Real);

  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();
}

QUIZ_CASE(poincare_context_user_variable_properties) {
  Shared::GlobalContext context;

  assert_expression_approximates_to<double>("[[1]]→a", "[[1]]");
  quiz_assert(Symbol::Builder('a').recursivelyMatches(Expression::IsMatrix, &context));

  /* [[x]]→f(x) expression contains a matrix, so its simplification is going
   * to be interrupted. We thus rather approximate it instead of simplifying it.
   * TODO: use parse_and_simplify when matrix are simplified. */

  assert_expression_approximates_to<double>("[[x]]→f(x)", "undef");
  quiz_assert(Function::Builder("f", 1, Symbol::Builder('x')).recursivelyMatches(Poincare::Expression::IsMatrix, &context));
  assert_expression_approximates_to<double>("0.2*x→g(x)", "undef");
  quiz_assert(Function::Builder("g", 1, Rational::Builder(2)).recursivelyMatches(Expression::IsApproximate, &context));

  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("a.mat").destroy();
  Ion::Storage::sharedStorage()->recordNamed("b.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();
  Ion::Storage::sharedStorage()->recordNamed("g.func").destroy();
}

QUIZ_CASE(poincare_context_function_evaluate_at_undef) {
  assert_reduce("0→f(x)");
  assert_reduce("f(1/0)→a");
  assert_parsed_expression_simplify_to("a", Undefined::Name());
  assert_reduce("f(1/0)→g(x)");
  assert_parsed_expression_simplify_to("g(1)", Undefined::Name());
  assert_reduce("f(undef)→b");
  assert_parsed_expression_simplify_to("b", Undefined::Name());

  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("b.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();
  Ion::Storage::sharedStorage()->recordNamed("g.func").destroy();
}

template void assert_parsed_expression_approximates_with_value_for_symbol(Poincare::Expression, const char *, float, float, Poincare::Preferences::ComplexFormat, Poincare::Preferences::AngleUnit);
template void assert_parsed_expression_approximates_with_value_for_symbol(Poincare::Expression, const char *, double, double, Poincare::Preferences::ComplexFormat, Poincare::Preferences::AngleUnit);
