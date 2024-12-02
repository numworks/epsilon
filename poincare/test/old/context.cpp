#include <apps/shared/global_context.h>
#include <poincare/old/undefined.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/dimension.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/simplification.h>

#include "helper.h"

using namespace Poincare;
using namespace Poincare::Internal;

template <typename T>
void assert_parsed_expression_approximates_with_value_for_symbol(
    const Tree* expression, const char* symbol, T value, T approximation,
    Poincare::Preferences::ComplexFormat complexFormat = Cartesian,
    Poincare::Preferences::AngleUnit angleUnit = Radian) {
  Shared::GlobalContext globalContext;
  Internal::ProjectionContext projContext = {
      .m_complexFormat = complexFormat,
      .m_angleUnit = angleUnit,
      .m_symbolic = SymbolicComputation::ReplaceDefinedSymbols,
      .m_context = &globalContext};
  Tree* e = expression->cloneTree();
  Simplification::ToSystem(e, &projContext);
  T result = Approximation::To<T>(
      e, value, Approximation::Parameters{.isRoot = true, .prepare = true});
  assert_roughly_equal(result, approximation, OMG::Float::Epsilon<T>(), true);
}

QUIZ_CASE(poincare_context_user_variable_simple) {
  // Fill variable
  assert_reduce_and_store("1+2→Adadas");
  assert_parsed_expression_simplify_to("Adadas", "3");

  // Fill f1
  assert_reduce_and_store("1+x→f1(x)");
  assert_parsed_expression_simplify_to("f1(4)", "5");
  assert_parsed_expression_simplify_to("f1(Adadas)", "4");

  // Fill f2
  assert_reduce_and_store("x-1→f2(x)");
  assert_parsed_expression_simplify_to("f2(4)", "3");
  assert_parsed_expression_simplify_to("f2(Adadas)", "2");

  // Define fBoth with f1 and f2
  assert_reduce_and_store("f1(x)+f2(x)→fBoth(x)");
  assert_parsed_expression_simplify_to("fBoth(4)", "8");
  assert_parsed_expression_simplify_to("fBoth(Adadas)", "6");

  // Change f2
  assert_reduce_and_store("x→f2(x)");
  assert_parsed_expression_simplify_to("f2(4)", "4");
  assert_parsed_expression_simplify_to("f2(Adadas)", "3");

  // Make sure fBoth has changed
  assert_parsed_expression_simplify_to("fBoth(4)", "9");
  assert_parsed_expression_simplify_to("fBoth(Adadas)", "7");
}

QUIZ_CASE(poincare_context_user_variable_2_circular_variables) {
  assert_reduce_and_store("a→b");
  assert_reduce_and_store("b→a");
  assert_expression_approximates_to<double>("a", Undefined::Name());
  assert_expression_approximates_to<double>("b", Undefined::Name());
}

QUIZ_CASE(poincare_context_user_variable_3_circular_variables) {
  assert_reduce_and_store("a→b");
  assert_reduce_and_store("b→c");
  assert_reduce_and_store("c→a");
  assert_expression_approximates_to<double>("a", Undefined::Name());
  assert_expression_approximates_to<double>("b", Undefined::Name());
  assert_expression_approximates_to<double>("c", Undefined::Name());
}

QUIZ_CASE(poincare_context_user_variable_1_circular_function) {
  // h: x → h(x)
  assert_reduce_and_store("h(x)→h(x)");
  assert_expression_approximates_to<double>("h(1)", Undefined::Name());
}

QUIZ_CASE(poincare_context_user_variable_2_circular_functions) {
  assert_reduce_and_store("1→f(x)");
  assert_reduce_and_store("f(x)→g(x)");
  assert_reduce_and_store("g(x)→f(x)");
  assert_expression_approximates_to<double>("f(1)", Undefined::Name());
  assert_expression_approximates_to<double>("g(1)", Undefined::Name());
}

QUIZ_CASE(poincare_context_user_variable_3_circular_functions) {
  assert_reduce_and_store("1→f(x)");
  assert_reduce_and_store("f(x)→g(x)");
  assert_reduce_and_store("g(x)→h(x)");
  assert_reduce_and_store("h(x)→f(x)");
  assert_expression_approximates_to<double>("f(1)", Undefined::Name());
  assert_expression_approximates_to<double>("g(1)", Undefined::Name());
  assert_expression_approximates_to<double>("h(1)", Undefined::Name());
}

QUIZ_CASE(poincare_context_user_variable_circular_variables_and_functions) {
  assert_reduce_and_store("a→b");
  assert_reduce_and_store("b→a");
  assert_reduce_and_store("a→f(x)");
  assert_expression_approximates_to<double>("f(1)", Undefined::Name());
  assert_expression_approximates_to<double>("a", Undefined::Name());
  assert_expression_approximates_to<double>("b", Undefined::Name());
}

QUIZ_CASE(poincare_context_user_variable_composed_functions) {
  // f: x→x^2
  assert_reduce_and_store("x^2→f(x)");
  // g: x→f(x-2)
  assert_reduce_and_store("f(x-2)→g(x)");
  assert_expression_approximates_to<double>("f(2)", "4");
  assert_expression_approximates_to<double>("g(3)", "1");
  assert_expression_approximates_to<double>("g(5)", "9");

  // g: x→f(x-2)+f(x+1)
  assert_reduce_and_store("f(x-2)+f(x+1)→g(x)");
  // Add a sum to bypass simplification
  assert_expression_approximates_to<double>("g(3)+sum(1, n, 2, 4)", "20");
  assert_expression_approximates_to<double>("g(5)", "45");

  // g: x→x+1
  assert_reduce_and_store("x+1→g(x)");
  assert_expression_approximates_to<double>("f(g(4))", "25");
  // Add a sum to bypass simplification
  assert_expression_approximates_to<double>("f(g(4))+sum(1, n, 2, 4)", "28");
}

QUIZ_CASE(poincare_context_user_variable_functions_approximation_with_value) {
  // f : x→ x^2
  assert_reduce_and_store("x^2→f(x)");
  // Approximate f(x-2) with x = 5

  assert_parsed_expression_approximates_with_value_for_symbol(
      KFun<"f">(KSub("x"_e, 2_e)), "x", 5.0, 9.0);
  // Approximate f(x-1)+f(x+1) with x = 3
  assert_parsed_expression_approximates_with_value_for_symbol(
      KAdd(KFun<"f">(KSub("x"_e, 1_e)), KFun<"f">(KAdd("x"_e, 1_e))), "x", 3.0,
      20.0);

  // Clean the storage for other tests
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("f.func").destroy();

  // f : x → √(-1)
  assert_reduce_and_store("√(-1)×√(-1)→f(x)");
  // Approximate f(x) with x = 5
  // Cartesian
  assert_parsed_expression_approximates_with_value_for_symbol(KFun<"f">("x"_e),
                                                              "x", 1.0, -1.0);
  // Real
  assert_parsed_expression_approximates_with_value_for_symbol(
      KFun<"f">("x"_e), "x", 1.0, (double)NAN, Real);
}

QUIZ_CASE(poincare_context_user_variable_properties) {
  Shared::GlobalContext context;

  assert_reduce_and_store("[[1]]→a");
  quiz_assert(Internal::Dimension::Get("a"_e, &context).isMatrix());

  assert_reduce_and_store("[[x]]→f(x)");
  quiz_assert(Internal::Dimension::Get(KFun<"f">("x"_e), &context).isMatrix());

  assert_reduce_and_store("0.2*x→g(x)");
  quiz_assert(Internal::Dimension::Get(KFun<"g">(2_e), &context).isScalar());
}

QUIZ_CASE(poincare_context_function_evaluate_at_undef) {
  assert_reduce_and_store("0→f(x)");
  assert_reduce_and_store("f(1/0)→a");
  assert_parsed_expression_simplify_to("a", Undefined::Name());
  assert_reduce_and_store("f(1/0)→g(x)");
  assert_parsed_expression_simplify_to("g(1)", Undefined::Name());
  assert_reduce_and_store("f(undef)→b");
  assert_parsed_expression_simplify_to("b", Undefined::Name());
}
