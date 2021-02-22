#include "helper.h"
#include <poincare/undefined.h>
#include <apps/shared/global_context.h>
#include <ion/storage.h>

using namespace Poincare;

template <size_t N>
void assert_expression_simplify_to_with_dependencies(
    const char * expression,
    const char * simplifiedExpression,
    const char * const (&dependencies)[N],
    ExpressionNode::ReductionTarget target = User,
    Preferences::AngleUnit angleUnit = Radian,
    Preferences::UnitFormat unitFormat = Metric,
    Preferences::ComplexFormat complexFormat = Cartesian,
    ExpressionNode::SymbolicComputation symbolicComputation = ReplaceAllDefinedSymbolsWithDefinition,
    ExpressionNode::UnitConversion unitConversion = DefaultUnitConversion)
{
  if (N == 1 && dependencies[0][0] == '\0') {
    assert_parsed_expression_simplify_to(expression, simplifiedExpression, target, angleUnit, unitFormat, complexFormat, symbolicComputation, unitConversion);
    return;
  }

  Shared::GlobalContext globalContext;
  Expression e = parse_expression(expression, &globalContext, false);
  ExpressionNode::ReductionContext reductionContext(&globalContext, complexFormat, angleUnit, unitFormat, target, symbolicComputation, unitConversion);
  Expression d = e.simplify(reductionContext);

  quiz_assert_print_if_failure(d.type() == ExpressionNode::Type::Dependency, expression);
  assert_expression_serialize_to(d.childAtIndex(0), simplifiedExpression);
  Expression m = d.childAtIndex(1);
  quiz_assert_print_if_failure(m.type() == ExpressionNode::Type::Matrix, expression);
  quiz_assert_print_if_failure(m.numberOfChildren() == N, expression);
  for (size_t i = 0; i < N; i++) {
    assert_expression_serialize_to(m.childAtIndex(i), dependencies[i]);
  }
}

QUIZ_CASE(poincare_no_dependency) {
  assert_reduce("1→a");

  assert_expression_simplify_to_with_dependencies("1+2", "3", {""});
  assert_expression_simplify_to_with_dependencies("a", "1", {""});
  assert_expression_simplify_to_with_dependencies("a+1", "2", {""});

  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
}

QUIZ_CASE(poincare_dependency_function) {
  assert_reduce("x+1→f(x)");
  assert_reduce("3→g(x)");
  assert_reduce("1+2→a");

  // A variable argument is used as dependency
  assert_expression_simplify_to_with_dependencies("f(x)", "x+1", {"x"});
  assert_expression_simplify_to_with_dependencies("f(x+y)", "x+y+1", {"x+y"});
  assert_expression_simplify_to_with_dependencies("g(x)", "3", {"x"});
  assert_expression_simplify_to_with_dependencies("g(x+a+3+4)", "3", {"x+10"});
  // A constant argument does not create dependencies
  assert_expression_simplify_to_with_dependencies("f(2)", "3", {""});
  assert_expression_simplify_to_with_dependencies("g(-1.23)", "3", {""});
  // Dependencies makes sure f(undef) = undef for all f
  assert_expression_simplify_to_with_dependencies("f(1/0)", Undefined::Name(), {""});
  assert_expression_simplify_to_with_dependencies("g(1/0)", Undefined::Name(), {""});

  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();
  Ion::Storage::sharedStorage()->recordNamed("g.func").destroy();
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
}

QUIZ_CASE(poincare_dependency_bubble_up) {
  assert_reduce("2x→f(x)");
  assert_reduce("x+1→g(x)");

  assert_expression_simplify_to_with_dependencies("1+f(x)", "2×x+1", {"x"});
  assert_expression_simplify_to_with_dependencies("g(y)+f(x)", "2×x+y+1", {"y", "x"});
  assert_expression_simplify_to_with_dependencies("g(f(x))", "2×x+1", {"x", "2×x"});
  assert_expression_simplify_to_with_dependencies("f(g(x))", "2×x+2", {"x", "x+1"});

  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();
  Ion::Storage::sharedStorage()->recordNamed("g.func").destroy();
}

QUIZ_CASE(poincare_dependency_derivative) {
  assert_reduce("x^2→f(x)");

  assert_expression_simplify_to_with_dependencies("diff(cos(x),x,0)", "0", {""});
  assert_expression_simplify_to_with_dependencies("diff(2x,x,y)", "2", {"2×y"});
  assert_expression_simplify_to_with_dependencies("diff(f(x),x,a)", "2×a", {"a", "a^2"});

  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();
}
