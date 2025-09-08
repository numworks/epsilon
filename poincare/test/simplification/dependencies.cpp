#include <poincare/src/expression/dependency.h>
#include <poincare/src/expression/k_tree.h>

#include "../helpers/symbol_store.h"
#include "helper.h"

using namespace Poincare;
using namespace Poincare::Internal;

QUIZ_CASE(pcj_simplification_dependencies) {
  /* Since a user symbol alone "x" is considered always defined, use "f(x)" to
   * see the dependencies. */
  simplifies_to("f(x)-f(x)", "dep(0,{0×f(x)})");
  simplifies_to("cos(re(f(x)))+inf", "dep(∞,{f(x)})");
  simplifies_to("diff(1+x, x, f(y))", "dep(1,{real(1+f(y)),real(f(y))})");
  simplifies_to("diff(1, x, f(y))", "dep(0,{dep(1,{f(y)}),real(f(y))})");
  simplifies_to("im(re(f(x)))", "dep(0,{f(x)})", cartesianCtx);
  simplifies_to("sign(abs(f(x))+1)", "dep(1,{f(x)})");
  simplifies_to("arg(1+1/abs(x))", "dep(0,{nonNull(abs(x))})");
  simplifies_to("arg(-i/abs(y))", "dep(-π/2,{nonNull(1/abs(y)),-1/abs(y)×i})");
  simplifies_to("0^(5+ln(5))", "0");
  simplifies_to("[[x/x]]", "[[dep(1,{x^0})]]");
  simplifies_to("lcm(undef, 2+x/x)", "undef");
  simplifies_to(
      "{(x, 2), (x/x, 2), (3, undef), (1 , piecewise(x/x + (a + b)^2 - 2*a*b, "
      "x + y/y>2, undef))}",
      "{(x,2),(dep(1,{x^0}),2),(3,undef),(1,dep(piecewise(dep(a^2+b^2+1,{x^0}),"
      "x+1>2,undef),{y^0}))}");
  simplifies_to("{1,undef}", "{1,undef}");
  simplifies_to("[[1,undef]]", "[[1,undef]]");
  simplifies_to("(1,undef)", "(1,undef)");
  // Dependency with non-scalar main-tree
  simplifies_to("sequence(k,k,3)×log(-7)", "{nonreal,nonreal,nonreal}");
  simplifies_to("arcsin(6)×[[1,1]]", "[[nonreal,nonreal]]");
  simplifies_to("sequence(k,k,3)×log(0)", "{undef,undef,undef}");
  simplifies_to_no_beautif("ln(0)×[[1,1]]", "[[undef,undef]]");
  // TODO should be "[[undef]]" or "undef" with beautification
  simplifies_to_no_beautif("[[0]]×[[∞]]×arcsin(6)", "[[nonreal]]");
  simplifies_to_no_beautif("[[0,∞]]×[[0,1][1,1]]×arcsin(6)",
                           "[[nonreal,nonreal]]");
  simplifies_to(
      "sequence(random(),k,10)<arcsin(4)",
      "dep(sequence(undef,k,10),{sequence(random(),k,10)<arcsin(4)})");

  Tree* e1 = KAdd(KDep(KMult(2_e, 3_e), KDepList(0_e)), 4_e)->cloneTree();
  const Tree* r1 = KDep(KAdd(KMult(2_e, 3_e), 4_e), KDepList(0_e));
  Dependency::ShallowBubbleUpDependencies(e1);
  assert_trees_are_equal(e1, r1);

  Tree* e2 =
      KAdd(KDep(KMult(2_e, 3_e), KDepList(0_e)), 4_e, KDep(5_e, KDepList(6_e)))
          ->cloneTree();
  const Tree* r2 = KDep(KAdd(KMult(2_e, 3_e), 4_e, 5_e), KDepList(0_e, 6_e));
  Dependency::ShallowBubbleUpDependencies(e2);
  assert_trees_are_equal(e2, r2);

  ProjectionContext context;
  Tree* e3 = KAdd(2_e, KPow("a"_e, 0_e))->cloneTree();
  const Tree* r3 = KDep(3_e, KDepList(KPow("a"_e, 0_e)));
  simplify(e3, context);
  assert_trees_are_equal(e3, r3);

  Tree* e4 = KDiff("x"_e, "y"_e, 1_e,
                   KDep("x"_e, KDepList(KFun<"f">("x"_e), KFun<"f">("z"_e))))
                 ->cloneTree();
  const Tree* r4 = KDep(1_e, KDepList(KFun<"f">("y"_e), KFun<"f">("z"_e)));
  simplify(e4, context);
  assert_trees_are_equal(e4, r4);

  Tree* e5 =
      KDep(1_e, KDepList(KAdd(KInf, "x"_e, KMult(-1_e, KInf))))->cloneTree();
  simplify(e5, context);
  assert_trees_are_equal(e5, KUndef);

  SharedTreeStack->flush();

  PoincareTest::SymbolStore symbolStore;
  ProjectionContext projCtx = {
      .m_symbolic = SymbolicComputation::ReplaceDefinedSymbols,
      .m_context = symbolStore};

  // No dependencies
  PoincareTest::store("1→a", symbolStore);
  simplifies_to("1+2", "3", projCtx);
  simplifies_to("a", "1", projCtx);
  simplifies_to("a+1", "2", projCtx);
  symbolStore.reset();
  // Function
  PoincareTest::store("x+1→f(x)", symbolStore);
  PoincareTest::store("3→g(x)", symbolStore);
  PoincareTest::store("1+2→a", symbolStore);
  // A variable argument is used as dependency only if needed
  simplifies_to("f(x)", "x+1", projCtx);
  simplifies_to("f(x+y)", "x+y+1", projCtx);
  simplifies_to("g(x)", "3", projCtx);
  simplifies_to("g(x+a+3+4)", "3", projCtx);
  // A constant argument does not create dependencies
  simplifies_to("f(2)", "3", projCtx);
  simplifies_to("g(-1.23)", "3", projCtx);
  // Dependencies makes sure f(undef) = undef for all f
  simplifies_to("f(1/0)", "undef", projCtx);
  simplifies_to("g(1/0)", "undef", projCtx);
  symbolStore.reset();

  // Derivative
  PoincareTest::store("x^2→f(x)", symbolStore);
  simplifies_to("diff(cos(x),x,0)", "0", projCtx);
  simplifies_to("diff(2x,x,y)", "2", projCtx);
  simplifies_to("diff(f(x),x,a)", "2×a", projCtx);
  symbolStore.reset();

  PoincareTest::store("3→f(x)", symbolStore);
  simplifies_to("diff(cos(x)+f(y),x,0)", "0", projCtx);
  simplifies_to("diff(cos(x)+f(y),x,x)", "-sin(x)", projCtx);
  PoincareTest::store("1/0→y", symbolStore);
  simplifies_to("diff(cos(x)+f(y),x,0)", "undef", projCtx);
  symbolStore.reset();

  // Parametric
  /* Dependencies are not bubbled up out of an integral, but they are still
   * present inside the integral. */
  PoincareTest::store("1→f(x)", symbolStore);
  simplifies_to("int(f(x)+f(a),x,0,1)", "2", projCtx);
  PoincareTest::store("1/0→a", symbolStore);
  simplifies_to("int(f(x)+f(a),x,0,1)", "undef", projCtx);
  symbolStore.reset();
  /* If the derivation is not handled properly, the symbol x could be reduced
   * to undef. */
  PoincareTest::store("x→f(x)", symbolStore);
  simplifies_to("int(diff(f(x),x,x),x,0,1)", "1", projCtx);
  symbolStore.reset();
  /* When trimming dependencies, we must be able to recognize unreduced
   * expression that are nevertheless undefined. */
  PoincareTest::store("1→f(x)", symbolStore);
  simplifies_to("f(a)", "1", projCtx);
  // Check that deepIsSymbolic works fine and remove parametered dependencies
  simplifies_to("f(sum(1/n,n,1,5))", "1", projCtx);
  symbolStore.reset();

  // Sequence
  // TODO: add sequence type to the symbol store
  symbolStore.setExpressionForUserNamed(KUndef, KSeq<"u">("n"_e));
  PoincareTest::store("3→f(x)", symbolStore);
  // Sequence are kept in dependency
  simplifies_to("f(u(n))", "dep(3,{u(n)})", projCtx);
  // Except if the sequence can already be approximated.
  simplifies_to("f(u(2))", "undef", projCtx);
  symbolStore.reset();

  // Power
  simplifies_to("1/(1/x)", "dep(x,{nonNull(x)})", cartesianCtx);
  simplifies_to("x/√(x)", "dep(√(x),{-ln(x)/2})", cartesianCtx);
  simplifies_to("(1+x)/(1+x)", "dep(1,{(x+1)^0})", cartesianCtx);
  simplifies_to("x^0", "dep(1,{x^0})", cartesianCtx);

  // Multiplication
  simplifies_to("ln(x)-ln(x)", "dep(0,{0×ln(x),nonNull(x)})", cartesianCtx);
  simplifies_to("0*random()", "0", cartesianCtx);
  simplifies_to("0*randint(1,0)", "dep(0,{randint(1,0)})", cartesianCtx);
  // Dependency is properly reduced even when containing symbols
  simplifies_to("0x^arcsin(π)", "dep(0,{0×x^arcsin(π)})", cartesianCtx);
  simplifies_to("0x^arcsin(π)", "dep(nonreal,{0×x^arcsin(π)})");
}
