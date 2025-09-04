#include <poincare/src/expression/advanced_reduction.h>
#include <poincare/src/expression/beautification.h>
#include <poincare/src/expression/dependency.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/list.h>
#include <poincare/src/expression/simplification.h>
#include <poincare/src/expression/systematic_reduction.h>
#include <poincare/src/expression/units/k_units.h>
#include <poincare/src/expression/variables.h>
#include <poincare/src/memory/tree_stack.h>

#include "helper.h"
#include "helpers/symbol_store.h"

using namespace Poincare::Internal;
using Poincare::AngleUnit;
using Poincare::ComplexFormat;
using Poincare::ReductionTarget;
using Poincare::Strategy;
using Poincare::SymbolicComputation;
using Poincare::UnitDisplay;
using UnitFormat = Poincare::Preferences::UnitFormat;

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

void algebraic_expand_to(const Tree* input, const Tree* output) {
  deepSystematicReduce_and_operation_to(
      input, AdvancedReduction::DeepExpandAlgebraic, output);
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
                 KDepList(KMult(0_e, KTrig(KAdd("x"_e, "y"_e), 1_e)))));
  expand_to(KLn(KMult(2_e, π_e)), KAdd(KLn(2_e), KLn(π_e)));
}

QUIZ_CASE(pcj_simplification_algebraic_expansion) {
  // A?*(B+C)*D? = A*D*B + A*D*C
  algebraic_expand_to(
      KMult("a"_e, KAdd("b"_e, "c"_e, "d"_e), "e"_e),
      KAdd(KMult("a"_e, "b"_e, "e"_e), KMult("a"_e, "c"_e, "e"_e),
           KMult("a"_e, "d"_e, "e"_e)));
  // (A + B)^2 = (A^2 + 2*A*B + B^2)
  algebraic_expand_to(KPow(KAdd(KTrig("x"_e, 0_e), KTrig("x"_e, 1_e)), 2_e),
                      KAdd(KPow(KTrig("x"_e, 0_e), 2_e),
                           KMult(2_e, KTrig("x"_e, 0_e), KTrig("x"_e, 1_e)),
                           KPow(KTrig("x"_e, 1_e), 2_e)));
  // (A + B + C)^2 = (A^2 + 2*A*B + B^2 + 2*A*C + 2*B*C + C^2)
  algebraic_expand_to(KPow(KAdd("x"_e, "y"_e, "z"_e), 2_e),
                      KAdd(KPow("x"_e, 2_e), KMult(2_e, "x"_e, "y"_e),
                           KPow("y"_e, 2_e), KMult(2_e, "x"_e, "z"_e),
                           KMult(2_e, "y"_e, "z"_e), KPow("z"_e, 2_e)));
  algebraic_expand_to(KList(KDep(KMult("a"_e, KAdd("b"_e, "c"_e)),
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
  contract_to(KAdd("e"_e, "f"_e, KLn(π_e), KLn(KMult(2_e, e_e))),
              KAdd("e"_e, "f"_e, KLn(KMult(2_e, π_e, e_e))));
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
  assert_trees_are_equal(Variables::GetUserSymbols(0_e), KList());
  const Tree* e = KMult(
      KAdd(KSin("y"_e), KSum("x"_e, 2_e, 4_e, KPow("z"_e, "x"_e))), "m"_e);
  assert_trees_are_equal(Variables::GetUserSymbols(e),
                         KList("m"_e, "y"_e, "z"_e));
  const Tree* s = KSum("k"_e, 1_e, 2_e, KAdd(KVarK, π_e));
  quiz_assert(!Variables::HasVariables(s));
  quiz_assert(Variables::HasVariables(s->lastChild()));
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
        simplify(tree, projectionContext, true);
      },
      projectionContext);
}

void projects_and_reduces_to(const char* input, const char* output,
                             ProjectionContext projectionContext = realCtx) {
  process_tree_and_compare(
      input, output,
      [](Tree* tree, ProjectionContext projectionContext) {
        simplify(tree, projectionContext, false);
        // Beautify anyway to compare input and outputs
        ReductionTarget previousReductionTarget =
            projectionContext.m_reductionTarget;
        projectionContext.m_reductionTarget = ReductionTarget::User;
        Beautification::DeepBeautify(tree, projectionContext);
        projectionContext.m_reductionTarget = previousReductionTarget;
      },
      projectionContext);
}

void simplifies_to_no_beautif(const char* input, const char* output,
                              ProjectionContext projectionContext = realCtx) {
  process_tree_and_compare(
      input, output,
      [](Tree* tree, ProjectionContext projectionContext) {
        simplify(tree, projectionContext, false);
      },
      projectionContext);
}

QUIZ_CASE(pcj_simplification_basic) {
  simplifies_to("x", "x");
  simplifies_to("x-x", "0");
  simplifies_to("2+2", "4");
  simplifies_to("(2×3(2^2)) + 2×2", "28");
  simplifies_to("36/8", "9/2");
  simplifies_to("2+36/8+2", "17/2");
  simplifies_to("a+a", "2×a");
  simplifies_to("b+a", "a+b");
  simplifies_to("a×b", "a×b");
  simplifies_to("b^2×a", "a×b^2");
  simplifies_to("(a×a)×a", "a^3");
  simplifies_to("a×(a×a)", "a^3");
  simplifies_to("(a×b)^2", "a^2×b^2");
  simplifies_to("(a×b×c)^2", "a^2×b^2×c^2");
  simplifies_to("(x^3)^2", "x^6");
  simplifies_to("a×a×a", "a^3");
  simplifies_to("a×a×a×b", "a^3×b");
  simplifies_to("a×2a×b×a×b×4", "8×a^3×b^2");
  simplifies_to("1×1×1×1", "1");
  simplifies_to("a+1/a+a×a", "a^2+a+1/a");
  simplifies_to("2a+3b+4a", "6×a+3×b");
  simplifies_to("-6×b-4×a×b-2×b+3×a×b-4×b+2×a×b+3×b+6×a×b", "(7×a-9)×b");
  simplifies_to("d+c+b+a", "a+b+c+d");
  simplifies_to("(a+b)×(d+f)×g-a×d×g-a×f×g", "b×(d+f)×g");
  simplifies_to("a*x*y+b*x*y+c*x", "x×(c+(a+b)×y)");
  simplifies_to("(e^(x))^2", "e^(2×x)");
  simplifies_to("e^(ln(x))", "dep(x,{nonNull(x),realPos(x)})");
  simplifies_to("e^(ln(1+x^2))", "x^2+1");
  simplifies_to("e^(ln(x+x))", "dep(2×x,{nonNull(x+x)})", cartesianCtx);
  simplifies_to("x+1+(-1)(x+1)", "0");
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
  simplifies_to("abs(abs(abs((-3)×x)))", "3×abs(x)");
  simplifies_to("abs(1+i)", "√(2)", cartesianCtx);
  simplifies_to("abs(-2i)+abs(2i)+abs(2)+abs(-2)", "8", cartesianCtx);
  simplifies_to("abs(x^2)", "x^2");
  simplifies_to("abs(a)*abs(b*c)-abs(a*b)*abs(c)", "0");
  simplifies_to("((abs(x)^(1/2))^(1/2))^8", "x^2");
  simplifies_to("(2+x)*(2-x)+(x+1)*(x-1)", "3");
  simplifies_to("abs(x)/x", "dep(sign(x),{x^0})");
  simplifies_to("x^(1+abs(x)/x)", "dep(x^(1+sign(x)),{x^0})");
  simplifies_to("abs((-3)^ln(5))", "3^ln(5)", cartesianCtx);
  simplifies_to("abs(acos(2)^4)", "abs(acos(2))^4", cartesianCtx);
  simplifies_to("abs(e^(π/2×i)+e^(π/6×i))", "√(3)");
  simplifies_to("2^101*2", "5070602400912917605986812821504");
  simplifies_to("2.3×10^45", "2.3×10^45");
}

QUIZ_CASE(pcj_simplification_big_nary) {
  // Exactly 255 children
  simplifies_to(
      "(a0+a1+a2+a3+a4+a5+a6+a7+a8+a9)+"
      "(b0+b1+b2+b3+b4+b5+b6+b7+b8+b9)+"
      "(c0+c1+c2+c3+c4+c5+c6+c7+c8+c9)+"
      "(d0+d1+d2+d3+d4+d5+d6+d7+d8+d9)+"
      "(e0+e1+e2+e3+e4+e5+e6+e7+e8+e9)+"
      "(f0+f1+f2+f3+f4+f5+f6+f7+f8+f9)+"
      "(g0+g1+g2+g3+g4+g5+g6+g7+g8+g9)+"
      "(h0+h1+h2+h3+h4+h5+h6+h7+h8+h9)+"
      "(i0+i1+i2+i3+i4+i5+i6+i7+i8+i9)+"
      "(j0+j1+j2+j3+j4+j5+j6+j7+j8+j9)+"
      "(k0+k1+k2+k3+k4+k5+k6+k7+k8+k9)+"
      "(l0+l1+l2+l3+l4+l5+l6+l7+l8+l9)+"
      "(m0+m1+m2+m3+m4+m5+m6+m7+m8+m9)+"
      "(n0+n1+n2+n3+n4+n5+n6+n7+n8+n9)+"
      "(o0+o1+o2+o3+o4+o5+o6+o7+o8+o9)+"
      "(p0+p1+p2+p3+p4+p5+p6+p7+p8+p9)+"
      "(q0+q1+q2+q3+q4+q5+q6+q7+q8+q9)+"
      "(r0+r1+r2+r3+r4+r5+r6+r7+r8+r9)+"
      "(s0+s1+s2+s3+s4+s5+s6+s7+s8+s9)+"
      "(t0+t1+t2+t3+t4+t5+t6+t7+t8+t9)+"
      "(u0+u1+u2+u3+u4+u5+u6+u7+u8+u9)+"
      "(v0+v1+v2+v3+v4+v5+v6+v7+v8+v9)+"
      "(w0+w1+w2+w3+w4+w5+w6+w7+w8+w9)+"
      "(x0+x1+x2+x3+x4+x5+x6+x7+x8+x9)+"
      "(y0+y1+y2+y3+y4+y5+y6+y7+y8+y9)+"
      "z0+z1+z2+z3+z4",
      "a0+a1+a2+a3+a4+a5+a6+a7+a8+a9+b0+b1+b2+b3+b4+b5+b6+b7+b8+b9+c0+c1+c2+c3+"
      "c4+c5+c6+c7+c8+c9+d0+d1+d2+d3+d4+d5+d6+d7+d8+d9+e0+e1+e2+e3+e4+e5+e6+e7+"
      "e8+e9+f0+f1+f2+f3+f4+f5+f6+f7+f8+f9+g0+g1+g2+g3+g4+g5+g6+g7+g8+g9+h0+h1+"
      "h2+h3+h4+h5+h6+h7+h8+h9+i0+i1+i2+i3+i4+i5+i6+i7+i8+i9+j0+j1+j2+j3+j4+j5+"
      "j6+j7+j8+j9+k0+k1+k2+k3+k4+k5+k6+k7+k8+k9+l0+l1+l2+l3+l4+l5+l6+l7+l8+l9+"
      "m0+m1+m2+m3+m4+m5+m6+m7+m8+m9+n0+n1+n2+n3+n4+n5+n6+n7+n8+n9+o0+o1+o2+o3+"
      "o4+o5+o6+o7+o8+o9+p0+p1+p2+p3+p4+p5+p6+p7+p8+p9+q0+q1+q2+q3+q4+q5+q6+q7+"
      "q8+q9+r0+r1+r2+r3+r4+r5+r6+r7+r8+r9+s0+s1+s2+s3+s4+s5+s6+s7+s8+s9+t0+t1+"
      "t2+t3+t4+t5+t6+t7+t8+t9+u0+u1+u2+u3+u4+u5+u6+u7+u8+u9+v0+v1+v2+v3+v4+v5+"
      "v6+v7+v8+v9+w0+w1+w2+w3+w4+w5+w6+w7+w8+w9+x0+x1+x2+x3+x4+x5+x6+x7+x8+x9+"
      "y0+y1+y2+y3+y4+y5+y6+y7+y8+y9+z0+z1+z2+z3+z4");
  // More than exactly 255 children
  simplifies_to(
      "(a0+a1+a2+a3+a4+a5+a6+a7+a8+a9)+"
      "(b0+b1+b2+b3+b4+b5+b6+b7+b8+b9)+"
      "(c0+c1+c2+c3+c4+c5+c6+c7+c8+c9)+"
      "(d0+d1+d2+d3+d4+d5+d6+d7+d8+d9)+"
      "(e0+e1+e2+e3+e4+e5+e6+e7+e8+e9)+"
      "(f0+f1+f2+f3+f4+f5+f6+f7+f8+f9)+"
      "(g0+g1+g2+g3+g4+g5+g6+g7+g8+g9)+"
      "(h0+h1+h2+h3+h4+h5+h6+h7+h8+h9)+"
      "(i0+i1+i2+i3+i4+i5+i6+i7+i8+i9)+"
      "(j0+j1+j2+j3+j4+j5+j6+j7+j8+j9)+"
      "(k0+k1+k2+k3+k4+k5+k6+k7+k8+k9)+"
      "(l0+l1+l2+l3+l4+l5+l6+l7+l8+l9)+"
      "(m0+m1+m2+m3+m4+m5+m6+m7+m8+m9)+"
      "(n0+n1+n2+n3+n4+n5+n6+n7+n8+n9)+"
      "(o0+o1+o2+o3+o4+o5+o6+o7+o8+o9)+"
      "(p0+p1+p2+p3+p4+p5+p6+p7+p8+p9)+"
      "(q0+q1+q2+q3+q4+q5+q6+q7+q8+q9)+"
      "(r0+r1+r2+r3+r4+r5+r6+r7+r8+r9)+"
      "(s0+s1+s2+s3+s4+s5+s6+s7+s8+s9)+"
      "(t0+t1+t2+t3+t4+t5+t6+t7+t8+t9)+"
      "(u0+u1+u2+u3+u4+u5+u6+u7+u8+u9)+"
      "(v0+v1+v2+v3+v4+v5+v6+v7+v8+v9)+"
      "(w0+w1+w2+w3+w4+w5+w6+w7+w8+w9)+"
      "(x0+x1+x2+x3+x4+x5+x6+x7+x8+x9)+"
      "(y0+y1+y2+y3+y4+y5+y6+y7+y8+y9)+"
      "(z0+z1+z2+z3+z4+z5+z6+z7+z8+z9)",
      "a0+a1+a2+a3+a4+a5+a6+a7+a8+a9+b0+b1+b2+b3+b4+b5+b6+b7+b8+b9+c0+c1+c2+c3+"
      "c4+c5+c6+c7+c8+c9+d0+d1+d2+d3+d4+d5+d6+d7+d8+d9+e0+e1+e2+e3+e4+e5+e6+e7+"
      "e8+e9+f0+f1+f2+f3+f4+f5+f6+f7+f8+f9+g0+g1+g2+g3+g4+g5+g6+g7+g8+g9+h0+h1+"
      "h2+h3+h4+h5+h6+h7+h8+h9+i0+i1+i2+i3+i4+i5+i6+i7+i8+i9+j0+j1+j2+j3+j4+j5+"
      "j6+j7+j8+j9+k0+k1+k2+k3+k4+k5+k6+k7+k8+k9+l0+l1+l2+l3+l4+l5+l6+l7+l8+l9+"
      "m0+m1+m2+m3+m4+m5+m6+m7+m8+m9+n0+n1+n2+n3+n4+n5+n6+n7+n8+n9+o0+o1+o2+o3+"
      "o4+o5+o6+o7+o8+o9+p0+p1+p2+p3+p4+p5+p6+p7+p8+p9+q0+q1+q2+q3+q4+q5+q6+q7+"
      "q8+q9+r0+r1+r2+r3+r4+r5+r6+r7+r8+r9+s0+s1+s2+s3+s4+s5+s6+s7+s8+s9+t0+t1+"
      "t2+t3+t4+t5+t6+t7+t8+t9+u0+u1+u2+u3+u4+u5+u6+u7+u8+u9+v0+v1+v2+v3+v4+v5+"
      "v6+v7+v8+v9+w0+w1+w2+w3+w4+w5+w6+w7+w8+w9+x0+x1+x2+x3+x4+x5+x6+x7+x8+x9+"
      "y0+y1+y2+y3+y4+y5+y6+y7+y8+y9+z0+z1+z2+z3+(z4+z5+z6+z7+z8+z9)");
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
        .m_symbolic = SymbolicComputation::ReplaceDefinedSymbols,
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
        .m_symbolic = SymbolicComputation::ReplaceDefinedSymbols,
        .m_context = symbolStore};

    store("0→a", symbolStore);
    projects_and_reduces_to("diff((-1)^(a*x),x,3)", "0", ctxWithSymbol);
  }
}

QUIZ_CASE(pcj_simplification_matrix) {
  simplifies_to("[[1+2]]", "[[3]]");
  simplifies_to("trace([[1,2][3,4]])", "5");
  simplifies_to("identity(2)", "[[1,0][0,1]]");
  simplifies_to("identity(0)", "undef");
  simplifies_to("identity(256)", "undef");
  simplifies_to("identity(-3)", "undef");
  simplifies_to("identity(floor(π)-round(3/2))", "undef");
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
  simplifies_to("trace([[√(2)+log(3)]])", "√(2)+log(3)");
  simplifies_to("transpose([[√(4)]])", "[[2]]");
  simplifies_to("[[0]]*[[0]]", "[[0]]");
  simplifies_to("sum([[k,0]],k,0,5)", "[[15,0]]");
  simplifies_to("sum([[k,0]],k,5,0)", "[[0,0]]");
  simplifies_to("product([[0,k][k,0]],k,1,5)", "[[0,120][120,0]]");
  simplifies_to("product([[0,k][k,0]],k,5,1)", "[[1,0][0,1]]");
  simplifies_to("det([[b]])", "det([[b]])");

  simplifies_to("0*[[2][4]]×[[1,2]]", "[[0,0][0,0]]");
  simplifies_to("0*[[2][inf]]", "[[0][undef]]");
  simplifies_to("0*inf*[[2][3]]", "undef");
  simplifies_to("undef*[[2][4]]", "undef");
  simplifies_to("[[1/0][4]]", "[[undef][4]]");
  simplifies_to("[[1,2]]^2", "undef");
  simplifies_to("[[1]]^π", "undef");

  simplifies_to("transpose(undef)", "undef");
  simplifies_to("transpose(identity(0))", "undef");
  simplifies_to("transpose(identity(-1))", "undef");
  simplifies_to("transpose(inverse([[0]]))", "undef");
  simplifies_to("dot([[1]], inverse([[0]]))", "undef");
  simplifies_to("log(det([[0]]^2))", "undef");
}

QUIZ_CASE(pcj_simplification_complex) {
  ProjectionContext ctx = {
      .m_complexFormat = ComplexFormat::Cartesian,
      .m_symbolic = SymbolicComputation::KeepAllSymbols,
  };
  ProjectionContext PolarCtx = {
      .m_complexFormat = ComplexFormat::Polar,
      .m_symbolic = SymbolicComputation::KeepAllSymbols,
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
  // z being a finite unknown real, arccos(z) is a finite unknown complex.
  simplifies_to("im(x+i*y+arccos(z))", "y+im(arccos(z))", ctx);
  simplifies_to("im(x)", "0", ctx);
  simplifies_to("im(i*y)", "y", ctx);
  simplifies_to("im(arccos(z))", "im(arccos(z))", ctx);
  simplifies_to("im(x1+x2+i*y1+i*y2+arccos(z1)+arccos(z2))",
                "y1+y2+im(arccos(z1))+im(arccos(z2))", ctx);
  simplifies_to("arccos(z)-i×im(arccos(z))", "re(arccos(z))", ctx);
  simplifies_to("i×im(arccos(z))+re(arccos(z))", "arccos(z)", ctx);
  simplifies_to("conj(√(i))", "√(2)/2-√(2)/2×i", ctx);
  simplifies_to("re(conj(arccos(z)))-re(arccos(z))", "0", ctx);
  simplifies_to("conj(conj(arccos(z)))", "arccos(z)", ctx);
  simplifies_to("re(arccos(z)+y)-y", "re(arccos(z))", ctx);
  simplifies_to("re(i×arccos(w))+im(arccos(w))", "0", ctx);
  simplifies_to("im(i×arccos(w))", "re(arccos(w))", ctx);
  simplifies_to("i×(conj(arccos(z)+i×arccos(w))+im(arccos(w))-re(arccos(z)))",
                "im(arccos(z))+re(arccos(w))", ctx);
  simplifies_to("im(re(arccos(z))+i×im(arccos(z)))", "im(arccos(z))", ctx);
  simplifies_to("re(re(arccos(z))+i×im(arccos(z)))", "re(arccos(z))", ctx);
  // TODO: Should simplify to 0
  simplifies_to(
      "abs(arccos(z)+i×arccos(w))^2-(-im(arccos(w))+re(arccos(z)))^2-(im("
      "arccos(z))+re(arccos(w)))^2",
      "im(arccos(z)+arccos(w)×i)^2-(im(arccos(z))+re(arccos(w)))^2", ctx);
  simplifies_to("arg(x+y×i)", "arg(x+y×i)", ctx);
  simplifies_to("arg(π+i×2)", "arctan(2/π)", ctx);
  simplifies_to("arg(-π+i×2)-π", "-arctan(2/π)", ctx);
  simplifies_to("arg(i×2)", "π/2", ctx);
  simplifies_to("arg(-i×2)", "-π/2", ctx);
  simplifies_to("arg(0)", "undef", ctx);
  simplifies_to("π-arg(-π+i×abs(y))", "arctan(abs(y)/π)", ctx);
  simplifies_to("arg(exp(i*π/7))", "π/7", ctx);
  simplifies_to("arg(exp(-i*π/7))", "-π/7", ctx);
  simplifies_to("arg(exp(i*π*10))", "0", ctx);
  simplifies_to("arg(exp(-i*π))", "π", ctx);
  simplifies_to("abs(arccos(z)^2)", "abs(arccos(z))^2", ctx);
  simplifies_to("e^(arg(e^(x×i))×i)", "e^(x×i)", PolarCtx);
  simplifies_to("e^(arg(e^(x×i))×i)", "cos(x)+sin(x)×i", ctx);
  simplifies_to("arg(abs(x)×e^(arg(z)×i))", "dep(arg(z),{nonNull(abs(x))})",
                ctx);
  simplifies_to("arg(-3×(x+y×i))", "arg(-(x+y×i))", ctx);
  // TODO: Simplify arg between ]-π,π]
  simplifies_to("arg(e^(534/7×i))", "arg(e^(534/7×i))");
  simplifies_to("arg(e^(3.1415×i))", "arg(e^(6283/2000×i))");
  // TODO: Improve sign detection
  simplifies_to("abs(-1+π)", "-1+π");
  simplifies_to("abs(1-π)", "-1+π");

  simplifies_to("2/(4+4*i)", "1/4-1/4×i", cartesianCtx);
  simplifies_to("e^(π*i/6)+e^(-π*i/3)", "(1+√(3))/2+(1/2-√(3)/2)×i",
                cartesianCtx);
  simplifies_to("e^(π*i/6)*e^(-π*i/3)", "√(3)/2-1/2×i", cartesianCtx);
  simplifies_to("re(√(√(-8)))", "root(2,4)", cartesianCtx);
  simplifies_to("re(√(15+2×i))", "√((15+√(229))/2)", cartesianCtx);
  simplifies_to("5*ln(1+i)-ln(exp(5*ln(1+i)))", "2×π×i", cartesianCtx);
  simplifies_to("0.3*ln(1+i)-ln(exp(0.3*ln(1+i)))", "0", cartesianCtx);
  simplifies_to("2×cos(π/12)×e^(5×i×π/12)", "1/2+(1+√(3)/2)×i", cartesianCtx);
  simplifies_to("(1+i)^201", "(1+i)^201", cartesianCtx);
}

QUIZ_CASE(pcj_simplification_polar) {
  simplifies_to("0", "0", polarCtx);
  simplifies_to("1", "1", polarCtx);
  simplifies_to("-1", "e^(π×i)", polarCtx);
  simplifies_to("2i", "2×e^(π/2×i)", polarCtx);
  simplifies_to("cos(i)", "cosh(1)", polarCtx);
  simplifies_to("[[42, -2/3][1+i, -iπ]]",
                "[[42,(2×e^(π×i))/3][√(2)×e^(π/4×i),π×e^(-π/2×i)]]", polarCtx);
  simplifies_to("-2×_m", "-2×_m", polarCtx);
  simplifies_to("(-2,i)", "(2×e^(π×i),e^(π/2×i))", polarCtx);
  simplifies_to("{-2,-i}", "{2×e^(π×i),e^(-π/2×i)}", polarCtx);
  simplifies_to("(y/y+3)×e^(i×(x-x+2))", "dep(4×e^(2×i),{y^0})", polarCtx);
  simplifies_to("3+4i", "5×e^(arctan(4/3)×i)", polarCtx);
  simplifies_to("e^(3.14×i)", "e^(157/50×i)", polarCtx);
  simplifies_to("e^(-2.1×i)", "e^(-21/10×i)", polarCtx);
  simplifies_to("root(-8,3)", "2×e^(π/3×i)", polarCtx);
  simplifies_to("e^(π/6×i)+e^(-π/3×i)", "√(2)×e^(-π/12×i)", polarCtx);
  simplifies_to("e^(π/6×i)+e^(-10π/6×i)", "(√(2)/2+√(6)/2)×e^(π/4×i)",
                polarCtx);
  // Not simplified into polars
  simplifies_to("-1+π", "-1+π", polarCtx);
  simplifies_to("1-π", "(-1+π)×e^(π×i)", polarCtx);
  // TODO: Simplify arg between ]-π,π]
  simplifies_to("e^(534/7×i)", "e^(534/7×i)", polarCtx);
  simplifies_to("e^(3.1415×i)", "e^(6283/2000×i)", polarCtx);
}

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
      .m_symbolic = SymbolicComputation::KeepAllSymbols,
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
      .m_symbolic = SymbolicComputation::ReplaceDefinedFunctions,
      .m_context = symbolStore,
  };
  simplifies_to("f'(4)", "8", ctx2);
  simplifies_to("g(4)", "8", ctx2);
  simplifies_to("h(4)", "8", ctx2);
}

QUIZ_CASE(pcj_simplification_factorial) {
  simplifies_to("0!", "1");
  simplifies_to("4!", "24");
  simplifies_to("(-4)!", "undef");
  simplifies_to("(3/5)!", "undef");
  simplifies_to("i!", "undef");
  simplifies_to("π!", "undef");
  simplifies_to("n!", "n!");
  // simplifies_to("(n+1)!/n!", "n+1"); TODO through parametric
}

QUIZ_CASE(pcj_simplification_hyperbolic_trigonometry) {
  simplifies_to("cosh(-x)", "cosh(x)");
  simplifies_to("sinh(-x)", "-sinh(x)");
  simplifies_to("tanh(-x)", "-tanh(x)");
  simplifies_to("arcosh(-x)", "arcosh(-x)");
  // TODO: Should simplify to -arsinh(x)
  simplifies_to("arsinh(-x)", "arsinh(-x)");
  simplifies_to("artanh(-x)", "artanh(-x)");
  simplifies_to("cosh(i)", "cos(1)");
  simplifies_to("sinh(i)", "sin(1)×i", cartesianCtx);
  simplifies_to("tanh(i)", "tan(1)×i", cartesianCtx);
  simplifies_to("arcosh(i)", "arcosh(i)", cartesianCtx);
  simplifies_to("arsinh(i)", "π/2×i", cartesianCtx);
  simplifies_to("artanh(i)", "π/4×i", cartesianCtx);
  simplifies_to("cosh(2*pi+1)", "cosh(1+2×π)", cartesianCtx);
  simplifies_to("cosh(-x)+sinh(x)", "e^(x)");
  simplifies_to("cosh(x)^2-sinh(-x)^2", "1");
  // TODO: Should simplify to 0
  simplifies_to("((1+tanh(x)^2)*tanh(2x)/2)-tanh(x)",
                "-tanh(x)-(-sinh(x)^2/(2×cosh(x)^2)-1/2)×tanh(2×x)");
  simplifies_to("arcosh(5)", "arcosh(5)", cartesianCtx);
  simplifies_to("arcosh(5)-ln(5+√(24))", "0", cartesianCtx);
  simplifies_to("arcosh(cosh(x))", "abs(x)", cartesianCtx);
  simplifies_to("arsinh(sinh(x))", "x", cartesianCtx);
  simplifies_to("artanh(tanh(x))", "x", cartesianCtx);
  simplifies_to("cosh(arcosh(x))", "x", cartesianCtx);
  simplifies_to("sinh(arsinh(x))", "x", cartesianCtx);
  simplifies_to("tanh(artanh(x))", "x", cartesianCtx);
}

QUIZ_CASE(pcj_simplification_advanced_trigonometry) {
  simplifies_to("cot(π/2)", "0");
  simplifies_to("cot(90)", "0", {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("arccot(0)", "90", {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("arccot(0)", "π/2");
  simplifies_to("sec(x)", "1/cos(x)");
  simplifies_to("csc(x)", "1/sin(x)");
  simplifies_to("cot(x)", "cot(x)");
  simplifies_to("arcsec(sec(π/6))", "π/6");
  simplifies_to("arccsc(csc(π/6))", "π/6");
  simplifies_to("arccot(cot(π/6))", "π/6");
  simplifies_to("arccot(-1)", "3π/4");
  simplifies_to("arccot(-1)", "135", {.m_angleUnit = AngleUnit::Degree});

  simplifies_to("csc(arccsc(9/7))", "9/7");
  simplifies_to("csc(arccsc(3/7))", "nonreal");
  simplifies_to("csc(arccsc(3/7))", "3/7", cartesianCtx);
  simplifies_to("sec(arcsec(9/7))", "9/7");
  simplifies_to("sec(arcsec(3/7))", "nonreal");

  simplifies_to("sec(arcsec(x))", "dep(x,{nonNull(x)})", cartesianCtx);
  simplifies_to("csc(arccsc(x))", "dep(x,{nonNull(x)})", cartesianCtx);
  // FIXME : Should be "dep(1+abs(x),{tan(arctan(1+abs(x)))})"
  simplifies_to("cot(arccot(1+abs(x)))", "tan(arctan(1+abs(x)))", cartesianCtx);

  simplifies_to("sin(x)*(cos(x)^-1)*ln(x)",
                "dep(tan(x)×ln(x),{nonNull(x),realPos(x)})");
  simplifies_to("ln(x)*tan(x)", "dep(tan(x)×ln(x),{nonNull(x),realPos(x)})");
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
  simplifies_to("factor(42*3)", "2×3^2×7",
                {.m_complexFormat = ComplexFormat::Polar});
  simplifies_to("105×10^14", "1.05×10^16");
  simplifies_to("factor(105×10^14)", "2^14×3×5^15×7");

  simplifies_to("quo(23,5)", "4");
  simplifies_to("rem(23,5)", "3");
  simplifies_to("gcd(14,28,21)", "7");
  simplifies_to("lcm(14,6)", "42");
  simplifies_to("gcd(6,y,2,x,4)", "gcd(2,x,y)");
  simplifies_to("sign(-2)", "-1");
  simplifies_to("sign(abs(x)+1)", "1");
  simplifies_to("ceil(8/3)", "3");
  simplifies_to("frac(8/3)", "2/3");
  simplifies_to("floor(8/3)", "2");
  simplifies_to("round(1/3,2)", "33/100");
  simplifies_to("round(3.3_m)", "undef");
  simplifies_to("ceil(x)", "ceil(x)");
  simplifies_to("ceil(-x)", "-floor(x)");
  simplifies_to("floor(x)+frac(x)", "dep(x,{floor(x)})");
  simplifies_to("permute(4,2)", "12");
  simplifies_to("binomial(4,2)", "6");
  simplifies_to("1 2/3", "5/3");
  simplifies_to("-1 2/3", "-5/3");

  simplifies_to("floor({1.3,3.9})", "{1,3}");
  // TODO: Approximation is undef
  simplifies_to("ceil(1+i)", "1+ceil(i)");
  // Reductions using approximation
  simplifies_to("floor(π)", "3");
  simplifies_to("frac(π+1)+floor(π+0.1)", "π");
  simplifies_to("log(ceil(2^15+π)-4,2)", "15");
  simplifies_to("frac(871616/2π)×2π", "871616-277442×π");
  simplifies_to("frac(2^24*π)", "-52707178+16777216×π");
  simplifies_to("frac(2^32*π)-π", "4294967295×π+ceil(-4294967296×π)");
  simplifies_to("frac(2^24+π)-π", "-3");
  simplifies_to("frac(sin(10^18))", "frac(sin(10^18))");
  simplifies_to("log(floor(2^54+π)-3, 2)", "54");
  simplifies_to("floor(random())", "floor(random())");
  simplifies_to("floor(sin(0.001))", "0");
  /* Above a certain threshold, we consider that the sine or cosine of a "big"
   * angle has too many approximation errors. This blocks the floor exact
   * reduction.  */
  simplifies_to("floor(cos(1000))", "floor(cos(1000))");
  simplifies_to("sin(frac(frac(exp(6))))", "sin(-403+e^(6))");
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
  ProjectionContext replaceSymbolCtx = {
      .m_symbolic = SymbolicComputation::ReplaceAllSymbols};
  simplifies_to("{1,2}+3", "{4,5}");
  simplifies_to("{1,2}*{3,4}", "{3,8}");
  simplifies_to("{0,1}=0", "{True,False}");
  simplifies_to("{}=1", "{}");
  simplifies_to("sequence(k, k, 2)<0", "{False,False}");
  simplifies_to("sequence(k<0, k, 2)", "{False,False}");
  simplifies_to("sequence(2*k, k, 3)+1", "{3,5,7}");
  simplifies_to("sum(sequence(2*k*t, k, 3)+1, t, 1, 3)", "{15,27,39}");
  simplifies_to("sum(sequence(1, k, 255))", "255");
  simplifies_to("sequence(1, k, 256)", "undef");
  simplifies_to("sequence(k,k,0)", "{}");
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
  simplifies_to("0^({3,4}*2)", "{0,0}");
  simplifies_to("{1,2}/{1,0}", "{1,undef}");
  simplifies_to("med(π*{undef,nonreal,x,3})", "undef");
  simplifies_to("sort(randintnorep(1,4,4))", "sort(randintnorep(1,4,4))");
  simplifies_to("med(sequence(random(),k,10))", "med(sequence(random(),k,10))");
  simplifies_to("sort({π,3,e})", "{e,3,π}");
  simplifies_to("sort({i,2})", "{undef,undef}");
  simplifies_to("sort(sort({3, 1, 2, 4}))", "{1, 2, 3, 4}");
  simplifies_to("max({π,e})", "π");
  simplifies_to("max({π,i})", "undef");
  simplifies_to("min({π,e})", "e");
  simplifies_to("med({π,3,e})", "3");
  // Ensure that [GetElement] returning [nullptr] doesn't crash
  simplifies_to("mean(sort({x,0}))", "undef", replaceSymbolCtx);
  simplifies_to("mean(sort({x,0}))", "mean(sort({x,0}))", keepAllSymbolsCtx);
  simplifies_to("max(sort({x,0}))", "max(sort({x,0}))", keepAllSymbolsCtx);
  simplifies_to("samplestddev({1,2},sort({x,0}))",
                "samplestddev({1,2},sort({x,0}))", keepAllSymbolsCtx);

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
  simplifies_to("{{0}}", "undef");
  simplifies_to("{0,{}}", "undef");
  simplifies_to("{0,{1}}", "undef");
  simplifies_to("{0}=x", "{undef}", replaceSymbolCtx);
  simplifies_to("{0,x}=1", "{False,undef}", replaceSymbolCtx);
  simplifies_to("{0,1,2,3,4,5,6,7,8,{9}}", "undef");
  simplifies_to("sequence(k*_Gs, k, 2)", "undef");
}

QUIZ_CASE(pcj_simplification_random) {
  // TODO: Handle them with {.m_strategy = Strategy::ApproximateToFloat}
  simplifies_to("randintnorep(1,10,5)", "randintnorep(1,10,5)");
  simplifies_to("randintnorep(1,10,11)", "undef");
  simplifies_to("randintnorep(1.5,10, 10)", "undef");
  simplifies_to("random()", "random()");
  simplifies_to("randint(1,10)", "randint(1,10)");
  simplifies_to("diff(random()+1,x,2)", "undef");
  simplifies_to("sum(k+randint(1,10),k,2,5)-14", "sum(randint(1,10),k,2,5)");
  simplifies_to("sequence(2*k+random(),k,3)+1", "1+sequence(2×k+random(),k,3)");
  simplifies_to("random()<acos(40)", "undef");
  Tree* randIntList =
      (KAdd(KRandInt(0_e, KList(0_e, 1_e)), KRandom))->cloneTree();
  simplify(randIntList, realCtx, true);
  assert_trees_are_equal(
      randIntList, KList(KAdd(KRandomSeeded<3>, KRandIntSeeded<1>(0_e, 0_e)),
                         KAdd(KRandomSeeded<3>, KRandIntSeeded<2>(0_e, 1_e))));

  Tree* randIntNoRep = (KRandIntNoRep(0_e, 1000_e, 100_e))->cloneTree();
  simplify(randIntNoRep, realCtx, true);
  assert_trees_are_equal(randIntNoRep,
                         KRandIntNoRepSeeded<1>(0_e, 1000_e, 100_e));
  randIntNoRep = (KRandIntNoRep(0_e, 1000_e, 101_e))->cloneTree();
  simplify(randIntNoRep, realCtx, true);
  /* NOTE: RandIntNoRep size is limited by
   * Random::Context::k_maxNumberOfVariables ->
   * 0-seeded RandIntNoRep means it will approximate to a list of undef */
  assert_trees_are_equal(randIntNoRep,
                         KRandIntNoRepSeeded<0>(0_e, 1000_e, 101_e));

  SharedTreeStack->flushFromBlock(randIntList);
}

QUIZ_CASE(pcj_simplification_power) {
  simplifies_to("1/a", "1/a");
  simplifies_to("1/(1/a)", "dep(a,{nonNull(a)})");
  simplifies_to("1/(a^-3)", "dep(a^3,{nonNull(a)})");
  simplifies_to("a×a^(-1)", "dep(1,{a^0})");
  simplifies_to("a×a^(1+1)", "a^3");
  simplifies_to("2×a^1×(2a)^(-1)", "dep(1,{a^0})");
  simplifies_to("cos(π×a×a^(-1))^(b×b^(-2)×b)", "dep(-1,{a^0,b^0})");
  simplifies_to("2^(64)", "18446744073709551616");
  simplifies_to("2^(64)/2^(63)", "2");
  simplifies_to("0^3.1", "0");
  simplifies_to("0^(-4.2)", "undef");
  simplifies_to("0^(1+x^2)", "0");
  simplifies_to("√(9)", "3");
  simplifies_to("√(-9)", "3×i", cartesianCtx);
  simplifies_to("√(i)", "√(2)/2+√(2)/2×i", cartesianCtx);
  simplifies_to("√(-i)", "√(2)/2-√(2)/2×i", cartesianCtx);
  simplifies_to("√(2eπ)*√(2eπ)", "2×π×e");
  simplifies_to("root(-8,3)", "-2");
  simplifies_to("(cos(x)^2+sin(x)^2-1)^π", "0", cartesianCtx);
  simplifies_to("1-e^(-(0.09/(5.63*10^-7)))", "1-e^(-90000000/563)");
  simplifies_to("(100/3)^(1/3)", "30^(2/3)/3");
  simplifies_to("(200/3)^(1/3)", "(2×15^(2/3))/3");
  simplifies_to("ln((2-√(3))^10)+ln((2+√(3))^10)", "0");
  simplifies_to("√(865)", "√(865)");
  simplifies_to("√(865)/6", "√(865)/6");

  // Real powers
  simplifies_to("√(x)^2", "√(x)^2");
  // - x^y if x is complex or positive
  simplifies_to("41^(1/3)", "root(41,3)");
  // - PowerReal(x,y) y is not a rational
  simplifies_to("x^(e^3)", "x^e^3");
  simplifies_to("(x^e)^3", "(x^e)^3");
  // - Looking at y's reduced rational form p/q :
  //   * PowerReal(x,y) if x is of unknown sign and p odd
  simplifies_to("x^(1/3)", "root(x,3)");
  //   * Unreal if q is even and x negative
  simplifies_to("(-1)^(1/2)", "nonreal");
  //   * |x|^y if p is even
  simplifies_to("(-41)^(4/5)", "41^(4/5)");
  //   * -|x|^y if p is odd
  simplifies_to("(-41)^(5/7)", "-(41^(5/7))");
  // Do not merge PowReal if the exponents are unknown
  simplifies_to("(-1)^y×(-1)^y", "((-1)^y)^2");
  simplifies_to("x^(1/3)×x^(1/3)×x^(1/3)", "root(x,3)^3");

  // Complex Power
  simplifies_to("√(x)^2", "x", cartesianCtx);
  /* TODO: Should be 0, (exp(i*(arg(A) + arg(B) - arg(A*B))) should be
   * simplified to 1 */
  simplifies_to("√(-i-1)*√(-i+1)+√((-i-1)*(-i+1))", "√(-2)+√(-1-i)×√(1-i)",
                cartesianCtx);

  // Expand/Contract
  simplifies_to("e^(ln(2)+π)", "2e^π");
  simplifies_to("√(12)-2×√(3)", "0");
  simplifies_to("3^(1/3)×41^(1/3)-123^(1/3)", "0");
  simplifies_to("√(2)*√(7)-√(14)", "0");
  simplifies_to("x^(1-y^0)", "dep(1,{x^0,y^0})");
  simplifies_to("i^5+i^10+i^15+i^20", "0");

  simplifies_to("2/√(2)", "√(2)");
  simplifies_to("4/√(2)", "2*√(2)");
  simplifies_to("1/√(2)", "√(2)/2");
  simplifies_to("√(2)/2", "√(2)/2");
  simplifies_to("√(-12)/2", "√(3)×i", cartesianCtx);
  simplifies_to("-2+√(-12)/2", "-2+√(3)×i", cartesianCtx);

  // Denesting of square roots
  simplifies_to("√(2+√(3))", "(√(2)+√(6))/2");
  simplifies_to("√(3-√(7))", "√(3-√(7))");
  simplifies_to("√(-2+√(3))", "√(2)×(-1/2+√(3)/2)×i", cartesianCtx);
  simplifies_to("√(-3-√(8))", "(1+√(2))×i", cartesianCtx);
  simplifies_to("√(17+4×√(13))", "2+√(13)");
  simplifies_to("√(√(1058)-√(896))", "root(2,4)×(4-√(7))", cartesianCtx);
  simplifies_to("√(57×√(17)+68×√(10))", "17^(3/4)×(1+(2×√(170))/17)");
  simplifies_to("(-8)^(1/3)-1-√(3)×i", "0", cartesianCtx);
  simplifies_to("√(-3)-√(3)×i", "0", cartesianCtx);
  simplifies_to("(√(2+√(3))+√(2-√(3))×i)^2", "2×√(3)+2i", cartesianCtx);

  // Development of mult and integer power
  simplifies_to("π*(π+1)", "π^2+π");
  simplifies_to("(π+1)^2", "π^2+1+2×π");
  simplifies_to("(π-1)^2", "π^2+1-2×π");
  simplifies_to("(π+1)^3", "π^3+3×π^2+1+3×π");
  simplifies_to("(π+1)^(-2)", "1/(π^2+1+2×π)");
}

QUIZ_CASE(pcj_simplification_float) {
  simplifies_to("2", "2", {.m_strategy = Strategy::ApproximateToFloat});
  simplifies_to("2.3", "2.3", {.m_strategy = Strategy::ApproximateToFloat});
  simplifies_to("1+π", "4.1415926535898",
                {.m_strategy = Strategy::ApproximateToFloat});
  simplifies_to("1+π+x", "x+4.1415926535898",
                {.m_strategy = Strategy::ApproximateToFloat});
  simplifies_to("cos(x-x)", "1", {.m_strategy = Strategy::ApproximateToFloat});
  simplifies_to("random()-random()", "random()-random()",
                {.m_strategy = Strategy::ApproximateToFloat});
  simplifies_to("y^3*x^-2", "y^3/x^2",
                {.m_strategy = Strategy::ApproximateToFloat});

  // This was raising asserts because of float approximation on flatten.
  Tree* u = (KMult(KPow(180_e, -1_e), π_e, KMult(180_de, "x"_e)))->cloneTree();
  SystematicReduction::ShallowReduce(u->child(0));
  QUIZ_ASSERT(SystematicReduction::ShallowReduce(u));
  QUIZ_ASSERT(!SystematicReduction::ShallowReduce(u));
}

QUIZ_CASE(pcj_simplification_constants) {
  simplifies_to("i", "nonreal", realCtx);
  simplifies_to("i", "i", cartesianCtx);
  simplifies_to("π", "π");
  simplifies_to("e", "e");
  simplifies_to("_mn + _mp", "3.34754942651ᴇ-27×_kg");
  simplifies_to("_mn + _G", "undef");
  simplifies_to("_c", "299792458×_m×_s^(-1)");
  simplifies_to("_e", "1.602176634ᴇ-19×_C");
  simplifies_to("_G", "6.6743ᴇ-11×_m^3×_kg^(-1)×_s^(-2)");
  simplifies_to("_g0", "9.80665×_m×_s^(-2)");
  simplifies_to("_k", "1.380649ᴇ-23×_J×_K^(-1)");
  simplifies_to("_ke", "8987551792.3×_m×_F^(-1)");
  simplifies_to("_me", "9.1093837139ᴇ-31×_kg");
  simplifies_to("_mn", "1.67492750056ᴇ-27×_kg");
  simplifies_to("_mp", "1.67262192595ᴇ-27×_kg");
  simplifies_to("_Na", "6.02214076ᴇ23×_mol^(-1)");
  simplifies_to("_R", "8.3144626181532×_J×_K^(-1)×_mol^(-1)");
  simplifies_to("_ε0", "8.8541878188ᴇ-12×_F×_m^(-1)");
  simplifies_to("_μ0", "1.25663706127ᴇ-6×_N×_A^(-2)");
  simplifies_to("_hplanck", "6.62607015ᴇ-34×_s×_J");
}

QUIZ_CASE(pcj_simplification_unit) {
  ProjectionContext replaceSymbolCtx = {
      .m_symbolic = SymbolicComputation::ReplaceAllSymbols};
  simplifies_to("12_m", "12×_m");
  simplifies_to("1_s", "1×_s");
  simplifies_to("1_m+1_s", "undef");
  simplifies_to("1_m+1_yd", "1.9144×_m");
  simplifies_to("1_m+x", "undef");
  simplifies_to("1_m+0", "undef");
  simplifies_to("1_mm+1_km", "1000.001×_m");
  simplifies_to("2_month×7_dm", "3681720×_s×_m");
  simplifies_to("2×_m/_m", "2");
  simplifies_to("1234_g", "1234×_g");
  simplifies_to("cos(0_rad)", "1");
  simplifies_to("sum(_s,x,0,1)", "2×_s");
  // NOTE: We only allow simple int operations in unit exponents
  simplifies_to("_s^-1", "1×_s^(-1)");
  simplifies_to("_s^(4-2^3+2×3)", "1×_s^2");
  simplifies_to("_s^floor(1)", "undef");
  simplifies_to("_s^ceil(2)", "undef");
  // NOTE: this is a test for ReduceSortedAddition, see comment there
  simplifies_to("0×_A + π×_A - π×_A", "0×_A");
  simplifies_to("sum(_s,x,2,0)", "0×_s");
  /* NOTE: Order of units should not matter. If an overflow can occur when
   * reordering the multiplication, we return undef, even if no overflow
   * technically happened */
  simplifies_to("_s^127 * _s^(-1) * _s", "undef");
  simplifies_to("_s^127 * _s * _s^(-1)", "undef");
#if 0
  // See comment in DeepCheckDimensions
  simplifies_to("abs(-3.3_m)", "3.3×_m");
#endif
  simplifies_to("10^(-6)_m^3", "1ᴇ-6×_m^3");
  simplifies_to("1000000_cm", "1000000×_cm");
  simplifies_to("normcdf(0,20,3)×_s", "1.3083978345207ᴇ-11×_s");
  simplifies_to("17_nohm*3+2_nOhm", "53×_nΩ");
  simplifies_to("1/(1/_A)", "1×_A");

  // No unit conversion
  // TODO: should return 1×_m+1×_cm
  simplifies_to("1_m+1_cm", "1×_cm+1×_m", {.m_unitDisplay = UnitDisplay::None});

  // Volumes
  simplifies_to("3_L+2_dL", "3.2×_L");
  simplifies_to("1_L+1_gal", "1.2641720523581×_gal");
  simplifies_to("1_mL+1_m^3", "1.000001×_m^3");
  simplifies_to("4_tsp", "1.3333333333333×_tbsp",
                {.m_unitDisplay = UnitDisplay::AutomaticImperial});
  simplifies_to("3_L*_c", "899377.374×_m^4×_s^(-1)");

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
  simplifies_to("_cKπ23", "72.256631032565×_cK");

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
  simplifies_to("π×_rad×_°", "π^2/180×_rad^2");
  simplifies_to("(_A×x)^2×_s", "undef", replaceSymbolCtx);
  simplifies_to("log(_s×x/(_s×y))",
                "dep(log(x/y),{nonNull(x),nonNull(1/"
                "(1×10^0×y)),realPos((1×10^0×x)/(1×10^0×y))})",
                keepAllSymbolsCtx);

  // BestRepresentative
  simplifies_to("1_m+1_km", "1.001×_km",
                {.m_unitDisplay = UnitDisplay::AutomaticMetric});
  simplifies_to("1ᴇ-9_s", "1×_ns",
                {.m_unitDisplay = UnitDisplay::AutomaticMetric});
  simplifies_to("1234_g", "1.234×_kg",
                {.m_unitDisplay = UnitDisplay::AutomaticMetric});
  simplifies_to("10^(-6)_m^3", "1×_cm^3",
                {.m_unitDisplay = UnitDisplay::AutomaticMetric});
  simplifies_to("12345×_tbsp", "182.54261122453×_L",
                {.m_unitDisplay = UnitDisplay::AutomaticMetric});
  // TODO_PCJ: Should be -173.15×_°C
  simplifies_to("100×_K", "100×_K",
                {.m_unitDisplay = UnitDisplay::AutomaticMetric});
  simplifies_to("3.6×_MN×_m", "3.6×_MJ",
                {.m_unitDisplay = UnitDisplay::AutomaticMetric});

  // TODO: Decide on implicit '_' parsing
  //   simplifies_to("1m+1km", "1_m+1_km" /  "m+k×m" / "m+km" );
  //   simplifies_to("1m+1s", "undef" / "m+s");
  //   simplifies_to("1m+x", "m+x" / "undef");

  // UnitFormat
  simplifies_to("1609.344_m", "1.609344×_km",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::AutomaticMetric});
  simplifies_to("2×π×_cK", "6.2831853071796×_cK",
                {.m_unitFormat = UnitFormat::Imperial});

  // Implicit additions
  simplifies_to("3h300min", "28800×_s");
  simplifies_to("3h300min", "8×_h",
                {.m_unitDisplay = UnitDisplay::AutomaticMetric});

  // 0
  simplifies_to("0×0×2×(_rad + _°)×_°", "0×_rad^2");
  simplifies_to("ln(2/2)_rad", "0×_rad");
  simplifies_to("ln(2/2)_°", "0×_°", {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("5×_°-5×_°", "0×_°",
                {.m_angleUnit = AngleUnit::Radian,
                 .m_unitDisplay = UnitDisplay::AutomaticMetric});
  simplifies_to("0×0×2×(_km + _m)×_km×_s", "0×_m^2×_s");
  simplifies_to("0×_°C", "0×_°C", {.m_unitDisplay = UnitDisplay::None});
  simplifies_to("6×0×_°F", "0×_°F", {.m_unitDisplay = UnitDisplay::None});
  simplifies_to("0×_K", "0×_K");
  simplifies_to("0×_K×_s×_s×(_g+4×_kg)", "0×_s^2×_kg×_K");
  simplifies_to("0×_L-0×_L", "0×_L");
  simplifies_to("3×_dm-3×_dm", "0×_dm");
  simplifies_to("30×_cm-3×_dm", "0×_m");
  simplifies_to("6×_dm-60×_cm", "0×_m");

  // Angles
  simplifies_to("_rad", "1×_rad");
  simplifies_to("360×_°", "2×π×_rad");
  simplifies_to("π×π×_rad", "π^2×_rad");
  simplifies_to("π×π×_rad", "180×π×_°", {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("1×π×_°", "π×_°", {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("π×π×_rad×_m", "9.8696044010894×_m×_rad");
  simplifies_to("π×π×_rad×_rad", "π^2×_rad^2");
  simplifies_to("0.2_rad", "1/5×_rad");
  simplifies_to("-0.2_rad", "-1/5×_rad");
  simplifies_to("0.2_rad^2", "1/5×_rad^2");

  // Decomposition
  simplifies_to("123_m", "undef",
                {.m_unitDisplay = UnitDisplay::Decomposition});
  simplifies_to("1241_yd", "undef",
                {.m_unitDisplay = UnitDisplay::Decomposition});
  simplifies_to("1×_kg", "undef",
                {.m_unitDisplay = UnitDisplay::Decomposition});
  simplifies_to("π_year",
                "3×_year+1×_month+21×_day+6×_h+42×_min+4.3249249999999×_s",
                {.m_unitDisplay = UnitDisplay::Decomposition});
  simplifies_to("61×_s", "1×_min+1×_s",
                {.m_unitDisplay = UnitDisplay::Decomposition});
  simplifies_to("123456×_s", "1×_day+10×_h+17×_min+36×_s",
                {.m_unitDisplay = UnitDisplay::Decomposition});
  simplifies_to("1×_week", "7×_day",
                {.m_unitDisplay = UnitDisplay::Decomposition});
  simplifies_to("100.125_gon", "90×_°+6×_'+45×_\"",
                {.m_unitDisplay = UnitDisplay::Decomposition});
  simplifies_to("35_gon", "31×_°+30×_'",
                {.m_unitDisplay = UnitDisplay::Decomposition});
  simplifies_to("1241_yd", "1241×_yd",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Decomposition});
  simplifies_to("1241_in", "34×_yd+1×_ft+5×_in",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Decomposition});
  simplifies_to("102038_in-1_ft", "1×_mi+1074×_yd+2×_in",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Decomposition});
  simplifies_to("1234567×_in", "19×_mi+853×_yd+1×_ft+7×_in",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Decomposition});
  // TODO_PCJ: Should be 1×_yd+7.700787×_in
  simplifies_to("1.11×_m", "undef",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Decomposition});
  simplifies_to("105454.5_oz", "3×_shtn+590×_lb+14.5×_oz",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Decomposition});
  simplifies_to("1×_lgtn", "1×_shtn+240×_lb",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Decomposition});
  // TODO_PCJ: Should be 2×_lb+3.273962×_oz
  simplifies_to("1×_kg", "undef",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Decomposition});
  simplifies_to("232.8_qt", "58×_gal+1×_pt+1.2×_cup",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Decomposition});
  simplifies_to("12345×_tbsp", "48×_gal+1×_pt+1.5625×_cup",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Decomposition});
  // TODO_PCJ: Should be 264×_gal+1×_pt+0.7528377×_cup
  simplifies_to("1×_m^3", "undef",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Decomposition});

  // Equivalent
  simplifies_to("3_s", "undef", {.m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("3_ft^3", "84.950539776×_L",
                {.m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("3_L", "3×_dm^3", {.m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("3_dm^3", "3×_L", {.m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("1×_m^3", "1000×_L",
                {.m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("5200_m^2", "0.52×_ha",
                {.m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("6.7_ha", "0.067×_km^2",
                {.m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("0_°F", "-17.777777777778×_°C",
                {.m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("23_°F", "-5×_°C", {.m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("-4×_°F", "-20×_°C",
                {.m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("0×_K", "-273.15×_°C",
                {.m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("3_rad", "171.88733853925×_°",
                {.m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("3_°+6_gon", "8.4×_°",
                {.m_unitDisplay = UnitDisplay::Equivalent});
  // TODO_PCJ: Should be 3.6×_km×_h^(-1)
  simplifies_to("1×_m/_s", "undef", {.m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("50_mi/_h", "80.4672×_km×_h^(-1)",
                {.m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("0.3_acre", "13068×_ft^2",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("30000_m^2", "7.413161444015×_acre",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("3000_m^2", "undef",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("3_ft^3", "22.441558441558×_gal",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("2_gal", "462×_in^3",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("35_°C", "95×_°F",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("-11.1×_°C", "12.02×_°F",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("300_K", "80.33×_°F",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("100×_K", "-279.67×_°F",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Equivalent});
  simplifies_to("30_km/_h", "18.64113576712×_mi×_h^(-1)",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Equivalent});
  // TODO_PCJ: Should be 2.236936×_mi×_h^(-1)
  simplifies_to("1×_m/_s", "undef",
                {.m_unitFormat = UnitFormat::Imperial,
                 .m_unitDisplay = UnitDisplay::Equivalent});

  // International System
  simplifies_to("_km", "1000×_m", {.m_unitDisplay = UnitDisplay::BasicSI});
  // TODO_PCJ: Order of units, should be 0.06×_m^(-1)×_s
  simplifies_to("_min/_km", "0.06×_s×_m^(-1)",
                {.m_unitDisplay = UnitDisplay::BasicSI});
  simplifies_to("_km^3", "1000000000×_m^3",
                {.m_unitDisplay = UnitDisplay::BasicSI});
  simplifies_to("1_m+_km", "1001×_m", {.m_unitDisplay = UnitDisplay::BasicSI});
  simplifies_to("_L^2×3×_s", "3ᴇ-6×_m^6×_s",
                {.m_unitDisplay = UnitDisplay::BasicSI});
  simplifies_to("1000000_cm", "10000×_m",
                {.m_unitDisplay = UnitDisplay::BasicSI});
  simplifies_to("(-1/2)×_'", "-π/21600×_rad",
                {.m_unitDisplay = UnitDisplay::BasicSI});
  simplifies_to("-11.1×_°C", "262.05×_K",
                {.m_unitDisplay = UnitDisplay::BasicSI});
  simplifies_to("-4×_°F", "253.15×_K", {.m_unitDisplay = UnitDisplay::BasicSI});

  // Conversions
  simplifies_to("180×_°→_rad", "π×_rad", {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("91.44_cm→_yd", "1×_yd");
}

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
  const Tree* r5 = KDep(1_e, KDepList(KAdd("x"_e, KInf, KOpposite(KInf))));
  simplify(e5, context);
  assert_trees_are_equal(e5, r5);

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
  simplifies_to("inf*(-π)", "-∞");
  simplifies_to("inf*2*inf", "∞");
  simplifies_to("0×inf", "undef");
  simplifies_to("3×inf", "∞");
  simplifies_to("-3×inf", "-∞");
  simplifies_to("inf×(-inf)", "-∞");
  simplifies_to("x×(-inf)", "∞×sign(-x)");
  simplifies_to("(abs(x)+1)*inf", "∞");
  simplifies_to("cos(x)+inf", "∞");
  simplifies_to("1/inf", "0");
  simplifies_to("0/inf", "0");
  simplifies_to("inf×i×i×i×i", "∞");
  simplifies_to("inf×2i", "∞×i", cartesianCtx);
  simplifies_to("-i×inf", "-∞×i", cartesianCtx);
  simplifies_to("inf×cos(3)×i", "∞×sign(cos(3)×i)", cartesianCtx);

  PoincareTest::SymbolStore symbolStore;
  PoincareTest::store("x→f(x)", symbolStore);
  ProjectionContext ctx = {
      .m_symbolic = SymbolicComputation::KeepAllSymbols,
      .m_context = symbolStore,
  };
  simplifies_to("f(x)-inf", "f(x)-∞", ctx);

  // x^inf
  // TODO simplifies_to("(-2)^inf", "undef");  // complex inf
  // TODO simplifies_to("(-2)^(-inf)", "0");
  simplifies_to("(-1)^inf", "undef");
  simplifies_to("(-1)^(-inf)", "undef");
  // TODO simplifies_to("(-0.3)^inf", "0");
  // TODO simplifies_to("(-0.3)^(-inf)", "undef");  // complex inf
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
  simplifies_to("log(inf,0)", "undef");
  simplifies_to("log(-inf,0)", "undef");
  simplifies_to("log(inf,1)", "undef");
  simplifies_to("log(-inf,1)", "undef");
  simplifies_to("log(inf,x)", "dep(∞×sign(1/ln(x)),{nonNull(x),realPos(x)})");
  simplifies_to("log(-inf,x)",
                "dep(nonreal,{nonNull(x),realPos(x),ln(-∞)/ln(x)})");
  simplifies_to("log(-inf,x)", "dep((∞+π×i)/ln(x),{nonNull(x)})", cartesianCtx);
  /* Should be nonreal, TODO return NonReal when evaluating PowReal(x) with x
   * non real */
  simplifies_to("log(inf,-3)", "undef");
  simplifies_to("log(inf,-3)", "∞×sign(1/ln(-3))", cartesianCtx);
  simplifies_to("log(0,inf)", "undef");
  simplifies_to("log(0,-inf)", "undef", cartesianCtx);
  simplifies_to("log(1,inf)", "0");
  simplifies_to("log(1,-inf)", "0", cartesianCtx);
  simplifies_to("log(x,inf)", "dep(0,{0×ln(x),nonNull(x),realPos(x)})");
  simplifies_to("log(x,-inf)", "dep(log(x,-∞),{nonNull(x)})", cartesianCtx);
  simplifies_to("log(inf,inf)", "undef");
  // TODO_PCJ simplifies_to("log(-inf,inf)", "undef", cartesianCtx);
  // TODO_PCJ simplifies_to("log(inf,-inf)", "undef", cartesianCtx);
  // TODO_PCJ simplifies_to("log(-inf,-inf)", "undef", cartesianCtx);
  simplifies_to("ln(inf)", "∞");
  simplifies_to("ln(-inf)", "nonreal");
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
  simplifies_to("cos({π/12,-19π/12})", "{(√(2)+√(6))/4,(√(2)×(-1+√(3)))/4}");
  simplifies_to("sin({π/12,-19π/12})", "{(√(2)×(-1+√(3)))/4,(√(2)+√(6))/4}");
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
                "{√((5-√(5))/8),√((5+√(5))/8),√((5+√(5))/8),√((5-√(5))/8)}");
  // test π/4 in all quadrants
  simplifies_to("cos({π/4,3π/4,-11π/4,7π/4})",
                "{√(2)/2,-√(2)/2,-√(2)/2,√(2)/2}");
  simplifies_to("sin({π/4,3π/4,-11π/4,7π/4})",
                "{√(2)/2,√(2)/2,-√(2)/2,-√(2)/2}");
  simplifies_to("cos(π)", "cos(π)", {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("cos(45)", "√(2)/2", {.m_angleUnit = AngleUnit::Degree});

  // Inverse trigonometry exact formulas
  simplifies_to("acos({-1, -√(3)/2, -√(2)/2, -1/2, 0, 1/2, √(2)/2, √(3)/2, 1})",
                "{π,5π/6,3π/4,2π/3,π/2,π/3,π/4,π/6,0}");
  simplifies_to("acos(-1/√(2))", "3π/4");
  simplifies_to("acos(1/√(2))", "π/4");
  simplifies_to("acos(-(√(6)+√(2))/4)", "11π/12");
  simplifies_to("acos(-(√(6)-√(2))/4)", "7π/12");
  simplifies_to("acos((√(6)-√(2))/4)", "5π/12");
  simplifies_to("acos((√(6)+√(2))/4)", "π/12");
  simplifies_to("asin(-1/√(2))", "-π/4");
  simplifies_to("asin(1/√(2))", "π/4");
  simplifies_to("asin({-1, -√(3)/2, -√(2)/2, -1/2, 0, 1/2, √(2)/2, √(3)/2, 1})",
                "{-π/2,-π/3,-π/4,-π/6,0,π/6,π/4,π/3,π/2}");
  simplifies_to("asin({-1, -√(3)/2, -√(2)/2, -1/2, 0, 1/2, √(2)/2, √(3)/2, 1})",
                "{-90,-60,-45,-30,0,30,45,60,90}",
                {.m_angleUnit = AngleUnit::Degree});

  simplifies_to("asin(-(√(6)+√(2))/4)", "-5π/12");
  simplifies_to("asin(-(√(6)-√(2))/4)", "-π/12");
  simplifies_to("asin((√(6)-√(2))/4)", "π/12");
  simplifies_to("asin((√(6)+√(2))/4)", "5π/12");

  simplifies_to("atan(0)", "0");
  simplifies_to("atan({1,-1}*(inf))", "{π/2, -π/2}");
  simplifies_to("atan({1,-1}*(√(3)))", "{π/3, -π/3}");
  simplifies_to("atan({1,-1}*(1))", "{π/4, -π/4}");
  simplifies_to("atan({1,-1}*(√(5-2√(5))))", "{π/5, -π/5}");
  simplifies_to("atan({1,-1}*(√(5+2√(5))))", "{2π/5, -2π/5}");
  simplifies_to("atan({1,-1}*(√(3)/3))", "{π/6, -π/6}");
  simplifies_to("atan({1,-1}*(-1+√(2)))", "{π/8, -π/8}");
  simplifies_to("atan({1,-1}*(1+√(2)))", "{3π/8, -3π/8}");
  simplifies_to("atan({1,-1}*(√(1-2√(5)/5)))", "{π/10, -π/10}");
  simplifies_to("atan({1,-1}*(√(1+2√(5)/5)))", "{3π/10, -3π/10}");
  simplifies_to("atan({1,-1}*(2-√(3)))", "{π/12, -π/12}");
  simplifies_to("atan({1,-1}*(2+√(3)))", "{5π/12, -5π/12}");

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
  simplifies_to("1/tan(x)", "dep(cot(x),{nonNull(cos(x))})");
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
                "{√(-x^2+1)/x,x/√(-x^2+1),x}", cartesianCtx);
  simplifies_to("tan({acos(-x), asin(-x), atan(-x)})",
                "{-√(-x^2+1)/x,-x/√(-x^2+1),-x}", cartesianCtx);
  simplifies_to("cos({acos(x), asin(x), atan(x)})",
                "{x,√(-x^2+1),cos(arctan(x))}",
                {.m_complexFormat = ComplexFormat::Cartesian,
                 .m_angleUnit = AngleUnit::Degree});
  simplifies_to("cos(acos(3/7))", "3/7");
  simplifies_to("cos(acos(9/7))", "nonreal");
  simplifies_to("cos(acos(9/7))", "9/7", cartesianCtx);
  simplifies_to("sin(asin(3/7))", "3/7");
  simplifies_to("sin(asin(9/7))", "nonreal");
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
  simplifies_to("atan(asin(cos(4)))", "-atan(-4+3π/2)");
  // TODO: fix
  // simplifies_to("atan(tan(π/10))", "π/10");
  simplifies_to("atan(tan(9))", "9", {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("acos(cos(9))", "9", {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("asin(sin(9))", "9", {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("acos(atan(tan(cos(9))))", "9",
                {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("asin(acos(atan(tan(cos(sin(9))))))", "9",
                {.m_angleUnit = AngleUnit::Degree});

  // Angle format with hyperbolic trigonometry
  simplifies_to("cos(2)+cosh(2)+cos(2)", "2×cos(2)+cosh(2)",
                {.m_angleUnit = AngleUnit::Degree});

#if 0
  // TODO: asin(x) = π/2 - acos(x) advanced reduction safe from infinite loops.
  simplifies_to("asin(x)-π/2", "-arccos(x)");
  simplifies_to("90-acos(x)", "arcsin(x)", {.m_angleUnit = AngleUnit::Degree});
#endif
  // TODO: Improve output with better advanced reduction.
  simplifies_to("(y*π+z/180)*asin(x)", "(π×y+z/180)×arcsin(x)",
                {.m_complexFormat = ComplexFormat::Cartesian,
                 .m_angleUnit = AngleUnit::Degree});
  simplifies_to("arg(cos(π/6)+i*sin(π/6))", "π/6");
  simplifies_to("sin(17×π/12)^2+cos(5×π/12)^2", "1", cartesianCtx);
  simplifies_to("sin(17)^2+cos(6)^2", "cos(6)^2+sin(17)^2", cartesianCtx);
  // Only works in cartesian, because Power VS PowerReal. See Projection::Expand
  simplifies_to("cos(atan(x))-√(-(x/√(x^(2)+1))^(2)+1)",
                "dep(0,{0×√(-x^2/(x^2+1)+1)})", cartesianCtx);
  // Strategy
  simplifies_to("sin(90)", "1",
                {.m_angleUnit = AngleUnit::Degree,
                 .m_strategy = Strategy::ApproximateToFloat});

  // Beautification and dependencies
  simplifies_to("re(2*acos(cos(x)))",
                "dep(2×re(arccos(cos(x))),{0×im(arccos(cos(x))),0×im(π×arccos("
                "cos(x))),piecewise(0,abs(cos((180×π×x)/(180×π)))≤1,nonreal)})",
                {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("tan(20)^2*tan(2)", "tan(2)*tan(20)^2");

  // atan(1/x)
  simplifies_to("arctan(1/(1+x^2))", "π/2-arctan(x^2+1)");
  simplifies_to("arctan(-1/(1+abs(x)))", "-π/2+arctan(1+abs(x))");
}

QUIZ_CASE(pcj_simplification_advanced) {
#if 0
  // TODO works but rejected by metric
  simplifies_to("sum(k+n, k, 1, n)", "sum(k, 1, n, k)+n^2");
  simplifies_to("sum(k+1, k, n, n+2)", "6+3×n");
  simplifies_to("sum(k+1, k, n-2, n)", "1");  // FIXME
  simplifies_to("product(k×π, k, 1, 12)", "479001600×π^(12)");

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
  simplifies_to("sum([[k][n]], k, 1, 4)", "[[10][4×n]]");
  simplifies_to("1-cos(x)^2-sin(x)^2", "0");
  simplifies_to("(a+b)^2", "a^2+b^2+2×a×b");
  simplifies_to("a^2+2a*b+b^2-(a+b)^2", "0");
  simplifies_to("(a^2+2a*b+b^2)/(a+b)", "dep(a+b,{nonNull(a+b)})");
  simplifies_to("a^2+b^2-2a*b-(-a+b)^2", "0");
  simplifies_to("1-2a+a^2-(a-1)^2", "0");
  simplifies_to("π^2-2*π*ln(2)+ln(2)^2-(-π+ln(2))^2", "0");
  simplifies_to("√(1/4+π+π^2)", "1/2+π");
  simplifies_to("√(1/4-π+π^2)", "-1/2+π");
  simplifies_to("2*a+b*(a+c)-b*c", "a×(b+2)");
  simplifies_to("e^(a*c)*e^(b*c)+(a+b)^2-a*(a+2*b)", "b^2+e^((a+b)×c)");
  // TODO: Should be 0
  simplifies_to(
      "cos(b)×cos(a)-1/2×cos(b)×cos(a)-1/2×sin(b)×sin(a)+1/2×cos(b)×cos(a)+1/"
      "4×cos(b+a)-1/4×cos(b-a)-cos(a+b)",
      "(3×cos(a)×cos(b))/4-(3×cos(a+b))/4-(3×sin(a)×sin(b))/4");
  simplifies_to("1/(i-1)^2", "1/2×i", cartesianCtx);
  simplifies_to("(x+y)^3-x^3-y^3-3*y^2*x-3*y*x^2", "0");
  // TODO_PCJ: we used to reduce to (π+1)/(π+2)
  simplifies_to("1/(1+1/(1+π))", "1/(1+1/(1+π))");
}

QUIZ_CASE(pcj_simplification_logarithm) {
  simplifies_to("log(3,27)", "1/3");
  simplifies_to("log(27,3)", "3");
  simplifies_to("ln(i)", "π/2×i", cartesianCtx);
  simplifies_to("π×ln(2)+ln(4)", "(2+π)×ln(2)");
  simplifies_to("ln(6)", "ln(2)+ln(3)");
  simplifies_to("log(6)", "log(2)+log(3)");
  simplifies_to("ln(6^2)", "2×ln(2)+2×ln(3)");
  simplifies_to("ln(1/999999)", "-(3×ln(3)+ln(7)+ln(11)+ln(13)+ln(37))");
  simplifies_to("300×ln(2)", "300×ln(2)");
  simplifies_to("ln(ln(25))", "ln(2)+ln(ln(5))");
  simplifies_to("log(log(25))", "log(log(5))+log(2)");
  simplifies_to("ln(5/2)", "-ln(2)+ln(5)");
  simplifies_to("log(2/7)", "log(2)-log(7)");
  simplifies_to("1+ln(x)+ln(y)",
                "dep(1+ln(x)+ln(y),{nonNull(x),nonNull(y),realPos(x),"
                "realPos(y)})");
  simplifies_to("ln(π)-ln(1/π)", "2×ln(π)");
  simplifies_to("cos(x)^2+sin(x)^2-ln(x)",
                "dep(1-ln(x),{nonNull(x),realPos(x)})");
  simplifies_to("1-ln(x)", "dep(1-ln(x),{nonNull(x)})", cartesianCtx);
  simplifies_to("ln(0)", "undef");
  simplifies_to("ln(0)", "undef", cartesianCtx);
  simplifies_to("ln(0^Z)×5", "undef", keepAllSymbolsCtx);
  simplifies_to("ln(cos(x)^2+sin(x)^2)", "dep(0,{nonNull(cos(x)^2+sin(x)^2)})");
  simplifies_to("ln(cos(x)^2+sin(x)^2-1)", "undef");
  simplifies_to("ln(-10)-ln(5)", "ln(2)+π×i", cartesianCtx);
  simplifies_to("im(ln(-120))", "π", cartesianCtx);
  simplifies_to("ln(-1-i)+ln(-1+i)", "ln(2)", cartesianCtx);
  simplifies_to("im(ln(i-2)+ln(i-1))-2π", "im(ln(1-3×i))", cartesianCtx);
  simplifies_to("ln(x)+ln(y)-ln(x×y)",
                "dep(ln(x)+ln(y)-ln(x×y),{nonNull(x),nonNull(y)})",
                cartesianCtx);
  simplifies_to(
      "ln(abs(x))+ln(abs(y))-ln(abs(x)×abs(y))",
      "dep(0,{0×ln(y×abs(x)×sign(y)),nonNull(abs(x)),nonNull(abs(y))})",
      cartesianCtx);
  simplifies_to("log(14142135623731/5000000000000)",
                "log(14142135623731/5000000000000)");

  // Use complex logarithm internally
  simplifies_to("√(x^2)", "√(x^2)", cartesianCtx);
  simplifies_to("√(abs(x)^2)", "abs(x)", cartesianCtx);
  simplifies_to("√(0)", "0", cartesianCtx);
  simplifies_to("√(cos(x)^2+sin(x)^2-1)", "0", cartesianCtx);

  // Simplification with exponential
  simplifies_to("e^(ln(x))", "dep(x,{nonNull(x)})", cartesianCtx);
  simplifies_to("ln(e^x)", "x", cartesianCtx);
  simplifies_to("ln(e^(i×π))", "π×i", cartesianCtx);
  simplifies_to("ln(e^(-i×π))", "π×i", cartesianCtx);
  simplifies_to("ln(e^(i×2×π))", "0", cartesianCtx);

  simplifies_to("log(9,7)", "2×log(3,7)");
  simplifies_to("log(9,8)", "2×log(3,8)");
  simplifies_to("log(3,7)+log(5,7)", "log(3,7)+log(5,7)");
  simplifies_to("log(3,8)+log(5,8)", "log(3,8)+log(5,8)");
}

QUIZ_CASE(pcj_simplification_large_integer_no_crash) {
  simplifies_to("diff(x,x,0,100000000000)", "diff(x,x,0,100000000000)");
  simplifies_to("40000000000ln(10)", "40000000000ln(10)");
}

QUIZ_CASE(pcj_simplification_large_numbers) {
  simplifies_to("123450000000000000000000000000", "1.2345×10^29");
  simplifies_to("123450000000000000000000000000/(5×7)", "(2.469×10^28)/7");
  simplifies_to("π/(12345×10^5)^2", "π/(1.52399025×10^18)");
  simplifies_to("10^14+i", "10^14+i", cartesianCtx);
  simplifies_to("e^(3×10^15)", "e^(3×10^15)");

  /* Edge cases */
  simplifies_to("1×10^12", "1000000000000");
  simplifies_to("1234567890123000", "1.234567890123×10^15");
  simplifies_to("12345678901234×10^15", "12345678901234000000000000000");
  simplifies_to("123456789012300", "123456789012300");
  simplifies_to("1234567890123456789012345678000",
                "1.234567890123456789012345678×10^30");
  // Hidden in Epsilon's calculation app exact results
  simplifies_to("123456789012345678901234567800",
                "123456789012345678901234567800");
  simplifies_to("1234567890123456789012345678901×10^15",
                "1234567890123456789012345678901000000000000000");
}

QUIZ_CASE(pcj_simplification_boolean) {
  simplifies_to("true", "true");
  simplifies_to("true and false", "false");

  simplifies_to("1+1=2", "True");
  simplifies_to("2!=3", "True");
  simplifies_to("2<1", "False");
  simplifies_to("1<2<=2", "True");
  simplifies_to("x≥2", "x>=2");
  simplifies_to("x>y>z", "x>y and y>z");
  simplifies_to("a>b≥c=d≤e<f", "a>b and b>=c and c=d and d<=e and e<f");
  simplifies_to("60>5≥1+3=4≤2+2<50", "True");
  simplifies_to("(x>y)>z", "undef");
  simplifies_to("(x and y)>z", "undef");
  simplifies_to("undef<0", "undef");
  simplifies_to("undef<0<1", "undef");
  simplifies_to("{0,1}<1", "{True,False}");
  simplifies_to("1<{1,2,3}<3", "{False,True,False}");
  simplifies_to("1<2<{1,2,3}<4", "{False,False,True}");
  simplifies_to("{undef,undef}<1", "{undef,undef}");
  simplifies_to("{1,2}<undef", "{undef,undef}");
  simplifies_to("0<{undef,undef}<1", "{undef,undef}");
  simplifies_to("{1,2}<3<undef", "{undef,undef}");
  simplifies_to("0<undef<{1,2}", "{undef,undef}");
  simplifies_to("1<i", "undef");
  simplifies_to("not ((i<1) and {True,False})", "{undef,undef}");
  simplifies_to("undef<{0,1}<1", "{undef,undef}");
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

QUIZ_CASE(pcj_simplification_distributions) {
  simplifies_to("binomcdf(3,5,0.4)", "binomcdf(3,5,2/5)");
  simplifies_to("binompdf(3.5,5,0.4)", "binompdf(3,5,2/5)");
  simplifies_to("normcdf(inf,5,0.4)", "1");
  simplifies_to("normcdf(-inf,5,0.4)", "0");
  simplifies_to("tcdfrange(-inf,inf,5)", "1");
  simplifies_to("tcdfrange(-inf,-inf,5)", "0");
}

QUIZ_CASE(pcj_simplification_function) {
  PoincareTest::SymbolStore symbolStore;
  PoincareTest::store("x→f(x)", symbolStore);
  ProjectionContext projCtx = {
      .m_symbolic = SymbolicComputation::KeepAllSymbols,
      .m_context = symbolStore,
  };
  simplifies_to("f(x)", "f(x)", projCtx);
  simplifies_to("f(2+2)", "f(4)");
  simplifies_to("f(y)+f(x)-f(x)", "dep(f(y),{0×f(x)})");
}

QUIZ_CASE(pcj_simplification_variable_replace) {
  PoincareTest::SymbolStore symbolStore;
  ProjectionContext projCtx = {
      .m_symbolic = SymbolicComputation::ReplaceDefinedSymbols,
      .m_context = symbolStore};

  PoincareTest::store("4→y", symbolStore);
  simplifies_to("x+y", "x+4", projCtx);

  PoincareTest::store("x^2→f(x)", symbolStore);
  simplifies_to("f(z+f(y))", "z^2+32×z+256", projCtx);

  PoincareTest::store("{5,4,3,2,1}→l", symbolStore);
  simplifies_to("sum(l)", "15", projCtx);
  simplifies_to("l(2)", "4", projCtx);
  simplifies_to("l(3^2-4)", "1", projCtx);
  simplifies_to("l(log(3))", "undef", projCtx);
  simplifies_to("l(-2) ", "undef", projCtx);
  simplifies_to("l(0)", "undef", projCtx);
  simplifies_to("l(256)", "undef", projCtx);
  simplifies_to("l(1,3)", "{5,4,3}", projCtx);
  // TODO: Properly parse list slices on variables
  // TODO_PCJ: implement list access and list slice on lists of points

  ProjectionContext projCtx2 = {
      .m_symbolic = SymbolicComputation::ReplaceAllSymbols,
      .m_context = symbolStore};
  simplifies_to("y+2", "6", projCtx2);
  simplifies_to("y+x", "undef", projCtx2);
  simplifies_to("diff(y*y, y, y)", "8", projCtx2);
  simplifies_to("diff(x*x, x, y)", "8", projCtx2);
  simplifies_to("diff(x*x, x, x)", "undef", projCtx2);

  // Local variables may be unit names
  simplifies_to("sum(t,t,1,3)", "6");
}

QUIZ_CASE(pcj_simplification_decimal) {
  Tree* tree = SharedTreeStack->pushDecimal();
  (124_e)->cloneTree();
  (-2_e)->cloneTree();
  ProjectionContext ctx = realCtx;
  simplify(tree, ctx);
  assert_trees_are_equal(tree, 12400_e);
  tree->removeTree();
  tree = SharedTreeStack->pushDecimal();
  (124_e)->cloneTree();
  (2_e)->cloneTree();
  simplify(tree, ctx);
  assert_trees_are_equal(tree, KDiv(31_e, 25_e));
  tree->removeTree();
  // Decimal with integers larger than ints
  simplifies_to(
      ".99999999999999999999999999999999 - 3*.33333333333333333333333333333333",
      "0");
}

QUIZ_CASE(pcj_simplification_rational_power) {
  // TODO: Fix or improve these cases
  // a^b/2 / a^c/2 => a^(b-c)/2
  simplifies_to("3^(3/2)/(3^(7/2))", "1/9");
  simplifies_to("3^(π/2)/(3^(e/2))", "3^(π/2-e/2)");  // "3^((π-e)/2)"
  // 1/(√a+√b) => (√a-√b)/(a-b)
  simplifies_to("1/(√(3)+√(5))",
                "-(√(3)-√(5))/2");  // TODO: Metric "(√(3)-√(5))/2"
  simplifies_to("1/(√(3)-√(5))", "-(√(3)+√(5))/2");
  simplifies_to("1/(√(120)+2√(30))", "√(30)/120");
  // 1/√a => √a/a
  simplifies_to("1/√(3)", "√(3)/3");
  simplifies_to("π^(-3/4)", "1/π^(3/4)");
  // √a/√b <=> √(a/b)
  simplifies_to("√(3)/√(5)-√(3/5)", "0");
  // (c/d)^(a/b) => root(c^a*d^f,b)/d^g
  simplifies_to("(2/3)^(5/7)", "root(288,7)/3");
  simplifies_to("(4/11)^(8/9)", "(2×root(1408,9))/11");
  simplifies_to("(5/2)^(-4/3)", "(2×root(50,3))/25");
  // (1+i)/(1-i) => i
  simplifies_to("(1+i)/(1-i)", "i", cartesianCtx);
  // Big rational
  simplifies_to("0.884^128",
                "12084008767286860548509735467047410159658206261001805757652392"
                "45454687681990772324916971524689144198921421500590345749945393"
                "64666113036736384566543019282479740087711945239256937830014765"
                "59171365003848415067345027145863208965899410032765954252899203"
                "02168954647648296678246317019331284036039922725071361/"
                "86361685550944446253863518628003995711160003644362813850237034"
                "70168591803162427057971507503472288226560547293946149663596995"
                "09894683194669365300377705807477468624711036682128906250000000"
                "00000000000000000000000000000000000000000000000000000000000000"
                "00000000000000000000000000000000000000000000000000000000000");
}

QUIZ_CASE(pcj_euclidean_division) {
#if POINCARE_EUCLIDEAN_DIVISION
  // FIXME simplifies_to fails when the expected expression is not parsable
  // simplifies_to("7⊦2", "Q=3,R=1");
  simplifies_to("2+7⊦2", "5");
  // simplifies_to("(2+7)⊦2", "Q=4,R=1");
  simplifies_to("-3⊦2", "-1");  // Parsed as -(3⊦2)
  // simplifies_to("(-3)⊦2", "Q=3,R=1");
#endif
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
}

QUIZ_CASE(pcj_simplification_roots) {
  simplifies_to("1/√(2+√(3))", "-(√(2)-√(6))/2");
  simplifies_to("1/√(-2×√(3)+3×√(2))", "√(√(2)/2+√(3)/3)");
  simplifies_to("1/√(-4+√(17))", "√(4+√(17))");
  // TODO: simplify the minus sign
  simplifies_to("1/√(-3+√(19))", "√(-(-3-√(19))/10)");
}

void reduces_to_tree(const Tree* input, const Tree* output) {
  Tree* reduced = input->cloneTree();
  simplify(reduced, {}, false);
  assert_trees_are_equal(reduced, output);
  reduced->removeTree();
}

QUIZ_CASE(pcj_simplification_undef) {
  reduces_to_tree(KUndef, KUndef);
  reduces_to_tree(KSub(KInf, KInf), KUndef);
  reduces_to_tree(KSqrt(-1_e), KNonReal);
  reduces_to_tree(KDiv(1_e, 0_e), KOutOfDefinition);
  reduces_to_tree(KPow(0_e, 0_e), KOutOfDefinition);
  reduces_to_tree(KAdd(1_e, KTrue), KUndefUnhandledDimension);
  reduces_to_tree(KAdd(KList(1_e, 2_e), KList(3_e)), KUndefUnhandledDimension);

  // Bubble up
  reduces_to_tree(KDep(1_e, KDepList(KUndef)), KUndef);
  reduces_to_tree(KDep(KUndef, KDepList(KNonReal)), KUndef);
  reduces_to_tree(KDep(KNonReal, KDepList(KUndef)), KUndef);
  reduces_to_tree(KDep(KTrue, KDepList(KUndef)), KUndefBoolean);
  reduces_to_tree(KDep(1_e, KDepList(KUndefBoolean)), KUndef);
  reduces_to_tree(KDep(KUnits::meter, KDepList(KUndef)), KUndefUnit);
  reduces_to_tree(KDep(1_e, KDepList(KUndefUnit)), KUndef);
}

QUIZ_CASE(pcj_simplification_for_approximation_and_analysis) {
  ProjectionContext ctxForApproximation = {
      .m_reductionTarget = ReductionTarget::SystemForApproximation};
  ProjectionContext ctxForAnalysis = {.m_reductionTarget =
                                          ReductionTarget::SystemForAnalysis};
  projects_and_reduces_to("(1+x)^3", "(x+1)^3", ctxForApproximation);
  projects_and_reduces_to("1+3×x+3×x^2+x^3-(1+x)^3", "0", ctxForApproximation);
  projects_and_reduces_to("(1+x)^3", "x^3+3×x^2+3×x+1", ctxForAnalysis);
  projects_and_reduces_to("ln(49000*x)",
                          "dep(ln(49000×x),{nonNull(x),realPos(49000×x)})",
                          ctxForApproximation);
  projects_and_reduces_to("ln(49000*x)",
                          "dep(ln(49000×x),{nonNull(x),realPos(49000×x)})",
                          ctxForAnalysis);
  // TODO: x^2*(1-x^2) might be better for approximation.
  projects_and_reduces_to("x^2-x^4", "-x^4+x^2", ctxForApproximation);
  projects_and_reduces_to("x^2-x^4", "-x^4+x^2", ctxForAnalysis);
}

QUIZ_CASE(pcj_simplification_context) {
  PoincareTest::SymbolStore symbolStore;
  ProjectionContext projCtx = {
      .m_symbolic = SymbolicComputation::ReplaceDefinedSymbols,
      .m_context = symbolStore};

  // Fill variable
  PoincareTest::store("1+2→Adadas", symbolStore);
  simplifies_to("Adadas", "3", projCtx);

  // Fill f1
  PoincareTest::store("1+x→f1(x)", symbolStore);
  simplifies_to("f1(4)", "5", projCtx);
  simplifies_to("f1(Adadas)", "4", projCtx);

  // Fill f2
  PoincareTest::store("x-1→f2(x)", symbolStore);
  simplifies_to("f2(4)", "3", projCtx);
  simplifies_to("f2(Adadas)", "2", projCtx);

  // Define fBoth with f1 and f2
  PoincareTest::store("f1(x)+f2(x)→fBoth(x)", symbolStore);
  simplifies_to("fBoth(4)", "8", projCtx);
  simplifies_to("fBoth(Adadas)", "6", projCtx);

  // Change f2
  PoincareTest::store("x→f2(x)", symbolStore);
  simplifies_to("f2(4)", "4", projCtx);
  simplifies_to("f2(Adadas)", "3", projCtx);

  // Make sure fBoth has changed
  simplifies_to("fBoth(4)", "9", projCtx);
  simplifies_to("fBoth(Adadas)", "7", projCtx);
  symbolStore.reset();

  // Circular variables
  PoincareTest::store("a→b", symbolStore);
  PoincareTest::store("b→a", symbolStore);
  simplifies_to("a", "undef", projCtx);
  simplifies_to("b", "undef", projCtx);
  symbolStore.reset();

  PoincareTest::store("a→b", symbolStore);
  PoincareTest::store("b→c", symbolStore);
  PoincareTest::store("c→a", symbolStore);
  simplifies_to("a", "undef", projCtx);
  simplifies_to("b", "undef", projCtx);
  simplifies_to("c", "undef", projCtx);
  symbolStore.reset();

  // Circular functions
  // f: x → f(x)
  PoincareTest::store("(f(x))→f(x)", symbolStore);
  simplifies_to("f(1)", "undef", projCtx);
  symbolStore.reset();

  PoincareTest::store("1→f(x)", symbolStore);
  PoincareTest::store("f(x)→g(x)", symbolStore);
  PoincareTest::store("g(x)→f(x)", symbolStore);
  simplifies_to("f(1)", "undef", projCtx);
  simplifies_to("g(1)", "undef", projCtx);
  symbolStore.reset();

  PoincareTest::store("1→f(x)", symbolStore);
  PoincareTest::store("f(x)→g(x)", symbolStore);
  PoincareTest::store("g(x)→h(x)", symbolStore);
  PoincareTest::store("h(x)→f(x)", symbolStore);
  simplifies_to("f(1)", "undef", projCtx);
  simplifies_to("g(1)", "undef", projCtx);
  simplifies_to("h(1)", "undef", projCtx);
  symbolStore.reset();

  // Circular variables and functions
  PoincareTest::store("a→b", symbolStore);
  PoincareTest::store("b→a", symbolStore);
  PoincareTest::store("a→f(x)", symbolStore);
  simplifies_to("f(1)", "undef", projCtx);
  simplifies_to("a", "undef", projCtx);
  simplifies_to("b", "undef", projCtx);

  // Composed functions
  // f: x→x^2
  PoincareTest::store("x^2→f(x)", symbolStore);
  // g: x→f(x-2)
  PoincareTest::store("f(x-2)→g(x)", symbolStore);
  simplifies_to("f(2)", "4", projCtx);
  simplifies_to("g(3)", "1", projCtx);
  simplifies_to("g(5)", "9", projCtx);

  // g: x→f(x-2)+f(x+1)
  PoincareTest::store("f(x-2)+f(x+1)→g(x)", symbolStore);
  // Add a sum to bypass simplification
  simplifies_to("g(3)+sum(1, n, 2, 4)", "20", projCtx);
  simplifies_to("g(5)", "45", projCtx);

  // g: x→x+1
  PoincareTest::store("x+1→g(x)", symbolStore);
  simplifies_to("f(g(4))", "25", projCtx);
  // Add a sum to bypass simplification
  simplifies_to("f(g(4))+sum(1, n, 2, 4)", "28", projCtx);

  // Evaluate at undef
  PoincareTest::store("0→f(x)", symbolStore);
  PoincareTest::store("f(1/0)→a", symbolStore);
  simplifies_to("a", "undef", projCtx);
  PoincareTest::store("f(1/0)→g(x)", symbolStore);
  simplifies_to("g(1)", "undef", projCtx);
  PoincareTest::store("f(undef)→b", symbolStore);
  simplifies_to("b", "undef", projCtx);
}
