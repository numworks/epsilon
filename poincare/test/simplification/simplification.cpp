#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/systematic_reduction.h>
#include <poincare/src/expression/units/k_units.h>
#include <poincare/src/expression/variables.h>
#include <poincare/src/memory/tree_stack.h>

#include "../helpers/symbol_store.h"
#include "helper.h"

using namespace Poincare::Internal;
using Poincare::ComplexFormat;
using Poincare::ProjectionContext;
using Poincare::ReductionTarget;
using Poincare::Strategy;
using Poincare::SymbolicComputation;
using Poincare::UnitDisplay;
using UnitFormat = Poincare::Preferences::UnitFormat;

QUIZ_CASE(pcj_simplification_expansion) {
  expand_to(KExp(KAdd("x"_e, "y"_e, "z"_e)),
            KMult(KExp("x"_e), KExp("y"_e), KExp("z"_e)));
  expand_to(KTrig(KAdd(π_e, "x"_e, "y"_e), 0_e),
            KDep(KAdd(KMult(-1_e, KTrig("x"_e, 0_e), KTrig("y"_e, 0_e)),
                      KMult(KTrig("x"_e, 1_e), KTrig("y"_e, 1_e))),
                 KDepList(KMult(0_e, KTrig(KAdd("x"_e, "y"_e), 1_e)))));
  expand_to(KLn(KMult(2_e, π_e)), KAdd(KLn(2_e), KLn(π_e)));

  // Pushing the rational on the stack because it can't be made a KTree.
  // 40000000/18358803711643
  Tree* bigRational = KMult.node<2>->cloneNode();
  (40000000_e)->cloneTree();
  KPow->cloneNode();
  (18358803711643_e)->cloneTree();
  (-1_e)->cloneTree();
  SystematicReduction::DeepReduce(bigRational);
  assert(bigRational->isRational());
  // ln(40000000/18358803711643)+ln(21)
  Tree* input = KAdd.node<2>->cloneNode();
  KLn->cloneNode();
  bigRational->cloneTree();
  KLn->cloneNode();
  (21_e)->cloneTree();
  // ln(40000000/18358803711643)+ln(3)+ln(7)
  Tree* expected = KAdd.node<3>->cloneNode();
  KLn->cloneNode();
  bigRational->cloneTree();
  KLn->cloneNode();
  (3_e)->cloneTree();
  KLn->cloneNode();
  (7_e)->cloneTree();
  expand_to(input, expected);
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

QUIZ_CASE(pcj_simplification_percent) {
  // % are left unreduced on purpose to show their exact formula
  simplifies_to("-25%", "-25/100");
  simplifies_to("2↗30%", "2×(1+30/100)");
  simplifies_to("-2-30%", "(-2)×(1-30/100)");
  simplifies_to("x-30%", "x×(1-30/100)",
                {.m_strategy = Strategy::ApproximateToFloat});
  simplifies_to("20%", "20/100");
  simplifies_to("20%%", "(20/100)/100");
  simplifies_to("80*20%", "80×20/100");
  simplifies_to("80/(20%)", "80/(20/100)");
  simplifies_to("80+20%", "80×(1+20/100)");
  simplifies_to("20%+80+20%", "(80+20/100)×(1+20/100)");
  simplifies_to("80+20%+20%", "80×(1+20/100)×(1+20/100)");
  simplifies_to("80-20%", "80×(1-20/100)");
  simplifies_to("80+20-20%", "100×(1-20/100)");
  simplifies_to("80+10*20%", "80+10×20/100");
  simplifies_to("80-10*20%", "80-10×20/100");
  simplifies_to("80+20%*10", "80+10×20/100");
  simplifies_to("80-20%*10", "80-10×20/100");
  simplifies_to("80+20%π", "80+π×20/100");
}

QUIZ_CASE(pcj_simplification_random) {
  simplifies_to("1/random()+1/3+1/4", "7/12+1/random()");
  simplifies_to("random()+random()", "random()+random()");
  simplifies_to("random()-random()", "random()-random()");
  simplifies_to("abs(random()-random())", "abs(random()-random())");
  simplifies_to("1/random()+1/3+1/4+1/random()", "7/12+1/random()+1/random()");
  simplifies_to("random()×random()", "random()×random()");
  simplifies_to("random()/random()", "random()/random()");
  simplifies_to("3^random()×3^random()", "3^(random()+random())");
  simplifies_to("random()×ln(2)×3+random()×ln(2)×5",
                "(3×random()+5×random())×ln(2)");

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

  // TODO_PCJ: used to simplify to 1
  simplifies_to("randint(1,1)", "randint(1,1)");
  //  Randint is not simplified if ReductionTarget = SystemForApproximation
  simplifies_to_no_beautif(
      "randint(1,3)", "randint(1,3)",
      {.m_reductionTarget = ReductionTarget::SystemForApproximation});
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

QUIZ_CASE(pcj_simplification_point) {
  simplifies_to("(1,2)", "(1,2)");
  simplifies_to("({1,3},{2,4})", "{(1,2),(3,4)}");
  simplifies_to("sequence((k,k+1),k,3)", "{(1,2),(2,3),(3,4)}");
  simplifies_to("(undef,2)", "(undef,2)");
  simplifies_to("0*(1,2)", "undef");
  simplifies_to("(1/0,2)", "(undef,2)");
  simplifies_to("(1,2)+3", "undef");
  simplifies_to("abs((1.23,4.56))", "undef");
  simplifies_to("{(1+2,3+4),(5+6,7+8)}", "{(3,7),(11,15)}");
  simplifies_to("sequence((k,-k+1),k,4)", "{(1,0),(2,-1),(3,-2),(4,-3)}");
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
  simplifies_to("piecewise(3,1<0,2)", "2");
  simplifies_to("piecewise(3,1>0,2)", "3");
  simplifies_to("piecewise(3,0>1,4,0>2,5,0<6,2)", "5");
  simplifies_to("piecewise(3,0<1,4,0<2,5,0<6,2)", "3");

  simplifies_to("piecewise(3,1<0,2,3=4)", "undef");
  simplifies_to("piecewise(3,1<0,undef)", "undef");
  simplifies_to("piecewise(3,1>0,undef)", "3");
  simplifies_to("piecewise(-x/x,x>0,0)", "piecewise(dep(-1,{x^0}),x>0,0)");
  simplifies_to("piecewise(3,4>0,2,2<a)", "undef",
                {.m_symbolic = SymbolicComputation::ReplaceAllSymbols});
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

QUIZ_CASE(pcj_simplification_roots) {
  simplifies_to("1/√(2+√(3))", "-(√(2)-√(6))/2");
  simplifies_to("1/√(-2×√(3)+3×√(2))", "√(√(2)/2+√(3)/3)");
  simplifies_to("1/√(-4+√(17))", "√(4+√(17))");
  // TODO: simplify the minus sign
  simplifies_to("1/√(-3+√(19))", "√(-(-3-√(19))/10)");
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

QUIZ_CASE(pcj_simplification_user_function) {
  PoincareTest::SymbolStore symbolStore;
  ProjectionContext projCtx = {
      .m_symbolic = SymbolicComputation::ReplaceDefinedSymbols,
      .m_context = symbolStore};
  // User defined function
  // f: x → x*3
  store("x*3→f(x)", symbolStore);
  simplifies_to("f(1+1)", "6", projCtx);
  simplifies_to("f({2,3})", "{6,9}", projCtx);
  // f: x → 3
  store("3→f(x)", symbolStore);
  simplifies_to("f(1/0)", "undef", projCtx);
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

QUIZ_CASE(pcj_simplification_store) {
  simplifies_to("1+2→x", "3→x");
  simplifies_to("0.2→f(x)", "0.2→f(x)",
                {.m_reductionTarget = ReductionTarget::SystemForAnalysis,
                 .m_symbolic = SymbolicComputation::ReplaceDefinedSymbols});
  simplifies_to("0.1+0.2→x", "3/10→x");
  simplifies_to("a→x", "a→x");
  simplifies_to("a→x", "undef→x",
                {.m_symbolic = SymbolicComputation::ReplaceAllSymbols});
}

QUIZ_CASE(pcj_simplification_store_correctly_parsed) {
  PoincareTest::SymbolStore symbolStore;
  ProjectionContext projCtx = {
      .m_complexFormat = ComplexFormat::Cartesian,
      .m_symbolic = SymbolicComputation::ReplaceDefinedSymbols,
      .m_context = symbolStore};
  simplifies_to("abc", "a×b×c", projCtx, false);
  simplifies_to("\"abc\"", "\"abc\"", projCtx, false);
  store("2→a", symbolStore);
  store("5→bc", symbolStore);
  simplifies_to("abc", "10", projCtx, false);  // a*, falsebc
  simplifies_to("aa", "4", projCtx, false);
  store("10→aa", symbolStore);
  simplifies_to("aa", "10", projCtx, false);
  simplifies_to("aaa", "20", projCtx, false);     // Parsed to a*aa
  simplifies_to("aaaaa", "200", projCtx, false);  // Parsed to a*aa*aa
  simplifies_to("acos(b)", "arccos(b)", projCtx, false);
  simplifies_to("aacos(b)", "2×arccos(b)", projCtx, false);
  store("t→bar(t)", symbolStore);
  store("8→foo", symbolStore);
  simplifies_to("foobar(x)", "8×x", projCtx, false);
  store("t^2→foobar(t)", symbolStore);
  simplifies_to("foobar(x)", "x^2", projCtx, false);

  simplifies_to("t", "1×_t", projCtx, false);
  store("2→t", symbolStore);
  simplifies_to("t", "2", projCtx, false);
}
