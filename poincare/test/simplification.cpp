#include <apps/shared/global_context.h>
#include <poincare/src/expression/advanced_reduction.h>
#include <poincare/src/expression/dependency.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/list.h>
#include <poincare/src/expression/simplification.h>
#include <poincare/src/expression/systematic_reduction.h>
#include <poincare/src/expression/variables.h>
#include <poincare/src/memory/tree_stack.h>

#include "helper.h"

using namespace Poincare::Internal;

constexpr ProjectionContext cartesianCtx = {.m_complexFormat =
                                                ComplexFormat::Cartesian};
constexpr ProjectionContext polarCtx = {.m_complexFormat =
                                            ComplexFormat::Polar};
// Default complex format
constexpr ProjectionContext realCtx = {.m_complexFormat = ComplexFormat::Real};

void deepSystematicReduce_and_operation_to(const Tree* input,
                                           Tree::Operation operation,
                                           const Tree* output) {
  Tree* tree = input->cloneTree();
  // Expand / contract expects a deep systematic reduced tree
  SystematicReduction::DeepReduce(tree);
  quiz_assert(operation(tree));
  assert_trees_are_equal(tree, output);
  tree->removeTree();
}

void expand_to(const Tree* input, const Tree* output) {
  deepSystematicReduce_and_operation_to(input, AdvancedReduction::DeepExpand,
                                        output);
}

void contract_to(const Tree* input, const Tree* output) {
  deepSystematicReduce_and_operation_to(input, AdvancedReduction::DeepContract,
                                        output);
}

QUIZ_CASE(pcj_simplification_expansion) {
  expand_to(KExp(KAdd("x"_e, "y"_e, "z"_e)),
            KMult(KExp("x"_e), KExp("y"_e), KExp("z"_e)));
  expand_to(KTrig(KAdd(π_e, "x"_e, "y"_e), 0_e),
            KDep(KAdd(KMult(-1_e, KTrig("x"_e, 0_e), KTrig("y"_e, 0_e)),
                      KMult(KTrig("x"_e, 1_e), KTrig("y"_e, 1_e))),
                 KDepList(KTrig(KAdd("x"_e, "y"_e), 1_e))));
  expand_to(KExp(KAdd("x"_e, "y"_e, "z"_e)),
            KMult(KExp("x"_e), KExp("y"_e), KExp("z"_e)));
  expand_to(KLn(KMult(2_e, π_e)),
            KDep(KAdd(KLn(2_e), KLn(π_e)), KDepList(i_e)));
  // Algebraic expand
  // A?*(B+C)*D? = A*D*B + A*D*C
  expand_to(KMult("a"_e, KAdd("b"_e, "c"_e, "d"_e), "e"_e),
            KAdd(KMult("a"_e, "b"_e, "e"_e), KMult("a"_e, "c"_e, "e"_e),
                 KMult("a"_e, "d"_e, "e"_e)));
  // (A + B)^2 = (A^2 + 2*A*B + B^2)
  expand_to(KPow(KAdd(KTrig("x"_e, 0_e), KTrig("x"_e, 1_e)), 2_e),
            KAdd(KPow(KTrig("x"_e, 0_e), 2_e),
                 KMult(2_e, KTrig("x"_e, 0_e), KTrig("x"_e, 1_e)),
                 KPow(KTrig("x"_e, 1_e), 2_e)));
  // (A + B + C)^2 = (A^2 + 2*A*B + B^2 + 2*A*C + 2*B*C + C^2)
  expand_to(KPow(KAdd("x"_e, "y"_e, "z"_e), 2_e),
            KAdd(KPow("x"_e, 2_e), KMult(2_e, "x"_e, "y"_e), KPow("y"_e, 2_e),
                 KMult(2_e, "x"_e, "z"_e), KMult(2_e, "y"_e, "z"_e),
                 KPow("z"_e, 2_e)));
  expand_to(KList(KDep(KMult("a"_e, KAdd("b"_e, "c"_e)),
                       KDepList(KMult("a"_e, KAdd("b"_e, "c"_e)))),
                  KMult("a"_e, KAdd("b"_e, "c"_e))),
            KList(KDep(KAdd(KMult("a"_e, "b"_e), KMult("a"_e, "c"_e)),
                       KDepList(KMult("a"_e, KAdd("b"_e, "c"_e)))),
                  KAdd(KMult("a"_e, "b"_e), KMult("a"_e, "c"_e))));
}

QUIZ_CASE(pcj_simplification_contraction) {
  contract_to(KMult(KExp("x"_e), KExp("y"_e), KExp("z"_e)),
              KExp(KAdd("x"_e, "y"_e, "z"_e)));
  contract_to(
      KMult(KTrig("x"_e, 1_e), KTrig("y"_e, 0_e)),
      KMult(1_e / 2_e,
            KAdd(KTrig(KAdd("x"_e, "y"_e), 1_e),
                 KMult(-1_e, KTrig(KAdd(KMult(-1_e, "x"_e), "y"_e), 1_e)))));
  contract_to(
      KMult(KAbs("x"_e), KAbs("y"_e), KExp("x"_e), KExp("y"_e),
            KTrig("x"_e, 1_e), KTrig("y"_e, 0_e)),
      KMult(1_e / 2_e,
            KAdd(KTrig(KAdd("x"_e, "y"_e), 1_e),
                 KMult(-1_e, KTrig(KAdd(KMult(-1_e, "x"_e), "y"_e), 1_e))),
            KAbs(KMult("x"_e, "y"_e)), KExp(KAdd("x"_e, "y"_e))));
  contract_to(KMult(KAbs("a"_e), KAbs(KMult("b"_e, "c"_e)), KAbs("d"_e),
                    KAbs(KMult("e"_e, "f"_e))),

              KAbs(KMult("a"_e, "b"_e, "c"_e, "d"_e, "e"_e, "f"_e)));
  contract_to(
      KAdd("e"_e, "f"_e, KLn(π_e), KLn(KMult(2_e, e_e))),
      KDep(KAdd("e"_e, "f"_e, KLn(KMult(2_e, π_e, e_e))), KDepList(i_e)));
  contract_to(KAdd("b"_e, "c"_e, "d"_e, KPow(KTrig("x"_e, 0_e), 2_e),
                   KPow(KTrig("x"_e, 1_e), 2_e)),
              KDep(KAdd(1_e, "b"_e, "c"_e, "d"_e), KDepList("x"_e)));
  contract_to(KMult(2_e, KTrig(KMult(1_e / 9_e, π_e), 1_e),
                    KTrig(KMult(1_e / 7_e, π_e), 1_e)),
              KAdd(KTrig(KMult(79_e / 63_e, π_e), 0_e),
                   KTrig(KMult(124_e / 63_e, π_e), 0_e)));
  contract_to(KMult(2_e, KTrig(KMult(1_e / 9_e, π_e), 0_e),
                    KTrig(KMult(1_e / 7_e, π_e), 1_e)),
              KAdd(KTrig(KMult(16_e / 63_e, π_e), 1_e),
                   KTrig(KMult(61_e / 63_e, π_e), 1_e)));
}

QUIZ_CASE(pcj_simplification_variables) {
  assert_trees_are_equal(Variables::GetUserSymbols(0_e), KSet());
  const Tree* e = KMult(
      KAdd(KSin("y"_e), KSum("x"_e, 2_e, 4_e, KPow("z"_e, "x"_e))), "m"_e);
  assert_trees_are_equal(Variables::GetUserSymbols(e),
                         KSet("m"_e, "y"_e, "z"_e));
}

QUIZ_CASE(pcj_replace_symbol_with_tree) {
  // replace with u(2) with 4 in 2*u(2)+u(0)+u(n)
  Tree* e1 = KAdd(KMult(2_e, KSeq<"u">(2_e)), KSeq<"u">(0_e), KSeq<"u">("n"_e))
                 ->cloneTree();
  Variables::ReplaceSymbolWithTree(e1, KSeq<"u">(2_e), 4_e);
  assert_trees_are_equal(
      e1, KAdd(KMult(2_e, 4_e), KSeq<"u">(0_e), KSeq<"u">("n"_e)));
  e1->removeTree();

  // replace with f(0) with 0 in f(f(0)) gives 0
  Tree* e2 = KFun<"f">(KFun<"f">(0_e))->cloneTree();
  Variables::ReplaceSymbolWithTree(e2, KFun<"f">(0_e), 0_e);
  assert_trees_are_equal(e2, 0_e);
  e2->removeTree();

  // replace with u(n) with 2n+3 in 4*u(5*n)
  Tree* e3 = KMult(4_e, KSeq<"u">(KMult(5_e, "n"_e)))->cloneTree();
  Variables::ReplaceSymbolWithTree(e3, KSeq<"u">("n"_e),
                                   KAdd(KMult(2_e, KUnknownSymbol), 3_e));
  assert_trees_are_equal(e3,
                         KMult(4_e, KAdd(KMult(2_e, KMult(5_e, "n"_e)), 3_e)));
  e3->removeTree();
}

void simplifies_to(const char* input, const char* output,
                   ProjectionContext projectionContext = realCtx) {
  process_tree_and_compare(
      input, output,
      [](Tree* tree, ProjectionContext projectionContext) {
        Simplification::SimplifyWithAdaptiveStrategy(tree, &projectionContext);
      },
      projectionContext);
}

QUIZ_CASE(pcj_simplification_basic) {
  simplifies_to("x", "x");
  simplifies_to("x-x", "dep(0,{x})");
  simplifies_to("2+2", "4");
  simplifies_to("(2×3(2^2)) + 2×2", "28");
  simplifies_to("36/8", "9/2");
  simplifies_to("2+36/8+2", "17/2");
  simplifies_to("a+a", "2×a");
  simplifies_to("b+a", "a+b");
  simplifies_to("(a×a)×a", "a^3");
  simplifies_to("a×(a×a)", "a^3");
  simplifies_to("(a×b)^2", "a^2×b^2");
  simplifies_to("(a×b×c)^2", "a^2×b^2×c^2");
  simplifies_to("(x^3)^2", "x^6");
  simplifies_to("a×a×a", "a^3");
  simplifies_to("a×a×a×b", "b×a^3");
  simplifies_to("a×2a×b×a×b×4", "8×a^3×b^2");
  simplifies_to("1×1×1×1", "1");
  simplifies_to("2a+3b+4a", "6×a+3×b");
  simplifies_to("-6×b-4×a×b-2×b+3×a×b-4×b+2×a×b+3×b+6×a×b", "(7×a-9)×b");
  simplifies_to("d+c+b+a", "a+b+c+d");
  simplifies_to("(a+b)×(d+f)×g-a×d×g-a×f×g", "dep(b×(d+f)×g,{a})");
  simplifies_to("a*x*y+b*x*y+c*x", "x×(c+(a+b)×y)");
  simplifies_to("(e^(x))^2", "e^(2×x)");
  simplifies_to("e^(ln(x))", "dep(x,{NonNull(x),RealPositive(x)})");
  simplifies_to("e^(ln(1+x^2))",
                "dep(x^2+1,{NonNull(1+x^2),RealPositive(1+x^2)})");
  simplifies_to("e^(ln(x))", "dep(x,{NonNull(x)})", cartesianCtx);
  simplifies_to("e^(ln(x+x))", "dep(2×x,{NonNull(x+x)})", cartesianCtx);
  simplifies_to("x+1+(-1)(x+1)", "dep(0,{x})");
  simplifies_to("0.1875", "3/16");
  simplifies_to("0.0001234", "617/5000000");
  simplifies_to("98765000", "98765000");
  simplifies_to("012345.67890", "123456789/10000");
  simplifies_to("012345.67890ᴇ5", "1234567890");
  simplifies_to("012345.67890ᴇ-3", "123456789/10000000");
  simplifies_to("123456789012345678901234567890",
                "123456789012345678901234567890");
  simplifies_to("1ᴇ10", "10000000000");
  simplifies_to("5.0", "5");
  simplifies_to("5.", "5");
  simplifies_to("5.ᴇ1", "50");
  simplifies_to("undef", "undef");
  // TODO: Simplify to 1/√(1+x^2).
  simplifies_to("√(-x^2/√(x^2+1)^2+1)", "√(-x^2/(x^2+1)+1)");
  simplifies_to("x×(-x^2+1)^(-1/2)", "x/√(-x^2+1)");
  // TODO: Simplify to x
  simplifies_to("(x×(-x^2/√(x^2+1)^2+1)^(-1/2))/√(x^2+1)",
                "x/(√(x^2+1)×√(-x^2/(x^2+1)+1))");
  simplifies_to("(a+b)/2+(a+b)/2", "a+b");
  simplifies_to("(a+b+c)*3/4+(a+b+c)*1/4", "a+b+c");
  // Sort order
  simplifies_to("π*atan(π)/π", "atan(π)");
  simplifies_to("π+atan(π)-π", "atan(π)");
  simplifies_to("π*(-π)/π", "-π");
  simplifies_to("π+1/π-π", "1/π");

  // Abs
  simplifies_to("abs(0)", "0");
  simplifies_to("abs(3)", "3");
  simplifies_to("abs(-3)", "3");
  simplifies_to("abs(3i)", "3");
  simplifies_to("abs(-3i)", "3");
  // TODO: Metric: 3×abs(x)
  simplifies_to("abs(abs(abs((-3)×x)))", "abs(-3×x)");
  simplifies_to("abs(-2i)+abs(2i)+abs(2)+abs(-2)", "8", cartesianCtx);
  simplifies_to("abs(x^2)", "x^2");
  // TODO: Remove ln(c^2) dependency
  simplifies_to("abs(a)*abs(b*c)-abs(a*b)*abs(c)", "dep(0,{a,b,ln(c^2)})");
  simplifies_to("((abs(x)^(1/2))^(1/2))^8", "x^2");
  simplifies_to("(2+x)*(2-x)+(x+1)*(x-1)", "dep(3,{x})");
}

QUIZ_CASE(pcj_simplification_derivative) {
  simplifies_to("diff(x, x, 2)", "1");
  simplifies_to("diff(a×x, x, 1)", "a");
  simplifies_to("diff(23, x, 1)", "0");
  simplifies_to("diff(1+x, x, y)", "dep(1,{y})");
  simplifies_to("diff(sin(ln(x)), x, y)",
                "dep(cos(ln(y))/y,{RealPositive(y)})");
  simplifies_to(
      "diff(((x^4)×ln(x)×e^(3x)), x, y)",
      "dep((3×ln(y)×y^4+(1+4×ln(y))×y^3)×e^(3×y),{arg(y),arg(y^4),NonNull("
      "y),RealPositive(y)})");
  simplifies_to("diff(diff(x^2, x, x)^2, x, y)", "8×y");
  simplifies_to("diff(x+x*floor(x), x, y)", "y×diff(floor(x),x,y)+1+floor(y)");
  /* TODO: Should be unreal but returns undef because dependency lnReal(-1)
   * approximates to undef and not nonreal. */
  simplifies_to("diff(ln(x), x, -1)", "undef");
  simplifies_to("diff(x^3,x,x,2)", "6×x");
  simplifies_to("diff(x*y*y*y*z,y,x,2)", "6×z×x^2");

  simplifies_to("k*x*sum(y*x*k,k,1,2)", "3×x^2×k×y");
  simplifies_to("diff(3×x^2×k×y,x,k,2)", "6×k×y");
  simplifies_to("diff(k*x*sum(y*x*k,k,1,2),x,k,2)", "6×k×y");
  simplifies_to("diff((x^2, floor(x)),x,k)", "(2×k,diff(floor(x),x,k))");
  simplifies_to("diff(floor(x), x, y, 1)", "diff(floor(x),x,y)");
  // There should be no dependencies, see Dependency::ContainsSameDependency
  simplifies_to("diff(floor(x)+x, x, y, 2)",
                "dep(diff(floor(x),x,y,2),{floor(y)})");
  simplifies_to("diff((sin(t),cos(t)),t,t,2)", "(-sin(t),-cos(t))");
  simplifies_to("diff((sin(t),floor(t)),t,t,2)",
                "(-sin(t),diff(floor(t),t,t,2))");
  simplifies_to("diff(diff(diff(diff(floor(a),a,b,2),b,c),c,d,3),d,x)",
                "dep(diff(floor(a),a,x,7),{diff(floor(a),a,x,2),diff(floor(a),"
                "a,x,3),diff(floor(a),a,x,6)})");
  simplifies_to("diff(diff(floor(a)+b*a,a,x),b,x)",
                "dep(1+diff(diff(floor(a),a,x),b,x),{floor(x)})");
}

QUIZ_CASE(pcj_simplification_matrix) {
  simplifies_to("[[1+2]]", "[[3]]");
  simplifies_to("trace([[1,2][3,4]])", "5");
  simplifies_to("identity(2)", "[[1,0][0,1]]");
  simplifies_to("transpose([[1][3]])", "[[1,3]]");
  simplifies_to("transpose([[1,2][3,4]])", "[[1,3][2,4]]");
  simplifies_to("dim([[1,2][3,4][5,6]])", "[[3,2]]");
  simplifies_to("ref([[1,2][3,4]])", "[[1,4/3][0,1]]");
  simplifies_to("rref([[1,2][3,4]])", "[[1,0][0,1]]");
  simplifies_to("ref([[0,2,-1][5,6,7][12,11,10]])",
                "[[1,11/12,5/6][0,1,-1/2][0,0,1]]");
  simplifies_to("det([[0,2,-1][5,6,7][12,11,10]])", "85");
  simplifies_to("det([[1,2][3,4]])", "-2");
  simplifies_to("inverse([[1,2][3,4]])", "[[-2,1][3/2,-1/2]]");
  simplifies_to("[[1,2][3,4]]^5", "[[1069,1558][2337,3406]]");
  simplifies_to("[[1,2][3,4]]^-1", "[[-2,1][3/2,-1/2]]");
  simplifies_to("[[1,2][3,4]]^0 - identity(2)", "[[0,0][0,0]]");
  simplifies_to("trace(identity(3))", "3");
  simplifies_to("2+[[3]]", "undef");
  simplifies_to("[[2]]+[[3]]", "[[5]]");
  simplifies_to("2×[[3]]", "[[6]]");
  simplifies_to("[[3]]×i", "[[3×i]]");
  simplifies_to("[[1,2][3,4]]×[[2,3][4,5]]", "[[10,13][22,29]]");
  simplifies_to("norm([[2,3,6]])", "7");
  simplifies_to("dot([[1,2,3]],[[4,5,6]])", "32");
  simplifies_to("cross([[1,2,3]],[[4,5,6]])", "[[-3,6,-3]]");
  simplifies_to("trace([[√(2)+log(3)]])", "√(2)+ln(3)/ln(10)");
  simplifies_to("transpose([[√(4)]])", "[[2]]");

  simplifies_to("0*[[2][4]]×[[1,2]]", "[[0,0][0,0]]");
  simplifies_to("0*[[2][inf]]", "[[0][undef]]");
  simplifies_to("0*inf*[[2][3]]", "[[undef][undef]]");
  simplifies_to("undef*[[2][4]]", "[[undef][undef]]");
  simplifies_to("[[1/0][4]]", "[[undef][4]]");
}

QUIZ_CASE(pcj_simplification_complex) {
  Shared::GlobalContext globalContext;
  ProjectionContext ctx = {
      .m_complexFormat = ComplexFormat::Cartesian,
      .m_symbolic = SymbolicComputation::DoNotReplaceAnySymbol,
      .m_context = &globalContext,
  };
  simplifies_to("2×i×i", "-2", ctx);
  simplifies_to("1+i×(1+i×(1+i))", "0", ctx);
  simplifies_to("(1+i)×(3+2i)", "1+5×i", ctx);
  simplifies_to("√(-1)", "i", ctx);
  simplifies_to("re(2+i×π)", "2", ctx);
  simplifies_to("im(2+i×π)", "π", ctx);
  simplifies_to("conj(2+i×π)", "2-π×i", ctx);
  simplifies_to("i×im(x)+re(x)", "x");
  simplifies_to("re(ln(1-3×i)+2×π×i)", "re(ln(1-3×i))", ctx);
  // z being an unknown real, arccos(z) is an unknown complex.
  simplifies_to("im(x+i*y+arccos(z))", "dep(y+im(arccos(z)),{x})", ctx);
  simplifies_to("im(x)", "dep(0,{x})", ctx);
  simplifies_to("im(i*y)", "y", ctx);
  simplifies_to("im(arccos(z))", "im(arccos(z))", ctx);
  simplifies_to("im(x1+x2+i*y1+i*y2+arccos(z1)+arccos(z2))",
                "dep(y1+y2+im(arccos(z1))+im(arccos(z2)),{x1,x2})", ctx);

  store("x→f(x)", &globalContext);
  simplifies_to("i×im(f(x))+re(f(x))", "f(x)", ctx);
  simplifies_to("re(conj(f(x)))-re(f(x))", "dep(0,{f(x)})", ctx);
  simplifies_to("conj(conj(f(x)))", "f(x)", ctx);
  simplifies_to("re(f(x)+y)-y", "dep(re(f(x)),{y})", ctx);
  simplifies_to("re(i×f(y))+im(f(y))", "dep(0,{f(y)})", ctx);
  simplifies_to("im(i×f(y))", "re(f(y))", ctx);
  simplifies_to("i×(conj(f(x)+i×f(y))+im(f(y))-re(f(x)))", "im(f(x))+re(f(y))",
                ctx);
  simplifies_to("im(re(f(x))+i×im(f(x)))", "im(f(x))", ctx);
  simplifies_to("re(re(f(x))+i×im(f(x)))", "re(f(x))", ctx);
  // TODO: Should simplify to 0
  simplifies_to(
      "abs(f(x)+i×f(y))^2-(-im(f(y))+re(f(x)))^2-(im(f(x))+re(f(y)))^2",
      "abs(f(x)+f(y)×i)^2-((-im(f(y))+re(f(x)))^2+(im(f(x))+re(f(y)))^2)", ctx);
  simplifies_to("arg(x+y×i)", "arg(x+y×i)", ctx);
  simplifies_to("arg(π+i×2)", "arctan(2/π)", ctx);
  simplifies_to("arg(-π+i×2)", "π+arctan(-2/π)", ctx);
  simplifies_to("arg(i×2)", "π/2", ctx);
  simplifies_to("arg(-i×2)", "-π/2", ctx);
  simplifies_to("arg(0)", "undef", ctx);
  simplifies_to("arg(-π+i×abs(y))", "π-arctan(abs(y)/π)", ctx);
  simplifies_to("abs(f(x)^2)", "abs(f(x)^2)", ctx);
}

QUIZ_CASE(pcj_simplification_polar) {
  simplifies_to("0", "0", polarCtx);
  simplifies_to("1", "1", polarCtx);
  simplifies_to("-1", "e^(π×i)", polarCtx);
  simplifies_to("2i", "2×e^(π/2×i)", polarCtx);
  simplifies_to("cos(i)", "cos(i)", polarCtx);
  simplifies_to("[[42, -2/3][1+i, -iπ]]",
                "[[42,(2×e^(π×i))/3][1+i,π×e^((-π/2)×i)]]", polarCtx);
  simplifies_to("-2×_m", "-2×_m", polarCtx);
  simplifies_to("(-2,i)", "(-2,i)", polarCtx);
  simplifies_to("{-2,-i}", "{2×e^(π×i),e^((-π/2)×i)}", polarCtx);
  // TODO : Better simplify abs and arg
  simplifies_to("(y/y+3)×e^(i×(x-x+2))",
                "dep(abs(4×e^(2×i))×e^(arg(4×e^(2×i))×i),{x,y^0})", polarCtx);
}

QUIZ_CASE(pcj_simplification_parametric) {
  // Leave and enter with a nested parametric
  const Tree* a = KSum("k"_e, 0_e, 2_e, KMult(KVarK, KVar<2, 0>));
  const Tree* b = KSum("k"_e, 0_e, 2_e, KMult(KVarK, KVar<1, 0>));
  Tree* e = a->cloneTree();
  Variables::LeaveScope(e);
  assert_trees_are_equal(e, b);
  Variables::EnterScope(e);
  assert_trees_are_equal(e, a);
  e->removeTree();

  // sum
  simplifies_to("sum(n,k,1,n)", "n^2");
  simplifies_to("sum(a*b,k,1,n)", "a×b×n");
  simplifies_to("sum(k,k,n,j)", "(j^2-n^2+j+n)/2");
  simplifies_to("2×sum(k,k,0,n)+n", "n×(n+2)");
  simplifies_to("2×sum(k,k,3,n)+n", "n^2+2×n-6");
  simplifies_to("sum(k^2,k,n,j)", "(j×(j+1)×(2×j+1)-n×(n-1)×(2×n-1))/6");
  simplifies_to("sum(k^2,k,2,5)", "54");
  simplifies_to("sum((2k)^2,k,2,5)", "216");
  simplifies_to("sum((2k)^2,k,0,n)", "(2×n×(n+1)×(2×n+1))/3");
  simplifies_to("sum((2k)^4,k,0,n)", "16×sum(k^4,k,0,n)");
  simplifies_to("sum(k*cos(k),k,1,n)", "sum(cos(k)×k,k,1,n)");
  simplifies_to("sum(sum(x*j,j,1,n),k,1,2)", "n×(n+1)×x");
  simplifies_to("sum(sum(a*k,a,0,j),k,1,n)", "(j×(j+1)×n×(n+1))/4");
  simplifies_to("sum(π^k,k,4,2)", "0");
  simplifies_to("sum(sin(k),k,a+10,a)", "0");
  simplifies_to("sum(sin(k),k,a,a-10)", "0");
  simplifies_to("sum(random()*k,k,0,n)", "sum(random()×k,k,0,n)");
  simplifies_to("sum(random(),k,0,10)", "sum(random(),k,0,10)");

  // product
  simplifies_to("product(p,k,j,n)", "p^(-j+n+1)");
  simplifies_to("product(p^3,k,j,n)", "p^3^(-j+n+1)");
  simplifies_to("product(k^3,k,j,n)", "product(k,k,j,n)^3");
  simplifies_to("product(k^x,k,j,n)", "product(k^x,k,j,n)");
  simplifies_to("product(x^k,k,j,n)", "product(x^k,k,j,n)");
  simplifies_to("product(π^k,k,2,1)", "1");
  simplifies_to("product(sin(k),k,a+10,a)", "1");
  simplifies_to("product(sin(k),k,a,a-10)", "1");
  simplifies_to("product(random()*k,k,0,n)", "product(random()×k,k,0,n)");
  simplifies_to("product(random(),k,0,10)", "product(random(),k,0,10)");

  // product(exp) <-> exp(sum)
  // TODO_PCJ: we should have b×product(k,k,a,b)^2 not product(k,k,a,b)^2×b
  simplifies_to(
      "exp(2*sum(ln(k),k,a,b) + ln(b))",
      "dep(product(k,k,a,b)^2×b,{sum(NonNull(k),k,a,b),sum(RealPositive(k),k,a,"
      "b),NonNull(b),RealPositive(b)})");
  simplifies_to("product(exp(2k),k,0,y)", "e^(y^2+y)");

  // expand sum
  simplifies_to("sum(k^3+4,k,n,n+3)-16", "sum(k^3,k,n,n+3)");
  simplifies_to("sum(x*k!,k,1,2)", "3*x");
  simplifies_to("sum(tan(k),k,a,a)", "tan(a)");

  // expand product
  simplifies_to("product(4*cos(k),k,n,n+3)/256", "product(cos(k),k,n,n+3)");
  simplifies_to("product(cos(k),k,2,4)-cos(2)×cos(3)×cos(4)", "0");
  simplifies_to("product(sin(k),k,a+1,a+1)", "sin(a+1)");

  Shared::GlobalContext globalContext;
  store("x→f(x)", &globalContext);
  ProjectionContext ctx = {
      .m_symbolic = SymbolicComputation::DoNotReplaceAnySymbol,
      .m_context = &globalContext,
  };

  // contract sum
  simplifies_to("sum(f(k),k,a,b)-sum(f(u),u,a,b)", "0", ctx);
  simplifies_to("sum(f(k),k,a,b+n)-sum(f(u),u,a,b)",
                "sum(f(k),k,a,b+n)-sum(f(u),u,a,b)", ctx);
  simplifies_to("sum(f(k),k,a,b+10)-sum(f(u),u,a,b)", "sum(f(k),k,b+1,b+10)",
                ctx);
  simplifies_to("sum(f(k),k,a,100)-sum(f(u),u,a,5)", "sum(f(k),k,6,100)", ctx);
  simplifies_to("sum(f(k),k,a,b)-sum(f(u),u,a,b+n)",
                "sum(f(k),k,a,b)-sum(f(u),u,a,b+n)", ctx);
  simplifies_to("sum(f(k),k,a,b)-sum(f(u),u,a,b+10)", "- sum(f(u),u,b+1,b+10)",
                ctx);
  simplifies_to("sum(f(k),k,a,5)-sum(f(u),u,a,100)", "- sum(f(u),u,6,100)",
                ctx);
  simplifies_to("sum(f(k),k,a+10,b)-sum(f(u),u,a,b)", "- sum(f(u),u,a,a+9)",
                ctx);
  simplifies_to("sum(f(k),k,100,b)-sum(f(u),u,5,b)", "- sum(f(u),u,5,99)", ctx);
  simplifies_to("sum(f(k),k,a,b)-sum(f(u),u,a+n,b)",
                "sum(f(k),k,a,b)-sum(f(u),u,a+n,b)", ctx);
  simplifies_to("sum(f(k),k,a,b)-sum(f(u),u,a+10,b)", "sum(f(k),k,a,a+9)", ctx);
  simplifies_to("sum(f(k),k,5,b)-sum(f(u),u,100,b)", "sum(f(k),k,5,99)", ctx);

  // contract product
  simplifies_to("product(f(k),k,a,b) / product(f(u),u,a,b)", "1", ctx);
  simplifies_to("product(f(k),k,a,b+n) / product(f(u),u,a,b)",
                "product(f(k),k,a,b+n) / product(f(u),u,a,b)", ctx);
  simplifies_to("product(f(k),k,a,b+10) / product(f(u),u,a,b)",
                "product(f(k),k,b+1,b+10)", ctx);
  simplifies_to("product(f(k),k,a,100) / product(f(u),u,a,5)",
                "product(f(k),k,6,100)", ctx);
  simplifies_to("product(f(k),k,a,b) / product(f(u),u,a,b+n)",
                "product(f(k),k,a,b) / product(f(u),u,a,b+n)", ctx);
  simplifies_to("product(f(k),k,a,b) / product(f(u),u,a,b+10)",
                "1 / product(f(u),u,b+1,b+10)", ctx);
  simplifies_to("product(f(k),k,a,5) / product(f(u),u,a,100)",
                "1 / product(f(u),u,6,100)", ctx);
  simplifies_to("product(f(k),k,a+10,b) / product(f(u),u,a,b)",
                "1 / product(f(u),u,a,a+9)", ctx);
  simplifies_to("product(f(k),k,100,b) / product(f(u),u,5,b)",
                "1 / product(f(u),u,5,99)", ctx);
  simplifies_to("product(f(k),k,a,b) / product(f(u),u,a+n,b)",
                "product(f(k),k,a,b) / product(f(u),u,a+n,b)", ctx);
  simplifies_to("product(f(k),k,a,b) / product(f(u),u,a+10,b)",
                "product(f(k),k,a,a+9)", ctx);
  simplifies_to("product(f(k),k,5,b) / product(f(u),u,100,b)",
                "product(f(k),k,5,99)", ctx);
  Ion::Storage::FileSystem::sharedFileSystem->destroyAllRecords();
}

QUIZ_CASE(pcj_simplification_factorial) {
  simplifies_to("0!", "1");
  simplifies_to("4!", "24");
  simplifies_to("(-4)!", "undef");
  simplifies_to("(3/5)!", "undef");
  // TODO_PCJ: use ComplexSign to set undef this factorial
  simplifies_to("i!", "i!");
  simplifies_to("π!", "undef");
  simplifies_to("n!", "n!");
  // simplifies_to("(n+1)!/n!", "n+1"); TODO through parametric
}

QUIZ_CASE(pcj_simplification_hyperbolic_trigonometry) {
  simplifies_to("cosh(-x)+sinh(x)", "e^(x)");
  // TODO : Should simplify to 1
  simplifies_to("cosh(x)^2-sinh(-x)^2",
                "(e^(x)+e^(-x))^2/4+1/2-(e^(-2×x)+e^(2×x))/4");
  // TODO: Should simplify to 0
  simplifies_to("((1+tanh(x)^2)*tanh(2x)/2)-tanh(x)",
                "-(-1+e^(2×x))/(1+e^(2×x))+((1+(-1+e^(2×x))^2/"
                "(1+e^(2×x))^2)×(-1+e^(4×x)))/(2×(1+e^(4×x)))");
  simplifies_to("arcosh(5)", "ln(5+√(24))", cartesianCtx);
  // TODO: Should simplify to x
  simplifies_to("arsinh(sinh(x))",
                "ln(e^(x)/2-e^(-x)/2+√(1/2+(e^(-2×x)+e^(2×x))/4))",
                cartesianCtx);
  // TODO: Should simplify to x
  simplifies_to(
      "artanh(tanh(x))",
      "(-ln(1-(-1+e^(2×x))/(1+e^(2×x)))+ln(1+(-1+e^(2×x))/(1+e^(2×x))))/2",
      cartesianCtx);
  // TODO: Should simplify to x
  simplifies_to("cosh(arcosh(x))",
                "(x+e^((ln(x-1)+ln(x+1))/2)+1/(x+e^((ln(x-1)+ln(x+1))/2)))/2",
                cartesianCtx);
  // TODO: Should simplify to x
  simplifies_to("sinh(arsinh(x))", "(x+√(x^2+1)-1/(x+√(x^2+1)))/2",
                cartesianCtx);
  // TODO: Should simplify to x
  simplifies_to("tanh(artanh(x))", "(-1+(x+1)/(-x+1))/(1+(x+1)/(-x+1))",
                cartesianCtx);
}

QUIZ_CASE(pcj_simplification_advanced_trigonometry) {
  simplifies_to("cot(π/2)", "0");
  simplifies_to("cot(90)", "0", {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("arccot(0)", "90", {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("sec(x)", "1/cos(x)");
  simplifies_to("csc(x)", "1/sin(x)");
  simplifies_to("cot(x)", "cot(x)");
  simplifies_to("arcsec(sec(π/6))", "π/6");
  simplifies_to("arccsc(csc(π/6))", "π/6");
  simplifies_to("arccot(cot(π/6))", "π/6");
  simplifies_to("arccot(-1)", "-π/4");

  simplifies_to("csc(arccsc(9/7))", "9/7");
  simplifies_to("csc(arccsc(3/7))", "undef");
  simplifies_to("csc(arccsc(3/7))", "3/7", cartesianCtx);
  simplifies_to("sec(arcsec(9/7))", "9/7");
  simplifies_to("sec(arcsec(3/7))", "undef");

  // TODO_PCJ: This return undef because one of the piecewise branch is undef
  // simplifies_to("arccot(0)", "π/2");
  simplifies_to("sec(arcsec(x))", "dep(x,{NonNull(x)})", cartesianCtx);
  simplifies_to("csc(arccsc(x))", "dep(x,{NonNull(x)})", cartesianCtx);
  // TODO: Should simplify to x
  simplifies_to("cot(arccot(1+abs(x)))", "cot(arctan(1/(1+abs(x))))",
                cartesianCtx);

  simplifies_to("sin(x)*(cos(x)^-1)*ln(x)",
                "dep(tan(x)×ln(x),{NonNull(x),RealPositive(x)})");
  simplifies_to("ln(x)*tan(x)",
                "dep(tan(x)×ln(x),{NonNull(x),RealPositive(x)})");
  simplifies_to("sin(x)*(cos(y)^-1)*(cos(x)^-1)*sin(y)", "tan(x)×tan(y)");
}

QUIZ_CASE(pcj_simplification_arithmetic) {
  simplifies_to("factor(0)", "0");
  simplifies_to("factor(1)", "1");
  simplifies_to("factor(23)", "23");
  simplifies_to("factor(42*3)", "2×3^2×7");
  simplifies_to("factor(-12)", "-2^2×3");
  simplifies_to("factor(-4/17)", "-2^2/17");
  simplifies_to("factor(2π)", "undef");

  simplifies_to("quo(23,5)", "4");
  simplifies_to("rem(23,5)", "3");
  simplifies_to("gcd(14,28,21)", "7");
  simplifies_to("lcm(14,6)", "42");
  simplifies_to("gcd(6,y,2,x,4)", "gcd(2,x,y)");
  simplifies_to("sign(-2)", "-1");
  simplifies_to("ceil(8/3)", "3");
  simplifies_to("frac(8/3)", "2/3");
  simplifies_to("floor(8/3)", "2");
  simplifies_to("round(1/3,2)", "33/100");
  simplifies_to("round(3.3_m)", "undef");
  // simplifies_to("ceil(x)", "ceil(x)"); // pb metric
  simplifies_to("ceil(-x)", "-floor(x)");
  simplifies_to("floor(x)+frac(x)", "dep(x,{floor(x)})");
  simplifies_to("permute(4,2)", "12");
  simplifies_to("binomial(4,2)", "6");
  simplifies_to("1 2/3", "5/3");
  simplifies_to("-1 2/3", "-5/3");

  simplifies_to("floor({1.3,3.9})", "{1,3}");
  // TODO: Approximation is undef
  simplifies_to("ceil(1+i)", "ceil(1+i)");
  // Reductions using approximation
  simplifies_to("floor(π)", "3");
  simplifies_to("frac(π+1)+floor(π+0.1)", "π");
  simplifies_to("log(ceil(2^15+π)-4,2)", "15");
  simplifies_to("frac(2^24+π)-π", "16777216-floor(16777216+π)");
  simplifies_to("log(floor(2^54+π)-3, 2)",
                "ln(-3+floor(18014398509481984+π))/ln(2)");
}

QUIZ_CASE(pcj_simplification_percent) {
  // % are left unreduced on purpose to show their exact formula
  simplifies_to("-25%", "-25/100");
  simplifies_to("2↗30%", "2×(1+30/100)");
  simplifies_to("-2-30%", "(-2)×(1-30/100)");
  simplifies_to("x-30%", "x×(1-30/100)",
                {.m_strategy = Strategy::ApproximateToFloat});
}

QUIZ_CASE(pcj_simplification_list_bubble_up) {
  // Bubble-up with no reduction to test the bubble-up itself
  Tree* l1 = KPow(2_e, KList(3_e, 4_e))->cloneTree();
  List::BubbleUp(l1, [](Tree*) { return false; });
  assert_trees_are_equal(l1, KList(KPow(2_e, 3_e), KPow(2_e, 4_e)));
  l1->removeTree();

  Tree* l2 =
      KMult(2_e, KAdd(3_e, KList(5_e, 6_e)), KList(7_e, 8_e))->cloneTree();
  List::BubbleUp(l2, [](Tree*) { return false; });
  assert_trees_are_equal(l2, KList(KMult(2_e, KAdd(3_e, 5_e), 7_e),
                                   KMult(2_e, KAdd(3_e, 6_e), 8_e)));
  l2->removeTree();
}

QUIZ_CASE(pcj_simplification_list) {
  simplifies_to("{1,2}+3", "{4,5}");
  simplifies_to("{1,2}*{3,4}", "{3,8}");
  simplifies_to("sequence(2*k, k, 3)+1", "{3,5,7}");
  simplifies_to("mean({1,3*x,2})", "x+1");
  simplifies_to("sum({1,3*x,2})", "3×(x+1)");
  simplifies_to("min({1,-4/7,2,-2})", "-2");
  simplifies_to("var(sequence(k,k,6))", "35/12");
  simplifies_to("sort({2+1,1,2})", "{1,2,3}");
  simplifies_to("med(π*{1,2,3},{4,1,1})", "π");
  simplifies_to("med(π*{4,2,1,3})", "(5×π)/2");
  simplifies_to("dim(1+sequence(k,k,3))", "3");
  simplifies_to("mean({1,2}, {2,1})", "4/3");
  simplifies_to("dim({{1,2}})", "undef");
  simplifies_to("{2*[[1]]}", "undef");
  simplifies_to("{(1)/2, 1}", "{1/2, 1}");
  simplifies_to("prod({1,1,1})", "1");
  simplifies_to("sort({})", "{}");
  simplifies_to("prod({})", "1");
  simplifies_to("{dim({2,4})}", "{2}");
  simplifies_to("min({})", "undef");
  simplifies_to("sequence(k,k,{1,2})", "undef");
  simplifies_to("{3,4}(2)", "4");
  simplifies_to("{3,4}(1,2)", "{3,4}");
  simplifies_to("{3,4}(0,5)", "{3,4}");
  simplifies_to("0*{3,4}", "{0,0}");
  simplifies_to("0*{3,inf}", "{0,undef}");
  simplifies_to("0*inf*{3,4}", "{undef,undef}");
  simplifies_to("{1,2}/{1,0}", "{1,undef}");
  simplifies_to("med(π*{undef,nonreal,x,3})", "undef");
  simplifies_to("sort(randintnorep(1,4,4))", "sort(randintnorep(1,4,4))");
  simplifies_to("med(sequence(random(),k,10))", "med(sequence(random(),k,10))");

  simplifies_to("{3,4}(0)", "undef");
  simplifies_to("{3,4}(4)", "undef");
  simplifies_to("{3,4}(-1)", "undef");
  simplifies_to("{3,4}(1/3)", "undef");
  simplifies_to("{3,4}([[2]])", "undef");
  simplifies_to("{3,4}(true)", "undef");
  simplifies_to("{3,4}(-1,0)", "undef");
  simplifies_to("{3,4}(5,6)", "{}");
  simplifies_to("{3,4}(2,1)", "{}");
  simplifies_to("{3,4}(2,2/3)", "undef");
  simplifies_to("{3,4}(2,true)", "undef");
  simplifies_to("{3,4}(2,[[2]])", "undef");
}

QUIZ_CASE(pcj_simplification_random) {
  // TODO: Handle them with {.m_strategy = Strategy::ApproximateToFloat}
  simplifies_to("randintnorep(1,10,5)", "randintnorep(1,10,5)");
  simplifies_to("random()", "random()");
  simplifies_to("randint(1,10)", "randint(1,10)");
  simplifies_to("diff(random()+1,x,2)", "undef");
  simplifies_to("sum(k+randint(1,10),k,2,5)-14", "sum(randint(1,10),k,2,5)");
  simplifies_to("sequence(2*k+random(),k,3)+1", "1+sequence(2×k+random(),k,3)");
}

QUIZ_CASE(pcj_simplification_power) {
  simplifies_to("1/a", "1/a");
  simplifies_to("1/(1/a)", "dep(a,{NonNull(a)})");
  simplifies_to("1/(a^-3)", "dep(a^3,{NonNull(a)})");
  simplifies_to("a×a^(-1)", "dep(1,{a^0})");
  simplifies_to("a×a^(1+1)", "a^3");
  simplifies_to("2×a^1×(2a)^(-1)", "dep(1,{a^0})");
  simplifies_to("cos(π×a×a^(-1))^(b×b^(-2)×b)", "dep(-1,{a^0,b^0})");
  simplifies_to("2^(64)", "18446744073709551616");
  simplifies_to("2^(64)/2^(63)", "2");
  simplifies_to("0^3.1", "0");
  simplifies_to("0^(-4.2)", "undef");
  simplifies_to("0^(1+x^2)", "0");
  simplifies_to("sqrt(9)", "3");
  simplifies_to("root(-8,3)", "-2");
  simplifies_to("(cos(x)^2+sin(x)^2-1)^π", "dep(0,{x})", cartesianCtx);

  // Real powers
  simplifies_to("√(x)^2", "√(x)^2");
  // - x^y if x is complex or positive
  simplifies_to("41^(1/3)", "root(41,3)");
  // - PowerReal(x,y) y is not a rational
  simplifies_to("x^(e^3)", "x^e^3");
  simplifies_to("(x^e)^3", "x^e^3");
  // - Looking at y's reduced rational form p/q :
  //   * PowerReal(x,y) if x is of unknown sign and p odd
  simplifies_to("x^(1/3)", "root(x,3)");
  //   * Unreal if q is even and x negative
  simplifies_to("(-1)^(1/2)", "nonreal");
  //   * |x|^y if p is even
  simplifies_to("(-41)^(4/5)", "41^(4/5)");
  //   * -|x|^y if p is odd
  simplifies_to("(-41)^(5/7)", "-(41^(5/7))");

  // Complex Power
  simplifies_to("√(x)^2", "x", cartesianCtx);
  // TODO: (exp(i*(arg(A) + arg(B) - arg(A*B))) should be simplified to 1
  simplifies_to("√(-i-1)*√(-i+1)+√((-i-1)*(-i+1))",
                "√(-2)+e^(ln(-1-i)/2+ln(1-i)/2)", cartesianCtx);

  // Expand/Contract
  simplifies_to("e^(ln(2)+π)", "2e^π");
  simplifies_to("√(12)-2×√(3)", "0");
  simplifies_to("3^(1/3)×41^(1/3)-123^(1/3)", "0");
  simplifies_to("√(2)*√(7)-√(14)", "0");
  simplifies_to("x^(1-y^0)", "dep(1,{x^0,y^0})");
  simplifies_to("i^5+i^10+i^15+i^20", "0");

  simplifies_to("2/√(2)", "√(2)");
  simplifies_to("4/√(2)", "2*√(2)");
  simplifies_to("1/√(2) - √(2)/2", "0");
  // TODO: metric issue, one form should be preferred
  simplifies_to("1/√(2)", "1/√(2)");
  simplifies_to("√(2)/2", "√(2)/2");
}

QUIZ_CASE(pcj_simplification_float) {
  simplifies_to("2", "2", {.m_strategy = Strategy::ApproximateToFloat});
  simplifies_to("2.3", "2.3", {.m_strategy = Strategy::ApproximateToFloat});
  simplifies_to("1+π", "4.1415926535898",
                {.m_strategy = Strategy::ApproximateToFloat});
  simplifies_to("1+π+x", "x+4.1415926535898",
                {.m_strategy = Strategy::ApproximateToFloat});
  simplifies_to("cos(x-x)", "dep(1,{x})",
                {.m_strategy = Strategy::ApproximateToFloat});
  simplifies_to("random()-random()", "random()-1×random()",
                {.m_strategy = Strategy::ApproximateToFloat});
  simplifies_to("y^3*x^-2", "y^3/x^2",
                {.m_strategy = Strategy::ApproximateToFloat});

  // This was raising asserts because of float approximation on flatten.
  Tree* u = (KMult(KPow(180_e, -1_e), π_e, KMult(180_de, "x"_e)))->cloneTree();
  SystematicReduction::ShallowReduce(u->child(0));
  QUIZ_ASSERT(SystematicReduction::ShallowReduce(u));
  QUIZ_ASSERT(!SystematicReduction::ShallowReduce(u));
}

QUIZ_CASE(pcj_simplification_unit) {
  simplifies_to("12_m", "12×_m");
  simplifies_to("1_s", "1×_s");
  simplifies_to("1_m+1_s", "undef");
  simplifies_to("1_m+1_yd", "1.9144×_m");
  simplifies_to("1_m+x", "undef");
  simplifies_to("1_mm+1_km", "1.000001×_km");
  // simplifies_to("2_month×7_dm", "3681720×_s×_m");
  simplifies_to("2×_m/_m", "2");
  simplifies_to("1234_g", "1.234×_kg");
  simplifies_to("cos(0_rad)", "1");
  simplifies_to("sum(_s,x,0,1)", "2×_s");
  simplifies_to("_s^-1", "1×_s^(-1)");
  simplifies_to("abs(-3.3_m)", "3.3×_m");

  // Temperature
  simplifies_to("4_°C", "4×_°C");
  // Note: this used to be undef in previous Poincare.
  simplifies_to("((4-2)_°C)×2", "4×_°C");
  simplifies_to("((4-2)_°F)×2", "4.0000000000001×_°F");  // TODO: Fix precision
  simplifies_to("8_°C/2", "4×_°C");
  simplifies_to("2_K+2_K", "4×_K");
  simplifies_to("2_K×2_K", "4×_K^2");
  simplifies_to("1/_K", "1×_K^(-1)");
  simplifies_to("(2_K)^2", "4×_K^2");

  // Undefined
  simplifies_to("2_°C-1_°C", "undef");
  simplifies_to("2_°C+2_K", "undef");
  simplifies_to("2_K+2_°C", "undef");
  simplifies_to("2_K×2_°C", "undef");
  simplifies_to("1/_°C", "undef");
  simplifies_to("(1_°C)^2", "undef");
  simplifies_to("tan(2_m)", "undef");
  simplifies_to("tan(2_rad^2)", "undef");
  // TODO_PCJ : Wasn't exact before
  simplifies_to("π×_rad×_°", "(π^2×_rad^2)/180");

  // BestRepresentative
  simplifies_to("1_m+1_km", "1.001×_km");
  simplifies_to("1ᴇ-9_s", "1×_ns");

  // TODO: Decide on implicit '_' parsing
  //   simplifies_to("1m+1km", "1_m+1_km" /  "m+k×m" / "m+km" );
  //   simplifies_to("1m+1s", "undef" / "m+s");
  //   simplifies_to("1m+x", "m+x" / "undef");

  // UnitFormat
  simplifies_to("1609.344_m", "1.609344×_km",
                {.m_unitFormat = UnitFormat::Imperial});

  // Constants
  simplifies_to("_mn + _mp", "3.34754942173ᴇ-24×_g");
  simplifies_to("_mn + _G", "undef");

  // Implicit additions
  simplifies_to("3h300min", "8×_h");

  // 0
  simplifies_to("0×0×2×(_rad + _°)×_°", "0×_rad^2");
  simplifies_to("0×0×2×(_km + _m)×_km×_s", "0×_m^2×_s");
  simplifies_to("0×_°C", "0×_°C", {.m_unitDisplay = UnitDisplay::None});
  simplifies_to("6×0×_°F", "0×_°F", {.m_unitDisplay = UnitDisplay::None});
  simplifies_to("0×_K", "0×_K");
  simplifies_to("0×_K×_s×_s×(_g+4×_kg)", "0×_kg×_K×_s^2");

  // Angles
  simplifies_to("360×_°", "2×π×_rad");
  simplifies_to("π×π×_rad", "_rad×π^2");
  simplifies_to("π×π×_rad", "180×π×_°", {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("π×π×_rad×_m", "9.8696044010894×_m×_rad");
  simplifies_to("π×π×_rad×_rad", "π^2×_rad^2");
}

QUIZ_CASE(pcj_simplification_dependencies) {
  simplifies_to("0^(5+ln(5))", "0");
  simplifies_to("[[x/x]]", "[[dep(1,{x^0})]]");
  simplifies_to("lcm(undef, 2+x/x)", "undef");
  simplifies_to(
      "{(x, 2), (x/x, 2), (3, undef), (1 , piecewise(x/x + (a + b)^2 - 2*a*b, "
      "x + y/y>2, undef))}",
      "{(x,2),(dep(1,{x^0}),2),(3,undef),(1,dep(piecewise(dep(a^2+b^2+1,{x^0}),"
      "(x+1)>2,undef),{y^0}))}");

  simplifies_to("{1,undef}", "{1,undef}");
  simplifies_to("[[1,undef]]", "[[1,undef]]");
  simplifies_to("(1,undef)", "(1,undef)");

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
  Simplification::SimplifyWithAdaptiveStrategy(e3, &context);
  assert_trees_are_equal(e3, r3);

  Tree* e4 = KDiff("x"_e, "y"_e, 1_e, KDep("x"_e, KDepList("x"_e, "z"_e)))
                 ->cloneTree();
  const Tree* r4 = KDep(1_e, KDepList("y"_e, "z"_e));
  Simplification::SimplifyWithAdaptiveStrategy(e4, &context);
  assert_trees_are_equal(e4, r4);
}

QUIZ_CASE(pcj_simplification_infinity) {
  simplifies_to("inf", "∞");
  simplifies_to("inf(-1)", "-∞");
  simplifies_to("inf-1", "∞");
  simplifies_to("-inf+1", "-∞");
  simplifies_to("inf+inf", "∞");
  simplifies_to("-inf-inf", "-∞");
  simplifies_to("inf-inf", "undef");
  simplifies_to("-inf+inf", "undef");
  simplifies_to("inf-inf+3*inf", "undef");
  simplifies_to("x-inf", "x-∞");
  simplifies_to("inf*(-π)", "-∞");
  simplifies_to("inf*2*inf", "∞");
  simplifies_to("0×inf", "undef");
  simplifies_to("3×inf", "∞");
  simplifies_to("-3×inf", "-∞");
  simplifies_to("inf×(-inf)", "-∞");
  simplifies_to("x×(-inf)", "∞×sign(-x)");
  simplifies_to("(abs(x)+1)*inf", "∞");
  simplifies_to("1/inf", "0");
  simplifies_to("0/inf", "0");

  // x^inf
  // simplifies_to("(-2)^inf", "undef");  // complex inf
  // simplifies_to("(-2)^(-inf)", "0");
  simplifies_to("(-1)^inf", "undef");
  simplifies_to("(-1)^(-inf)", "undef");
  // simplifies_to("(-0.3)^inf", "0");
  // simplifies_to("(-0.3)^(-inf)", "undef");  // complex inf
  simplifies_to("0^inf", "0");
  simplifies_to("0^(-inf)", "undef");  // complex inf
  simplifies_to("0.3^inf", "0");
  simplifies_to("0.3^(-inf)", "∞");
  simplifies_to("1^inf", "undef");
  simplifies_to("1^(-inf)", "undef");
  simplifies_to("2^inf", "∞");
  simplifies_to("2^(-inf)", "0");
  simplifies_to("x^inf", "e^(∞×sign(ln(x)))");

  // inf^x
  simplifies_to("inf^i", "undef");
  simplifies_to("inf^6", "∞");
  simplifies_to("(-inf)^6", "∞");
  simplifies_to("inf^7", "∞");
  simplifies_to("(-inf)^7", "-∞");
  simplifies_to("inf^-6", "0");
  simplifies_to("(-inf)^-6", "0");
  simplifies_to("inf^0", "undef");
  simplifies_to("(-inf)^0", "undef");
  simplifies_to("inf^inf", "∞");
  simplifies_to("inf^(-inf)", "0");
  simplifies_to("(-inf)^inf", "undef");  // complex inf
  simplifies_to("(-inf)^(-inf)", "0");
  simplifies_to("inf^x", "e^(∞×sign(x))");

  // functions
  simplifies_to("exp(inf)", "∞");
  simplifies_to("exp(-inf)", "0");
  // TODO_PCJ simplifies_to("log(inf,0)", "undef");
  // TODO_PCJ simplifies_to("log(-inf,0)", "undef");
  simplifies_to("log(inf,1)", "undef");
  simplifies_to("log(-inf,1)", "undef");
  simplifies_to("log(inf,x)",
                "dep(∞×sign(1/ln(x)),{NonNull(x),RealPositive(x)})");
  // TODO: should be nonreal
  simplifies_to("log(-inf,x)", "undef");
  simplifies_to("log(-inf,x)", "dep(ln(-∞)/ln(x),{NonNull(x)})", cartesianCtx);
  // TODO: should be nonreal
  simplifies_to("log(inf,-3)", "undef");
  simplifies_to("log(inf,-3)", "∞×sign(1/ln(-3))", cartesianCtx);
  // TODO_PCJ simplifies_to("log(0,inf)", "undef");
  // TODO_PCJ simplifies_to("log(0,-inf)", "undef");
  simplifies_to("log(1,inf)", "0");
  // TODO_PCJ simplifies_to("log(1,-inf)", "0");
  simplifies_to("log(x,inf)", "dep(0,{ln(x),NonNull(x),RealPositive(x)})");
  // TODO_PCJ simplifies_to("log(x,-inf)", "dep(∞×sign(1/ln(x)),{ln(x)})");
  simplifies_to("log(inf,inf)", "undef");
  // TODO_PCJ simplifies_to("log(-inf,inf)", "undef");
  // TODO_PCJ simplifies_to("log(inf,-inf)", "undef");
  // TODO_PCJ simplifies_to("log(-inf,-inf)", "undef");
  simplifies_to("ln(inf)", "∞");
  // TODO: should be nonreal
  simplifies_to("ln(-inf)", "undef");
  simplifies_to("cos(inf)", "undef");
  simplifies_to("cos(-inf)", "undef");
  simplifies_to("sin(inf)", "undef");
  simplifies_to("sin(-inf)", "undef");
  simplifies_to("atan(inf)", "π/2");
  simplifies_to("atan(-inf)", "-π/2");
  simplifies_to("atan(e^inf)", "π/2");
}

QUIZ_CASE(pcj_simplification_trigonometry) {
  // Direct trigonometry exact formulas
  simplifies_to("cos({0,π/2,π,3π/2,-4π})", "{1,0,-1,0,1}");
  simplifies_to("sin({0,π/2,π,3π/2,-4π})", "{0,1,0,-1,0}");
  // test π/12 in top-right quadrant
  simplifies_to("cos({π/12,-19π/12})",
                "{(√(2)×(1+√(3)))/4,(-1+√(3))/(2×√(2))}");
  simplifies_to("sin({π/12,-19π/12})",
                "{(√(2)×(-1+√(3)))/4,(√(2)×(1+√(3)))/4}");
  // test π/10 in top-left quadrant
  simplifies_to("cos({66π/10,-31π/10})", "{-(-1+√(5))/4,-√((5+√(5))/8)}");
  simplifies_to("sin({66π/10,-31π/10})", "{√((5+√(5))/8),(-1+√(5))/4}");
  // test π/8 in bottom-left quadrant
  simplifies_to("cos({9π/8,59π/8})", "{-√(2+√(2))/2,-√(2-√(2))/2}");
  simplifies_to("sin({9π/8,59π/8})", "{-√(2-√(2))/2,-√(2+√(2))/2}");
  // test π/6 in bottom-right quadrant
  simplifies_to("cos({22π/6,11π/6})", "{1/2,√(3)/2}");
  simplifies_to("sin({22π/6,11π/6})", "{-√(3)/2,-1/2}");
  // test π/5 in all quadrants
  simplifies_to("cos({6π/5,-33π/5,18π/5,-π/5})",
                "{-(1+√(5))/4,-(-1+√(5))/4,(-1+√(5))/4,(1+√(5))/4}");
  simplifies_to("sin({π/5,2π/5,3π/5,-6π/5})",
                "{√(5/8-√(5)/8),√((5+√(5))/8),√((5+√(5))/8),√(5/8-√(5)/8)}");
  // test π/4 in all quadrants
  simplifies_to("cos({π/4,3π/4,-11π/4,7π/4})",
                "{1/√(2),-1/√(2),-1/√(2),1/√(2)}");
  simplifies_to("sin({π/4,3π/4,-11π/4,7π/4})",
                "{1/√(2),1/√(2),-1/√(2),-1/√(2)}");
  simplifies_to("cos(π)", "cos(π)", {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("cos(45)", "1/√(2)", {.m_angleUnit = AngleUnit::Degree});

  // Inverse trigonometry exact formulas
  simplifies_to("acos({-1, -√(3)/2, -√(2)/2, -1/2, 0, 1/2, √(2)/2, √(3)/2, 1})",
                "{π,5π/6,3π/4,2π/3,π/2,π/3,π/4,π/6,0}");
  simplifies_to("acos(-1/√(2))", "3π/4");
  simplifies_to("acos(1/√(2))", "π/4");
#if 0  // TODO_PCJ
  simplifies_to("acos(-(√(6)+√(2))/4)", "11π/12");
  simplifies_to("acos(-(√(6)-√(2))/4)", "7π/12");
  simplifies_to("acos((√(6)-√(2))/4)", "5π/12");
  simplifies_to("acos((√(6)+√(2))/4)", "π/12");
#endif
  simplifies_to("asin(-1/√(2))", "-π/4");
  simplifies_to("asin(1/√(2))", "π/4");
  simplifies_to("asin({-1, -√(3)/2, -√(2)/2, -1/2, 0, 1/2, √(2)/2, √(3)/2, 1})",
                "{-π/2,-π/3,-π/4,-π/6,0,π/6,π/4,π/3,π/2}");
  simplifies_to("asin({-1, -√(3)/2, -√(2)/2, -1/2, 0, 1/2, √(2)/2, √(3)/2, 1})",
                "{-90,-60,-45,-30,0,30,45,60,90}",
                {.m_angleUnit = AngleUnit::Degree});
#if 0  // TODO_PCJ
  simplifies_to("asin(-(√(6)+√(2))/4)", "-5π/12");
  simplifies_to("asin(-(√(6)-√(2))/4)", "-π/12");
  simplifies_to("asin((√(6)-√(2))/4)", "π/12");
  simplifies_to("asin((√(6)+√(2))/4)", "5π/12");
#endif
  simplifies_to("atan(-1/√(3))", "-π/6");
  simplifies_to("atan(1/√(3))", "π/6");
  simplifies_to("atan({-inf, -√(3), -1, -√(3)/3, 0, 1, √(3)/3, √(3), inf})",
                "{-π/2,-π/3,-π/4,-π/6,0,π/4,π/6,π/3,π/2}");

  // Trig diff
  simplifies_to("2×sin(2y)×sin(y)+cos(3×y)", "cos(y)");
  simplifies_to("2×sin(2y)×cos(y)-sin(3×y)", "sin(y)");
  simplifies_to("2×cos(2y)×sin(y)+sin(y)", "sin(3×y)");
  simplifies_to("2×cos(2y)×cos(y)-cos(y)", "cos(3×y)");
  simplifies_to("sin(4π/21)sin(π/7)", "(-1/2+cos((41×π)/21))/2");
  simplifies_to("cos(13π/42)sin(π/7)", "(-1/2+sin((19×π)/42))/2");

  // Direct trigonometry
  simplifies_to("cos(-x)", "cos(x)");
  simplifies_to("-sin(-x)", "sin(x)");
  simplifies_to("tan(-x)", "-tan(x)");
  simplifies_to("cos({-inf,inf})", "{undef, undef}");
  simplifies_to("sin({-inf,inf})", "{undef, undef}");
  simplifies_to("sin((241/120)π)", "sin(π/120)");

  // Direct advanced trigonometry
  simplifies_to("1/tan(x)", "dep(cot(x),{NonNull(cos(x))})");
  simplifies_to("1/tan(3)", "cot(3)");

  // Inverse trigonometry
  simplifies_to("acos(-x)", "arccos(-x)", cartesianCtx);
  simplifies_to("asin(-x)", "arcsin(-x)", cartesianCtx);
  simplifies_to("atan(-x)", "-arctan(x)", cartesianCtx);

  // trig(atrig)
  simplifies_to("cos({acos(x), asin(x), atan(x)})",
                "{x,√(-x^2+1),cos(arctan(x))}", cartesianCtx);
  simplifies_to("cos({acos(-x), asin(-x), atan(-x)})",
                "{-x,√(-x^2+1),cos(arctan(x))}", cartesianCtx);
  simplifies_to("sin({acos(x), asin(x), atan(x)})",
                "{√(-x^2+1),x,sin(arctan(x))}", cartesianCtx);
  simplifies_to("sin({acos(-x), asin(-x), atan(-x)})",
                "{√(-x^2+1),-x,-sin(arctan(x))}", cartesianCtx);
  simplifies_to("tan({acos(x), asin(x), atan(x)})",
                "{√(-x^2+1)/x,x/√(-x^2+1),tan(arctan(x))}", cartesianCtx);
  simplifies_to("tan({acos(-x), asin(-x), atan(-x)})",
                "{-√(-x^2+1)/x,-x/√(-x^2+1),-tan(arctan(x))}", cartesianCtx);
  simplifies_to("cos({acos(x), asin(x), atan(x)})",
                "{x,√(-x^2+1),cos(arctan(x))}",
                {.m_complexFormat = ComplexFormat::Cartesian,
                 .m_angleUnit = AngleUnit::Degree});
  simplifies_to("cos(acos(3/7))", "3/7");
  simplifies_to("cos(acos(9/7))", "undef");  // TODO: nonreal ?
  simplifies_to("cos(acos(9/7))", "9/7", cartesianCtx);
  simplifies_to("sin(asin(3/7))", "3/7");
  simplifies_to("sin(asin(9/7))", "undef");  // TODO: nonreal ?
  simplifies_to("sin(asin(9/7))", "9/7", cartesianCtx);

  // atrig(trig)
  simplifies_to("acos({cos(x), sin(x), tan(x)})",
                "{arccos(cos(x)),arccos(sin(x)),arccos(tan(x))}", cartesianCtx);
  simplifies_to("asin({cos(x), sin(x), tan(x)})",
                "{arcsin(cos(x)),arcsin(sin(x)),arcsin(tan(x))}", cartesianCtx);
  simplifies_to("atan({cos(x), sin(x), tan(x)})",
                "{arctan(cos(x)),arctan(sin(x)),arctan(tan(x))}", cartesianCtx);
  simplifies_to("acos({cos(x), sin(x), tan(x)})",
                "{arccos(cos(x)),arccos(sin(x)),arccos(tan(x))}",
                {.m_complexFormat = ComplexFormat::Cartesian,
                 .m_angleUnit = AngleUnit::Degree});
  simplifies_to("acos(cos({3π/7, -11π/15,34π/15, 40π/13}))",
                "{(3×π)/7,(11×π)/15,(4×π)/15,(12×π)/13}");
  simplifies_to("asin(sin({3π/7, -11π/15,34π/15, 40π/13}))",
                "{(3×π)/7,-(4×π)/15,(4×π)/15,-π/13}");
  simplifies_to("atan(tan({3π/7, -11π/15,34π/15, 40π/13}))",
                "{(3×π)/7,(4×π)/15,(4×π)/15,π/13}");
  simplifies_to("acos({cos(683), sin(683)})", "{117,183}",
                {.m_angleUnit = AngleUnit::Gradian});
  simplifies_to("acos(sin({π*23/7, -23*π/7}))/π", "{11/14,3/14}");
  simplifies_to("asin(cos({π*23/7, -23*π/7}))/π", "{-3/14,-3/14}");
  simplifies_to("atan({tan(-3π/13), -sin(3π/13)/cos(3π/13)})",
                "{-3π/13,-3π/13}");
  simplifies_to("atan({sin(55π/13)/cos(3π/13),sin(55π/13)/cos(-101π/13)})",
                "{3π/13,3π/13}");
  simplifies_to("atan(sin({3,10,3,16,3,23}π/13)/cos({36,55,42,55,75,55}π/13))",
                "{-3π/13,3π/13,-3π/13,-3π/13,3π/13,-3π/13}");

#if 0
  // TODO: asin(x) = π/2 - acos(x) advanced reduction safe from infinite loops.
  simplifies_to("asin(x)-π/2", "-arccos(x)");
  simplifies_to("90-acos(x)", "arcsin(x)", {.m_angleUnit = AngleUnit::Degree});
#endif
  // TODO: Improve output with better advanced reduction.
  simplifies_to("(y*π+z/180)*asin(x)", "(π×y+z/180)×arcsin(x)",
                {.m_complexFormat = ComplexFormat::Cartesian,
                 .m_angleUnit = AngleUnit::Degree});
  // TODO: Undetected magic value.
  simplifies_to("arg(cos(π/6)+i*sin(π/6))", "π/6");
  simplifies_to("sin(17×π/12)^2+cos(5×π/12)^2", "1", cartesianCtx);
  simplifies_to("sin(17)^2+cos(6)^2", "cos(6)^2+sin(17)^2", cartesianCtx);
  // Only works in cartesian, because Power VS PowerReal. See Projection::Expand
  simplifies_to("cos(atan(x))-√(-(x/√(x^(2)+1))^(2)+1)",
                "dep(0,{ln(-x^2/(x^2+1)+1)})", cartesianCtx);
}

QUIZ_CASE(pcj_simplification_advanced) {
#if 0
  // TODO works but rejected by metric
  simplifies_to("sum(k+n, k, 1, n)", "sum(k, 1, n, k)+n^2");
  simplifies_to("sum(k+1, k, n, n+2)", "6+3×n");
  simplifies_to("sum(k+1, k, n-2, n)", "1");  // FIXME
  simplifies_to("product(k×π, k, 1, 12)", "479001600×π^(12)");

  // TODO ReduceAddition on matrices
  simplifies_to("sum([[k][n]], k, 1, 4)", "[[10][4×n]]");

  simplifies_to("diff(√(4-x^2),x,x)", "-x/√(4-x^2)");
  simplifies_to("1/x + 1/y - (x+y)/(x×y)", "0");
  simplifies_to("(x^2 - 1) / (x - 1)", "x+1");
  simplifies_to("1 / (1/a + c/(a×b)) + (a×b×c+a×c^2)/(b+c)^2", "a");

  simplifies_to("sin(x)^3+cos(x+π/6)^3-sin(x+π/3)^3+sin(3×x)×3/4", "0");
  simplifies_to("sin(x)+sin(y)-2×sin(x/2 + y/2)×cos(x/2 - y/2)", "0");
  simplifies_to("(√(10)-√(2))×√(5-√(5))-4×√(5-2×√(5))", "0");

  simplifies_to("1/(1 - (1/(1 - (1/(1-x)))))", "x");
  simplifies_to(
      "abs(diff(diff(√(4-x^2),x,x),x,x))/(1+diff(√(4-x^2),x,x)^2)^(3/2)",
      "1/2");

  simplifies_to("((x×y)^(1/2)×z^2)^2", "x×y×z^4");
  simplifies_to("1-cos(x)^2", "sin(x)^2");
#endif
  simplifies_to("1-cos(x)^2-sin(x)^2", "dep(0,{x})");
  simplifies_to("(a+b)^2", "(a+b)^2");
  simplifies_to("2*a+b*(a+c)-b*c", "dep(a×(b+2),{c})");
  simplifies_to("e^(a*c)*e^(b*c)+(a+b)^2-a*(a+2*b)", "b^2+e^((a+b)×c)");
  // TODO: Should be 0
  simplifies_to(
      "cos(b)×cos(a)-1/2×cos(b)×cos(a)-1/2×sin(b)×sin(a)+1/2×cos(b)×cos(a)+1/"
      "4×cos(b+a)-1/4×cos(b-a)-cos(a+b)",
      "(3×cos(a)×cos(b))/4-(3×cos(a+b))/4-(3×sin(a)×sin(b))/4");
}

QUIZ_CASE(pcj_simplification_logarithm) {
  simplifies_to("π×ln(2)+ln(4)", "(2+π)×ln(2)");
  // TODO: Metric: 1+ln(x×y)
  simplifies_to("1+ln(x)+ln(y)",
                "dep(1+ln(x)+ln(y),{NonNull(x),NonNull(y),RealPositive(x),"
                "RealPositive(y)})");
  // TODO: Metric: 2×ln(π)
  simplifies_to("ln(π)-ln(1/π)", "ln(π^2)");
  simplifies_to("cos(x)^2+sin(x)^2-ln(x)",
                "dep(1-ln(x),{arg(1/x),arg(x),RealPositive(x)})");
  simplifies_to("1-ln(x)", "dep(1-ln(x),{NonNull(x)})", cartesianCtx);
  simplifies_to("ln(0)", "undef");
  simplifies_to("ln(0)", "undef", cartesianCtx);
  simplifies_to(
      "ln(cos(x)^2+sin(x)^2)",
      "dep(0,{NonNull(cos(x)^2+sin(x)^2),RealPositive(cos(x)^2+sin(x)^2)})");
  simplifies_to("ln(-10)-ln(5)", "ln(-2)", cartesianCtx);
  simplifies_to("im(ln(-120))", "π", cartesianCtx);
  simplifies_to("ln(-1-i)+ln(-1+i)", "ln(2)", cartesianCtx);
  simplifies_to("im(ln(i-2)+ln(i-1))-2π", "im(ln(1-3×i))", cartesianCtx);
  simplifies_to("ln(x)+ln(y)-ln(x×y)",
                "dep(ln(x)+ln(y)-ln(x×y),{NonNull(x),NonNull(y),NonNull(x×y)})",
                cartesianCtx);
  simplifies_to("ln(abs(x))+ln(abs(y))-ln(abs(x)×abs(y))",
                "dep(0,{ln(abs(x)),ln(abs(y)),NonNull(abs(x)),NonNull(abs(y)),"
                "NonNull(abs(x)×abs(y))})",
                cartesianCtx);

  // Use complex logarithm internally
  simplifies_to("√(x^2)", "√(x^2)", cartesianCtx);
  simplifies_to("√(abs(x)^2)", "abs(x)", cartesianCtx);
  simplifies_to("√(0)", "0", cartesianCtx);
  simplifies_to("√(cos(x)^2+sin(x)^2-1)", "dep(0,{x})", cartesianCtx);
}

QUIZ_CASE(pcj_simplification_boolean) {
  simplifies_to("true", "true");
  simplifies_to("true and false", "false");

  simplifies_to("1+1=2", "True");
  simplifies_to("2!=3", "True");
  simplifies_to("2<1", "False");
  simplifies_to("1<2<=2", "True");
  simplifies_to("x≥2", "x>=2");
}

QUIZ_CASE(pcj_simplification_point) {
  simplifies_to("(1,2)", "(1,2)");
  simplifies_to("({1,3},{2,4})", "{(1,2),(3,4)}");
  simplifies_to("sequence((k,k+1),k,3)", "{(1,2),(2,3),(3,4)}");
  simplifies_to("(undef,2)", "(undef,2)");
  simplifies_to("0*(1,2)", "undef");
}

QUIZ_CASE(pcj_simplification_piecewise) {
  simplifies_to("piecewise(x)", "x");
  simplifies_to("piecewise(x, True, y)", "x");
  simplifies_to("piecewise(x, False, y)", "y");
  simplifies_to("piecewise(x, u<1, y, 2<1, z)", "piecewise(x, u<1, z)");
  simplifies_to("piecewise(x, u<1 and 1<2, y, 1<3, z, u<3, w)",
                "piecewise(x, u<1, y)");
  simplifies_to("piecewise(x, u<1, y, True)", "piecewise(x, u<1, y)");
  simplifies_to("piecewise(1, True, undef)", "1");
  simplifies_to("piecewise(3,1>0,2,undef)", "undef");
  simplifies_to("piecewise(-1,undef,i)", "undef");
  simplifies_to("piecewise(4^2,undef,6,4>2)", "undef");
}

QUIZ_CASE(pcj_distributions) {
  simplifies_to("binomcdf(3,5,0.4)", "binomcdf(3,5,2/5)");
  simplifies_to("binompdf(3.5,5,0.4)", "binompdf(3,5,2/5)");
  simplifies_to("normcdf(inf,5,0.4)", "1");
  simplifies_to("normcdf(-inf,5,0.4)", "0");
  simplifies_to("tcdfrange(-inf,inf,5)", "1");
  simplifies_to("tcdfrange(-inf,-inf,5)", "0");
}

QUIZ_CASE(pcj_simplification_function) {
  Shared::GlobalContext globalContext;
  store("x→f(x)", &globalContext);
  ProjectionContext projCtx = {
      .m_symbolic = SymbolicComputation::DoNotReplaceAnySymbol,
      .m_context = &globalContext,
  };
  simplifies_to("f(x)", "f(x)", projCtx);
  simplifies_to("f(2+2)", "f(4)");
  simplifies_to("f(y)+f(x)-f(x)", "dep(f(y),{f(x)})");
  Ion::Storage::FileSystem::sharedFileSystem->destroyAllRecords();
}

QUIZ_CASE(pcj_simplification_variable_replace) {
  Shared::GlobalContext globalContext;
  assert(
      Ion::Storage::FileSystem::sharedFileSystem->numberOfRecords() ==
      Ion::Storage::FileSystem::sharedFileSystem->numberOfRecordsWithExtension(
          "sys"));

  ProjectionContext projCtx = {
      .m_symbolic = SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition,
      .m_context = &globalContext};

  store("4→y", &globalContext);
  simplifies_to("x+y", "x+4", projCtx);

  store("x^2→f(x)", &globalContext);
  simplifies_to("f(z+f(y))", "(z+16)^2", projCtx);

  store("{5,4,3,2,1}→l", &globalContext);
  simplifies_to("sum(l)", "15", projCtx);
  simplifies_to("l(2)", "4", projCtx);
  simplifies_to("l(1,3)", "{5,4,3}", projCtx);
  // TODO: Properly parse list slices on variables
  // TODO_PCJ: implement list access and list slice on lists of points

  ProjectionContext projCtx2 = {
      .m_symbolic =
          SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined,
      .m_context = &globalContext};
  simplifies_to("y+2", "6", projCtx2);
  simplifies_to("y+x", "undef", projCtx2);
  simplifies_to("diff(y*y, y, y)", "8", projCtx2);
  simplifies_to("diff(x*x, x, y)", "8", projCtx2);
  simplifies_to("diff(x*x, x, x)", "undef", projCtx2);

  // Local variables may be unit names
  simplifies_to("sum(t,t,1,3)", "6");

  Ion::Storage::FileSystem::sharedFileSystem->destroyAllRecords();
}

QUIZ_CASE(pcj_decimal) {
  Tree* tree = SharedTreeStack->pushDecimal();
  (124_e)->cloneTree();
  (-2_e)->cloneTree();
  ProjectionContext ctx = realCtx;
  Simplification::SimplifyWithAdaptiveStrategy(tree, &ctx);
  assert_trees_are_equal(tree, 12400_e);
  tree->removeTree();
  tree = SharedTreeStack->pushDecimal();
  (124_e)->cloneTree();
  (2_e)->cloneTree();
  Simplification::SimplifyWithAdaptiveStrategy(tree, &ctx);
  assert_trees_are_equal(tree, KDiv(31_e, 25_e));
}
