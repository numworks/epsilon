#include <poincare_junior/src/expression/dependency.h>
#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/expression/variables.h>
#include <poincare_junior/src/layout/layoutter.h>

#include "helper.h"

using namespace PoincareJ;

QUIZ_CASE(pcj_simplification_expansion) {
  EditionReference ref1(KExp(KAdd("x"_e, "y"_e, "z"_e)));
  quiz_assert(Simplification::DeepExpand(ref1));
  assert_trees_are_equal(ref1, KMult(KExp("x"_e), KExp("y"_e), KExp("z"_e)));

  EditionReference ref2(KTrig(KAdd(π_e, "x"_e, "y"_e), 0_e));
  Simplification::DeepSystematicReduce(ref2);
  quiz_assert(Simplification::DeepExpand(ref2));
  assert_trees_are_equal(ref2,
                         KAdd(KMult(-1_e, KTrig("x"_e, 0_e), KTrig("y"_e, 0_e)),
                              KMult(KTrig("x"_e, 1_e), KTrig("y"_e, 1_e))));

  EditionReference ref3(KExp(KAdd("x"_e, "y"_e, "z"_e)));
  quiz_assert(Simplification::DeepExpand(ref3));
  assert_trees_are_equal(ref3, KMult(KExp("x"_e), KExp("y"_e), KExp("z"_e)));

  EditionReference ref4(KAbs(KMult("x"_e, "y"_e, "z"_e)));
  quiz_assert(Simplification::DeepExpand(ref4));
  assert_trees_are_equal(ref4, KMult(KAbs("x"_e), KAbs("y"_e), KAbs("z"_e)));

  EditionReference ref5(KLn(KMult("x"_e, "y"_e, "z"_e)));
  quiz_assert(Simplification::DeepExpand(ref5));
  assert_trees_are_equal(ref5, KAdd(KLn("x"_e), KLn("y"_e), KLn("z"_e)));
}

QUIZ_CASE(pcj_simplification_contraction) {
  EditionReference ref1(KMult(KExp("x"_e), KExp("y"_e), KExp("z"_e)));
  quiz_assert(Simplification::DeepContract(ref1));
  assert_trees_are_equal(ref1, KExp(KAdd("x"_e, "y"_e, "z"_e)));

  EditionReference ref2(KMult(KTrig("x"_e, 1_e), KTrig("y"_e, 0_e)));
  quiz_assert(Simplification::DeepContract(ref2));
  assert_trees_are_equal(
      ref2, KMult(KHalf, KAdd(KTrig(KAdd("x"_e, "y"_e), 1_e),
                              KTrig(KAdd("x"_e, KMult(-1_e, "y"_e)), 1_e))));

  EditionReference ref3(KMult(KAbs("x"_e), KAbs("y"_e), KExp("x"_e),
                              KExp("y"_e), KTrig("x"_e, 1_e),
                              KTrig("y"_e, 0_e)));
  quiz_assert(Simplification::DeepContract(ref3));
  assert_trees_are_equal(
      ref3, KMult(KHalf, KAbs(KMult("x"_e, "y"_e)), KExp(KAdd("x"_e, "y"_e)),
                  KAdd(KTrig(KAdd("x"_e, "y"_e), 1_e),
                       KTrig(KAdd("x"_e, KMult(-1_e, "y"_e)), 1_e))));

  EditionReference ref4(KMult(KAbs("a"_e), KAbs(KMult("b"_e, "c"_e)),
                              KAbs("d"_e), KAbs(KMult("e"_e, "f"_e))));
  quiz_assert(Simplification::DeepContract(ref4));
  assert_trees_are_equal(ref4,
                         KAbs(KMult("a"_e, "b"_e, "c"_e, "d"_e, "e"_e, "f"_e)));

  EditionReference ref5(
      KAdd("e"_e, "f"_e, KLn("a"_e), KLn("b"_e), KLn(KMult("c"_e, "d"_e))));
  quiz_assert(Simplification::DeepContract(ref5));
  assert_trees_are_equal(
      ref5, KAdd("e"_e, "f"_e, KLn(KMult("a"_e, "b"_e, "c"_e, "d"_e))));

  EditionReference ref7(KAdd("b"_e, "c"_e, "d"_e, KPow(KTrig("x"_e, 0_e), 2_e),
                             KPow(KTrig("x"_e, 1_e), 2_e)));
  quiz_assert(Simplification::DeepContract(ref7));
  assert_trees_are_equal(ref7, KAdd(1_e, "b"_e, "c"_e, "d"_e));
}

QUIZ_CASE(pcj_simplification_algebraic_expansion) {
  // A?*(B+C)*D? = A*D*B + A*D*C
  EditionReference ref1(KMult("a"_e, KAdd("b"_e, "c"_e, "d"_e), "e"_e));
  quiz_assert(Simplification::DeepExpand(ref1));
  assert_trees_are_equal(
      ref1, KAdd(KMult("a"_e, "b"_e, "e"_e), KMult("a"_e, "c"_e, "e"_e),
                 KMult("a"_e, "d"_e, "e"_e)));
  // (A + B)^2 = (A^2 + 2*A*B + B^2)
  EditionReference ref3(KPow(KAdd(KTrig("x"_e, 0_e), KTrig("x"_e, 1_e)), 2_e));
  quiz_assert(Simplification::DeepExpand(ref3));
  assert_trees_are_equal(ref3,
                         KAdd(KPow(KTrig("x"_e, 0_e), 2_e),
                              KMult(2_e, KTrig("x"_e, 0_e), KTrig("x"_e, 1_e)),
                              KPow(KTrig("x"_e, 1_e), 2_e)));
  // (A + B + C)^2 = (A^2 + 2*A*B + B^2 + 2*A*C + 2*B*C + C^2)
  EditionReference ref4(KPow(KAdd("x"_e, "y"_e, "z"_e), 2_e));
  quiz_assert(Simplification::DeepExpand(ref4));
  assert_trees_are_equal(
      ref4, KAdd(KPow("x"_e, 2_e), KMult(2_e, "x"_e, "y"_e), KPow("y"_e, 2_e),
                 KMult(2_e, "x"_e, "z"_e), KMult(2_e, "y"_e, "z"_e),
                 KPow("z"_e, 2_e)));
}

void simplifies_to(const char* input, const char* output,
                   ProjectionContext projectionContext = {}) {
  EditionReference expected = TextToTree(output);
  EditionReference expression = TextToTree(input);
  Simplification::Simplify(expression, projectionContext);
  quiz_assert(!expression.isUninitialized());
  bool ok = expression->treeIsIdenticalTo(expected);
  if (!ok) {
    EditionReference outputLayout =
        Layoutter::LayoutExpression(expression->clone(), true);
    quiz_assert(!outputLayout.isUninitialized());
    constexpr size_t bufferSize = 256;
    char buffer[bufferSize];
    *Layout::Serialize(outputLayout, buffer, buffer + bufferSize) = 0;
    outputLayout->removeTree();
    bool visuallyOk = strcmp(output, buffer) == 0;
    if (visuallyOk) {
      ok = true;
    } else {
#ifndef PLATFORM_DEVICE
      std::cout << input << " reduced to " << buffer << " instead of " << output
                << std::endl;
#endif
    }
  }
  quiz_assert(ok);
  expression->removeTree();
  expected->removeTree();
  assert(SharedEditionPool->numberOfTrees() == 0);
}

QUIZ_CASE(pcj_basic_simplification) {
  // Default context is Real
  ProjectionContext cartesianCtx = {.m_complexFormat =
                                        ComplexFormat::Cartesian};
  simplifies_to("x", "x");
  simplifies_to("x-x", "0");
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
  simplifies_to("(x^3)^2", "x^(6)");
  simplifies_to("a×a×a", "a^3");
  simplifies_to("a×a×a×b", "b×a^3");
  simplifies_to("a×2a×b×a×b×4", "8×a^3×b^2");
  simplifies_to("1×1×1×1", "1");
  simplifies_to("2a+3b+4a", "6×a+3×b");
  simplifies_to("-6×b-4×a×b-2×b+3×a×b-4×b+2×a×b+3×b+6×a×b", "(7×a-9)×b");
  simplifies_to("d+c+b+a", "a+b+c+d");
  simplifies_to("(a+b)×(d+f)×g-a×d×g-a×f×g", "b×(d+f)×g");
  simplifies_to("a*x*y+b*x*y+c*x", "x×(c+(a+b)×y)");
  simplifies_to("(e^(x))^2", "e^(2×x)");
  simplifies_to("e^(ln(x))", "dep(x,{ln(x),ln(x)})");
  simplifies_to("e^(ln(1+x^2))", "x^2+1");
  simplifies_to("e^(ln(x))", "dep(x,{ln(x)})", cartesianCtx);
  simplifies_to("e^(ln(x+x))", "dep(2×x,{ln(2×x)})", cartesianCtx);
  simplifies_to("diff(x, x, 2)", "1");
  simplifies_to("diff(a×x, x, 1)", "a");
  simplifies_to("diff(23, x, 1)", "0");
  simplifies_to("diff(1+x, x, y)", "1");
  simplifies_to("diff(sin(ln(x)), x, y)",
                "dep(cos(ln(y))/y,{diff(ln(x),x,y)})");
  simplifies_to("diff(((x^4)×ln(x)×e^(3x)), x, y)",
                "dep(3×y^4×e^(3×y)×ln(y)+(e^(3×y)+4×e^(3×y)×ln(y))×y^3,{diff("
                "ln(x),x,y)})");
  simplifies_to("diff(diff(x^2, x, x)^2, x, y)", "8×y");
  simplifies_to("diff(x+x*abs(x), x, y)", "y×diff(abs(x),x,y)+1+abs(y)");
  // TODO: Should be undef
  simplifies_to("diff(ln(x), x, -1)", "dep(-1,{diff(ln(x),x,-1)})");
  // TODO: Metric: 3×abs(x)
  simplifies_to("abs(abs(abs((-3)×x)))", "abs(-3×x)");
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
  simplifies_to("π×ln(2)+ln(4)", "(2+π)×ln(2)");
  simplifies_to("undef", "undef");
  // TODO: Metric: 1+ln(x×y)
  simplifies_to("1+ln(x)+ln(y)", "dep(1+ln(x)+ln(y),{ln(x),ln(y)})");
  // TODO: Metric: 2×ln(π)
  simplifies_to("ln(π)-ln(1/π)", "ln(π^2)");
  simplifies_to("cos(x)^2+sin(x)^2-ln(x)", "dep(1+ln(1/x),{ln(x)})");
  simplifies_to("1-ln(x)", "1+ln(1/x)", cartesianCtx);
  // TODO : Simplify to 1/√(1+x^2).
  simplifies_to("√(-x^2/√(x^2+1)^2+1)", "√(-x^2/(x^2+1)+1)");
  // TODO : Simplify to x/√(-x^2+1)
  simplifies_to("x×(-x^2+1)^(-1/2)", "x×(-x^2+1)^(-1/2)");
  // TODO : Simplify to x
  simplifies_to("(x×(-x^2/√(x^2+1)^2+1)^(-1/2))/√(x^2+1)",
                "x×(x^2+1)^(-1/2)×(-x^2/(x^2+1)+1)^(-1/2)");
  simplifies_to("(a+b)/2+(a+b)/2", "a+b");
  simplifies_to("(a+b+c)*3/4+(a+b+c)*1/4", "a+b+c");

  // Sort order
  simplifies_to("π*floor(π)/π", "floor(π)");
  simplifies_to("π+floor(π)-π", "floor(π)");
  simplifies_to("π*(-π)/π", "-π");
  simplifies_to("π+1/π-π", "1/π");

  // TODO : Should simplify to undef
  simplifies_to("ln(0)", "ln(0)");
  simplifies_to("ln(0)", "ln(0)", cartesianCtx);

  simplifies_to("ln(cos(x)^2+sin(x)^2)", "0");
  simplifies_to("ln(-10)-ln(5)", "ln(-2)", cartesianCtx);
  simplifies_to("im(ln(-120))", "π", cartesianCtx);
  simplifies_to("sin(17×π/12)^2+cos(5×π/12)^2", "1", cartesianCtx);
  simplifies_to("cos(π)", "cos(π)", {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("cos(45)", "2^(-1/2)", {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("(cos(x)^2+sin(x)^2-1)^π", "0", cartesianCtx);
  // Matrices
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
  simplifies_to("[[1,2][3,4]]×[[2,3][4,5]]", "[[10,13][22,29]]");
  simplifies_to("norm([[2,3,6]])", "7");
  simplifies_to("dot([[1,2,3]],[[4,5,6]])", "32");
  simplifies_to("cross([[1,2,3]],[[4,5,6]])", "[[-3,6,-3]]");
  // Power
  simplifies_to("1/a", "1/a");
  simplifies_to("a×a^(-1)", "dep(1,{1/a})");
  simplifies_to("a×a^(1+1)", "a^3");
  simplifies_to("a×a^(-1)", "dep(1,{1/a})",
                {.m_complexFormat = ComplexFormat::Real});
  simplifies_to("a×a^(1+1)", "a^3", {.m_complexFormat = ComplexFormat::Real});
  simplifies_to("2×a^1×(2a)^(-1)", "dep(1,{1/a})");
  simplifies_to("cos(π×a×a^(-1))^(b×b^(-2)×b)", "dep(-1,{1/a,1/b})");
  simplifies_to("2^(64)", "18446744073709551616");
  simplifies_to("2^(64)/2^(63)", "2");
  simplifies_to("0^3.1", "0");
  simplifies_to("0^(-4.2)", "undef");
  simplifies_to("0^(1+x^2)", "0");
  // Complexes
  simplifies_to("2×i×i", "-2", cartesianCtx);
  simplifies_to("1+i×(1+i×(1+i))", "0", cartesianCtx);
  simplifies_to("(1+i)×(3+2i)", "1+5×i", cartesianCtx);
  simplifies_to("√(-1)", "i", cartesianCtx);
  simplifies_to("re(2+i×π)", "2", cartesianCtx);
  simplifies_to("im(2+i×π)", "π", cartesianCtx);
  simplifies_to("conj(2+i×π)", "2-π×i", cartesianCtx);
  simplifies_to("re(conj(x))-re(x)", "0", cartesianCtx);
  simplifies_to("conj(conj(x))", "x", cartesianCtx);
  simplifies_to("re(x+im(y))-im(y)", "re(x)", cartesianCtx);
  simplifies_to("re(x)+i×im(x)", "x", cartesianCtx);
  simplifies_to("re(x+i×y)+im(y)", "re(x)", cartesianCtx);
  simplifies_to("im(x+i×y)", "im(x)+re(y)", cartesianCtx);
  simplifies_to("i×(conj(x+i×y)+im(y)-re(x))", "im(x)+re(y)", cartesianCtx);
  simplifies_to("im(re(x)+i×im(x))", "im(x)", cartesianCtx);
  simplifies_to("re(re(x)+i×im(x))", "re(x)", cartesianCtx);
  simplifies_to("abs(x+i×y)", "√((-im(y)+re(x))^2+(im(x)+re(y))^2)",
                cartesianCtx);
  simplifies_to("arg(re(x)+i×re(y))", "arg(re(x)+re(y)×i)", cartesianCtx);
  simplifies_to("arg(π+i×2)", "arctan(2/π)", cartesianCtx);
  simplifies_to("arg(-π+i×2)", "π+arctan(-2/π)", cartesianCtx);
  simplifies_to("arg(i×2)", "π/2", cartesianCtx);
  simplifies_to("arg(-i×2)", "-π/2", cartesianCtx);
  simplifies_to("arg(0)", "undef", cartesianCtx);
  simplifies_to("arg(-π+i×abs(y))", "π+arctan(-abs(y)/π)", cartesianCtx);
  // Parametrics
  simplifies_to("sum(n, k, 1, n)", "n^2");
  simplifies_to("product(p, k, m, n)", "p^(-m+n+1)");
  simplifies_to("sum((2k)^2, k, 2, 5)", "216");
  simplifies_to("sum(k^2, k, 2, 5)", "54");
  simplifies_to("2×sum(k, k, 0, n)+n", "n^2 + 2n");
  simplifies_to("2×sum(k, k, 3, n)+n", "n^2+2×n-6");
  // Hyperbolic trigonometry
  simplifies_to("cosh(-x)+sinh(x)", "e^x");
  simplifies_to("cosh(x)^2-sinh(-x)^2", "1");
  // TODO: Should simplify to 0
  simplifies_to("((1+tanh(x)^2)*tanh(2x)/2)-tanh(x)",
                "-(-1+e^(2×x))/(1+e^(2×x))+((1+(-1+e^(2×x))^2/"
                "(1+e^(2×x))^2)×(-1+e^(4×x)))/(2×(1+e^(4×x)))");
  // TODO: Should simplify to log(5+2√(6))
  simplifies_to("arcosh(5)", "ln(5+√(24))", cartesianCtx);
  // TODO: Should simplify to x
  simplifies_to("arsinh(sinh(x))",
                "ln((e^x-e^(-x))/2+√(1/2+(e^(-2×x)+e^(2×x))/4))", cartesianCtx);
  // TODO: Should simplify to x
  simplifies_to(
      "artanh(tanh(x))",
      "ln((1+(-1+e^(2×x))/(1+e^(2×x)))/(1-(-1+e^(2×x))/(1+e^(2×x))))/2",
      cartesianCtx);
  // TODO: Should simplify to x
  simplifies_to("cosh(arcosh(x))",
                "dep((x+√(x-1)×√(x+1)+1/(x+√(x^2-1)))/2,{ln(x+√(x-1)×√(x+1))})",
                cartesianCtx);
  // TODO: Should simplify to x
  simplifies_to("sinh(arsinh(x))",
                "dep((x+√(x^2+1)-1/(x+√(x^2+1)))/2,{ln(x+√(x^2+1))})",
                cartesianCtx);
  // TODO: Should simplify to x
  simplifies_to("tanh(artanh(x))",
                "dep((-1+(x+1)/(-x+1))/(1+(x+1)/(-x+1)),{ln(x+1)})",
                cartesianCtx);
  // Advanced trigonometry
  simplifies_to("sec(x)", "1/cos(x)");
  simplifies_to("csc(x)", "1/sin(x)");
  simplifies_to("cot(x)", "cos(x)/sin(x)");
  simplifies_to("arcsec(sec(π/6))", "π/6");
  simplifies_to("arccsc(csc(π/6))", "π/6");
  simplifies_to("arccot(cot(π/6))", "π/6");
  simplifies_to("sec(arcsec(x))", "x");
  simplifies_to("csc(arccsc(x))", "x");
  // TODO: Should simplify to x
  simplifies_to("cot(arccot(x))", "sin(arctan(x))/cos(arctan(x))",
                cartesianCtx);

  // Arithmetic
  simplifies_to("quo(23,5)", "4");
  simplifies_to("rem(23,5)", "3");
  simplifies_to("gcd(14,28,21)", "7");
  simplifies_to("lcm(14,6)", "42");
  simplifies_to("factor(42*3)", "2×3^2×7");
  simplifies_to("gcd(6,y,2,x,4)", "gcd(2,x,y)");

  simplifies_to("sign(-2)", "-1");
  simplifies_to("ceil(8/3)", "3");
  simplifies_to("frac(8/3)", "2/3");
  simplifies_to("round(1/3,2)", "33/100");
  simplifies_to("round(3.3_m)", "3×_m");

  // simplifies_to("ceil(x)", "ceil(x)"); // pb metric
  simplifies_to("ceil(-x)", "-floor(x)");
  simplifies_to("floor(x)+frac(x)", "x");

  simplifies_to("4!", "24");
  simplifies_to("permute(4,2)", "12");
  simplifies_to("binomial(4,2)", "6");
  // simplifies_to("(n+1)!/n!", "n+1");

  // Lists
  simplifies_to("{1,2}+3", "{4,5}");
  simplifies_to("{1,2}*{3,4}", "{3,8}");
  simplifies_to("sequence(2*k, k, 3)+1", "{3,5,7}");
  simplifies_to("mean({1,3*x,2})", "x+1");
  simplifies_to("sum({1,3*x,2})", "3*x+3");
  simplifies_to("min({1,-4/7,2,-2})", "-2");
  simplifies_to("var(sequence(k,k,6))", "35/12");
  simplifies_to("sort({2+1,1,2})", "{1,2,3}");
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

  // Random
  // TODO : Handle them with {.m_strategy = Strategy::ApproximateToFloat}
  simplifies_to("randintnorep(1,10,5)", "randintnorep(1,10,5)");
  simplifies_to("random()", "random()");
  simplifies_to("randint(1,10)", "randint(1,10)");
  simplifies_to("diff(random()+1,x,2)", "undef");
  simplifies_to("sum(k+randint(1,10),k,2,5)-14", "sum(randint(1,10),k,2,5)");
  simplifies_to("sequence(2*k+random(),k,3)+1", "1+sequence(2×k+random(),k,3)");
}

QUIZ_CASE(pcj_power_simplification) {
  // Real powers
  // - x^y if x is complex or positive
  simplifies_to("41^(1/3)", "41^(1/3)");
  // - PowerReal(x,y) y is not a rational
  simplifies_to("x^(e^3)", "x^(e^3)");
  // - Looking at y's reduced rational form p/q :
  //   * PowerReal(x,y) if x is of unknown sign and p odd
  simplifies_to("x^(1/3)", "x^(1/3)");
  //   * Unreal if q is even and x negative
  simplifies_to("(-1)^(1/2)", "nonreal");
  //   * |x|^y if p is even
  simplifies_to("(-41)^(4/5)", "41^(4/5)");
  //   * -|x|^y if p is odd
  simplifies_to("(-41)^(5/7)", "-41^(5/7)");

  simplifies_to("√(x)^2", "√(x)^2", {.m_complexFormat = ComplexFormat::Real});
  // Complex Power
  simplifies_to("√(x)^2", "dep(x,{ln(x)})",
                {.m_complexFormat = ComplexFormat::Cartesian});
  // TODO : Should be 0
  simplifies_to("√(-i-1)*√(-i+1)+√((-i-1)*(-i+1))", "2×√(-2)",
                {.m_complexFormat = ComplexFormat::Cartesian});

  // Power expand/Contract
  simplifies_to("e^(ln(2)+π)", "2e^π");
  simplifies_to("√(12)-2×√(3)", "0");
  simplifies_to("3^(1/3)×41^(1/3)-123^(1/3)", "0");
  simplifies_to("√(2)*√(7)-√(14)", "0");

  simplifies_to("x^(1-y^0)", "dep(1,{1/x,1/y})");
}

QUIZ_CASE(pcj_variables) {
  QUIZ_ASSERT(Variables::GetUserSymbols(0_e)->treeIsIdenticalTo(KSet()));
  const Tree* e = KMult(
      KAdd(KSin("y"_e), KSum("x"_e, 2_e, 4_e, KPow("z"_e, "x"_e))), "m"_e);
  QUIZ_ASSERT(Variables::GetUserSymbols(e)->treeIsIdenticalTo(
      KSet("m"_e, "y"_e, "z"_e)));
}

QUIZ_CASE(pcj_float_simplification) {
  simplifies_to("2", "2", {.m_strategy = Strategy::ApproximateToFloat});
  simplifies_to("2.3", "2.3", {.m_strategy = Strategy::ApproximateToFloat});
  simplifies_to("1+π", "4.1415926535898",
                {.m_strategy = Strategy::ApproximateToFloat});
  simplifies_to("1+π+x", "x+4.1415926535898",
                {.m_strategy = Strategy::ApproximateToFloat});
  simplifies_to("cos(x-x)", "1", {.m_strategy = Strategy::ApproximateToFloat});
  simplifies_to("random()-random()", "random()+-1×random()",
                {.m_strategy = Strategy::ApproximateToFloat});

  // This was raising asserts because of float approximation on flatten.
  Tree* u = (KMult(KPow(180_e, -1_e), π_e, KMult(180_de, "x"_e)))->clone();
  Simplification::ShallowSystematicReduce(u->child(0));
  QUIZ_ASSERT(Simplification::ShallowSystematicReduce(u));
  QUIZ_ASSERT(!Simplification::ShallowSystematicReduce(u));
}

QUIZ_CASE(pcj_unit_simplification) {
  simplifies_to("12_m", "12×_m");
  simplifies_to("1_s", "1×_s");
  simplifies_to("1_m+1_s", "undef");
  simplifies_to("1_m+x", "undef");
  simplifies_to("1_mm+1_km", "1.000001×_km");
  // simplifies_to("2_month×7_dm", "3681720×_s×_m");
  simplifies_to("2×_m/_m", "2");
  simplifies_to("1234_g", "1.234×_kg");
  simplifies_to("cos(0_rad)", "1");

  simplifies_to("4_°C", "4×_°C");
  // Note: this used to be undef in previous Poincare.
  simplifies_to("((4-2)_°C)×2", "4×_°C");
  simplifies_to("((4-2)_°F)×2", "4×_°F");
  simplifies_to("8_°C/2", "4×_°C");

  simplifies_to("2_K+2_K", "4×_K");
  simplifies_to("2_K×2_K", "4×_K^2");
  simplifies_to("1/_K", "1×_K^(-1)");
  simplifies_to("(2_K)^2", "4×_K^2");

  simplifies_to("2_°C-1_°C", "undef");
  simplifies_to("2_°C+2_K", "undef");
  simplifies_to("2_K+2_°C", "undef");
  simplifies_to("2_K×2_°C", "undef");
  simplifies_to("1/_°C", "undef");
  simplifies_to("(1_°C)^2", "undef");
  simplifies_to("tan(2_m)", "undef");
  simplifies_to("tan(2_rad^2)", "undef");
  simplifies_to("π×_rad×_°", "π^2/180×_rad^2");

  simplifies_to("sum(_s,x,0,1)", "2×_s");

  // BestRepresentative
  simplifies_to("1_m+1_km", "1.001×_km");
  simplifies_to("1ᴇ-9_s", "1×_ns");

  // TODO: Decide on implicit '_' parsing
  //   simplifies_to("1m+1km", "1_m+1_km" /  "m+k×m" / "m+km" );
  //   simplifies_to("1m+1s", "undef" / "m+s");
  //   simplifies_to("1m+x", "m+x" / "undef");

  // UnitFormat
  simplifies_to("1609.344_m", "1×_mi", {.m_unitFormat = UnitFormat::Imperial});
}

QUIZ_CASE(pcj_dependencies) {
  Tree* e1 = KAdd(KDep(KMult(2_e, 3_e), KSet(0_e)), 4_e)->clone();
  const Tree* r1 = KDep(KAdd(KMult(2_e, 3_e), 4_e), KSet(0_e));
  Dependency::ShallowBubbleUpDependencies(e1);
  QUIZ_ASSERT(e1->treeIsIdenticalTo(r1));

  Tree* e2 = KAdd(KDep(KMult(2_e, 3_e), KSet(0_e)), 4_e, KDep(5_e, KSet(6_e)))
                 ->clone();
  const Tree* r2 = KDep(KAdd(KMult(2_e, 3_e), 4_e, 5_e), KSet(0_e, 6_e));
  Dependency::ShallowBubbleUpDependencies(e2);
  QUIZ_ASSERT(e2->treeIsIdenticalTo(r2));

  Tree* e3 = KAdd(2_e, KPow("a"_e, 0_e))->clone();
  const Tree* r3 = KDep(3_e, KSet(KDiv(1_e, "a"_e)));
  Simplification::Simplify(e3);
  QUIZ_ASSERT(e3->treeIsIdenticalTo(r3));

  Tree* e4 =
      KDiff("x"_e, "y"_e, KDep("x"_e, KSet(KAbs("x"_e), "z"_e)))->clone();
  const Tree* r4 = KDep(1_e, KSet(KDiff("x"_e, "y"_e, KAbs("x"_e)), "z"_e));
  Simplification::Simplify(e4);
  QUIZ_ASSERT(e4->treeIsIdenticalTo(r4));
}

QUIZ_CASE(pcj_infinity) {
  simplifies_to("∞", "∞");
  simplifies_to("-∞+1", "-∞");
  simplifies_to("∞*(-π)", "-∞");
  simplifies_to("x-∞", "x+-∞");
  simplifies_to("1/∞", "0");
  simplifies_to("2^-∞", "0");
  simplifies_to("0^∞", "0");
  simplifies_to("e^-∞", "0");
  simplifies_to("inf^x", "e^(∞×x)");
  simplifies_to("log(inf,x)", "dep(∞/ln(x),{ln(x)})");
  simplifies_to("0×∞", "undef");
  simplifies_to("∞-∞", "undef");
  simplifies_to("cos(∞)", "undef");
  // FIXME: These cases should be undef
  // simplifies_to("1^∞", "1"); // TODO false on device
  simplifies_to("∞^0", "1");
  simplifies_to("log(inf,-3)", "undef");
}

QUIZ_CASE(pcj_trigonometry) {
  // Trigonometry identities
  simplifies_to("cos(0)", "1");
  simplifies_to("sin(π)", "0");
  simplifies_to("cos(π)", "-1");
  simplifies_to("cos(7×π/12)", "-(2^(-1/2)×(-1+√(3)))/2");
  simplifies_to("cos(13×π/12)", "-(2^(-1/2)×(1+√(3)))/2");
  simplifies_to("sin(π/3)", "√(3)/2");
  simplifies_to("cos(π×2/3)", "-1/2");
  simplifies_to("cos(π×15/4)", "2^(-1/2)");

  simplifies_to("2×sin(2y)×sin(y)+cos(3×y)", "cos(y)");
  simplifies_to("2×sin(2y)×cos(y)-sin(3×y)", "sin(y)");
  simplifies_to("2×cos(2y)×sin(y)+sin(y)", "sin(3×y)");
  simplifies_to("2×cos(2y)×cos(y)-cos(y)", "cos(3×y)");
  simplifies_to("cos(π×7/10)+√(5/8-√(5)/8)", "0",
                {.m_complexFormat = ComplexFormat::Cartesian});
  simplifies_to("arg(cos(π/6)+i*sin(π/6))", "π/6");

  simplifies_to(
      "{cos(π×7/10),cos(π×7/5),cos(π×-7/8),cos(π×11/12),cos(π×13/6),sin(π×7/"
      "10),sin(π×7/5),sin(π×-7/8),sin(π×11/12),sin(π×13/6)}",
      "{-√((5-√(5))/8),-(-1+√(5))/4,-√(2+√(2))/2,-(2^(-1/2)×(1+√(3)))/2,√(3)/"
      "2,(1+√(5))/4,-√((5+√(5))/8),-√(2-√(2))/2,(2^(-1/2)×(-1+√(3)))/2,1/2}");
}

QUIZ_CASE(pcj_inverse_trigonometry) {
  // Only works in cartesian, because Power VS PowerReal. See Projection::Expand
  simplifies_to("cos(atan(x))-√(-(x/√(x^(2)+1))^(2)+1)", "0",
                {.m_complexFormat = ComplexFormat::Cartesian});

  simplifies_to("cos({acos(x), asin(x), atan(x)})",
                "{x,√(-x^2+1),cos(arctan(x))}");
  simplifies_to("sin({acos(x), asin(x), atan(x)})",
                "{√(-x^2+1),x,sin(arctan(x))}");
  simplifies_to("tan({acos(x), asin(x), atan(x)})",
                "{tan(arccos(x)),tan(arcsin(x)),tan(arctan(x))}");

  simplifies_to("acos(cos(x))", "acos(cos(x))");
  simplifies_to("acos({cos(-23*π/7), sin(-23*π/7)})/π", "{5/7,3/14}");
  simplifies_to("acos({cos(π*23/7), sin(π*23/7)})/π", "{5/7,11/14}");
  simplifies_to("asin({cos(-23*π/7), sin(-23*π/7)})/π", "{-3/14,2/7}");
  simplifies_to("asin({cos(π*23/7), sin(π*23/7)})/π", "{-3/14,-2/7}");
  simplifies_to("acos({-1, -√(3)/2, -√(2)/2, -1/2, 0, 1/2, √(2)/2, √(3)/2, 1})",
                "{π,5π/6,3π/4,2π/3,π/2,π/3,π/4,π/6,0}");
  simplifies_to("asin({-1, -√(3)/2, -√(2)/2, -1/2, 0, 1/2, √(2)/2, √(3)/2, 1})",
                "{-π/2,-π/3,-π/4,-π/6,0,π/6,π/4,π/3,π/2}");

  // Other angle units :
  simplifies_to("cos({acos(x), asin(x), atan(x)})",
                "{x,√(-x^2+1),cos(arctan(x))}",
                {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("acos(cos(x))", "acos(cos(x))",
                {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("acos({cos(683), sin(683)})/200", "{117/200,183/200}",
                {.m_angleUnit = AngleUnit::Gradian});
  simplifies_to("asin({-1, -√(3)/2, -√(2)/2, -1/2, 0, 1/2, √(2)/2, √(3)/2, 1})",
                "{-90,-60,-45,-30,0,30,45,60,90}",
                {.m_angleUnit = AngleUnit::Degree});
#if 0
  // TODO: asin(x) = π/2 - acos(x) advanced reduction safe from infinite loops.
  simplifies_to("asin(x)-π/2", "-arccos(x)");
  simplifies_to("90-acos(x)", "arcsin(x)", {.m_angleUnit = AngleUnit::Degree});
#endif
  // TODO: Improve output with better advanced reduction.
  simplifies_to("(y*π+z/180)*asin(x)", "(π×y+z/180)×arcsin(x)",
                {.m_angleUnit = AngleUnit::Degree});
#if 0
  // TODO: Add more exact values.
  simplifies_to(
      "acos({-(√(6)+√(2))/4, -(√(6)-√(2))/4, (√(6)-√(2))/4, (√(6)+√(2))/4})",
      "{11π/12,7π/12,5π/12,π/12}");
  simplifies_to(
      "asin({-(√(6)+√(2))/4, -(√(6)-√(2))/4, (√(6)-√(2))/4, (√(6)+√(2))/4})",
      "{-5π/12,-π/12,π/12,5π/12}");
#endif
}

QUIZ_CASE(pcj_advanced_reduction) {
#if 0
  // TODO works but rejected by metric
  simplifies_to("sum(k+n, k, 1, n)", "sum(k, 1, n, k)+n^2");
  simplifies_to("sum(k+1, k, n, n+2)", "6+3×n");
  simplifies_to("sum(k+1, k, n-2, n)", "1");  // FIXME
  simplifies_to("product(k×π, k, 1, 12)", "479001600×π^(12)");

  // TODO SimplifyAddition on matrices
  simplifies_to("sum([[k][n]], k, 1, 4)", "[[10][4×n]]");


  // Not working yet
  simplifies_to("abs(x^2)", "x^2");

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

  simplifies_to("((abs(x)^(1/2))^(1/2))^8", "abs(x)^2");
  simplifies_to("((x×y)^(1/2)×z^2)^2", "x×y×z^4");
  simplifies_to("1-cos(x)^2", "sin(x)^2");
#endif
  simplifies_to("1-cos(x)^2-sin(x)^2", "0");
  simplifies_to("(a+b)^2", "(a+b)^2");
  simplifies_to("abs(a)*abs(bc)-abs(ab)*abs(c)", "0");
  simplifies_to("2*a+b*(a+c)-b*c", "a×(b+2)");
  simplifies_to("e^(a*c)*e^(b*c)+(a+b)^2-a*(a+2*b)", "b^(2)+e^((a+b)×c)");
#if 0
  /* TODO: This can Expand/contract infinitely and overflow the pool on any
   * strategy */
  simplifies_to(
      "cos(b)×cos(a)-1/2×cos(b)×cos(a)-1/2×sin(b)×sin(a)+1/2×cos(b)×cos(a)+1/"
      "4×cos(b+a)-1/4×cos(b-a)-cos(a+b)",
      "0");
#endif
}
