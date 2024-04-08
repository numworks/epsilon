#include <poincare/addition.h>
#include <poincare/arithmetic.h>
#include <poincare/complex_argument.h>
#include <poincare/complex_cartesian.h>
#include <poincare/division.h>
#include <poincare/float.h>
#include <poincare/imaginary_part.h>
#include <poincare/least_common_multiple.h>
#include <poincare/nth_root.h>
#include <poincare/opposite.h>
#include <poincare/polynomial.h>
#include <poincare/power.h>
#include <poincare/real_part.h>
#include <poincare/sign_function.h>
#include <poincare/square_root.h>
#include <poincare/subtraction.h>
#include <poincare/undefined.h>

namespace Poincare {

int Polynomial::LinearPolynomialRoots(OExpression a, OExpression b,
                                      OExpression *root,
                                      ReductionContext reductionContext,
                                      bool beautifyRoots) {
  assert(root);
  assert(!(a.isUninitialized() || b.isUninitialized()));
  *root = Division::Builder(Opposite::Builder(b), a)
              .cloneAndReduceOrSimplify(reductionContext, beautifyRoots);
  return !root->isUndefined();
}

int Polynomial::QuadraticPolynomialRoots(OExpression a, OExpression b,
                                         OExpression c, OExpression *root1,
                                         OExpression *root2, OExpression *delta,
                                         ReductionContext reductionContext,
                                         bool *approximateSolutions,
                                         bool beautifyRoots) {
  assert(root1 && root2 && delta);
  assert(!(a.isUninitialized() || b.isUninitialized() || c.isUninitialized()));

  bool approximate = approximateSolutions ? *approximateSolutions : false;

  Context *context = reductionContext.context();
  ApproximationContext approximationContext(reductionContext);

  *delta = Subtraction::Builder(
      Power::Builder(b.clone(), Rational::Builder(2)),
      Multiplication::Builder(Rational::Builder(4), a.clone(), c.clone()));
  *delta = delta->cloneAndSimplify(reductionContext);
  if (delta->isUndefined()) {
    *root1 = Undefined::Builder();
    *root2 = Undefined::Builder();
    return 0;
  }

  bool multipleRoot = false;
  TrinaryBoolean deltaNull = delta->isNull(context);
  if (deltaNull == TrinaryBoolean::True ||
      (deltaNull == TrinaryBoolean::Unknown &&
       delta->approximateToScalar<double>(approximationContext) == 0.)) {
    *root1 = Division::Builder(
        Opposite::Builder(b.clone()),
        Multiplication::Builder(Rational::Builder(2), a.clone()));
    *root2 = Undefined::Builder();
    multipleRoot = true;
  } else {
    // Grapher relies on the order here to properly navigate implicit curves.
    int offset = 0;
    TrinaryBoolean aPositive = a.isPositive(context);
    if (aPositive != TrinaryBoolean::True &&
        (aPositive == TrinaryBoolean::False ||
         a.approximateToScalar<double>(approximationContext) < 0.)) {
      // Coefficient a is negative, swap root1 and root 2 to preseverve order.
      offset = 1;
    }
    OExpression *roots[2] = {root1, root2};
    *roots[offset % 2] = Division::Builder(
        Subtraction::Builder(Opposite::Builder(b.clone()),
                             SquareRoot::Builder(delta->clone())),
        Multiplication::Builder(Rational::Builder(2), a.clone()));
    *roots[(1 + offset) % 2] = Division::Builder(
        Addition::Builder(Opposite::Builder(b.clone()),
                          SquareRoot::Builder(delta->clone())),
        Multiplication::Builder(Rational::Builder(2), a.clone()));
  }

  if (!approximate) {
    *root1 = root1->cloneAndReduceOrSimplify(reductionContext, beautifyRoots);
    *root2 = root2->cloneAndReduceOrSimplify(reductionContext, beautifyRoots);
    if (root1->otype() == ExpressionNode::Type::Undefined ||
        (!multipleRoot && root2->otype() == ExpressionNode::Type::Undefined)) {
      // Simplification has been interrupted, recompute approximated roots.
      approximate = true;
      if (approximateSolutions) {
        *approximateSolutions = true;
      }
      return QuadraticPolynomialRoots(a, b, c, root1, root2, delta,
                                      reductionContext, &approximate,
                                      beautifyRoots);
    }
  } else {
    *delta = delta->approximate<double>(approximationContext);
    *root1 = root1->approximate<double>(approximationContext);
    *root2 = root2->approximate<double>(approximationContext);
  }
  assert(!(root1->isUninitialized() || root2->isUninitialized()));

  if (root1->isUndefined()) {
    *root1 = *root2;
    *root2 = Undefined::Builder();
  }

  return !root1->isUndefined() + !root2->isUndefined();
}

static bool rootSmallerThan(const OExpression *root1, const OExpression *root2,
                            const ApproximationContext *approximationContext) {
  if (root2->otype() == ExpressionNode::Type::Undefined ||
      root2->otype() == ExpressionNode::Type::Nonreal) {
    return true;
  }
  if (root1->isUndefined()) {
    return false;
  }
  float r1 = root1->approximateToScalar<float>(*approximationContext);
  float r2 = root2->approximateToScalar<float>(*approximationContext);

  if (!std::isnan(r1) || !std::isnan(r2)) {
    // std::isnan(r1) => (r1 <= r2) is false
    return std::isnan(r2) || r1 <= r2;
  }

  // r1 and r2 aren't finite, compare the real part
  float rr1 = RealPart::Builder(root1->clone())
                  .approximateToScalar<float>(*approximationContext);
  float rr2 = RealPart::Builder(root2->clone())
                  .approximateToScalar<float>(*approximationContext);

  if (rr1 != rr2) {
    return rr1 <= rr2;
  }

  // Compare the imaginary part
  float ir1 = ImaginaryPart::Builder(root1->clone())
                  .approximateToScalar<float>(*approximationContext);
  float ir2 = ImaginaryPart::Builder(root2->clone())
                  .approximateToScalar<float>(*approximationContext);
  return ir1 <= ir2;
}

int Polynomial::CubicPolynomialRoots(OExpression a, OExpression b,
                                     OExpression c, OExpression d,
                                     OExpression *root1, OExpression *root2,
                                     OExpression *root3, OExpression *delta,
                                     ReductionContext reductionContext,
                                     bool *approximateSolutions,
                                     bool beautifyRoots) {
  assert(root1 && root2 && root3 && delta);
  assert(!(a.isUninitialized() || b.isUninitialized() || c.isUninitialized() ||
           d.isUninitialized()));

  Context *context = reductionContext.context();
  ApproximationContext approximationContext(reductionContext);

  const OExpression coefficients[] = {d, c, b, a};
  constexpr int degree = 3;
  static_assert(
      OExpression::k_maxPolynomialDegree >= degree,
      "The maximal polynomial degree is too low to handle cubic equations.");

  bool approximate = approximateSolutions ? *approximateSolutions : false;
  const bool equationIsReal = a.isReal(context) && b.isReal(context) &&
                              c.isReal(context) && d.isReal(context);

  // Cube roots of unity.
  OExpression roots[3] = {
      Rational::Builder(1),
      Division::Builder(
          ComplexCartesian::Builder(Rational::Builder(-1),
                                    SquareRoot::Builder(Rational::Builder(3))),
          Rational::Builder(2)),
      Division::Builder(
          ComplexCartesian::Builder(Rational::Builder(1),
                                    SquareRoot::Builder(Rational::Builder(3))),
          Rational::Builder(-2))};

  // b^2*c^2 + 18abcd - 27a^2*d^2 - 4ac^3 - 4db^3
  *delta = Addition::Builder(
      {Power::Builder(Multiplication::Builder(b.clone(), c.clone()),
                      Rational::Builder(2)),
       Multiplication::Builder(
           {Rational::Builder(18), a.clone(), b.clone(), c.clone(), d.clone()}),
       Multiplication::Builder(
           Rational::Builder(-27),
           Power::Builder(Multiplication::Builder(a.clone(), d.clone()),
                          Rational::Builder(2))),
       Multiplication::Builder(Rational::Builder(-4), a.clone(),
                               Power::Builder(c.clone(), Rational::Builder(3))),
       Multiplication::Builder(
           Rational::Builder(-4), d.clone(),
           Power::Builder(b.clone(), Rational::Builder(3)))});
  if (!approximate) {
    *delta = delta->cloneAndSimplify(reductionContext);
    if (delta->otype() == ExpressionNode::Type::Undefined) {
      // Simplification has been interrupted, recompute approximated roots.
      approximate = true;
      if (approximateSolutions != nullptr) {
        *approximateSolutions = approximate;
      }
      return CubicPolynomialRoots(a, b, c, d, root1, root2, root3, delta,
                                  reductionContext, &approximate);
    }
    if (delta->numberOfDescendants(true) >
        k_maxNumberOfNodesBeforeApproximatingDelta) {
      // Delta is too complex anyway, approximate it.
      *delta = delta->approximate<double>(approximationContext);
    }
  } else {
    *delta = delta->approximate<double>(approximationContext);
  }
  assert(!delta->isUninitialized());
  if (delta->otype() == ExpressionNode::Type::Undefined) {
    // There is an undefined coefficient/delta, do not return any solution.
    *root1 = Undefined::Builder();
    *root2 = Undefined::Builder();
    *root3 = Undefined::Builder();
    return 0;
  }

  /* To avoid applying Cardano's formula right away, we use techniques to find
   * a simple solution, based on some particularly common forms of cubic
   * equations in school problems. */
  *root1 = OExpression();
  /* If d is null, the polynom can easily be factored by X. We handle it here
   * (even though in most case it would be caught by the following case) in
   * case c is null. */
  if (d.isNull(context) == TrinaryBoolean::True ||
      d.approximateToScalar<double>(approximationContext) == 0.) {
    *root1 = Rational::Builder(0);
  }
  /* Polynoms of the form "ax^3+d=0" have a simple solutions : x1 = sqrt(-d/a,3)
   * x2 = roots[1] * x1 and x3 = roots[2] * x1. */
  if (root1->isUninitialized() &&
      (b.isNull(context) == TrinaryBoolean::True ||
       b.approximateToScalar<double>(approximationContext) == 0.) &&
      (c.isNull(context) == TrinaryBoolean::True ||
       c.approximateToScalar<double>(approximationContext) == 0.)) {
    *root1 = NthRoot::Builder(
        Division::Builder(Opposite::Builder(d.clone()), a.clone()),
        Rational::Builder(3));
    *root1 = root1->cloneAndReduceOrSimplify(reductionContext, beautifyRoots);
    if (root1->numberOfDescendants(true) * 2 >
            k_maxNumberOfNodesBeforeApproximatingDelta ||
        (reductionContext.complexFormat() ==
             Preferences::ComplexFormat::Polar &&
         !equationIsReal)) {
      /* Approximate roots if root1 is uninitialized, too big (roots 2 and 3
       * might be twice root1's size), or if complex format is Polar, which can
       * serverly complexify roots when beautifying.
       * TODO: Improve simplification on Polar complex format. */
      approximate = true;
      *root1 = NthRoot::Builder(
          Division::Builder(Opposite::Builder(d.clone()), a.clone()),
          Rational::Builder(3));
    }
    /* We compute the three solutions here because they are quite simple, and
     * to avoid generating very complex coefficients when creating the remaining
     * quadratic equation. */
    *root2 = Multiplication::Builder(root1->clone(), roots[1]);
    *root3 = Multiplication::Builder(root1->clone(), roots[2]);
  }
  /* Polynoms of the forms "kx^2(cx+d)+cx+d" and "kx(bx^2+d)+bx^2+d" have a
   * simple solution x1 = -d/c. */
  OExpression r = Division::Builder(Opposite::Builder(d.clone()), c.clone());
  if (root1->isUninitialized() &&
      IsRoot(coefficients, degree, r, reductionContext)) {
    *root1 = r;
  }
  if (root1->isUninitialized() && a.otype() == ExpressionNode::Type::Rational &&
      b.otype() == ExpressionNode::Type::Rational &&
      c.otype() == ExpressionNode::Type::Rational &&
      d.otype() == ExpressionNode::Type::Rational) {
    /* The equation can be written with integer coefficients. Under that form,
     * since d/a = x1*x2*x3, a rational root p/q must be so that p divides d
     * and q divides a. */
    *root1 = RationalRootSearch(coefficients, degree, reductionContext);
  }
  if (root1->isUninitialized()) {
    /* b is the opposite of the sum of all roots counted with their
     * multiplicity. As additions containing roots or powers are in general not
     * reducible, if there exists an irrational root, it might still be
     * explicit in the expression for b. */
    *root1 = SumRootSearch(coefficients, degree, 2, reductionContext);
  }

  if (!root1->isUninitialized() && root2->isUninitialized()) {
    /* We have found one simple solution, we can factor and solve the quadratic
     * equation. */
    OExpression beta =
        Addition::Builder(
            {b.clone(), Multiplication::Builder(a.clone(), root1->clone())})
            .cloneAndSimplify(reductionContext);
    OExpression gamma =
        root1->isNull(context) == TrinaryBoolean::True
            ? c.clone()
            : Opposite::Builder(Division::Builder(d.clone(), root1->clone()))
                  .cloneAndSimplify(reductionContext);
    OExpression delta2;
    QuadraticPolynomialRoots(a.clone(), beta, gamma, root2, root3, &delta2,
                             reductionContext, nullptr, beautifyRoots);
    assert(!root2->isUninitialized() && !root3->isUninitialized());
  } else if (root1->isUninitialized()) {
    /* We did not manage to find any simple root : we resort to using Cardano's
     * formula. */
    int deltaSign;
    if (delta->isNull(context) == TrinaryBoolean::True) {
      deltaSign = 0;
    } else {
      double deltaValue =
          delta->approximateToScalar<double>(approximationContext);
      /* A complex delta (NAN deltaValue) must be handled like a negative delta
       * This ternary operator's condition order is important here. */
      deltaSign = deltaValue == 0. ? 0 : deltaValue > 0. ? 1 : -1;
      assert(!std::isnan(deltaValue) || deltaSign == -1);
    }
    // b^2 - 3ac
    OExpression delta0 =
        Subtraction::Builder(
            Power::Builder(b.clone(), Rational::Builder(2)),
            Multiplication::Builder(Rational::Builder(3), a.clone(), c.clone()))
            .cloneAndSimplify(reductionContext);
    if (deltaSign == 0) {
      if (delta0.isNull(context) == TrinaryBoolean::True ||
          delta0.approximateToScalar<double>(approximationContext) == 0.) {
        // -b / 3a
        *root1 = Division::Builder(
            b.clone(),
            Multiplication::Builder(Rational::Builder(-3), a.clone()));
        *root2 = Undefined::Builder();
        *root3 = Undefined::Builder();
      } else {
        // (9ad - bc) / (2*delta0)
        *root1 = Division::Builder(
            Subtraction::Builder(Multiplication::Builder(Rational::Builder(9),
                                                         a.clone(), d.clone()),
                                 Multiplication::Builder(b.clone(), c.clone())),
            Multiplication::Builder(Rational::Builder(2), delta0.clone()));
        // (4abc - 9da^2 - b^3) / (a*delta0)
        *root2 = Division::Builder(
            Addition::Builder(
                {Multiplication::Builder(Rational::Builder(4), a.clone(),
                                         b.clone(), c.clone()),
                 Multiplication::Builder(
                     Rational::Builder(-9),
                     Power::Builder(a.clone(), Rational::Builder(2)),
                     d.clone()),
                 Opposite::Builder(
                     Power::Builder(b.clone(), Rational::Builder(3)))}),
            Multiplication::Builder(a.clone(), delta0.clone()));
        *root3 = Undefined::Builder();
      }
    } else {
      // 2b^3 - 9abc + 27da^2
      OExpression delta1 =
          Addition::Builder(
              {Multiplication::Builder(
                   Rational::Builder(2),
                   Power::Builder(b.clone(), Rational::Builder(3))),
               Multiplication::Builder(Rational::Builder(-9), a.clone(),
                                       b.clone(), c.clone()),
               Multiplication::Builder(
                   Rational::Builder(27),
                   Power::Builder(a.clone(), Rational::Builder(2)), d.clone())})
              .cloneAndSimplify(reductionContext);
      /* Cardano's formula is famous for introducing complex numbers in the
       * resolution of some real equations. As such, we temporarily set the
       * complex format to Cartesian. */
      ReductionContext complexContext = reductionContext;
      complexContext.setComplextFormat(Preferences::ComplexFormat::Cartesian);
      complexContext.setTarget(ReductionTarget::SystemForApproximation);
      ApproximationContext approximationComplexContext(complexContext);
      OExpression cardano =
          CardanoNumber(delta0, delta1, &approximate, complexContext);
      if (cardano.otype() == ExpressionNode::Type::Undefined ||
          cardano.deepIsOfType(
              {ExpressionNode::Type::Undefined, ExpressionNode::Type::Infinity},
              context)) {
        if (approximateSolutions) {
          *approximateSolutions = true;
        }
        // Due to overflows, cardano contains infinite or couldn't be computed
        *root1 = Undefined::Builder();
        *root2 = Undefined::Builder();
        *root3 = Undefined::Builder();
        return 0;
      }
      /* cardano is only null when there is a triple root. This should have been
       * already handled when computing delta, since delta should be equal to 0
       * in this case. This means there was approximation errors during
       * computation of delta. Restore correct delta value here.
       *
       * Example:
       * For the family of equations (111 111 000 000 000 x - K)^3 = 0,
       * delta should be equal to zero, but it's not due to approximations
       * errors. For K = 6, we find cardano == 0, so the error is caught here.
       * But for K = 2 or K = 9, we still find delta != 0 and cardano != 0, so
       * the error is not fixed.
       *
       * TODO: Enhance delta computation and/or add a formal solve of equations
       * of type (AX+B)^3=0. */
      if (cardano.isNull(context) == TrinaryBoolean::True) {
        // -b / 3a
        *root1 = Division::Builder(
            b.clone(),
            Multiplication::Builder(Rational::Builder(-3), a.clone()));
        *root2 = Undefined::Builder();
        *root3 = Undefined::Builder();
        *delta = Rational::Builder(0);
      } else {
        int loneRealRootIndex = -1;
        float minimalImaginaryPart = static_cast<float>(INFINITY);
        for (int i = 0; i < 3; i++) {
          /* The roots can be computed from Cardano's number using the cube
           * roots of unity. */
          OExpression cz = Multiplication::Builder(cardano.clone(), roots[i]);
          roots[i] = Division::Builder(
              Addition::Builder({b.clone(), cz.clone(),
                                 Division::Builder(delta0, cz.clone())}),
              Multiplication::Builder(Rational::Builder(-3), a.clone()));
          if (approximate) {
            if (equationIsReal && deltaSign > 0) {
              /* delta > 0, the three solutions are real. We need to get rid of
               * the imaginary part that might have appeared. */
              roots[i] = RealPart::Builder(roots[i]);
            }
            roots[i] =
                roots[i].approximate<double>(approximationComplexContext);
            if (equationIsReal && deltaSign < 0) {
              /* We know there is exactly one real root (and two complex
               * conjugate). Because of approximation errors, this real root can
               * have an infinitesimal imaginary size. As such, we strip the
               * imaginary part from the root with the smallest imaginary part.
               */
              float im = std::fabs(
                  ImaginaryPart::Builder(roots[i]).approximateToScalar<float>(
                      approximationComplexContext));
              if (im < minimalImaginaryPart) {
                minimalImaginaryPart = im;
                loneRealRootIndex = i;
              }
            }
          } else {
            roots[i] = roots[i].cloneAndReduceOrSimplify(reductionContext,
                                                         beautifyRoots);
          }
        }
        if (loneRealRootIndex >= 0) {
          roots[loneRealRootIndex] =
              RealPart::Builder(roots[loneRealRootIndex])
                  .approximate<double>(approximationComplexContext);
        }
        *root1 = roots[0];
        *root2 = roots[1];
        *root3 = roots[2];
      }
    }
  }

  /* Simplify the results with the correct complexFormat */
  if (!approximate) {
    *root1 = root1->cloneAndReduceOrSimplify(reductionContext, beautifyRoots);
    bool simplificationFailed =
        root1->otype() == ExpressionNode::Type::Undefined;
    if (root2->isUninitialized() ||
        root2->otype() != ExpressionNode::Type::Undefined) {
      *root2 = root2->cloneAndReduceOrSimplify(reductionContext, beautifyRoots);
      simplificationFailed = simplificationFailed ||
                             root2->otype() == ExpressionNode::Type::Undefined;
    }
    if (root3->isUninitialized() ||
        root3->otype() != ExpressionNode::Type::Undefined) {
      *root3 = root3->cloneAndReduceOrSimplify(reductionContext, beautifyRoots);
      simplificationFailed = simplificationFailed ||
                             root3->otype() == ExpressionNode::Type::Undefined;
    }
    if (simplificationFailed) {
      // Simplification has been interrupted, recompute approximated roots.
      approximate = true;
      if (approximateSolutions != nullptr) {
        *approximateSolutions = approximate;
      }
      return CubicPolynomialRoots(a, b, c, d, root1, root2, root3, delta,
                                  reductionContext, &approximate,
                                  beautifyRoots);
    }
  } else {
    *root1 = root1->approximate<double>(approximationContext);
    *root2 = root2->approximate<double>(approximationContext);
    *root3 = root3->approximate<double>(approximationContext);
    /* Approximate delta in case it was not already (if it was, this cost no
     * time anyway). */
    *delta = delta->approximate<double>(approximationContext);
  }

  /* Remove duplicates */
  if (root3->isIdenticalTo(*root1) || root3->isIdenticalTo(*root2)) {
    *root3 = Undefined::Builder();
  }
  if (root2->isIdenticalTo(*root1) || root2->isUndefined()) {
    *root2 = *root3;
    *root3 = Undefined::Builder();
  }
  if (root1->isUndefined()) {
    *root1 = *root2;
    *root2 = *root3;
    *root3 = Undefined::Builder();
  }

  /* Sort the roots. The real roots go first, in ascending order, then the
   * complex roots in order of ascending imaginary part. */
  void *pack[] = {root1, root2, root3, &approximationContext};
  Helpers::Sort(
      [](int i, int j, void *ctx, int n) {  // Swap method
        assert(i < n && j < n);
        OExpression **tab =
            reinterpret_cast<OExpression **>(reinterpret_cast<void **>(ctx));
        OExpression t = *tab[i];
        *tab[i] = *tab[j];
        *tab[j] = t;
      },
      [](int i, int j, void *ctx, int n) {  // Comparison method
        assert(i < n && j < n);
        void **pack = reinterpret_cast<void **>(ctx);
        OExpression **tab = reinterpret_cast<OExpression **>(pack);
        ApproximationContext *approximationContext =
            reinterpret_cast<ApproximationContext *>(pack[3]);
        return rootSmallerThan(tab[j], tab[i], approximationContext);
      },
      pack, degree);

  if (approximateSolutions != nullptr) {
    *approximateSolutions = approximate;
  }
  return !root1->isUndefined() + !root2->isUndefined() + !root3->isUndefined();
}

OExpression Polynomial::ReducePolynomial(
    const OExpression *coefficients, int degree, OExpression parameter,
    const ReductionContext &reductionContext) {
  Addition polynomial = Addition::Builder();
  polynomial.addChildAtIndexInPlace(coefficients[0].clone(), 0, 0);
  for (int i = 1; i <= degree; i++) {
    polynomial.addChildAtIndexInPlace(
        Multiplication::Builder(
            coefficients[i].clone(),
            Power::Builder(parameter.clone(), Rational::Builder(i))),
        i, i);
  }
  // Try to simplify polynomial
  OExpression simplifiedReducedPolynomial =
      polynomial.cloneAndSimplify(reductionContext);
  return simplifiedReducedPolynomial;
}

Rational Polynomial::ReduceRationalPolynomial(const Rational *coefficients,
                                              int degree, Rational parameter) {
  Rational result = coefficients[degree];
  for (int i = degree - 1; i <= 0; i--) {
    result = Rational::Addition(Rational::Multiplication(result, parameter),
                                coefficients[i]);
  }
  return result;
}

OExpression Polynomial::RationalRootSearch(
    const OExpression *coefficients, int degree,
    const ReductionContext &reductionContext) {
  assert(degree <= OExpression::k_maxPolynomialDegree);

  const Rational *rationalCoefficients =
      static_cast<const Rational *>(coefficients);
  LeastCommonMultiple lcm = LeastCommonMultiple::Builder();
  for (int i = 0; i <= degree; i++) {
    assert(coefficients[i].otype() == ExpressionNode::Type::Rational);
    lcm.addChildAtIndexInPlace(
        Rational::Builder(rationalCoefficients[i].integerDenominator()), i, i);
  }
  OExpression lcmResult = lcm.shallowReduce(reductionContext);
  assert(lcmResult.otype() == ExpressionNode::Type::Rational);
  Rational rationalLCM = static_cast<Rational &>(lcmResult);

  Integer a0Int =
      Rational::Multiplication(static_cast<const Rational &>(coefficients[0]),
                               rationalLCM)
          .unsignedIntegerNumerator();
  Integer aNInt =
      Rational::Multiplication(
          static_cast<const Rational &>(coefficients[degree]), rationalLCM)
          .unsignedIntegerNumerator();

  Integer a0Divisors[Arithmetic::k_maxNumberOfDivisors];
  int a0NumberOfDivisors, aNNumberOfDivisors;

  Arithmetic arithmetic;
  /* We need to compare two lists of divisors, but Arithmetic only allows
   * access to one list of factors. We thus need to store the first list in
   * its own buffer. */
  a0NumberOfDivisors = arithmetic.PositiveDivisors(a0Int);
  for (int i = 0; i < a0NumberOfDivisors; i++) {
    a0Divisors[i] = *arithmetic.divisorAtIndex(i);
  }

  // Reset Arithmetic lock before computing aNInt divisors
  Arithmetic::resetLock();

  aNNumberOfDivisors = arithmetic.PositiveDivisors(aNInt);
  for (int i = 0; i < a0NumberOfDivisors; i++) {
    for (int j = 0; j < aNNumberOfDivisors; j++) {
      /* If i and j are not coprime, i/j has already been tested. */
      Integer p = a0Divisors[i], q = *arithmetic.divisorAtIndex(j);
      if (Arithmetic::GCD(p, q).isOne()) {
        Rational r = Rational::Builder(p, q);
        if (ReduceRationalPolynomial(rationalCoefficients, degree, r)
                .isZero()) {
          return std::move(r);
        }
        r = Rational::Multiplication(Rational::Builder(-1), r);
        if (ReduceRationalPolynomial(rationalCoefficients, degree, r)
                .isZero()) {
          return std::move(r);
        }
      }
    }
  }
  return OExpression();
}

OExpression Polynomial::SumRootSearch(
    const OExpression *coefficients, int degree, int relevantCoefficient,
    const ReductionContext &reductionContext) {
  OExpression a = coefficients[degree];
  OExpression b = coefficients[relevantCoefficient].clone();

  if (b.otype() != ExpressionNode::Type::Addition) {
    OExpression r = Opposite::Builder(Division::Builder(b, a.clone()));
    return IsRoot(coefficients, degree, r, reductionContext) ? r
                                                             : OExpression();
  }
  int n = b.numberOfChildren();
  for (int i = 0; i < n; i++) {
    OExpression r =
        Opposite::Builder(Division::Builder(b.childAtIndex(i), a.clone()));
    if (IsRoot(coefficients, degree, r, reductionContext)) {
      return r;
    }
  }

  return OExpression();
}

OExpression Polynomial::CardanoNumber(
    OExpression delta0, OExpression delta1, bool *approximate,
    const ReductionContext &reductionContext) {
  assert(approximate != nullptr);

  /* C = root((delta1 ± sqrt(delta1^2 - 4*delta0^3)) / 2, 3)
   * The sign of ± must be chosen so that C is not null:
   *   - if delta0 is null, we enforce C = root(delta1, 3).
   *   - otherwise, ± takes the sign of delta1. This way, we do not run the
   *     risk of subtracting two very close numbers when delta0 << delta1. */

  OExpression C;
  ApproximationContext approximationContext(reductionContext);
  if (delta0.isNull(reductionContext.context()) == TrinaryBoolean::True) {
    C = delta1.clone();
  } else {
    OExpression rootDeltaDifference = SquareRoot::Builder(Subtraction::Builder(
        Power::Builder(delta1.clone(), Rational::Builder(2)),
        Multiplication::Builder(
            Rational::Builder(4),
            Power::Builder(delta0.clone(), Rational::Builder(3)))));
    OExpression diff;
    if (SignFunction::Builder(delta1).approximateToScalar<double>(
            approximationContext) <= 0.0) {
      diff = Subtraction::Builder(delta1.clone(), rootDeltaDifference);
    } else {
      diff = Addition::Builder(delta1.clone(), rootDeltaDifference);
    }
    C = Division::Builder(diff, Rational::Builder(2));
  }
  C = NthRoot::Builder(C, Rational::Builder(3))
          .cloneAndSimplify(reductionContext);

  if (C.isNull(reductionContext.context()) == TrinaryBoolean::Unknown) {
    C = C.approximate<double>(approximationContext);
    *approximate = true;
  } else {
    *approximate = false;
  }
  return C;
}
}  // namespace Poincare
