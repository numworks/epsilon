#include <poincare/numeric/roots.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/simplification.h>

#include "helper.h"

using namespace Poincare::Internal;

namespace cubic_solver_policies {
/* The Solver App may have different policies:
 * - display the exact result or approximate it
 * - approximate the result knowing or not that the input coefficients are all
 * real numbers.
 * The following structs simulate those different policies. They allow to test
 * the resulting roots either against some expected exact roots, or against some
 * expected approximate roots.
 */

struct ExactSolve {
  static Tree* process(const Tree* a, const Tree* b, const Tree* c,
                       const Tree* d) {
    TreeRef discriminant = Roots::CubicDiscriminant(a, b, c, d);
    TreeRef roots = Roots::Cubic(a, b, c, d, discriminant);
    discriminant->removeTree();
    return roots;
  }
};

struct ExactSolveAndApproximate {
  static Tree* process(const Tree* a, const Tree* b, const Tree* c,
                       const Tree* d) {
    TreeRef discriminant = Roots::CubicDiscriminant(a, b, c, d);
    TreeRef roots = Roots::Cubic(a, b, c, d, discriminant);
    TreeRef approximateRoots = Approximation::RootTreeToTree<double>(roots);
    roots->removeTree();
    discriminant->removeTree();
    return approximateRoots;
  }
};

struct ExactSolveAndRealCubicApproximate {
  static Tree* process(const Tree* a, const Tree* b, const Tree* c,
                       const Tree* d) {
    assert(GetComplexSign(a).isReal() && GetComplexSign(b).isReal() &&
           GetComplexSign(c).isReal() && GetComplexSign(d).isReal());
    TreeRef discriminant = Roots::CubicDiscriminant(a, b, c, d);
    TreeRef roots = Roots::Cubic(a, b, c, d, discriminant);
    TreeRef approximateRoots =
        Roots::ApproximateRootsOfRealCubic(roots, discriminant);
    roots->removeTree();
    discriminant->removeTree();
    return approximateRoots;
  }
};

}  // namespace cubic_solver_policies

template <typename CubicSolverPolicy = cubic_solver_policies::ExactSolve>
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
          case 4: {
            Tree* a = tree->child(0);
            Tree* b = tree->child(1);
            Tree* c = tree->child(2);
            Tree* d = tree->child(3);
            tree->moveTreeOverTree(CubicSolverPolicy::process(a, b, c, d));
            break;
          }
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
  using namespace cubic_solver_policies;

  assert_roots_are("{1, undef}", "undef");
  assert_roots_are("{6, 2}", "-1/3");
  assert_roots_are("{1, -1}", "1");
  assert_roots_are("{2x+z, 4y-1}", "(-4×y+1)/(2×x+z)");
  assert_roots_are("{1, -2, 1}", "{1}");
  assert_roots_are("{π, -2π, π}", "{1}");
  assert_roots_are("{1, -1, -6}", "{-2,3}");
  assert_roots_are("{1,-x-1,x}",
                   "{-(-x-1+√(x^2-2×x+1))/2,(x+1+√(x^2-2×x+1))/2}");
  assert_roots_are("{1, 0, 1}", "{-i,i}");

  assert_roots_are("{1, 1, undef, 1}", "undef");
  assert_roots_are("{0, 1, 0, 1}", "{-i,i}");
  assert_roots_are("{1, -2, 1, 0}", "{0,1}");
  assert_roots_are("{1, 0, 0, -8}", "{2,-1-√(3)×i,-1+√(3)×i}");
  assert_roots_are("{2, -4, -1, 2}", "{-√(2)/2,√(2)/2,2}");
  assert_roots_are("{1, -4, 6, -24}",
                   "{4,-√(-24)/2,√(-24)/2}");  // TODO: this should simplify to
                                               // "{4,-√(6)×i,√(6)×i}"
  assert_roots_are("{1, 0, -3, -2}",
                   "{-1,-1,2}");  // TODO: duplicate roots
  assert_roots_are("{4, 0, -12, -8}",
                   "{-1,-1,2}");  // TODO: duplicate roots
  assert_roots_are("{1, -i, -1, i}", "{-1,1,i}");
  assert_roots_are("{1, -3×i, -3, i}", "{i,i}");  // TODO: duplicate roots
  assert_roots_are("{1, -3×√(2), 6, -2×√(2)}",
                   "{√(2),√(2)}");  // TODO: duplicate roots
  assert_roots_are("{1,π-2×√(3),3-2×√(3)×π,3×π}", "{-π,√(3)}");
  assert_roots_are("{1,-900,270000,-27000000}", "{300}");
  assert_roots_are("{1,-√(2),-16,24×√(2)}", "{-3×√(2),2×√(2)}");

  assert_roots_are<ExactSolveAndRealCubicApproximate>(
      "{1,1,0,1}",
      "{-1.4655712318768,0.23278561593838-0.79255199251545×i,0.23278561593838+"
      "0.79255199251545×i}");
  assert_roots_are<ExactSolveAndRealCubicApproximate>(
      "{1,0,-3,1}", "{-1.8793852415718,0.34729635533386,1.532088886238}");

  assert_roots_are<ExactSolveAndApproximate>(
      "{1,i,-1,1}",
      "{-1.2237724884371-0.40117404251929×i,0.70354372055869-0."
      "97874867081524×i,0.52022876787839+0.37992271333452×i}");

  /* In the two following tests, integer coefficients are too large for a
   * rational search (divisors list is full), but applying the Cardano method
   * then applying the real cubic root approximation succeeds */
  assert_roots_are<ExactSolveAndRealCubicApproximate>("{1, -69, 1478, -10080}",
                                                      "{16,18,35}");
  assert_roots_are<ExactSolveAndRealCubicApproximate>("{1,90,2125,-31250}",
                                                      "{10,-50-25×i,-50+25×i}");

  /* The following cubic has {1.23} as a triple root. The discriminant
   * calculation fails to find zero. Note that WolframAlpha gives exactly the
   * same roots, so this might be non trivial to fix. */
  assert_roots_are<ExactSolveAndRealCubicApproximate>(
      "{1,-3.69,4.5387,-1.86087}",
      "{1.2444224957031,1.2227887521485-0.012490247664834×i,1.2227887521485+0."
      "012490247664834×i}");

  // TODO: when there are very large numbers among coefficients
  // assert_solves_to("x^3+x^2=10^200", {"delta=-27×10^400+4×10^200"});
}
