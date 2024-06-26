#include <poincare/numeric/roots.h>
#include <poincare/src/expression/simplification.h>

#include "helper.h"

using namespace Poincare::Internal;

void assert_roots_are(const char* coefficients, const char* expectedRoots) {
  ProjectionContext projCtx = {.m_complexFormat = ComplexFormat::Cartesian};
  Tree* coeff = parse(coefficients);
  Simplification::ToSystem(coeff, &projCtx);
  Simplification::ReduceSystem(coeff, true);
  int numberOfCoefficients = coeff->numberOfChildren();
  Tree* result = nullptr;
  switch (numberOfCoefficients) {
    case 2:
      result = Roots::Linear(coeff->child(0), coeff->child(1));
      break;
    case 3:
      result =
          Roots::Quadratic(coeff->child(0), coeff->child(1), coeff->child(2));
      break;
    default:
      // Not handled
      quiz_assert(false);
  }
  // TODO: Advanced reduction is needed to simplify roots of rationals
  Simplification::ReduceSystem(result, true);
  Tree* expected = parse(expectedRoots);
  Simplification::ToSystem(expected, &projCtx);
  Simplification::ReduceSystem(expected, false);
  assert_trees_are_equal(expected, result);
  expected->removeTree();
  result->removeTree();
  coeff->removeTree();
}

QUIZ_CASE(pcj_roots) {
  assert_roots_are("{6, 2}", "-1/3");
  assert_roots_are("{1, -1}", "1");
  assert_roots_are("{2x+z, 4y-1}", "(1-4y)/(2x+z)");
  assert_roots_are("{1, -2, 1}", "1");
  assert_roots_are("{π, -2π, π}", "1");
  assert_roots_are("{1, -1, -6}", "{3, -2}");
  assert_roots_are("{1,-x-1,x}",
                   "{1/2×(1+x+exp(1/2×ln(1-2×x+x^2))), "
                   "-1/2×(-1-1×x+exp(1/2×ln(1-2×x+x^2)))}");
  // TODO: Results should be simpler but ADVANCED_MAX_DEPTH is too small
  // {-1×i,i}
  assert_roots_are("{1, 0, 1}", "{exp(1/2×π×i), -1/2×exp(1/2×ln(-4))}");
}
