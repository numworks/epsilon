#include <poincare_junior/include/expression.h>
#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/expression/k_creator.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/layout/k_creator.h>
#include <poincare_junior/src/layout/parsing/rack_parser.h>

#ifdef PCJ_PERF
#include <chrono>
#include <iomanip>
#include <iostream>
#endif

#include "helper.h"

using namespace PoincareJ;

QUIZ_CASE(pcj_simplification_expansion) {
  EditionReference ref1(KExp(KAdd(1_e, 2_e)));
  quiz_assert(Simplification::Expand(&ref1));
  assert_trees_are_equal(ref1, KMult(KExp(1_e), KExp(2_e)));

  EditionReference ref2(KTrig(KAdd(π_e, KPow("x"_e, 2_e)), 0_e));
  quiz_assert(Simplification::Expand(&ref2));
  assert_trees_are_equal(
      ref2,
      KAdd(KMult(KTrig(π_e, 0_e), KTrig(KPow("x"_e, 2_e), 0_e)),
           KMult(KTrig(π_e, KAdd(0_e, -1_e)), KTrig(KPow("x"_e, 2_e), 1_e))));

  EditionReference ref3(KExp(KAdd(1_e, 2_e, 3_e)));
  quiz_assert(Simplification::Expand(&ref3));
  assert_trees_are_equal(ref3, KMult(KExp(1_e), KExp(KAdd(2_e, 3_e))));

  EditionReference ref4(KAbs(KMult(1_e, 2_e)));
  quiz_assert(Simplification::Expand(&ref4));
  assert_trees_are_equal(ref4, KMult(KAbs(1_e), KAbs(2_e)));

  EditionReference ref5(KLn(KMult(1_e, 2_e, 3_e)));
  quiz_assert(Simplification::Expand(&ref5));
  assert_trees_are_equal(ref5, KAdd(KLn(1_e), KLn(KMult(2_e, 3_e))));
}

QUIZ_CASE(pcj_simplification_contraction) {
  EditionReference ref1(KMult(KExp(1_e), KExp(2_e)));
  quiz_assert(Simplification::Contract(&ref1));
  assert_trees_are_equal(ref1, KExp(KAdd(1_e, 2_e)));

  EditionReference ref2(
      KMult(KTrig(KLog(3_e), 1_e), KTrig(KAdd(1_e, KLog("x"_e)), 0_e)));
  quiz_assert(Simplification::Contract(&ref2));
  assert_trees_are_equal(
      ref2,
      KMult(0.5_e,
            KAdd(KTrig(KAdd(KLog(3_e), KMult(-1_e, KAdd(1_e, KLog("x"_e)))),
                       KTrigDiff(1_e, 0_e)),
                 KTrig(KAdd(KLog(3_e), 1_e, KLog("x"_e)), KAdd(0_e, 1_e)))));

  EditionReference ref4(KMult(KAbs(1_e), KAbs(KMult(2_e, 3_e))));
  quiz_assert(Simplification::Contract(&ref4));
  assert_trees_are_equal(ref4, KAbs(KMult(1_e, 2_e, 3_e)));

  EditionReference ref5(KAdd(KLn(1_e), KLn(2_e), 3_e, 4_e));
  quiz_assert(Simplification::Contract(&ref5));
  assert_trees_are_equal(ref5, KAdd(KLn(KMult(1_e, 2_e)), 3_e, 4_e));
}

QUIZ_CASE(pcj_simplification_algebraic_expansion) {
  // A?*(B+C)*D? = A*D*B + A*D*C
  EditionReference ref1(KMult(2_e, KAdd("x"_e, 1_e), "y"_e));
  quiz_assert(Simplification::AlgebraicExpand(&ref1));
  assert_trees_are_equal(
      ref1, KAdd(KMult(2_e, "x"_e, "y"_e), KMult(2_e, 1_e, "y"_e)));
  // (A*B)^C = A^C * B^C
  EditionReference ref2(KPow(KMult(5_e, "x"_e, "y"_e), 4_e));
  quiz_assert(Simplification::AlgebraicExpand(&ref2));
  assert_trees_are_equal(ref2,
                         KMult(KPow(5_e, 4_e), KPow(KMult("x"_e, "y"_e), 4_e)));
  // (A + B)^2 = (A^2 + 2*A*B + B^2)
  EditionReference ref3(KPow(KAdd(KTrig("x"_e, 0_e), KTrig("x"_e, 1_e)), 2_e));
  quiz_assert(Simplification::AlgebraicExpand(&ref3));
  assert_trees_are_equal(ref3,
                         KAdd(KPow(KTrig("x"_e, 0_e), 2_e),
                              KMult(2_e, KTrig("x"_e, 0_e), KTrig("x"_e, 1_e)),
                              KPow(KTrig("x"_e, 1_e), 2_e)));
}

QUIZ_CASE(pcj_simplification_projection) {
  EditionReference ref1(KCos(KSin(KTan(
      KPow(KPow(KPow(e_e, KLogarithm(KLogarithm(KLog(π_e), 2_e), e_e)), π_e),
           3_e)))));
  ref1 = Simplification::SystemProjection(ref1);
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
  ref2 = Simplification::SystemProjection(
      ref2, Simplification::ProjectionContext::NumbersToFloat);
  assert_trees_are_equal(
      ref2,
      KAdd(KTrig(KAdd(2065.0_e, KMult(-1.0_e, 2065.0_e)), 0.0_e), KExp("x"_e)));
  ref2 = Simplification::SystemProjection(
      ref2, Simplification::ProjectionContext::ApproximateToFloat);
  assert_trees_are_equal(ref2, KAdd(1.0_e, KExp("x"_e)));
}

#if PLATFORM_DEVICE
#define QUIZ_ASSERT(test) quiz_assert(test)
#else
#include <iostream>
#define QUIZ_ASSERT(test)                                   \
  if (!(test)) {                                            \
    std::cerr << "Assert failed " << __LINE__ << std::endl; \
  }
#endif

QUIZ_CASE(pcj_compare) {
  QUIZ_ASSERT(Comparison::Compare("a"_e, "a"_e) == 0);
  QUIZ_ASSERT(Comparison::Compare("a"_e, "b"_e) == -1);
  QUIZ_ASSERT(Comparison::Compare("b"_e, "a"_e) == 1);
  EditionReference c(KAdd(2_e, "a"_e));
  EditionReference d(KAdd(3_e, "a"_e));
  EditionReference e(KAdd(2_e, "b"_e));
  QUIZ_ASSERT(Comparison::Compare(2_e, 3_e) == -1);
  QUIZ_ASSERT(Comparison::Compare(c, d) == -1);
}

void simplifies_to(const char* input, const char* output) {
#ifdef PCJ_PERF
  static bool printHeader = true;
  if (printHeader) {
    std::cout << std::left << std::setw(160) << "input" << std::right
              << std::setw(10) << "micros" << std::setw(10) << "refs"
              << std::endl;
    printHeader = false;
  }
#endif
  EditionReference inputLayout = Layout::EditionPoolTextToLayout(input);
  EditionReference expression = RackParser(inputLayout).parse();
  inputLayout.removeTree();
  quiz_assert(!expression.isUninitialized());
  EditionReference projected = Simplification::SystemProjection(expression);
  quiz_assert(!projected.isUninitialized());
#ifdef PCJ_PERF
  auto start = std::chrono::high_resolution_clock::now();
  int refId;
  {
    EditionReference r(0_e);
    refId = r.identifier();
  }
#endif
  Simplification::AutomaticSimplify(&projected);
#ifdef PCJ_PERF
  auto elapsed = std::chrono::high_resolution_clock::now() - start;
  {
    EditionReference r(0_e);
    refId = r.identifier() - refId;
  }
  std::cout
      << std::left << std::setw(50) << input << std::right << std::setw(10)
      << std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count()
      << std::setw(10) << refId << std::endl;
#endif
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
  simplifies_to("a^b*a^c", "a^(b+c)");
  simplifies_to("a^b*a^b", "a^(2*b)");
  simplifies_to("(a^b)^2", "a^(2*b)");
}

QUIZ_CASE(pcj_perf_simplification) {
  simplifies_to("2+2", "4");
  simplifies_to("2+2+2+2", "8");
  simplifies_to("2+2+2+2+2+2+2+2", "16");
  simplifies_to("2+2+2+2+2+2+2+2+2+2+2+2+2+2+2+2", "32");
  simplifies_to(
      "2+2+2+2+2+2+2+2+2+2+2+2+2+2+2+2+2+2+2+2+2+2+2+2+2+2+2+2+2+2+2+2", "64");
  simplifies_to(
      "2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2",
      "4294967296");
  simplifies_to("(2+(2+(2+(2))))", "8");
  simplifies_to("(2+(2+(2+(2+(2+(2+(2+(2))))))))", "16");
  simplifies_to(
      "(2+(2+(2+(2+(2+(2+(2+(2+(2+(2+(2+(2+(2+(2+(2+(2))))))))))))))))", "32");
  simplifies_to(
      "(2+(2+(2+(2+(2+(2+(2+(2+(2+(2+(2+(2+(2+(2+(2+(2+(2+(2+(2+(2+(2+(2+(2+(2+"
      "(2+(2+(2+(2+(2+(2+(2+(2))))))))))))))))))))))))))))))))",
      "64");
  simplifies_to("((((2)+2)+2)+2)", "8");
  simplifies_to("((((((((2)+2)+2)+2)+2)+2)+2)+2)", "16");
  simplifies_to(
      "((((((((((((((((2)+2)+2)+2)+2)+2)+2)+2)+2)+2)+2)+2)+2)+2)+2)+2)", "32");
  simplifies_to(
      "((((((((((((((((((((((((((((((((2)+2)+2)+2)+2)+2)+2)+2)+2)+2)+2)+2)+2)+"
      "2)+2)+2)+2)+2)+2)+2)+2)+2)+2)+2)+2)+2)+2)+2)+2)+2)+2)+2)",
      "64");
  simplifies_to("a+a", "2*a");
  simplifies_to("a+a+a+a", "4*a");
  simplifies_to("a+a+a+a+a+a+a+a", "8*a");
  simplifies_to("a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a", "16*a");
  simplifies_to(
      "a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a+a",
      "32*a");
  simplifies_to(
      "a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a",
      "a^(32)");
  simplifies_to("(a+(a+(a+(a))))", "4*a");
  simplifies_to("(a+(a+(a+(a+(a+(a+(a+(a))))))))", "8*a");
  simplifies_to(
      "(a+(a+(a+(a+(a+(a+(a+(a+(a+(a+(a+(a+(a+(a+(a+(a))))))))))))))))",
      "16*a");
  simplifies_to(
      "(a+(a+(a+(a+(a+(a+(a+(a+(a+(a+(a+(a+(a+(a+(a+(a+(a+(a+(a+(a+(a+(a+(a+(a+"
      "(a+(a+(a+(a+(a+(a+(a+(a))))))))))))))))))))))))))))))))",
      "32*a");
  simplifies_to("((((a)+a)+a)+a)", "4*a");
  simplifies_to("((((((((a)+a)+a)+a)+a)+a)+a)+a)", "8*a");
  simplifies_to(
      "((((((((((((((((a)+a)+a)+a)+a)+a)+a)+a)+a)+a)+a)+a)+a)+a)+a)+a)",
      "16*a");
  simplifies_to(
      "((((((((((((((((((((((((((((((((a)+a)+a)+a)+a)+a)+a)+a)+a)+a)+a)+a)+a)+"
      "a)+a)+a)+a)+a)+a)+a)+a)+a)+a)+a)+a)+a)+a)+a)+a)+a)+a)+a)",
      "32*a");
  simplifies_to("a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p",
                "a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p");
  simplifies_to("p+o+n+m+l+k+j+i+h+g+f+e+d+c+b+a",
                "a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p");
}
