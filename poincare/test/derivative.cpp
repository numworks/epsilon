#include <poincare/derivative.h>
#include <poincare/init.h>
#include <poincare/src/parsing/parser.h>
#include <poincare_nodes.h>
#include <apps/shared/global_context.h>
#include "helper.h"

using namespace Poincare;

void fillGlobalContext() {
  assert_reduce("-1→y");
  assert_reduce("9→a");
  assert_reduce("2→b");
  assert_reduce("3→c");
  assert_reduce("2x→f");
  assert_reduce("x^2→g");
}

void emptyGlobalContext() {
  Ion::Storage::sharedStorage()->recordNamed("y.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("b.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("c.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("f.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("g.exp").destroy();

}

void assert_parses_and_reduces_as(const char * expression, const char * simplifiedDerivative) {
#define SYMBOLIC 0
#if SYMBOLIC
  ExpressionNode::SymbolicComputation symbolicComputation = ReplaceAllDefinedSymbolsWithDefinition;
#else
  ExpressionNode::SymbolicComputation symbolicComputation = ReplaceAllSymbolsWithDefinitionsOrUndefined;
#endif

  assert_parsed_expression_simplify_to(expression, simplifiedDerivative, User, Radian, Cartesian, symbolicComputation);
}

QUIZ_CASE(poincare_derivative_literals) {
  fillGlobalContext();

  assert_parses_and_reduces_as("diff(undef,x,0)", "undef");

  assert_parses_and_reduces_as("diff(1,x,1)", "0");
  assert_parses_and_reduces_as("diff(1,x,y)", "0");

  assert_parses_and_reduces_as("diff(a,x,1)", "0");
  assert_parses_and_reduces_as("diff(a,x,y)", "0");

  assert_parses_and_reduces_as("diff(x,x,1)", "1");
  assert_parses_and_reduces_as("diff(x,x,y)", "1");

#if SYMBOLIC
  assert_parses_and_reduces_as("diff(1,x,z)", "0");
  assert_parses_and_reduces_as("diff(a,x,z)", "0");
  assert_parses_and_reduces_as("diff(x,x,z)", "1");
#else
  assert_parses_and_reduces_as("diff(1,x,z)", "undef");
  assert_parses_and_reduces_as("diff(a,x,z)", "undef");
  assert_parses_and_reduces_as("diff(x,x,z)", "undef");
#endif

  emptyGlobalContext();
}

QUIZ_CASE(poincare_derivative_additions) {
  fillGlobalContext();

  assert_parses_and_reduces_as("diff(1+x,x,1)", "1");
  assert_parses_and_reduces_as("diff(a+b,x,y)", "0");

#if SYMBOLIC
  assert_parses_and_reduces_as("diff(x+a,x,z)", "1");
#else
  assert_parses_and_reduces_as("diff(x+a,x,z)", "undef");
#endif

  emptyGlobalContext();
}

QUIZ_CASE(poincare_derivative_multiplications) {
  fillGlobalContext();

  assert_parses_and_reduces_as("diff(2x,x,1)", "2");
  assert_parses_and_reduces_as("diff(x*a,x,y)", "9");
  assert_parses_and_reduces_as("diff(a*b+c,x,1)", "0");
  assert_parses_and_reduces_as("diff(-x,x,y)", "-1");
  assert_parses_and_reduces_as("diff(f,x,1)", "2");

#if SYMBOLIC
  assert_parses_and_reduces_as("diff(a*x+b,x,z)", "9");
  assert_parses_and_reduces_as("diff(2-5x,x,z)", "-5");
#else
  assert_parses_and_reduces_as("diff(a*x+b,x,z)", "undef");
  assert_parses_and_reduces_as("diff(2-5x,x,z)", "undef");
#endif

  emptyGlobalContext();
}

QUIZ_CASE(poincare_derivative_powers) {
  fillGlobalContext();

  assert_parses_and_reduces_as("diff(x*x,x,1)", "2");
  assert_parses_and_reduces_as("diff(x^2,x,y)", "-2");
  assert_parses_and_reduces_as("diff(x^3/3,x,y)", "1");
  assert_parses_and_reduces_as("diff(1/x,x,1)", "-1");
  assert_parses_and_reduces_as("diff(2^x,x,y)", "ln(2)/2");
  assert_parses_and_reduces_as("diff(x^(-2),x,2)", "-1/4");
  assert_parses_and_reduces_as("diff(a^b,x,1)", "0");
  assert_parses_and_reduces_as("diff(a*x^2+b*x+c,x,1)", "20");
  assert_parses_and_reduces_as("diff((1+x)(2-x),x,1)", "-1");
  assert_parses_and_reduces_as("diff(g,x,3)", "6");

#if SYMBOLIC
  assert_parses_and_reduces_as("diff(x^3/3,x,z)", "z^2");
  assert_parses_and_reduces_as("diff(x^(-2),x,z)", "-2/z^3");
  assert_parses_and_reduces_as("diff(a*x^2+b*x+c,x,z)", "18×z+2");
  assert_parses_and_reduces_as("diff(x^c,x,z)", "3×z^2");
  assert_parses_and_reduces_as("diff(diff(x^3/6,x,t),t,z)", "z");
#else
  assert_parses_and_reduces_as("diff(x^3/3,x,z)", "undef");
  assert_parses_and_reduces_as("diff(x^(-2),x,z)", "undef");
  assert_parses_and_reduces_as("diff(a*x^2+b*x+c,x,z)", "undef");
  assert_parses_and_reduces_as("diff(x^c,x,z)", "undef");
  assert_parses_and_reduces_as("diff(diff(x^3/6,x,t),t,z)", "undef");
#endif

  emptyGlobalContext();
}

QUIZ_CASE(poincare_derivative_functions) {
  fillGlobalContext();

  assert_parses_and_reduces_as("diff(tan(x),x,0)", "1");
  assert_parses_and_reduces_as("diff(sin(a)+cos(b)+tan(c),x,y)", "0");
  assert_parses_and_reduces_as("diff(sin(cos(x)),x,y)", "sin(1)×cos(cos(1))");
  assert_parses_and_reduces_as("diff(ln(a),x,1)", "0");
  assert_parses_and_reduces_as("diff(tanh(x),x,0)", "1");
  assert_parses_and_reduces_as("diff(ln(cosh(x)),x,0)", "0");
  assert_parses_and_reduces_as("diff(log(x,7),x,1/2)", "2/ln(7)");

#if SYMBOLIC
  assert_parses_and_reduces_as("diff(sin(x),x,z)", "cos(z)");
  assert_parses_and_reduces_as("diff(cos(x),x,z)", "-sin(z)");
  assert_parses_and_reduces_as("diff(ln(x),x,z)", "1/z");
  assert_parses_and_reduces_as("diff(ln(c*x),x,z)", "1/z");
  assert_parses_and_reduces_as("diff(ln(cos(x)),x,z)", "-tan(z)");
  assert_parses_and_reduces_as("diff(diff(ln(x),x,1/tan(x)),x,z)", "1/cos(z)^2");
  assert_parses_and_reduces_as("diff(sinh(x),x,z)", "cosh(z)");
  assert_parses_and_reduces_as("diff(cosh(x),x,z)", "sinh(z)");
#else
  assert_parses_and_reduces_as("diff(sin(x),x,z)", "undef");
  assert_parses_and_reduces_as("diff(cos(x),x,z)", "undef");
  assert_parses_and_reduces_as("diff(ln(x),x,z)", "undef");
  assert_parses_and_reduces_as("diff(ln(c*x),x,z)", "undef");
  assert_parses_and_reduces_as("diff(ln(cos(x)),x,z)", "undef");
  assert_parses_and_reduces_as("diff(diff(ln(x),x,1/tan(x)),x,z)", "undef");
  assert_parses_and_reduces_as("diff(sinh(x),x,z)", "undef");
  assert_parses_and_reduces_as("diff(cosh(x),x,z)", "undef");
#endif

  emptyGlobalContext();
}