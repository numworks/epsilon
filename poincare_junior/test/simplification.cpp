#include <poincare_junior/include/expression.h>
#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/layout/k_tree.h>
#include <poincare_junior/src/layout/parsing/rack_parser.h>

#include "helper.h"
#include "poincare_junior/src/expression/variables.h"

using namespace PoincareJ;

QUIZ_CASE(pcj_simplification_expansion) {
  EditionReference ref1(KExp(KAdd("x"_e, "y"_e, "z"_e)));
  quiz_assert(Simplification::ShallowExpand(ref1));
  assert_trees_are_equal(ref1, KMult(KExp("x"_e), KExp("y"_e), KExp("z"_e)));

  EditionReference ref2(KTrig(KAdd(π_e, "x"_e, "y"_e), 0_e));
  Simplification::DeepSystematicReduce(ref2);
  quiz_assert(Simplification::ShallowExpand(ref2));
  assert_trees_are_equal(ref2,
                         KAdd(KMult(-1_e, KTrig("x"_e, 0_e), KTrig("y"_e, 0_e)),
                              KMult(KTrig("x"_e, 1_e), KTrig("y"_e, 1_e))));

  EditionReference ref3(KExp(KAdd("x"_e, "y"_e, "z"_e)));
  quiz_assert(Simplification::ShallowExpand(ref3));
  assert_trees_are_equal(ref3, KMult(KExp("x"_e), KExp("y"_e), KExp("z"_e)));

  EditionReference ref4(KAbs(KMult("x"_e, "y"_e, "z"_e)));
  quiz_assert(Simplification::ShallowExpand(ref4));
  assert_trees_are_equal(ref4, KMult(KAbs("x"_e), KAbs("y"_e), KAbs("z"_e)));

  EditionReference ref5(KLn(KMult("x"_e, "y"_e, "z"_e)));
  quiz_assert(Simplification::ShallowExpand(ref5));
  assert_trees_are_equal(ref5, KAdd(KLn("x"_e), KLn("y"_e), KLn("z"_e)));
}

QUIZ_CASE(pcj_simplification_contraction) {
  EditionReference ref1(KMult(KExp("x"_e), KExp("y"_e), KExp("z"_e)));
  quiz_assert(Simplification::ShallowContract(ref1));
  assert_trees_are_equal(ref1, KExp(KAdd("x"_e, "y"_e, "z"_e)));

  EditionReference ref2(  // 4_e,
      KMult(KTrig("x"_e, 1_e), KTrig("y"_e, 0_e), KTrig("z"_e, 0_e)));
  quiz_assert(Simplification::ShallowContract(ref2));
  assert_trees_are_equal(
      ref2,
      KMult(KHalf,
            KAdd(KMult(KHalf, KAdd(KTrig(KAdd("x"_e, "y"_e, "z"_e), 1_e),
                                   KTrig(KAdd("x"_e, "y"_e, KMult(-1_e, "z"_e)),
                                         1_e))),
                 KMult(KHalf,
                       KAdd(KTrig(KAdd("x"_e, KMult(-1_e, "y"_e), "z"_e), 1_e),
                            KTrig(KAdd("x"_e, KMult(-1_e, "y"_e),
                                       KMult(-1_e, "z"_e)),
                                  1_e))))));

  EditionReference ref3(KMult(KAbs("x"_e), KAbs("y"_e), KExp("x"_e),
                              KExp("y"_e), KTrig("x"_e, 1_e),
                              KTrig("y"_e, 0_e)));
  quiz_assert(Simplification::ShallowContract(ref3));
  assert_trees_are_equal(
      ref3, KMult(KHalf, KAbs(KMult("x"_e, "y"_e)), KExp(KAdd("x"_e, "y"_e)),
                  KAdd(KTrig(KAdd("x"_e, "y"_e), 1_e),
                       KTrig(KAdd("x"_e, KMult(-1_e, "y"_e)), 1_e))));

  EditionReference ref4(KMult(KAbs("a"_e), KAbs(KMult("b"_e, "c"_e)),
                              KAbs("d"_e), KAbs(KMult("e"_e, "f"_e))));
  quiz_assert(Simplification::ShallowContract(ref4));
  assert_trees_are_equal(ref4,
                         KAbs(KMult("a"_e, "b"_e, "c"_e, "d"_e, "e"_e, "f"_e)));

  EditionReference ref5(
      KAdd("e"_e, "f"_e, KLn("a"_e), KLn("b"_e), KLn(KMult("c"_e, "d"_e))));
  quiz_assert(Simplification::ShallowContract(ref5));
  assert_trees_are_equal(
      ref5, KAdd("e"_e, "f"_e, KLn(KMult("a"_e, "b"_e, "c"_e, "d"_e))));

  EditionReference ref7(KAdd("b"_e, "c"_e, "d"_e, KPow(KTrig("x"_e, 0_e), 2_e),
                             KPow(KTrig("x"_e, 1_e), 2_e)));
  quiz_assert(Simplification::ShallowContract(ref7));
  assert_trees_are_equal(ref7, KAdd(1_e, "b"_e, "c"_e, "d"_e));
}

QUIZ_CASE(pcj_simplification_algebraic_expansion) {
  // A?*(B+C)*D? = A*D*B + A*D*C
  EditionReference ref1(KMult("a"_e, KAdd("b"_e, "c"_e, "d"_e), "e"_e));
  quiz_assert(Simplification::ShallowAlgebraicExpand(ref1));
  assert_trees_are_equal(
      ref1, KAdd(KMult("a"_e, "b"_e, "e"_e), KMult("a"_e, "c"_e, "e"_e),
                 KMult("a"_e, "d"_e, "e"_e)));
  // (A + B)^2 = (A^2 + 2*A*B + B^2)
  EditionReference ref3(KPow(KAdd(KTrig("x"_e, 0_e), KTrig("x"_e, 1_e)), 2_e));
  quiz_assert(Simplification::ShallowAlgebraicExpand(ref3));
  assert_trees_are_equal(
      ref3, KAdd(KMult(2_e, KTrig("x"_e, 0_e), KTrig("x"_e, 1_e)),
                 KPow(KTrig("x"_e, 0_e), 2_e), KPow(KTrig("x"_e, 1_e), 2_e)));
  // (A + B + C)^2 = (A^2 + 2*A*B + B^2 + 2*A*C + 2*B*C + C^2)
  EditionReference ref4(KPow(KAdd("x"_e, "y"_e, "z"_e), 2_e));
  quiz_assert(Simplification::ShallowAlgebraicExpand(ref4));
  assert_trees_are_equal(
      ref4, KAdd(KMult(2_e, "x"_e, "y"_e), KMult(2_e, "x"_e, "z"_e),
                 KMult(2_e, "y"_e, "z"_e), KPow("x"_e, 2_e), KPow("y"_e, 2_e),
                 KPow("z"_e, 2_e)));
}

QUIZ_CASE(pcj_simplification_projection) {
  EditionReference ref(KCos(KSin(KTan(
      KPow(KPow(KPow(e_e, KLogarithm(KLogarithm(KLog(π_e), 2_e), e_e)), π_e),
           3_e)))));
  Simplification::DeepSystemProjection(
      ref, {.m_complexFormat = ComplexFormat::Cartesian});
  assert_trees_are_equal(
      ref,
      KTrig(
          KTrig(
              KMult(
                  KTrig(
                      KPow(KPow(KExp(KLn(KMult(
                                    KLn(KMult(KLn(π_e), KPow(KLn(10_e), -1_e))),
                                    KPow(KLn(2_e), -1_e)))),
                                π_e),
                           3_e),
                      1_e),
                  KPow(KTrig(KPow(KPow(KExp(KLn(KMult(
                                           KLn(KMult(KLn(π_e),
                                                     KPow(KLn(10_e), -1_e))),
                                           KPow(KLn(2_e), -1_e)))),
                                       π_e),
                                  3_e),
                             0_e),
                       -1_e)),
              1_e),
          0_e));

  CloneTreeOverTree(ref, KAdd(KCos(KSub(2065_e, 2065_e)), KPow(e_e, "x"_e)));
  Simplification::DeepSystemProjection(
      ref, {.m_complexFormat = ComplexFormat::Cartesian,
            .m_strategy = Strategy::NumbersToFloat});
  assert_trees_are_equal(
      ref,
      KAdd(KTrig(KAdd(2065.0_e, KMult(-1.0_e, 2065.0_e)), 0.0_e), KExp("x"_e)));

  CloneTreeOverTree(ref, KAdd(KCos(KSub(2065_e, 2065_e)), KPow(2_e, "x"_e),
                              KPow(KLn(e_e), KDiv(1_e, 10_e))));
  Simplification::DeepSystemProjection(
      ref, {.m_complexFormat = ComplexFormat::Cartesian,
            .m_strategy = Strategy::ApproximateToFloat});
  assert_trees_are_equal(ref, KAdd(1.0_e, KPow(2.0_e, "x"_e), 1.0_e));

  CloneTreeOverTree(ref, KCos(100_e));
  Simplification::DeepSystemProjection(ref, {.m_angleUnit = AngleUnit::Degree});
  assert_trees_are_equal(ref,
                         KTrig(KMult(100_e, π_e, KPowReal(180_e, -1_e)), 0_e));

  CloneTreeOverTree(ref, KSqrt("y"_e));
  Simplification::DeepSystemProjection(
      ref, {.m_complexFormat = ComplexFormat::Cartesian});
  assert_trees_are_equal(ref, KPow("y"_e, KHalf));

  CloneTreeOverTree(ref, KSqrt("y"_e));
  Simplification::DeepSystemProjection(
      ref, {.m_complexFormat = ComplexFormat::Real});
  assert_trees_are_equal(ref, KPowReal("y"_e, KHalf));

  ref->removeTree();
}

QUIZ_CASE(pcj_simplification_beautify) {
  EditionReference ref1(KAdd(KTrig(3_e, 0_e), KTrig("x"_e, 1_e),
                             KMult(-1_e, 2_e, KExp(KMult(KLn(5_e), "y"_e))),
                             KMult(KLn(2_e), KPow(KLn(4_e), -1_e))));
  Simplification::DeepBeautify(ref1);
  assert_trees_are_equal(ref1, KAdd(KSub(KAdd(KCos(3_e), KSin("x"_e)),
                                         KMult(2_e, KPow(5_e, "y"_e))),
                                    KLogarithm(2_e, 4_e)));

  EditionReference ref2(KTrig(π_e, 1_e));
  Simplification::DeepBeautify(ref2, {.m_angleUnit = AngleUnit::Gradian});
  assert_trees_are_equal(ref2, KSin(200_e));

  EditionReference ref3(KExp(KMult(KHalf, KLn("y"_e))));
  Simplification::DeepBeautify(ref3);
  assert_trees_are_equal(ref3, KSqrt("y"_e));

  EditionReference ref4(KExp(KMult(2.5_e, KLn("y"_e))));
  Simplification::DeepBeautify(ref4);
  assert_trees_are_equal(ref4, KPow("y"_e, 2.5_e));

  EditionReference ref5(
      KAdd(KMult(-1_e, "w"_e), "x"_e, KMult(-1_e, "y"_e), KMult(-1_e, "z"_e)));
  Simplification::DeepBeautify(ref5);
  assert_trees_are_equal(
      ref5, KSub(KSub(KAdd(KMult(-1_e, "w"_e), "x"_e), "y"_e), "z"_e));
}

void simplifies_to(const char* input, const char* output,
                   ProjectionContext projectionContext = {}) {
  EditionReference inputLayout = Layout::EditionPoolTextToLayout(input);
  EditionReference expression = RackParser(inputLayout).parse();
  quiz_assert(!expression.isUninitialized());
  inputLayout->removeTree();
  Simplification::Simplify(expression, projectionContext);
  quiz_assert(!expression.isUninitialized());
  EditionReference outputLayout =
      Expression::EditionPoolExpressionToLayout(expression);
  quiz_assert(!outputLayout.isUninitialized());
  constexpr size_t bufferSize = 256;
  char buffer[bufferSize];
  *Layout::Serialize(outputLayout, buffer, buffer + bufferSize) = 0;
  outputLayout->removeTree();
  assert(SharedEditionPool->numberOfTrees() == 0);
  bool b = strcmp(output, buffer) == 0;
  if (!b) {
#ifndef PLATFORM_DEVICE
    std::cout << input << " reduced to " << buffer << " instead of " << output
              << std::endl;
#endif
  }
  quiz_assert(b);
  SharedEditionPool->flush();
}

QUIZ_CASE(pcj_basic_simplification) {
  simplifies_to("x-x", "0");
  simplifies_to("2+2", "4");
  simplifies_to("(2*3(2^2)) + 2*2", "28");
  simplifies_to("36/8", "9/2");
  simplifies_to("2+36/8+2", "17/2");
  simplifies_to("a+a", "2*a");
  simplifies_to("b+a", "a+b");
  simplifies_to("(a*a)*a", "a^(3)");
  simplifies_to("a*(a*a)", "a^(3)");
  simplifies_to("(a*b)^2", "a^(2)*b^(2)");
  simplifies_to("(a*b*c)^2", "a^(2)*b^(2)*c^(2)");
  // simplifies_to("a*a^(-1)", "1"); // FIXME
  // simplifies_to("2*a^1*(2a)^(-1)", "1");
  simplifies_to("(x^3)^2", "x^(6)");
  simplifies_to("a*a*a", "a^(3)");
  simplifies_to("a*a*a*b", "b*a^(3)");
  simplifies_to("a*2a*b*a*b*4", "8*a^(3)*b^(2)");
  simplifies_to("1*1*1*1", "1");
  simplifies_to("2a+3b+4a", "6*a+3*b");
  simplifies_to("-6*b-4*a*b-2*b+3*a*b-4*b+2*a*b+3*b+6*a*b", "-9*b+7*a*b");
  simplifies_to("d+c+b+a", "a+b+c+d");
  simplifies_to("(a+b)*(d+f)*g-a*d*g-a*f*g", "b*d*g+b*f*g");
  simplifies_to("(e^(x))^2", "e^(2*x)");
  simplifies_to("e^(ln(x))", "x");
  simplifies_to("e^(ln(x+x))", "2*x");
  simplifies_to("diff(x, x, 2)", "1");
  simplifies_to("diff(23, x, 1)", "0");
  simplifies_to("diff(1+x, x, y)", "1");
  simplifies_to("diff(sin(ln(x)), x, y)", "cos(ln(y))*y^(-1)");
  simplifies_to("diff(((x^4)*ln(x)*e^(3x)), x, y)",
                "e^(3*y)*y^(3)+4*e^(3*y)*ln(y)*y^(3)+3*e^(3*y)*ln(y)*y^(4)");
  simplifies_to("diff(diff(x^2, x, x)^2, x, y)", "8*y");
  simplifies_to("abs(abs(abs((-3)*x)))", "3*abs(x)");
  simplifies_to("0.1875", "3/16");
  simplifies_to("0.0001234", "617/5000000");
  simplifies_to("98765000", "98765000");
  simplifies_to("012345.67890", "123456789/10000");
  simplifies_to("012345.67890E5", "1234567890");
  simplifies_to("012345.67890E-3", "123456789/10000000");
  simplifies_to("123456789012345678901234567890",
                "123456789012345678901234567890");
  simplifies_to("5.0", "5");
  simplifies_to("5.", "5");
  simplifies_to("5.E1", "50");
  // Trigonometry identities
  simplifies_to("sin(π)", "0");
  simplifies_to("cos(π)", "-1");
  simplifies_to("cos(7*π/12)", "1/2*2^(-1/2)+-1/2*2^(-1/2)*√(3)");
  simplifies_to("cos(13*π/12)", "-1/2*2^(-1/2)+-1/2*2^(-1/2)*√(3)");
  simplifies_to("sin(π/3)", "1/2*√(3)");
  simplifies_to("cos(π*2/3)", "-1/2");
  simplifies_to("cos(π*15/4)", "2^(-1/2)");
  simplifies_to("2*sin(2y)*sin(y)", "cos(y)-cos(3*y)");
  simplifies_to("2*sin(2y)*cos(y)", "sin(y)+sin(3*y)");
  simplifies_to("2*cos(2y)*sin(y)", "-1*sin(y)+sin(3*y)");
  simplifies_to("2*cos(2y)*cos(y)", "cos(y)+cos(3*y)");
  simplifies_to("ln(0)", "undef");
  simplifies_to("ln(cos(x)^2+sin(x)^2)", "0");
  simplifies_to("sin(17*π/12)^2+cos(5*π/12)^2", "1",
                {.m_complexFormat = ComplexFormat::Cartesian});
  // Matrices
  simplifies_to("[[1+2]]", "[[3]]");
  simplifies_to("trace([[1,2][3,4]])", "5");
  simplifies_to("identity(2)", "[[1,0][0,1]]");
  simplifies_to("transpose([[1][3]])", "[[1,3]]");
  simplifies_to("transpose([[1,2][3,4]])", "[[1,3][2,4]]");
  simplifies_to("dim([[1,2][3,4][5,6]])", "[[3,2]]");
  simplifies_to("ref([[1,2][3,4]])", "[[1,4/3][0,1]]");
  simplifies_to("rref([[1,2][3,4]])", "[[1,0][0,1]]");
  simplifies_to("det([[1,2][3,4]])", "-2");
  simplifies_to("inverse([[1,2][3,4]])", "[[-2,1][3/2,-1/2]]");
  simplifies_to("[[1,2][3,4]]^5", "[[1069,1558][2337,3406]]");
  simplifies_to("[[1,2][3,4]]^-1", "[[-2,1][3/2,-1/2]]");
  simplifies_to("[[1,2][3,4]]^0 - identity(2)", "[[0,0][0,0]]");
  simplifies_to("trace(identity(3))", "3");
  simplifies_to("2+[[3]]", "undef");
  simplifies_to("[[2]]+[[3]]", "[[5]]");
  simplifies_to("2*[[3]]", "[[6]]");
  simplifies_to("[[1,2][3,4]]*[[2,3][4,5]]", "[[10,13][22,29]]");
  simplifies_to("norm([[1,2,3]])", "√(14)");
  simplifies_to("dot([[1,2,3]],[[4,5,6]])", "32");
  simplifies_to("cross([[1,2,3]],[[4,5,6]])", "[[-3,6,-3]]");
  simplifies_to("N*M", "N*M");
  simplifies_to("N+M", "M+N");
  simplifies_to("N^2*M", "N^(2)*M");
  simplifies_to("N^2*M^2", "N^(2)*M^(2)");
  simplifies_to("n^2*m", "m*n^(2)");
  simplifies_to("det(M)", "det(M)");
  // Power
  simplifies_to("a*a^(-1)", "1");
  simplifies_to("a*a^(1+1)", "a^(3)");
  simplifies_to("a*a^(-1)", "1", {.m_complexFormat = ComplexFormat::Real});
  simplifies_to("a*a^(1+1)", "a^(3)", {.m_complexFormat = ComplexFormat::Real});
  simplifies_to("2^(64)", "18446744073709551616");
  simplifies_to("2^(64)/2^(63)", "2");
  // Complexes
  simplifies_to("2*i*i", "-2");
  simplifies_to("1+i*(1+i*(1+i))", "0");
  simplifies_to("(1+i)*(3+2i)", "1+5*i");
  simplifies_to("√(-1)", "i", {.m_complexFormat = ComplexFormat::Cartesian});
  simplifies_to("re(2+i*π)", "2");
  simplifies_to("im(2+i*π)", "π");
  simplifies_to("conj(2+i*π)", "2-π*i");
  simplifies_to("re(conj(x))-re(x)", "0");
  simplifies_to("conj(conj(x))", "x");
  simplifies_to("re(x+im(y))", "im(y)+re(x)");
  simplifies_to("re(x)+i*im(x)", "x");
  simplifies_to("re(x+i*y)", "-1*im(y)+re(x)");
  simplifies_to("im(x+i*y)", "im(x)+re(y)");
  simplifies_to("conj(x+i*y)", "-1*im(y)+re(x)-(im(x)+re(y))*i");
  simplifies_to("im(re(x)+i*im(x))", "im(x)");
  simplifies_to("re(re(x)+i*im(x))", "re(x)");
  simplifies_to("abs(x+i*y)",
                "√(-2*im(y)*re(x)+2*im(x)*re(y)+im(x)^(2)+im(y)^(2)+re(x)^(2)+"
                "re(y)^(2))");
}

QUIZ_CASE(pcj_power_simplification) {
  // Real powers
  // - x^y if x is complex or positive
  simplifies_to("123^(1/3)", "123^(1/3)");
  // - PowerReal(x,y) y is not a rational
  simplifies_to("x^(e^(3))", "x^(e^(3))");
  // - Looking at y's reduced rational form p/q :
  //   * PowerReal(x,y) if x is of unknown sign and p odd
  simplifies_to("x^(1/3)", "x^(1/3)");
  //   * Unreal if q is even and x negative
  simplifies_to("(-1)^(1/2)", "undef");
  //   * |x|^y if p is even
  simplifies_to("(-123)^(4/5)", "123^(4/5)");
  //   * -|x|^y if p is odd
  simplifies_to("(-123)^(5/7)", "-1*123^(5/7)");

  simplifies_to("√(x)^2", "√(x)^(2)", {.m_complexFormat = ComplexFormat::Real});
  // Complex Power
  simplifies_to("√(x)^2", "x", {.m_complexFormat = ComplexFormat::Cartesian});
}

QUIZ_CASE(pcj_variables) {
  QUIZ_ASSERT(Variables::GetVariables(0_e, KSet()));
  const Tree* e = KMult(
      "x"_e, KAdd(KSin("y"_e), KDiff(KPow(3_e, "x"_e), "x"_e, 2_e)), "m"_e);
  QUIZ_ASSERT(Variables::GetVariables(e, KSet("m"_e, "x"_e, "y"_e)));
}
