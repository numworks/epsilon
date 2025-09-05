#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/variables.h>

#include "../helpers/symbol_store.h"
#include "helper.h"

using namespace Poincare::Internal;
using Poincare::ProjectionContext;

QUIZ_CASE(pcj_simplification_parametric) {
  // Leave and enter with a nested parametric
  const Tree* a =
      KSum("k"_e, 0_e, 2_e,
           KMult(KVarK, KDiscreteVar<Parametric::k_localVariableId + 2>));
  const Tree* b =
      KSum("k"_e, 0_e, 2_e,
           KMult(KVarK, KDiscreteVar<Parametric::k_localVariableId + 1>));
  Tree* e = a->cloneTree();
  Variables::LeaveScope(e);
  assert_trees_are_equal(e, b);
  Variables::EnterScope(e);
  assert_trees_are_equal(e, a);
  e->removeTree();

  // sum
  simplifies_to("sum(1,k,3,4)", "2");
  simplifies_to("sum(k,k,3,4)", "7");
  simplifies_to("sum(1,k,a,a)", "dep(1,{realInteger(a)})");
  simplifies_to("sum(π,k,a,a+1)", "dep(2×π,{realInteger(a)})");
  simplifies_to("sum(1,k,1,floor(tan(-11)))", "225");
  simplifies_to("sum(n,k,1,n)", "dep(n^2,{realInteger(n)})");
  simplifies_to("sum(a*b,k,1,n)", "dep(a×b×n,{realInteger(n)})");
  simplifies_to("sum(k,k,n,j)",
                "dep((j^2-n^2+j+n)/2,{realInteger(n),realInteger(j)})");
  simplifies_to("2×sum(k,k,0,n)+n", "dep(n×(n+2),{realInteger(n)})");
  simplifies_to("2×sum(k,k,3,n)+n", "dep(n×(n+2)-6,{realInteger(n)})");
  simplifies_to("sum(k^2,k,n,j)",
                "dep((j×(j+1)×(2×j+1)-n×(n-1)×(2×n-1))/"
                "6,{realInteger(n),realInteger(j)})");
  simplifies_to("sum(k^2,k,2,5)", "54");
  simplifies_to("sum((2k)^2,k,2,5)", "216");
  simplifies_to("sum((2k)^2,k,0,n)",
                "dep((2×n×(n+1)×(2×n+1))/3,{realInteger(n)})");
  simplifies_to("sum((2k)^4,k,0,n)", "dep(16×sum(k^4,k,0,n),{realInteger(n)})");
  simplifies_to("sum(k*cos(k),k,1,n)", "sum(cos(k)×k,k,1,n)");
  simplifies_to("sum(sum(x*j,j,1,n),k,1,2)", "dep(n×(n+1)×x,{realInteger(n)})");
  simplifies_to("sum(sum(a*k,a,0,j),k,1,n)",
                "dep((j×(j+1)×n×(n+1))/4,{realInteger(j),realInteger(n)})");
  simplifies_to("sum(sum(j,j,0,k),k,1,n)",
                "dep(n^3/6+n^2/2+n/3,{realInteger(n)})");
  simplifies_to("sum(sum(j,j,0,k+π),k,1,n)", "undef");
  simplifies_to("sum(π^k,k,4,2)", "0");
  simplifies_to("sum(sin(k),k,a+10,a)", "dep(0,{realInteger(a)})");
  simplifies_to("sum(sin(k),k,a,a-10)", "dep(0,{realInteger(a)})");
  simplifies_to("sum(random()*k,k,0,n)", "sum(random()×k,k,0,n)");
  simplifies_to("sum(random(),k,0,10)", "sum(random(),k,0,10)");
  simplifies_to("sum(k/k,k,1,n)",
                "dep(n,{sum(k^0,k,1,n),sum(1/k,k,1,n),realInteger(n)})");
  simplifies_to("sum(k/k,k,0,n)",
                "dep(n+1,{sum(k^0,k,0,n),sum(1/k,k,0,n),realInteger(n)})");
  simplifies_to("sum(k/k,k,-1,n)",
                "dep(n+2,{sum(k^0,k,-1,n),sum(1/k,k,-1,n),realInteger(n)})");

  // product
  simplifies_to("product(p,k,j,n)",
                "dep(p^(-j+n+1),{realInteger(j),realInteger(n)})");
  simplifies_to("product(p^3,k,j,n)",
                "dep((p^3)^(-j+n+1),{realInteger(j),realInteger(n)})");
  simplifies_to("product(k^3,k,j,n)",
                "dep(product(k,k,j,n)^3,{realInteger(j),realInteger(n)})");
  simplifies_to("product(k^x,k,j,n)", "product(k^x,k,j,n)");
  simplifies_to("product(x^k,k,j,n)", "product(x^k,k,j,n)");
  simplifies_to("product(π^k,k,2,1)", "1");
  simplifies_to("product(sin(k),k,a+10,a)", "dep(1,{realInteger(a)})");
  simplifies_to("product(sin(k),k,a,a-10)", "dep(1,{realInteger(a)})");
  simplifies_to("product(random()*k,k,0,n)", "product(random()×k,k,0,n)");
  simplifies_to("product(random(),k,0,10)", "product(random(),k,0,10)");

  // product(exp) <-> exp(sum)
  simplifies_to(
      "exp(2*sum(ln(k),k,a,b) + ln(b))",
      "dep(b×product(k,k,a,b)^2,{sum(nonNull(k),k,a,b),sum(realPos(k),k,a,b),"
      "nonNull(b),realPos(b),realInteger(a),realInteger(b)})");
  simplifies_to("product(exp(2k),k,0,y)", "dep(e^(y^2+y),{realInteger(y)})");

  // expand sum
  simplifies_to("sum(k^3+4,k,n,n+3)-16",
                "dep(sum(k^3,k,n,n+3),{realInteger(n)})");
  simplifies_to("sum(x*k!,k,1,2)", "3*x");
  simplifies_to("sum(tan(k),k,a,a)", "dep(tan(a),{realInteger(a)})");

  // expand product
  simplifies_to("product(4*cos(k),k,n,n+3)/256",
                "dep(product(cos(k),k,n,n+3),{realInteger(n)})");
  simplifies_to("product(cos(k),k,2,4)-cos(2)×cos(3)×cos(4)", "0");
  simplifies_to("product(sin(k),k,a+1,a+1)", "dep(sin(a+1),{realInteger(a)})");

  PoincareTest::SymbolStore symbolStore;
  PoincareTest::store("x→f(x)", symbolStore);
  ProjectionContext ctx = {
      .m_symbolic = Poincare::SymbolicComputation::KeepAllSymbols,
      .m_context = symbolStore,
  };

  // contract sum
  simplifies_to("sum(f(k),k,a,b)-sum(f(u),u,a,b)",
                "dep(0,{realInteger(a),realInteger(b)})", ctx);
  simplifies_to("sum(f(k),k,a,b+n)-sum(f(u),u,a,b)",
                "sum(f(k),k,a,b+n)-sum(f(u),u,a,b)", ctx);
  simplifies_to("sum(f(k),k,a,b+10)-sum(f(u),u,a,b)",
                "dep(sum(f(k),k,b+1,b+10),{realInteger(a)})", ctx);
  simplifies_to("sum(f(k),k,a,100)-sum(f(u),u,a,5)",
                "dep(sum(f(k),k,6,100),{realInteger(a)})", ctx);
  simplifies_to("sum(f(k),k,a,b)-sum(f(u),u,a,b+n)",
                "sum(f(k),k,a,b)-sum(f(u),u,a,b+n)", ctx);
  simplifies_to("sum(f(k),k,a,b)-sum(f(u),u,a,b+10)",
                "dep(-sum(f(u),u,b+1,b+10),{realInteger(a)})", ctx);
  simplifies_to("sum(f(k),k,a,5)-sum(f(u),u,a,100)",
                "dep(-sum(f(u),u,6,100),{realInteger(a)})", ctx);
  simplifies_to("sum(f(k),k,a+10,b)-sum(f(u),u,a,b)",
                "dep(-sum(f(u),u,a,a+9),{realInteger(b)})", ctx);
  simplifies_to("sum(f(k),k,100,b)-sum(f(u),u,5,b)",
                "dep(-sum(f(u),u,5,99),{realInteger(b)})", ctx);
  simplifies_to("sum(f(k),k,a,b)-sum(f(u),u,a+n,b)",
                "sum(f(k),k,a,b)-sum(f(u),u,a+n,b)", ctx);
  simplifies_to("sum(f(k),k,a,b)-sum(f(u),u,a+10,b)",
                "dep(sum(f(k),k,a,a+9),{realInteger(b)})", ctx);
  simplifies_to("sum(f(k),k,5,b)-sum(f(u),u,100,b)",
                "dep(sum(f(k),k,5,99),{realInteger(b)})", ctx);

  // contract product
  simplifies_to("product(f(k),k,a,b) / product(f(u),u,a,b)",
                "dep(1,{realInteger(a),realInteger(b)})", ctx);
  simplifies_to("product(f(k),k,a,b+n) / product(f(u),u,a,b)",
                "product(f(k),k,a,b+n)/product(f(u),u,a,b)", ctx);
  simplifies_to("product(f(k),k,a,b+10) / product(f(u),u,a,b)",
                "dep(product(f(k),k,b+1,b+10),{realInteger(a)})", ctx);
  simplifies_to("product(f(k),k,a,100) / product(f(u),u,a,5)",
                "dep(product(f(k),k,6,100),{realInteger(a)})", ctx);
  simplifies_to("product(f(k),k,a,b) / product(f(u),u,a,b+n)",
                "product(f(k),k,a,b)/product(f(u),u,a,b+n)", ctx);
  simplifies_to("product(f(k),k,a,b) / product(f(u),u,a,b+10)",
                "dep(1/product(f(u),u,b+1,b+10),{realInteger(a)})", ctx);
  simplifies_to("product(f(k),k,a,5) / product(f(u),u,a,100)",
                "dep(1/product(f(u),u,6,100),{realInteger(a)})", ctx);
  simplifies_to("product(f(k),k,a+10,b) / product(f(u),u,a,b)",
                "dep(1/product(f(u),u,a,a+9),{realInteger(b)})", ctx);
  simplifies_to("product(f(k),k,100,b) / product(f(u),u,5,b)",
                "dep(1/product(f(u),u,5,99),{realInteger(b)})", ctx);
  simplifies_to("product(f(k),k,a,b)/product(f(u),u,a+n,b)",
                "product(f(k),k,a,b)/product(f(u),u,a+n,b)", ctx);
  simplifies_to("product(f(k),k,a,b)/product(f(u),u,a+10,b)",
                "dep(product(f(k),k,a,a+9),{realInteger(b)})", ctx);
  simplifies_to("product(f(k),k,5,b)/product(f(u),u,100,b)",
                "dep(product(f(k),k,5,99),{realInteger(b)})", ctx);

  // undef
  simplifies_to("sum(k,k,1/2,10)", "undef");
  simplifies_to("product(1,k,2,pi)", "undef");
  simplifies_to("product(1,k,pi,pi+1)", "undef");
  simplifies_to("sum(1,k,i,i+1)", "undef");
  simplifies_to("sum(1,k,1,n×i)", "sum(1,k,1,n×i)");
  simplifies_to_no_beautif("sum([[0]],k,i,0)", "[[undef]]");
  simplifies_to_no_beautif("product([[0]],k,1,pi)", "[[undef]]");

  // Parametric in functions
  PoincareTest::store("x^2→f(x)", symbolStore);
  PoincareTest::store("f'(x)→g(x)", symbolStore);
  PoincareTest::store("diff(f(x),x,x)→h(x)", symbolStore);
  ProjectionContext ctx2 = {
      .m_symbolic = Poincare::SymbolicComputation::ReplaceDefinedFunctions,
      .m_context = symbolStore,
  };
  simplifies_to("f'(4)", "8", ctx2);
  simplifies_to("g(4)", "8", ctx2);
  simplifies_to("h(4)", "8", ctx2);

  /* Test with no symbolic computation to check that n inside a sum expression
   * is not replaced by Undefined */
  simplifies_to(
      "sum(n,n,1,5)", "15",
      {.m_symbolic = Poincare::SymbolicComputation::ReplaceAllSymbols});
  simplifies_to(
      "sum(1/n,n,1,2)", "3/2",
      {.m_symbolic = Poincare::SymbolicComputation::ReplaceAllSymbols});
}

QUIZ_CASE(pcj_simplification_derivative) {
  // Undefined
  simplifies_to("diff(undef,x,x)", "undef");
  simplifies_to("diff(nonreal,x,x)", "nonreal");
  simplifies_to("diff(inf,x,x)", "undef");

  // Constants
  simplifies_to("diff(i,x,x)", "undef");
  simplifies_to("diff(π,x,x)", "0");
  simplifies_to("diff(e,x,x)", "0");
  simplifies_to("diff(1,x,x)", "0");

  // Trigonometry
  simplifies_to("diff(sin(x),x,x)", "cos(x)");
  simplifies_to("diff(sin(x),x,x)", "(π×cos(x))/180", degreeCtx);
  simplifies_to("diff(cos(x),x,x)", "-sin(x)");
  simplifies_to("diff(cos(x),x,x)", "-(π×sin(x))/200", gradianCtx);
  simplifies_to("diff(tan(x),x,x)", "dep(tan(x)^2+1,{tan(x)})");
  simplifies_to("diff(tan(x),x,x)",
                "dep((π×(tan(x)^2+1))/180,{tan((180×π×x)/(180×π))})",
                degreeCtx);
  simplifies_to(
      "diff(acos(x),x,x)",
      "dep(-1/√(-x^2+1),{piecewise(0,abs(x)≤1,nonreal),real(arccos(x))})");
  simplifies_to("diff(acos(x),x,x)",
                "dep(-180/"
                "(π×√(-x^2+1)),{piecewise(0,abs(x)≤1,nonreal),real(("
                "180×π×arccos(x))/(180×π))})",
                degreeCtx);
  simplifies_to(
      "diff(asin(x),x,x)",
      "dep(1/√(-x^2+1),{piecewise(0,abs(x)≤1,nonreal),real(arcsin(x))})");
  simplifies_to("diff(asin(x),x,x)",
                "dep(180/"
                "(π×√(-x^2+1)),{piecewise(0,abs(x)≤1,nonreal),real(("
                "180×π×arcsin(x))/(180×π))})",
                degreeCtx);
  simplifies_to("diff(atan(x),x,x)", "1/(x^2+1)");
  simplifies_to("diff(atan(x),x,x)", "180/(π×x^2+π)", degreeCtx);
  simplifies_to("diff(sinh(x),x,x)", "cosh(x)");
  simplifies_to("diff(cosh(x),x,x)", "sinh(x)");
  simplifies_to("diff(tanh(x),x,x)", "dep(-(tanh(x)^2)+1,{-tanh(x)×i×i})");
  simplifies_to("diff(arcosh(x),x,x)", "dep(1/√(x^2-1),{real(arcosh(x))})");
  simplifies_to("diff(arsinh(x),x,x)", "1/√(x^2+1)");
  simplifies_to("diff(artanh(x),x,x)",
                "dep(1/(-x^2+1),{real(-artanh(x)×i×i)})");
  simplifies_to("diff(sec(x),x,x)", "sin(x)/cos(x)^2");
  simplifies_to("diff(csc(x),x,x)", "-cos(x)/sin(x)^2");
  simplifies_to("diff(cot(x),x,x)", "dep(-(1+cot(x)^2),{cot(x)})");
  simplifies_to("diff(arcsec(x),x,x)",
                "dep(1/(x^2×√(1-1/x^2)),{piecewise(0,abs(1/"
                "x)≤1,nonreal),real(arccos(1/x))})");
  simplifies_to("diff(arccsc(x),x,x)",
                "dep(-1/(x^2×√(1-1/x^2)),{piecewise(0,abs(1/"
                "x)≤1,nonreal),real(arcsin(1/x))})");
  simplifies_to("diff(arccot(x),x,x)", "-1/(x^2+1)");
  simplifies_to("diff(sin(x)^2,x,x)", "sin(2×x)");
  simplifies_to("diff(sinh(sin(y)),x,x)", "0");

  // Others
  simplifies_to("diff(y,x,x)", "0");
  simplifies_to("diff(x,x,x)", "1");
  simplifies_to("diff(x^2,x,x)", "2×x");
  simplifies_to("diff(x, x, 2)", "1");
  simplifies_to("diff(a×x, x, 1)", "a");
  simplifies_to("diff(23, x, 1)", "0");
  simplifies_to("diff(1+x, x, y)", "1");
  simplifies_to("diff(sin(ln(x)), x, y)",
                "dep(cos(ln(y))/y,{real(sin(ln(y))),realPos(y)})");
  simplifies_to("diff(((x^4)×ln(x)×e^(3x)), x, y)",
                "dep((3×y^4×ln(y)+y^3×(1+4×ln(y)))×e^(3×y),{nonNull(y),real(y^"
                "4×ln(y)×e^(3×y)),realPos(y)})");
  simplifies_to("diff(diff(x^2, x, x)^2, x, y)", "8×y");
  simplifies_to("diff((x-1)(x-2)(x-3),x,x)", "3×x^2-12×x+11");
  simplifies_to("diff(√(x),x,x)", "dep(1/(2×√(x)),{real(√(x))})", cartesianCtx);
  simplifies_to("diff(1/x,x,x)", "-1/x^2");
  simplifies_to("diff(diff(x^3,x,x),x,x)", "6×x");
  simplifies_to("diff(e^x,x,x)", "e^(x)");
  simplifies_to("diff(2^x,x,x)", "ln(2)×2^x");
  simplifies_to("diff(ln(x),x,x)", "dep(1/x,{real(ln(x))})", cartesianCtx);
  // TODO: fix beautification ln(2)+ln(5) -> ln(10)
  simplifies_to("diff(log(x),x,x)",
                "dep(1/(x×(ln(2)+ln(5))),{real(log(x)),realPos(x)})");
  simplifies_to("diff(x+x*floor(x), x, y)", "y×diff(floor(x),x,y)+1+floor(y)");
  simplifies_to("diff(ln(x), x, -1)", "undef");
  simplifies_to("diff(randint(0,5), x, 2)", "undef");
  simplifies_to("diff(x+floor(random()), x, 2)", "undef");

  // Derivating a complex expression is forbidden
  simplifies_to("diff(i,x,2)", "undef");
  simplifies_to("diff(e^(i×x),x,3)", "undef");
  simplifies_to("diff(ln(x),x,-1)", "undef");

  // Derivating a variable evaluated at a complex value is forbidden
  simplifies_to("diff(x,x,-i)", "undef");
  simplifies_to("diff(abs(x),x,i)", "undef");
  simplifies_to("diff(ln(x),x,i)", "undef");
  simplifies_to("diff(x,x,ln(-3))", "undef");
  simplifies_to("diff(floor(z), x, y)", "dep(0,{floor(z)})");

  // Piecewise
#if TODO_PCJ
  simplifies_to("diff(piecewise(x,x≥1,-x,x≠0),x,x)",
                "dep(piecewise(1,x>1,undef,x≥1,-1,"
                "x≠0,undef,x=0),{piecewise(x,x≥1,-x,x≠0)})");
  simplifies_to("diff(piecewise(x,x=1,2×x,x<5,3),x,x)",
                "dep(piecewise(undef,x=1,2,x<5,undef,x≤5,0),{"
                "piecewise(x,x=1,2×x,x<5,3)})");
#endif

  // Matrices
  simplifies_to("diff([[x]],x,x)", "undef");
  simplifies_to("diff([[2t,3t]],t,t)", "undef");
  simplifies_to("diff([[2t][3t]],t,t)", "undef");

  // With context
  {
    PoincareTest::SymbolStore symbolStore;
    store("2→a", symbolStore);
    store("-1→b", symbolStore);
    store("3→c", symbolStore);
    store("x/2→f(x)", symbolStore);

    ProjectionContext ctx = {
        .m_symbolic = Poincare::SymbolicComputation::ReplaceDefinedSymbols,
        .m_context = symbolStore};

    simplifies_to("diff(a×x^2+b×x+c,x,x)", "4×x-1", ctx);
    simplifies_to("diff(f(x),x,x)", "1/2", ctx);
    simplifies_to("diff(a^2,a,x)", "2×x", ctx);
    simplifies_to("diff(a^2,a,a)", "4", ctx);
    simplifies_to("diff(b^2,b,2)", "4", ctx);

    symbolStore.reset();

    // On points
    simplifies_to("diff((sin(t),cos(t)),t,t)", "(cos(t),-sin(t))", ctx);
    store("(3t,-2t^2)→f(t)", symbolStore);
    simplifies_to("diff(f(t),t,t)", "(3,-4×t)", ctx);
  }

  // On points
  simplifies_to("diff((x^2, floor(x)),x,k)", "(2×k,diff(floor(x),x,k))");
  simplifies_to("diff((sin(t),cos(t)),t,π/6)", "(√(3)/2,-1/2)");
  simplifies_to("diff((1,2),t,1)", "(0,0)");
}

QUIZ_CASE(pcj_simplification_derivative_higher_order) {
  simplifies_to("diff(x^3,x,x,2)", "6×x");
  simplifies_to("diff(x^3,x,x,0)", "x^3");
  simplifies_to("diff(x^3,x,x,-1)", "undef");
  simplifies_to("diff(x^3,x,x,1.3)", "undef");
  simplifies_to("diff(x^3,x,x,n)", "diff(x^3,x,x,n)");

  simplifies_to("diff(cos(x),x,x,3)", "sin(x)");
  simplifies_to("diff(x^5+1,x,x,10)", "0");
  simplifies_to("diff(e^(2x),x,x,8)", "256×e^(2×x)");

  simplifies_to("diff(x*y*y*y*z,y,x,2)", "6×x^2×z");
  simplifies_to("k*x*sum(y*x*k,k,1,2)", "3×k×x^2×y");
  simplifies_to("diff(3×x^2×k×y,x,k,2)", "6×k×y");
  simplifies_to("diff(k*x*sum(y*x*k,k,1,2),x,k,2)", "6×k×y");

  simplifies_to("diff(floor(x), x, y, 1)", "diff(floor(x),x,y)");
  // There should be no dependencies, see Dependency::ContainsSameDependency
  simplifies_to("diff(floor(x)+x, x, y, 2)",
                "dep(diff(floor(x),x,y,2),{floor(y)})");
  simplifies_to("diff(diff(diff(diff(floor(a),a,b,2),b,c),c,d,3),d,x)",
                "dep(diff(floor(a),a,x,7),{diff(floor(a),a,x,2),diff(floor(a),"
                "a,x,3),diff(floor(a),a,x,6)})");
  simplifies_to("diff(diff(floor(a)+b*a,a,x),b,x)",
                "dep(1,{floor(x),diff(floor(a),a,x)})");

  // On points
  simplifies_to("diff((sin(t),cos(t)),t,t,2)", "(-sin(t),-cos(t))");
  simplifies_to("diff((t,2t^2),t,t,3)", "(0,0)");
  simplifies_to("diff((sin(t),floor(t)),t,t,2)",
                "(-sin(t),diff(floor(t),t,t,2))");
}

QUIZ_CASE(pcj_simplification_derivative_for_approximation) {
  ProjectionContext ctxForApprox = {
      .m_reductionTarget = Poincare::ReductionTarget::SystemForApproximation};
  ProjectionContext cartesianCtxForApprox = {
      .m_complexFormat = Poincare::ComplexFormat::Cartesian,
      .m_reductionTarget = Poincare::ReductionTarget::SystemForApproximation};
  projects_and_reduces_to("diff(ln(x),x,1)", "1", ctxForApprox);
  projects_and_reduces_to("diff(ln(x),x,2.2)", "5/11", ctxForApprox);
  projects_and_reduces_to("diff(ln(x),x,0)", "undef", ctxForApprox);
  projects_and_reduces_to("diff(ln(x),x,-3.1)", "undef", ctxForApprox);
  projects_and_reduces_to("diff(log(x),x,-10)", "undef", ctxForApprox);

  projects_and_reduces_to("diff(abs(x),x,123)", "1", ctxForApprox);
  projects_and_reduces_to("diff(abs(x),x,-2.34)", "-1", ctxForApprox);
  projects_and_reduces_to("diff(abs(x),x,0)", "undef", ctxForApprox);

  projects_and_reduces_to("diff(sign(x),x,123)", "0", ctxForApprox);
  projects_and_reduces_to("diff(sign(x),x,-2.34)", "0", ctxForApprox);
  projects_and_reduces_to("diff(sign(x),x,0)", "undef", ctxForApprox);

  projects_and_reduces_to("diff(√(x),x,-1)", "undef", cartesianCtxForApprox);

  projects_and_reduces_to("diff(asin(x),x,1)", "undef", ctxForApprox);
  projects_and_reduces_to("diff(asin(x),x,-1)", "undef", ctxForApprox);
  projects_and_reduces_to("diff(acos(x),x,1)", "undef", ctxForApprox);
  projects_and_reduces_to("diff(acos(x),x,-1)", "undef", ctxForApprox);
  projects_and_reduces_to("diff(arccot(x),x,0)", "-1", ctxForApprox);

  projects_and_reduces_to("diff(1/x,x,-2)", "-1/4", ctxForApprox);
  projects_and_reduces_to("diff(x^3+5*x^2,x,0)", "0", ctxForApprox);
  projects_and_reduces_to("diff(5^(sin(x)),x,3)", "cos(3)×ln(5)×5^sin(3)",
                          ctxForApprox);

  projects_and_reduces_to("diff((-1)^(4-2*2),x,3)", "0", ctxForApprox);

  // With context
  {
    PoincareTest::SymbolStore symbolStore;
    ProjectionContext ctxWithSymbol = {
        .m_reductionTarget = Poincare::ReductionTarget::SystemForApproximation,
        .m_symbolic = Poincare::SymbolicComputation::ReplaceDefinedSymbols,
        .m_context = symbolStore};

    store("0→a", symbolStore);
    projects_and_reduces_to("diff((-1)^(a*x),x,3)", "0", ctxWithSymbol);
  }
}

QUIZ_CASE(pcj_simplification_integral) {
  simplifies_to("int(3*π,x,0,1)", "3×π");
  simplifies_to("int(3a,x,-2,2)", "12×a");
  simplifies_to("int(x+x,x,-1,1)", "2×int(x,x,-1,1)");
  simplifies_to("int(3x^2+x-15,x,-2,1)", "-45+3×int(x^2,x,-2,1)+int(x,x,-2,1)");
  simplifies_to("int(6e^x+sin(x),x,1,3)",
                "int(sin(x),x,1,3)+6×int(e^(x),x,1,3)");
  simplifies_to("int((x-3)(x+1),x,1,2)", "-3+int(x^2,x,1,2)-2×int(x,x,1,2)");
  simplifies_to("int(x*e^x,x,0,0)", "int(e^(x)×x,x,0,0)");
  simplifies_to("diff(int(x^2*t/2,t,0,1),x,y)",
                "dep(y×int(t,t,0,1),{real((y^2×int(t,t,0,1))/2)})");
  simplifies_to("diff(int(x^2/2+t,t,0,1),x,y)",
                "dep(y,{real(y^2/2+int(t,t,0,1))})");
  simplifies_to("int(diff(x^2,x,x),x,0,1)", "2×int(x,x,0,1)");
  simplifies_to("int(tan(x),x,0,x)", "int(tan(x),x,0,x)");
  simplifies_to("int(arccot(x),x,0,x)", "(π×x)/2-int(arctan(x),x,0,x)");
}
