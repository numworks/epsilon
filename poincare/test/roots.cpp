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
          case 4:
            tree->moveTreeOverTree(Roots::Cubic(tree->child(0), tree->child(1),
                                                tree->child(2),
                                                tree->child(3)));
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
  assert_roots_are("{1, -2, 1}", "{1}");
  assert_roots_are("{π, -2π, π}", "{1}");
  assert_roots_are("{1, -1, -6}", "{-2,3}");
  assert_roots_are("{1,-x-1,x}",
                   "{-(-x-1+√(x^2-2×x+1))/2,(x+1+√(x^2-2×x+1))/2}");
  assert_roots_are("{1, 0, 1}", "{-i,i}");

  assert_roots_are("{1, undef, 0, 0}", "{}");
  assert_roots_are("{1, -2, 1, 0}", "{1,0}");
  assert_roots_are("{1,-x-1,x,0}",
                   "{-(-x-1+√(x^2-2×x+1))/2,(x+1+√(x^2-2×x+1))/2,0}");
  assert_roots_are("{1, 0, 0, -8}", "{2,-1+√(3)×i,-1-√(3)×i}");
  assert_roots_are("{2, -4, -1, 2}", "{-√(2)/2,√(2)/2,2}");

  // assert_roots_are(
  //     "{1, -4, 6, -24}",
  //     "{-√(-24)/2,√(-24)/2,4}");  // TODO: this should be simplified
  // assert_roots_are("{1, 0, -3, -2}", "{-1,2}");

  // assert_solves_to("x^3+x+1=0", {"x=-0.6823278038",
  // "x=0.3411639019-1.1615414×i",
  //                                "x=0.3411639019+1.1615414×i", "delta=-31"});
  // assert_solves_to("x^3+x^2+1=0",
  //                  {"x=-1.465571232", "x=0.2327856159-0.7925519925×i",
  //                   "x=0.2327856159+0.7925519925×i", "delta=-31"});
  // assert_solves_to("x^3+x^2=10^200", {"delta=-27×10^400+4×10^200"});
  // assert_solves_to("x^3-4x^2+6x-24=0",
  //                  {"x=4", "x=-√(6)×i", "x=√(6)×i", "delta=-11616"});
}
