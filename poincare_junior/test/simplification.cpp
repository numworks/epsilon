#include <poincare_junior/include/expression.h>
#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/expression/k_creator.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/layout/k_creator.h>
#include <poincare_junior/src/layout/parsing/rack_parser.h>

#include "helper.h"

using namespace PoincareJ;

QUIZ_CASE(pcj_simplification_expansion) {
  EditionReference ref1(KExp(KAdd(1_e, 2_e)));
  quiz_assert(Simplification::ShallowExpand(&ref1));
  assert_trees_are_equal(ref1, KMult(KExp(1_e), KExp(2_e)));

  EditionReference ref2(KTrig(KAdd(π_e, KPow("x"_e, 2_e)), 0_e));
  quiz_assert(Simplification::ShallowExpand(&ref2));
  assert_trees_are_equal(
      ref2,
      KAdd(KMult(KTrig(π_e, 0_e), KTrig(KPow("x"_e, 2_e), 0_e)),
           KMult(KTrig(π_e, KAdd(0_e, -1_e)), KTrig(KPow("x"_e, 2_e), 1_e))));

  EditionReference ref3(KExp(KAdd(1_e, 2_e, 3_e)));
  quiz_assert(Simplification::ShallowExpand(&ref3));
  assert_trees_are_equal(ref3, KMult(KExp(1_e), KExp(KAdd(2_e, 3_e))));

  EditionReference ref4(KAbs(KMult(1_e, 2_e)));
  quiz_assert(Simplification::ShallowExpand(&ref4));
  assert_trees_are_equal(ref4, KMult(KAbs(1_e), KAbs(2_e)));

  EditionReference ref5(KLn(KMult(1_e, 2_e, 3_e)));
  quiz_assert(Simplification::ShallowExpand(&ref5));
  assert_trees_are_equal(ref5, KAdd(KLn(1_e), KLn(KMult(2_e, 3_e))));
}

QUIZ_CASE(pcj_simplification_contraction) {
  EditionReference ref1(KMult(KExp(1_e), KExp(2_e)));
  quiz_assert(Simplification::ShallowContract(&ref1));
  assert_trees_are_equal(ref1, KExp(KAdd(1_e, 2_e)));

  EditionReference ref2(
      KMult(KTrig(KLog(3_e), 1_e), KTrig(KAdd(1_e, KLog("x"_e)), 0_e)));
  quiz_assert(Simplification::ShallowContract(&ref2));
  assert_trees_are_equal(
      ref2,
      KMult(0.5_e,
            KAdd(KTrig(KAdd(KLog(3_e), KMult(-1_e, KAdd(1_e, KLog("x"_e)))),
                       KTrigDiff(1_e, 0_e)),
                 KTrig(KAdd(KLog(3_e), 1_e, KLog("x"_e)), KAdd(0_e, 1_e)))));

  EditionReference ref4(KMult(KAbs(1_e), KAbs(KMult(2_e, 3_e))));
  quiz_assert(Simplification::ShallowContract(&ref4));
  assert_trees_are_equal(ref4, KAbs(KMult(1_e, 2_e, 3_e)));

  EditionReference ref5(KAdd(KLn(1_e), KLn(2_e), 3_e, 4_e));
  quiz_assert(Simplification::ShallowContract(&ref5));
  assert_trees_are_equal(ref5, KAdd(KLn(KMult(1_e, 2_e)), 3_e, 4_e));
}

QUIZ_CASE(pcj_simplification_algebraic_expansion) {
  // A?*(B+C)*D? = A*D*B + A*D*C
  EditionReference ref1(KMult(2_e, KAdd("x"_e, 1_e), "y"_e));
  quiz_assert(Simplification::ShallowAlgebraicExpand(&ref1));
  assert_trees_are_equal(
      ref1, KAdd(KMult(2_e, "x"_e, "y"_e), KMult(2_e, 1_e, "y"_e)));
  // (A + B)^2 = (A^2 + 2*A*B + B^2)
  EditionReference ref3(KPow(KAdd(KTrig("x"_e, 0_e), KTrig("x"_e, 1_e)), 2_e));
  quiz_assert(Simplification::ShallowAlgebraicExpand(&ref3));
  assert_trees_are_equal(ref3,
                         KAdd(KPow(KTrig("x"_e, 0_e), 2_e),
                              KMult(2_e, KTrig("x"_e, 0_e), KTrig("x"_e, 1_e)),
                              KPow(KTrig("x"_e, 1_e), 2_e)));
}

QUIZ_CASE(pcj_simplification_projection) {
  EditionReference ref1(KCos(KSin(KTan(
      KPow(KPow(KPow(e_e, KLogarithm(KLogarithm(KLog(π_e), 2_e), e_e)), π_e),
           3_e)))));
  ref1 = Simplification::DeepSystemProjection(ref1);
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
  ref2 = Simplification::DeepSystemProjection(
      ref2, Simplification::ProjectionContext::NumbersToFloat);
  assert_trees_are_equal(
      ref2,
      KAdd(KTrig(KAdd(2065.0_e, KMult(-1.0_e, 2065.0_e)), 0.0_e), KExp("x"_e)));
  ref2 = Simplification::DeepSystemProjection(
      ref2, Simplification::ProjectionContext::ApproximateToFloat);
  assert_trees_are_equal(ref2, KAdd(1.0_e, KExp("x"_e)));
}

QUIZ_CASE(pcj_simplification_beautify) {
  EditionReference ref1(KAdd(KTrig(3_e, 0_e), KTrig("x"_e, 1_e),
                             KMult(-1_e, KExp(KMult(KLn(5_e), "y"_e))),
                             KMult(KLn(2_e), KPow(KLn(4_e), -1_e))));
  ref1 = Simplification::DeepBeautify(ref1);
  assert_trees_are_equal(
      ref1, KAdd(KSub(KAdd(KCos(3_e), KSin("x"_e)), KPow(5_e, "y"_e)),
                 KLogarithm(2_e, 4_e)));
}

void simplifies_to(const char* input, const char* output) {
  EditionReference inputLayout = Layout::EditionPoolTextToLayout(input);
  EditionReference expression = RackParser(inputLayout).parse();
  inputLayout.removeTree();
  quiz_assert(!expression.isUninitialized());
  EditionReference projected = Simplification::DeepSystemProjection(expression);
  quiz_assert(!projected.isUninitialized());
  Simplification::SystematicReduce(&projected);
  quiz_assert(!projected.isUninitialized());
  EditionReference outputLayout =
      Expression::EditionPoolExpressionToLayout(projected);
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
  simplifies_to("2+2", "4");
  simplifies_to("(2*3(2^2)) + 2*2", "28");
  simplifies_to("36/8", "9/2");
  simplifies_to("2+36/8+2", "17/2");
  simplifies_to("a+a", "2*a");
  simplifies_to("b+a", "a+b");
  simplifies_to("(a*a)*a", "a^(3)");
  simplifies_to("a*(a*a)", "a^(3)");
  simplifies_to("(a*b)^2", "a^(2)*(b^(2))");
  simplifies_to("(a*b*c)^2", "a^(2)*(b^(2))*(c^(2))");
  simplifies_to("a*a*a", "a^(3)");
  simplifies_to("a*2a*b*a*b*4", "8*(a^(3))*(b^(2))");
  simplifies_to("d+c+b+a", "a+b+c+d");
}
