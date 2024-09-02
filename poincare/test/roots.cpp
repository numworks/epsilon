#include <poincare/numeric/roots.h>
#include <poincare/src/expression/simplification.h>

#include "helper.h"

using namespace Poincare::Internal;

void assert_roots_are(const char* coefficients, const char* expectedRoots) {
  ProjectionContext projCtx = {.m_complexFormat = ComplexFormat::Cartesian};
  process_tree_and_compare(
      coefficients, expectedRoots,
      [](Tree* tree, ProjectionContext projCtx) {
        Simplification::ProjectAndReduce(tree, &projCtx, true);
        int numberOfCoefficients = tree->numberOfChildren();
        switch (numberOfCoefficients) {
          case 2:
            tree->moveTreeOverTree(
                Roots::Linear(tree->child(0), tree->child(1)));
            break;
          case 3:
            tree->moveTreeOverTree(Roots::Quadratic(
                tree->child(0), tree->child(1), tree->child(2)));
            break;
          default:
            // Not handled
            quiz_assert(false);
        }
        // TODO: Advanced reduction is needed to simplify roots of rationals
        Simplification::ReduceSystem(tree, true);
        Simplification::BeautifyReduced(tree, &projCtx);
      },
      projCtx);
}

QUIZ_CASE(pcj_roots) {
  assert_roots_are("{6, 2}", "-1/3");
  assert_roots_are("{1, -1}", "1");
  assert_roots_are("{2x+z, 4y-1}", "(-4×y+1)/(2×x+z)");
  assert_roots_are("{1, -2, 1}", "1");
  assert_roots_are("{π, -2π, π}", "1");
  assert_roots_are("{1, -1, -6}", "{3,-2}");
  assert_roots_are("{1,-x-1,x}",
                   "{(x+1+√(x^2-2×x+1))/2,-(-x-1+√(x^2-2×x+1))/2}");
  // TODO: Should simplify to {-1×i,i}
  assert_roots_are("{1, 0, 1}", "{√(-4)/2,-i}");
}
