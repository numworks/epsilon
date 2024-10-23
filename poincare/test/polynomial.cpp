#include <apps/shared/global_context.h>
#include <ion/storage/file_system.h>
#include <poincare/src/expression/advanced_reduction.h>
#include <poincare/src/expression/degree.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/polynomial.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/expression/systematic_reduction.h>

#include "helper.h"

using namespace Poincare::Internal;

void assert_polynomial_is_parsed(const Tree* e, const Tree* expectedVariables,
                                 const Tree* expectedPolynomial) {
  SharedTreeStack->flush();
  Tree* variables = PolynomialParser::GetVariables(e);
  assert_trees_are_equal(variables, expectedVariables);
  Tree* clone = e->cloneTree();
  SystematicReduction::DeepReduce(clone);
  AdvancedReduction::DeepExpandAlgebraic(clone);
  Tree* polynomial = PolynomialParser::RecursivelyParse(clone, variables);
  assert_trees_are_equal(polynomial, expectedPolynomial);
}

QUIZ_CASE(pcj_polynomial_parsing) {
  assert_polynomial_is_parsed(
      /* 42 */ 42_e,
      /* variables = {} */ KSet(),
      /* polynomial */ 42_e);
  assert_polynomial_is_parsed(
      /* x^3 + 3*x^2*y + 3*x*y^2 + y^3 */ KAdd(
          KPow("x"_e, 3_e), KMult(3_e, KPow("x"_e, 2_e), "y"_e),
          KMult(3_e, KPow("y"_e, 2_e), "x"_e), KPow("y"_e, 3_e)),
      /* variables = {x, y} */ KSet("x"_e, "y"_e),
      /* polynomial */
      KPol(Exponents<3, 2, 1, 0>(), "x"_e, 1_e,
           KPol(Exponents<1>(), "y"_e, 3_e), KPol(Exponents<2>(), "y"_e, 3_e),
           KPol(Exponents<3>(), "y"_e, 1_e)));
  // TODO: parse polynomial with float coefficients?
}

QUIZ_CASE(pcj_polynomial_variables) {
  assert_trees_are_equal(
      PolynomialParser::GetVariables(KMult(2_e, "x"_e, "y"_e)),
      KSet("x"_e, "y"_e));
  assert_trees_are_equal(
      PolynomialParser::GetVariables(KAdd(KPow("x"_e, 2_e), KLn("x"_e))),
      KSet("x"_e, KLn("x"_e)));
  assert_trees_are_equal(PolynomialParser::GetVariables(
                             KAdd(KPow("x"_e, 2_e), KPow("y"_e, 2_e),
                                  KMult(-1_e, KPow(KAdd("x"_e, "y"_e), 2_e)))),
                         KSet("x"_e, "y"_e, KAdd("x"_e, "y"_e)));
  assert_trees_are_equal(
      PolynomialParser::GetVariables(KAdd("x"_e, KPow("y"_e, -1_e))),
      KSet("x"_e, KPow("y"_e, -1_e)));
  assert_trees_are_equal(
      PolynomialParser::GetVariables(KAdd(
          KMult(KExp(KMult(1_e / 2_e, KLn(2_e))), "x"_e), KMult(π_e, i_e))),
      KSet("x"_e));
}

QUIZ_CASE(pcj_polynomial_operations) {
  /* A = x^2 + 3*x*y + y + 1 */
  const Tree* polA =
      KPol(Exponents<2, 1, 0>(), "x"_e, 1_e, KPol(Exponents<1>(), "y"_e, 3_e),
           KPol(Exponents<1, 0>(), "y"_e, 1_e, 1_e));
  /* B = x^3 + 2*x*y^2 + 7*x*y + 23 */
  const Tree* polB = KPol(Exponents<3, 1, 0>(), "x"_e, 1_e,
                          KPol(Exponents<2, 1>(), "y"_e, 2_e, 7_e), 23_e);
  /* C = -2x^3 + 7*x*y + 23 */
  const Tree* polC = KPol(Exponents<3, 1, 0>(), "x"_e, -2_e,
                          KPol(Exponents<1>(), "y"_e, 7_e), 23_e);

  /* Inverse(A) = -x^2 - 3*x*y - y - 1 */
  Tree* inv = polA->cloneTree();
  Polynomial::Inverse(inv);
  assert_trees_are_equal(inv, KPol(Exponents<2, 1, 0>(), "x"_e, -1_e,
                                   KPol(Exponents<1>(), "y"_e, -3_e),
                                   KPol(Exponents<1, 0>(), "y"_e, -1_e, -1_e)));

  /* Normalize(C) = 2x^3 - 7*x*y - 23*/
  Tree* norm = polC->cloneTree();
  Polynomial::Normalize(norm);
  assert_trees_are_equal(norm, KPol(Exponents<3, 1, 0>(), "x"_e, 2_e,
                                    KPol(Exponents<1>(), "y"_e, -7_e), -23_e));

  /* A + B = x^3 + x^2 + 2*x*y^2 + 10*x*y + y + 24 */
  assert_trees_are_equal(
      Polynomial::Addition(polA->cloneTree(), polB->cloneTree()),
      TreeRef(KPol(Exponents<3, 2, 1, 0>(), "x"_e, 1_e, 1_e,
                   KPol(Exponents<2, 1>(), "y"_e, 2_e, 10_e),
                   KPol(Exponents<1, 0>(), "y"_e, 1_e, 24_e))));
  SharedTreeStack->flush();

  /* B + A = x^3 + x^2 + 2*x*y^2 + 10*x*y + y + 24 */
  assert_trees_are_equal(
      Polynomial::Addition(polB->cloneTree(), polA->cloneTree()),
      KPol(Exponents<3, 2, 1, 0>(), "x"_e, 1_e, 1_e,
           KPol(Exponents<2, 1>(), "y"_e, 2_e, 10_e),
           KPol(Exponents<1, 0>(), "y"_e, 1_e, 24_e)));
  SharedTreeStack->flush();

  // TODO: test A-B and B-A!

  /* A * B = x^5 + 3yx^4 + (2y^2+8y+1)*x^3 + (6y^3+21y^2+23)x^2 +
  (2y^3+9y^2+
   * 76y)x + 23y + 23 */
  assert_trees_are_equal(
      Polynomial::Multiplication(TreeRef(polA->cloneTree()),
                                 TreeRef(polB->cloneTree())),
      KPol(Exponents<5, 4, 3, 2, 1, 0>(), "x"_e, 1_e,
           KPol(Exponents<1>(), "y"_e, 3_e),
           KPol(Exponents<2, 1, 0>(), "y"_e, 2_e, 8_e, 1_e),
           KPol(Exponents<3, 2, 0>(), "y"_e, 6_e, 21_e, 23_e),
           KPol(Exponents<3, 2, 1>(), "y"_e, 2_e, 9_e, 76_e),
           KPol(Exponents<1, 0>(), "y"_e, 23_e, 23_e)));
  SharedTreeStack->flush();

  /* Test variable order:
   * (y^2) + ((y+1)x + 1 = (y+1)x + y^2 + 1 */
  assert_trees_are_equal(
      Polynomial::Addition(
          TreeRef(KPol(Exponents<2>(), "y"_e, 1_e)),
          TreeRef(KPol(Exponents<1, 0>(), "x"_e,
                       KPol(Exponents<1, 0>(), "y"_e, 1_e, 1_e), 1_e))),
      KPol(Exponents<1, 0>(), "x"_e, KPol(Exponents<1, 0>(), "y"_e, 1_e, 1_e),
           KPol(Exponents<2, 0>(), "y"_e, 1_e, 1_e)));
  SharedTreeStack->flush();

  // A = x^2y^2 + y
  polA = KPol(Exponents<2, 0>(), "x"_e, KPol(Exponents<2>(), "y"_e, 1_e),
              KPol(Exponents<1>(), "y"_e, 1_e));
  // B = xy + 1
  polB = KPol(Exponents<1, 0>(), "x"_e, KPol(Exponents<1>(), "y"_e, 1_e), 1_e);
  // A / B = (xy + 1)*(xy - 1) + y + 1
  auto [quotient, remainder] =
      Polynomial::PseudoDivision(polA->cloneTree(), polB->cloneTree());
  assert_trees_are_equal(
      quotient,
      KPol(Exponents<1, 0>(), "x"_e, KPol(Exponents<1>(), "y"_e, 1_e), -1_e));
  assert_trees_are_equal(remainder, KPol(Exponents<1, 0>(), "y"_e, 1_e, 1_e));
}

void assert_polynomial_degree_is(ProjectionContext projectionContext,
                                 const char* input, int expectedDegree,
                                 const char* symbolName = "x") {
  Tree* expression = parse(input, projectionContext.m_context);
  Simplification::ProjectAndReduce(expression, &projectionContext);
  int degree = Degree::Get(expression, symbolName);
  quiz_assert(degree == expectedDegree);
  expression->removeTree();
}

QUIZ_CASE(pcj_polynomial_degree) {
  Shared::GlobalContext globalContext;
  assert(
      Ion::Storage::FileSystem::sharedFileSystem->numberOfRecords() ==
      Ion::Storage::FileSystem::sharedFileSystem->numberOfRecordsWithExtension(
          "sys"));

  ProjectionContext projCtx = {
      .m_complexFormat = ComplexFormat::Cartesian,
      .m_symbolic = SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition,
      .m_context = &globalContext,
      .m_advanceReduce = false};

  assert_polynomial_degree_is(projCtx, "x+1", 1);
  assert_polynomial_degree_is(projCtx, "cos(2)+1", 0);
  assert_polynomial_degree_is(projCtx, "diff(3×x+x,x,2)", 0);
  assert_polynomial_degree_is(projCtx, "diff(3×x+x,x,x)", 0);
  assert_polynomial_degree_is(projCtx, "diff(3×x+x,x,x)", 0, "a");
  assert_polynomial_degree_is(projCtx, "(3×x+2)/3", 1);
  assert_polynomial_degree_is(projCtx, "(3×x+2)/x", -1);
  assert_polynomial_degree_is(projCtx, "int(2×x,x, 0, 1)", -1);
  assert_polynomial_degree_is(projCtx, "int(2×x,x, 0, 1)", 0, "a");
  assert_polynomial_degree_is(projCtx, "[[1,2][3,4]]", -1);
  assert_polynomial_degree_is(projCtx, "(x^2+2)×(x+1)", 3);
  assert_polynomial_degree_is(projCtx, "-(x+1)", 1);
  assert_polynomial_degree_is(projCtx, "(x^2+2)^(3)", 6);
  assert_polynomial_degree_is(projCtx, "2-x-x^3", 3);
  assert_polynomial_degree_is(projCtx, "π×x", 1);
  assert_polynomial_degree_is(projCtx, "x+π^(-3)", 1);

  // f: y→y^2+πy+1
  store("1+π×y+y^2→f(y)", &globalContext);
  assert_polynomial_degree_is(projCtx, "f(x)", 2);
  // With y=1
  store("1→y", &globalContext);
  assert_polynomial_degree_is(projCtx, "f(x)", 2);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("f.func").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("y.exp").destroy();
  // a : undef and f : y→ay+πy+1
  store("undef→a", &globalContext);
  store("1+π×y+y×a→f(y)", &globalContext);
  assert_polynomial_degree_is(projCtx, "f(x)", 0);  // a is undefined
  // With a = 1
  store("1→a", &globalContext);
  assert_polynomial_degree_is(projCtx, "f(x)", 1);
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("f.func").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("a.exp").destroy();

  projCtx.m_complexFormat = ComplexFormat::Real;
  assert_polynomial_degree_is(projCtx, "√(-1)×x", 0);
  assert_polynomial_degree_is(projCtx, "√(x)", -1);
}
