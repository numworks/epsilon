#include <poincare_junior/include/expression.h>
#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/expression/k_creator.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/layout/k_creator.h>
#include <poincare_junior/src/layout/parsing/rack_parser.h>

#include "helper.h"

using namespace PoincareJ;

QUIZ_CASE(pcj_simplification_expansion) {
  EditionReference ref1(KExp(KAdd(1_e, 2_e, 3_e)));
  quiz_assert(Simplification::ShallowExpand(&ref1));
  assert_trees_are_equal(ref1, KMult(KExp(1_e), KExp(2_e), KExp(3_e)));

  EditionReference ref2(KTrig(KAdd(π_e, "x"_e, "y"_e), 0_e));
  quiz_assert(Simplification::ShallowExpand(&ref2));
  assert_trees_are_equal(
      ref2, KAdd(KMult(KAdd(KMult(KTrig(π_e, 0_e), KTrig("x"_e, 0_e)),
                            KMult(-1_e, KTrig(π_e, 1_e), KTrig("x"_e, 1_e))),
                       KTrig("y"_e, 0_e)),
                 KMult(-1_e,
                       KAdd(KMult(KTrig(π_e, 0_e), KTrig("x"_e, 1_e)),
                            KMult(KTrig(π_e, 1_e), KTrig("x"_e, 0_e))),
                       KTrig("y"_e, 1_e))));

  EditionReference ref3(KExp(KAdd(1_e, 2_e, 3_e)));
  quiz_assert(Simplification::ShallowExpand(&ref3));
  assert_trees_are_equal(ref3, KMult(KExp(1_e), KExp(2_e), KExp(3_e)));

  EditionReference ref4(KAbs(KMult(1_e, 2_e, 3_e)));
  quiz_assert(Simplification::ShallowExpand(&ref4));
  assert_trees_are_equal(ref4, KMult(KAbs(1_e), KAbs(2_e), KAbs(3_e)));

  EditionReference ref5(KLn(KMult(1_e, 2_e, 3_e)));
  quiz_assert(Simplification::ShallowExpand(&ref5));
  assert_trees_are_equal(ref5, KAdd(KLn(1_e), KLn(2_e), KLn(3_e)));
}

QUIZ_CASE(pcj_simplification_contraction) {
  EditionReference ref1(KMult(KExp(1_e), KExp(2_e), KExp(3_e)));
  quiz_assert(Simplification::ShallowContract(&ref1));
  assert_trees_are_equal(ref1, KExp(KAdd(1_e, 2_e, 3_e)));

  EditionReference ref2(
      KMult(KTrig("x"_e, 1_e), KTrig("y"_e, 0_e), KTrig("z"_e, 0_e)));
  quiz_assert(Simplification::ShallowContract(&ref2));
  assert_trees_are_equal(
      ref2,
      KMult(KAdd(KMult(KAdd(KTrig(KAdd("x"_e, KMult(-1_e, "y"_e),
                                       KMult(-1_e, "z"_e)),
                                  1_e),
                            KTrig(KAdd("x"_e, KMult(-1_e, "y"_e), "z"_e), 1_e)),
                       0.5_e),
                 KMult(KAdd(KTrig(KAdd("x"_e, "y"_e, KMult(-1_e, "z"_e)), 1_e),
                            KTrig(KAdd("x"_e, "y"_e, "z"_e), 1_e)),
                       0.5_e)),
            0.5_e));

  EditionReference ref3(KMult(KAbs(1_e), KAbs(2_e), KTrig("x"_e, 1_e),
                              KTrig("y"_e, 0_e), KExp(1_e), KExp(2_e)));
  quiz_assert(Simplification::ShallowContract(&ref3));
  assert_trees_are_equal(
      ref3,
      KMult(KAdd(KMult(KExp(KAdd(1_e, 2_e)),
                       KTrig(KAdd("x"_e, KMult(-1_e, "y"_e)), 1_e)),
                 KMult(KExp(KAdd(1_e, 2_e)), KTrig(KAdd("x"_e, "y"_e), 1_e))),
            KAbs(KMult(1_e, 2_e)), 0.5_e));

  EditionReference ref4(KMult(KAbs(1_e), KAbs(KMult(2_e, 3_e)), KAbs(4_e),
                              KAbs(KMult(5_e, 6_e))));
  quiz_assert(Simplification::ShallowContract(&ref4));
  assert_trees_are_equal(ref4, KAbs(KMult(1_e, 2_e, 3_e, 4_e, 5_e, 6_e)));

  EditionReference ref5(
      KAdd(KLn(1_e), KLn(2_e), KLn(KMult(3_e, 4_e)), 5_e, 6_e));
  quiz_assert(Simplification::ShallowContract(&ref5));
  assert_trees_are_equal(ref5, KAdd(KLn(KMult(1_e, 2_e, 3_e, 4_e)), 5_e, 6_e));

  EditionReference ref6(KPow(KExp("x"_e), 2_e));
  quiz_assert(Simplification::ShallowContract(&ref6));
  assert_trees_are_equal(ref6, KExp(KMult("x"_e, 2_e)));

  EditionReference ref7(KAdd(2_e, KPow(KTrig("x"_e, 0_e), 2_e),
                             KPow(KTrig("x"_e, 1_e), 2_e), 3_e, 4_e));
  quiz_assert(Simplification::ShallowContract(&ref7));
  assert_trees_are_equal(ref7, KAdd(1_e, 2_e, 3_e, 4_e));
}

QUIZ_CASE(pcj_simplification_algebraic_expansion) {
  // A?*(B+C)*D? = A*D*B + A*D*C
  EditionReference ref1(KMult(2_e, KAdd("x"_e, 1_e, 2_e), "y"_e));
  quiz_assert(Simplification::ShallowAlgebraicExpand(&ref1));
  assert_trees_are_equal(ref1,
                         KAdd(KMult(2_e, "x"_e, "y"_e), KMult(2_e, 1_e, "y"_e),
                              KMult(2_e, 2_e, "y"_e)));
  // (A + B)^2 = (A^2 + 2*A*B + B^2)
  EditionReference ref3(KPow(KAdd(KTrig("x"_e, 0_e), KTrig("x"_e, 1_e)), 2_e));
  quiz_assert(Simplification::ShallowAlgebraicExpand(&ref3));
  assert_trees_are_equal(ref3,
                         KAdd(KPow(KTrig("x"_e, 0_e), 2_e),
                              KMult(2_e, KTrig("x"_e, 0_e), KTrig("x"_e, 1_e)),
                              KPow(KTrig("x"_e, 1_e), 2_e)));
  // (A + B + C)^2 = (A^2 + 2*A*B + B^2 + 2*A*C + 2*B*C + C^2)
  EditionReference ref4(KPow(KAdd("x"_e, "y"_e, "z"_e), 2_e));
  quiz_assert(Simplification::ShallowAlgebraicExpand(&ref4));
  assert_trees_are_equal(
      ref4, KAdd(KPow("x"_e, 2_e), KMult(2_e, "x"_e, "y"_e), KPow("y"_e, 2_e),
                 KMult(2_e, "x"_e, "z"_e), KMult(2_e, "y"_e, "z"_e),
                 KPow("z"_e, 2_e)));
}

QUIZ_CASE(pcj_simplification_projection) {
  EditionReference ref1(KCos(KSin(KTan(
      KPow(KPow(KPow(e_e, KLogarithm(KLogarithm(KLog(π_e), 2_e), e_e)), π_e),
           3_e)))));
  Simplification::DeepSystemProjection(&ref1);
  assert_trees_are_equal(
      ref1,
      KTrig(
          KTrig(
              KMult(KTrig(KPow(KExp(KMult(KLn(KExp(KLn(KMult(
                                              KLn(KMult(KLn(π_e),
                                                        KPow(KLn(10_e), -1_e))),
                                              KPow(KLn(2_e), -1_e))))),
                                          π_e)),
                               3_e),
                          1_e),
                    KPow(KTrig(KPow(KExp(KMult(
                                        KLn(KExp(KLn(KMult(
                                            KLn(KMult(KLn(π_e),
                                                      KPow(KLn(10_e), -1_e))),
                                            KPow(KLn(2_e), -1_e))))),
                                        π_e)),
                                    3_e),
                               0_e),
                         -1_e)),
              1_e),
          0_e));

  EditionReference ref2(KAdd(KCos(KSub(2065_e, 2065_e)), KPow(e_e, "x"_e)));
  Simplification::DeepSystemProjection(
      &ref2, {.m_strategy = Strategy::NumbersToFloat});
  assert_trees_are_equal(
      ref2,
      KAdd(KTrig(KAdd(2065.0_e, KMult(-1.0_e, 2065.0_e)), 0.0_e), KExp("x"_e)));
  Simplification::DeepSystemProjection(
      &ref2, {.m_strategy = Strategy::ApproximateToFloat});
  assert_trees_are_equal(ref2, KAdd(1.0_e, KExp("x"_e)));

  EditionReference ref3(KCos(100_e));
  Simplification::DeepSystemProjection(&ref3,
                                       {.m_angleUnit = AngleUnit::Degree});
  assert_trees_are_equal(ref3,
                         KTrig(KMult(100_e, π_e, KPow(180_e, -1_e)), 0_e));

  EditionReference ref4(KSqrt("y"_e));
  Simplification::DeepSystemProjection(&ref4);
  assert_trees_are_equal(ref4, KExp(KMult(0.5_e, KLn("y"_e))));
}

QUIZ_CASE(pcj_simplification_beautify) {
  EditionReference ref1(KAdd(KTrig(3_e, 0_e), KTrig("x"_e, 1_e),
                             KMult(-1_e, 2_e, KExp(KMult(KLn(5_e), "y"_e))),
                             KMult(KLn(2_e), KPow(KLn(4_e), -1_e))));
  Simplification::DeepBeautify(&ref1);
  assert_trees_are_equal(ref1, KAdd(KSub(KAdd(KCos(3_e), KSin("x"_e)),
                                         KMult(2_e, KPow(5_e, "y"_e))),
                                    KLogarithm(2_e, 4_e)));

  EditionReference ref2(KTrig(π_e, 1_e));
  Simplification::DeepBeautify(&ref2, {.m_angleUnit = AngleUnit::Gradian});
  assert_trees_are_equal(ref2, KSin(200_e));

  EditionReference ref3(KExp(KMult(0.5_e, KLn("y"_e))));
  Simplification::DeepBeautify(&ref3);
  assert_trees_are_equal(ref3, KSqrt("y"_e));

  EditionReference ref5(
      KAdd(KMult(-1_e, "w"_e), "x"_e, KMult(-1_e, "y"_e), KMult(-1_e, "z"_e)));
  Simplification::DeepBeautify(&ref5);
  assert_trees_are_equal(
      ref5, KSub(KSub(KAdd(KMult(-1_e, "w"_e), "x"_e), "y"_e), "z"_e));
}

void simplifies_to(const char* input, const char* output) {
  EditionReference inputLayout = Layout::EditionPoolTextToLayout(input);
  EditionReference expression = RackParser(inputLayout).parse();
  inputLayout.removeTree();
  quiz_assert(!expression.isUninitialized());
  Simplification::DeepSystemProjection(&expression);
  quiz_assert(!expression.isUninitialized());
  Simplification::SystematicReduce(&expression);
  quiz_assert(!expression.isUninitialized());
  Simplification::DeepBeautify(&expression);
  quiz_assert(!expression.isUninitialized());
  EditionReference outputLayout =
      Expression::EditionPoolExpressionToLayout(expression);
  quiz_assert(!outputLayout.isUninitialized());
  constexpr size_t bufferSize = 256;
  char buffer[bufferSize];
  *Layout::Serialize(outputLayout, buffer, buffer + bufferSize) = 0;
  outputLayout.removeTree();
  assert(EditionPool::sharedEditionPool()->numberOfTrees() == 0);
  bool b = strcmp(output, buffer) == 0;
  if (!b) {
#ifndef PLATFORM_DEVICE
    std::cout << input << " reduced to " << buffer << " instead of " << output
              << std::endl;
#endif
  }
  quiz_assert(b);
  EditionPool::sharedEditionPool()->flush();
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
  simplifies_to("a*a*a", "a^(3)");
  simplifies_to("a*2a*b*a*b*4", "8*a^(3)*b^(2)");
  simplifies_to("d+c+b+a", "a+b+c+d");
}
