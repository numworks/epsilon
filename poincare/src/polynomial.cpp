#include <poincare/polynomial.h>
#include <poincare/addition.h>
#include <poincare/arithmetic.h>
#include <poincare/complex_argument.h>
#include <poincare/complex_cartesian.h>
#include <poincare/division.h>
#include <poincare/exception_checkpoint.h>
#include <poincare/float.h>
#include <poincare/least_common_multiple.h>
#include <poincare/nth_root.h>
#include <poincare/opposite.h>
#include <poincare/power.h>
#include <poincare/real_part.h>
#include <poincare/square_root.h>
#include <poincare/subtraction.h>
#include <poincare/undefined.h>

namespace Poincare {

int Polynomial::QuadraticPolynomialRoots(Expression a, Expression b, Expression c, Expression * root1, Expression * root2, Expression * delta, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) {
  assert(root1 && root2 && delta);
  assert(!(a.isUninitialized() || b.isUninitialized() || c.isUninitialized()));

  ExpressionNode::ReductionContext reductionContext(context, complexFormat, angleUnit, Preferences::UnitFormat::Metric, ExpressionNode::ReductionTarget::User);

  *delta = Subtraction::Builder(Power::Builder(b.clone(), Rational::Builder(2)), Multiplication::Builder(Rational::Builder(4), a.clone(), c.clone()));
  *delta = delta->simplify(reductionContext);
  assert(!delta->isUninitialized());
  if (delta->isUndefined()){
    *root1 = Undefined::Builder();
    *root2 = Undefined::Builder();
    return 0;
  }

  ExpressionNode::NullStatus deltaNull = delta->nullStatus(context);
  if (deltaNull == ExpressionNode::NullStatus::Null
   || (deltaNull == ExpressionNode::NullStatus::Unknown && delta->approximateToScalar<double>(context, complexFormat, angleUnit) == 0.))
  {
    *root1 = Division::Builder(Opposite::Builder(b), Multiplication::Builder(Rational::Builder(2), a)).simplify(reductionContext);
    *root2 = Undefined::Builder();
  } else {
    *root1 = Division::Builder(
        Subtraction::Builder(Opposite::Builder(b.clone()), SquareRoot::Builder(delta->clone())),
        Multiplication::Builder(Rational::Builder(2), a.clone())
        ).simplify(reductionContext);
    *root2 = Division::Builder(
        Addition::Builder(Opposite::Builder(b), SquareRoot::Builder(delta->clone())),
        Multiplication::Builder(Rational::Builder(2), a)
        ).simplify(reductionContext);
  }

  if (root1->isUndefined()) {
    *root1 = *root2;
    *root2 = Undefined::Builder();
  }

  return !root1->isUndefined() + !root2->isUndefined();
}

int Polynomial::CubicPolynomialRoots(Expression a, Expression b, Expression c, Expression d, Expression * root1, Expression * root2, Expression * root3, Expression * delta, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, bool * approximateSolutions) {
  assert(root1 && root2 && root3 && delta);
  assert(!(a.isUninitialized() || b.isUninitialized() || c.isUninitialized() || d.isUninitialized()));
  const Expression coefficients[] = { d, c, b, a };
  constexpr int degree = 3;
  static_assert(Expression::k_maxPolynomialDegree >= degree, "The maximal polynomial degree is too low to handle cubic equations.");

  ExpressionNode::ReductionContext reductionContext(context, complexFormat, angleUnit, Preferences::UnitFormat::Metric, ExpressionNode::ReductionTarget::User);
  bool approximate = false;

  // b^2*c^2 + 18abcd - 27a^2*d^2 - 4ac^3 - 4db^3
  *delta = Addition::Builder({
      Power::Builder(Multiplication::Builder(b.clone(), c.clone()), Rational::Builder(2)),
      Multiplication::Builder({Rational::Builder(18), a.clone(), b.clone(), c.clone(), d.clone()}),
      Multiplication::Builder(Rational::Builder(-27), Power::Builder(Multiplication::Builder(a.clone(), d.clone()), Rational::Builder(2))),
      Multiplication::Builder(Rational::Builder(-4), a.clone(), Power::Builder(c.clone(), Rational::Builder(3))),
      Multiplication::Builder(Rational::Builder(-4), d.clone(), Power::Builder(b.clone(), Rational::Builder(3)))});
  *delta = delta->simplify(reductionContext);

  /* To avoid applying Cardano's formula right away, we use techniques to find
   * a simple solution, based on some particularly common forms of cubic
   * equations in school problems. */
  *root1 = Expression();
  /* If d is null, the polynom can easily be factored by X. We handle it here
   * (even though in most case it would be caught by the following case) in
   * case c is null. */
  if (d.nullStatus(context) == ExpressionNode::NullStatus::Null || d.approximateToScalar<double>(context, complexFormat, angleUnit) == 0.) {
    *root1 = Rational::Builder(0);
  }
  /* Polynoms of the forms "kx^2(cx+d)+cx+d" and "kx(bx^2+d)+bx^2+d" have a
   * simple solution x1 = -d/c. */
  Expression r = Division::Builder(Opposite::Builder(d.clone()), c.clone());
  if (root1->isUninitialized() && IsRoot(coefficients, degree, r, reductionContext)) {
    *root1 = r;
  }
  if (root1->isUninitialized() && a.type() == ExpressionNode::Type::Rational && b.type() == ExpressionNode::Type::Rational && c.type() == ExpressionNode::Type::Rational && d.type() == ExpressionNode::Type::Rational) {
    /* Since d/a = x1*x2*x3, a rational root p/q must be so that p divides the
     * numerator of d/a, and q divides its denominator. */
    *root1 = RationalRootSearch(coefficients, degree, reductionContext);
  }
  if (root1->isUninitialized()) {
    /* b is the opposite of the sum of all roots counted with their
     * multiplicity. As additions containing roots or powers are in general not
     * reducible, if there exists an irrational root, it might still be
     * explicit in the expression for b. */
    *root1 = PowerRootSearch(coefficients, degree, 2, reductionContext);
  }

  if (!root1->isUninitialized()) {
    /* We have found a simple solution, we can factor and solve the quadratic
     * equation. */
    Expression beta = Addition::Builder({b, Multiplication::Builder(a.clone(), root1->clone())}).simplify(reductionContext);
    Expression gamma = root1->nullStatus(context) == ExpressionNode::NullStatus::Null ? c : Opposite::Builder(Division::Builder(d, root1->clone())).simplify(reductionContext);
    Expression delta2;
    QuadraticPolynomialRoots(a, beta, gamma, root2, root3, &delta2, context, complexFormat, angleUnit);
    assert(!root2->isUninitialized() && !root3->isUninitialized());
  } else {
    /* We did not manage to find any simple root : we resort to using Cardano's
     * formula. */
    int deltaSign;
    if (delta->nullStatus(context) == ExpressionNode::NullStatus::Null) {
      deltaSign = 0;
    } else {
      double deltaValue = delta->approximateToScalar<double>(context, complexFormat, angleUnit);
      deltaSign = deltaValue == 0. ? 0 : deltaValue > 0. ? 1 : -1;
    }
    // b^2 - 3ac
    Expression delta0 = Subtraction::Builder(Power::Builder(b.clone(), Rational::Builder(2)), Multiplication::Builder(Rational::Builder(3), a.clone(), c.clone())).simplify(reductionContext);
    if (deltaSign == 0) {
      if (delta0.nullStatus(context) == ExpressionNode::NullStatus::Null || delta0.approximateToScalar<double>(context, complexFormat, angleUnit) == 0.) {
        // -b / 3a
        *root1 = Division::Builder(b, Multiplication::Builder(Rational::Builder(-3), a));
        *root2 = Undefined::Builder();
        *root3 = Undefined::Builder();
      } else {
        // (9ad - bc) / (2*delta0)
        *root1 = Division::Builder(Subtraction::Builder(Multiplication::Builder(Rational::Builder(9), a.clone(), d.clone()), Multiplication::Builder(b.clone(), c.clone())), Multiplication::Builder(Rational::Builder(2), delta0.clone()));
        // (4abc - 9da^2 - b^3) / (a*delta0)
        *root2 = Division::Builder(Addition::Builder({Multiplication::Builder(Rational::Builder(4), a.clone(), b.clone(), c.clone()), Multiplication::Builder(Rational::Builder(-9), Power::Builder(a.clone(), Rational::Builder(2)), d.clone()), Opposite::Builder(Power::Builder(b.clone(), Rational::Builder(3)))}), Multiplication::Builder(a.clone(), delta0.clone()));
        *root3 = Undefined::Builder();
      }
    } else {
      // 2b^3 - 9abc + 27da^2
      Expression delta1 = Addition::Builder({
          Multiplication::Builder(Rational::Builder(2), Power::Builder(b.clone(), Rational::Builder(3))),
          Multiplication::Builder(Rational::Builder(-9), a.clone(), b.clone(), c.clone()),
          Multiplication::Builder(Rational::Builder(27), Power::Builder(a.clone(), Rational::Builder(2)), d.clone())
          }).simplify(reductionContext);
      /* Cardano's formula is famous for introducing complex numbers in the
       * resolution of some real equations. As such, we temporarily set the
       * complex format to Cartesian. */
      ExpressionNode::ReductionContext complexContext(context, Preferences::ComplexFormat::Cartesian, angleUnit, Preferences::UnitFormat::Metric, ExpressionNode::ReductionTarget::SystemForApproximation);
      Expression cardano = CardanoNumber(delta0, delta1, &approximate, complexContext);
      /* cardano is only null when there is a triple root. */
      assert(cardano.nullStatus(context) != ExpressionNode::NullStatus::Null);
      /* The roots can be computed from Cardano's number using the cube roots of
       * unity. */
      Expression roots[3] = {
        Rational::Builder(1),
        Division::Builder(ComplexCartesian::Builder(Rational::Builder(-1), SquareRoot::Builder(Rational::Builder(3))), Rational::Builder(2)),
        Division::Builder(ComplexCartesian::Builder(Rational::Builder(1), SquareRoot::Builder(Rational::Builder(3))), Rational::Builder(-2))
      };
      for (int i = 0; i < 3; i++) {
        Expression cz = Multiplication::Builder(cardano.clone(), roots[i]);
        roots[i] = Division::Builder(
            Addition::Builder({b.clone(), cz.clone(), Division::Builder(delta0, cz.clone())}),
            Multiplication::Builder(Rational::Builder(-3), a.clone()));
        if (approximate) {
          if (deltaSign > 0) {
            /* delta > 0, the three solutions are real. We need to get rid of
             * the imaginary part that might have appeared. */
            roots[i]= RealPart::Builder(roots[i]);
          }
          roots[i] = roots[i].approximate<double>(context, complexContext.complexFormat(), complexContext.angleUnit());
        } else {
          roots[i] = roots[i].simplify(complexContext);
        }
      }
      *root1 = roots[0];
      *root2 = roots[1];
      *root3 = roots[2];
    }
  }

  /* Simplify the results with the correct complexFormat */
  if (approximate) {
    *root1 = root1->approximate<double>(context, complexFormat, angleUnit);
    *root2 = root2->approximate<double>(context, complexFormat, angleUnit);
    *root3 = root3->approximate<double>(context, complexFormat, angleUnit);
  } else {
    *root1 = root1->simplify(reductionContext);
    *root2 = root2->simplify(reductionContext);
    *root3 = root3->simplify(reductionContext);
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

  if (approximateSolutions != nullptr) {
    *approximateSolutions = approximate;
  }
  return !root1->isUndefined() + !root2->isUndefined() + !root3->isUndefined();
}

Expression Polynomial::ReducePolynomial(const Expression * coefficients, int degree, Expression parameter, ExpressionNode::ReductionContext reductionContext) {
  Addition polynomial = Addition::Builder();
  polynomial.addChildAtIndexInPlace(coefficients[0].clone(), 0, 0);
  for (int i = 1; i <= degree; i++) {
    polynomial.addChildAtIndexInPlace(Multiplication::Builder(coefficients[i].clone(), Power::Builder(parameter.clone(), Rational::Builder(i))), i, i);
  }
  return polynomial.simplify(reductionContext);
}

Expression Polynomial::RationalRootSearch(const Expression * coefficients, int degree, ExpressionNode::ReductionContext reductionContext) {
  assert(degree <= Expression::k_maxPolynomialDegree);

  LeastCommonMultiple lcm = LeastCommonMultiple::Builder();
  for (int i = 0; i <= degree; i++) {
    assert(coefficients[i].type() == ExpressionNode::Type::Rational);
    lcm.addChildAtIndexInPlace(Rational::Builder(static_cast<const Rational &>(coefficients[i]).integerDenominator()), i, i);
  }
  Expression lcmResult = lcm.shallowReduce(reductionContext);
  assert(lcmResult.type() == ExpressionNode::Type::Rational);
  Rational rationalLCM = static_cast<Rational &>(lcmResult);

  Integer a0Int = Rational::Multiplication(static_cast<const Rational &>(coefficients[0]), rationalLCM).unsignedIntegerNumerator();
  Integer aNInt = Rational::Multiplication(static_cast<const Rational &>(coefficients[degree]), rationalLCM).unsignedIntegerNumerator();

  Integer a0Divisors[Arithmetic::k_maxNumberOfFactors];
  int a0NumberOfDivisors, aNNumberOfDivisors;
  {
    ExceptionCheckpoint ecp;
    if (ExceptionRun(ecp)) {
      Arithmetic arithmetic;
      /* We need to compare two lists of divisors, but Arithmetic only allows
       * access to one list of factors. We thus need to store the first list in
       * its own buffer. */
      a0NumberOfDivisors = arithmetic.PositiveDivisors(a0Int);
      for (int i = 0; i < a0NumberOfDivisors; i++) {
        a0Divisors[i] = *arithmetic.divisorAtIndex(i);
      }
    } else {
      Arithmetic::resetLock();
      return Expression();
    }
  }

  {
    ExceptionCheckpoint ecp;
    if (ExceptionRun(ecp)) {
      Arithmetic arithmetic;
      aNNumberOfDivisors = arithmetic.PositiveDivisors(aNInt);
      for (int i = 0; i < a0NumberOfDivisors; i++) {
        for (int j = 0; j < aNNumberOfDivisors; j++) {
          /* If i and j are not coprime, i/j has already been tested. */
          Integer p = a0Divisors[i], q = *arithmetic.divisorAtIndex(j);
          if (Arithmetic::GCD(p, q).isOne()) {
            Rational r = Rational::Builder(p, q);
            if (IsRoot(coefficients, degree, r, reductionContext)) {
              return std::move(r);
            }
            r = Rational::Multiplication(Rational::Builder(-1), r);
            if (IsRoot(coefficients, degree, r, reductionContext)) {
              return std::move(r);
            }
          }
        }
      }
    } else {
      Arithmetic::resetLock();
      return Expression();
    }
  }

  return Expression();
}

Expression Polynomial::PowerRootSearch(const Expression * coefficients, int degree, int relevantCoefficient, ExpressionNode::ReductionContext reductionContext) {
  Expression a = coefficients[relevantCoefficient];

  for (int i = 0; i < a.numberOfChildren(); i++) {
    Expression b = a.childAtIndex(i);
    if (b.hasExpression([](Expression e, const void * context) {
          return e.type() == ExpressionNode::Type::Power || e.type() == ExpressionNode::Type::NthRoot || e.type() == ExpressionNode::Type::SquareRoot;
          }, nullptr))
    {
      Expression r = Opposite::Builder(b.clone());
      if (IsRoot(coefficients, degree, r, reductionContext)) {
        return r;
      }
    }
  }

  return Expression();
}

Expression Polynomial::CardanoNumber(Expression delta0, Expression delta1, bool * approximate, ExpressionNode::ReductionContext reductionContext) {
  assert(approximate != nullptr);

  /* C = root(delta1 ± sqrt(delta1^2 - 4*delta0^3), 3)
   * The sign of ± is chosen so that C is not null:
   * - if delta0 != 0, we chose +
   * - if delta0 = 0:
   *   - if arg(delta1) < π (i.e. sqrt(delta1^2) = delta1), we chose +
   *   - if arg(delta1) >= π (i.e. sqrt(delta1^2) = -delta1), we chose - */

  Expression rootDeltaDifference = SquareRoot::Builder(Subtraction::Builder(
        Power::Builder(delta1.clone(), Rational::Builder(2)),
        Multiplication::Builder(Rational::Builder(4), Power::Builder(delta0.clone(), Rational::Builder(3)))
        ));
  Expression C;

  if (delta0.nullStatus(reductionContext.context()) == ExpressionNode::NullStatus::Null
   && ComplexArgument::Builder(delta1.clone()).approximateToScalar<float>(reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit()) >= M_PI) {
    C = Subtraction::Builder(delta1.clone(), rootDeltaDifference);
  } else {
    C = Addition::Builder(delta1.clone(), rootDeltaDifference);
  }
  C = NthRoot::Builder(Division::Builder(C, Rational::Builder(2)), Rational::Builder(3)).simplify(reductionContext);

  if (C.type() == ExpressionNode::Type::NthRoot || C.nullStatus(reductionContext.context()) == ExpressionNode::NullStatus::Unknown) {
    C = C.approximate<double>(reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit());
    *approximate = true;
  } else {
    *approximate = false;
  }
  assert(C.nullStatus(reductionContext.context()) != ExpressionNode::NullStatus::Null);
  return C;
}
}
