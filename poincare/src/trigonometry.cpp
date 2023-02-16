#include <assert.h>
#include <float.h>
#include <ion.h>
#include <limits.h>
#include <poincare/addition.h>
#include <poincare/arc_cosine.h>
#include <poincare/arc_sine.h>
#include <poincare/arc_tangent.h>
#include <poincare/constant.h>
#include <poincare/cosecant.h>
#include <poincare/cosine.h>
#include <poincare/cotangent.h>
#include <poincare/decimal.h>
#include <poincare/dependency.h>
#include <poincare/derivative.h>
#include <poincare/division.h>
#include <poincare/float.h>
#include <poincare/multiplication.h>
#include <poincare/opposite.h>
#include <poincare/parenthesis.h>
#include <poincare/power.h>
#include <poincare/preferences.h>
#include <poincare/rational.h>
#include <poincare/secant.h>
#include <poincare/serialization_helper.h>
#include <poincare/sign_function.h>
#include <poincare/simplification_helper.h>
#include <poincare/sine.h>
#include <poincare/subtraction.h>
#include <poincare/symbol.h>
#include <poincare/tangent.h>
#include <poincare/trigonometry.h>
#include <poincare/trigonometry_cheat_table.h>
#include <poincare/undefined.h>
#include <poincare/unit.h>

#include <cmath>

namespace Poincare {

/* The values must be in the order defined in poincare/preferences:
 * Radians / Degrees / Gradians */

static int PiDivisor(Preferences::AngleUnit angleUnit) {
  switch (angleUnit) {
    case Preferences::AngleUnit::Radian:
      return 1;
    case Preferences::AngleUnit::Degree:
      return 180;
    default:
      assert(angleUnit == Preferences::AngleUnit::Gradian);
      return 200;
  }
}

Expression Trigonometry::PiExpressionInAngleUnit(
    Preferences::AngleUnit angleUnit) {
  switch (angleUnit) {
    case Preferences::AngleUnit::Radian:
      return Constant::PiBuilder();
      ;
    case Preferences::AngleUnit::Degree:
      return Rational::Builder(180);
    default:
      assert(angleUnit == Preferences::AngleUnit::Gradian);
      return Rational::Builder(200);
  }
}

double Trigonometry::PiInAngleUnit(Preferences::AngleUnit angleUnit) {
  switch (angleUnit) {
    case Preferences::AngleUnit::Radian:
      return M_PI;
    case Preferences::AngleUnit::Degree:
      return 180.0;
    default:
      assert(angleUnit == Preferences::AngleUnit::Gradian);
      return 200.0;
  }
}

bool Trigonometry::isDirectTrigonometryFunction(const Expression& e) {
  return e.type() == ExpressionNode::Type::Cosine ||
         e.type() == ExpressionNode::Type::Sine ||
         e.type() == ExpressionNode::Type::Tangent;
}

bool Trigonometry::isInverseTrigonometryFunction(const Expression& e) {
  return e.type() == ExpressionNode::Type::ArcCosine ||
         e.type() == ExpressionNode::Type::ArcSine ||
         e.type() == ExpressionNode::Type::ArcTangent;
}

bool Trigonometry::isAdvancedTrigonometryFunction(const Expression& e) {
  return e.type() == ExpressionNode::Type::Secant ||
         e.type() == ExpressionNode::Type::Cosecant ||
         e.type() == ExpressionNode::Type::Cotangent;
}

bool Trigonometry::isInverseAdvancedTrigonometryFunction(const Expression& e) {
  return e.type() == ExpressionNode::Type::ArcSecant ||
         e.type() == ExpressionNode::Type::ArcCosecant ||
         e.type() == ExpressionNode::Type::ArcCotangent;
}

bool Trigonometry::AreInverseFunctions(const Expression& directFunction,
                                       const Expression& inverseFunction) {
  if (!isDirectTrigonometryFunction(directFunction)) {
    return false;
  }
  ExpressionNode::Type correspondingType;
  switch (directFunction.type()) {
    case ExpressionNode::Type::Cosine:
      correspondingType = ExpressionNode::Type::ArcCosine;
      break;
    case ExpressionNode::Type::Sine:
      correspondingType = ExpressionNode::Type::ArcSine;
      break;
    default:
      assert(directFunction.type() == ExpressionNode::Type::Tangent);
      correspondingType = ExpressionNode::Type::ArcTangent;
      break;
  }
  return inverseFunction.type() == correspondingType;
}

Expression Trigonometry::UnitConversionFactor(Preferences::AngleUnit fromUnit,
                                              Preferences::AngleUnit toUnit) {
  if (fromUnit == toUnit) {
    // Just an optimisation to gain some time at reduction
    return Rational::Builder(1);
  }
  return Division::Builder(PiExpressionInAngleUnit(toUnit),
                           PiExpressionInAngleUnit(fromUnit));
}

bool Trigonometry::ExpressionIsTangentOrInverseOfTangent(const Expression& e,
                                                         bool inverse) {
  // We look for (sin(x) * cos(x)^-1) or (sin(x)^-1 * cos(x))
  assert(ExpressionNode::Type::Sine < ExpressionNode::Type::Cosine);
  ExpressionNode::Type numeratorType =
      inverse ? ExpressionNode::Type::Cosine : ExpressionNode::Type::Sine;
  ExpressionNode::Type denominatorType =
      inverse ? ExpressionNode::Type::Sine : ExpressionNode::Type::Cosine;
  int numeratorIndex = inverse ? 1 : 0;  // Cos is always after sin;
  int denominatorIndex = inverse ? 0 : 1;
  if (e.type() == ExpressionNode::Type::Multiplication &&
      e.childAtIndex(numeratorIndex).type() == numeratorType &&
      e.childAtIndex(denominatorIndex).type() == ExpressionNode::Type::Power &&
      e.childAtIndex(denominatorIndex).childAtIndex(0).type() ==
          denominatorType &&
      e.childAtIndex(denominatorIndex).childAtIndex(1).type() ==
          ExpressionNode::Type::Rational &&
      e.childAtIndex(denominatorIndex)
          .childAtIndex(1)
          .convert<Rational>()
          .isMinusOne() &&
      e.childAtIndex(numeratorIndex)
          .childAtIndex(0)
          .isIdenticalTo(e.childAtIndex(denominatorIndex)
                             .childAtIndex(0)
                             .childAtIndex(0))) {
    return true;
  }
  return false;
}

bool Trigonometry::ExpressionIsEquivalentToTangent(const Expression& e) {
  return ExpressionIsTangentOrInverseOfTangent(e, false);
}

bool Trigonometry::ExpressionIsEquivalentToInverseOfTangent(
    const Expression& e) {
  return ExpressionIsTangentOrInverseOfTangent(e, true);
}

Expression Trigonometry::shallowReduceDirectFunction(
    Expression& e, ReductionContext reductionContext) {
  assert(isDirectTrigonometryFunction(e));

  // Step 0.0 Map on list child if possible
  {
    Expression eReduced = SimplificationHelper::defaultShallowReduce(
        e, &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::KeepUnits,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists);
    if (!eReduced.isUninitialized()) {
      return eReduced;
    }
  }

  // Step 0.1 Eliminate angle unit if there is one
  Expression unit;
  e.childAtIndex(0).removeUnit(&unit);
  if (!unit.isUninitialized()) {
    // _unit^-1 and _unit*_unit cannot be valid angle units
    if (unit.type() != ExpressionNode::Type::Unit) {
      return e.replaceWithUndefinedInPlace();
    }
    Unit unitRef = static_cast<Unit&>(unit);
    if (unitRef.representative()->dimensionVector() !=
        Unit::AngleRepresentative::Default().dimensionVector()) {
      return e.replaceWithUndefinedInPlace();
    }
    // The child has been converted to radians, turn it into the current unit
    if (reductionContext.angleUnit() != Preferences::AngleUnit::Radian) {
      e.replaceChildAtIndexInPlace(
          0, Multiplication::Builder(
                 e.childAtIndex(0),
                 UnitConversionFactor(Preferences::AngleUnit::Radian,
                                      reductionContext.angleUnit())
                     .shallowReduce(reductionContext))
                 .shallowReduce(reductionContext));
    }
  }

  // Step 1. Try finding an easy standard calculation reduction
  Expression lookup = TrigonometryCheatTable::Table()->simplify(
      e.childAtIndex(0), e.type(), reductionContext);
  if (!lookup.isUninitialized()) {
    e.replaceWithInPlace(lookup);
    return lookup;
  }

  // Step 2. Look for an expression of type "cos(acos(x))", return x
  if (AreInverseFunctions(e, e.childAtIndex(0))) {
    Expression result = e.childAtIndex(0).childAtIndex(0);
    if (reductionContext.complexFormat() == Preferences::ComplexFormat::Real) {
      List listOfDependencies = List::Builder();
      listOfDependencies.addChildAtIndexInPlace(e.childAtIndex(0).clone(), 0,
                                                0);
      result = Dependency::Builder(e.childAtIndex(0).childAtIndex(0),
                                   listOfDependencies);
      result = result.shallowReduce(reductionContext);
    }
    e.replaceWithInPlace(result);
    return result;
  }

  // Step 3. Look for an expression of type "cos(asin(x))" or "sin(acos(x)),
  // return sqrt(1-x^2) These equalities are true on complexes
  if ((e.type() == ExpressionNode::Type::Cosine &&
       e.childAtIndex(0).type() == ExpressionNode::Type::ArcSine) ||
      (e.type() == ExpressionNode::Type::Sine &&
       e.childAtIndex(0).type() == ExpressionNode::Type::ArcCosine)) {
    Expression sqrt = Power::Builder(
        Addition::Builder(Rational::Builder(1),
                          Multiplication::Builder(
                              Rational::Builder(-1),
                              Power::Builder(e.childAtIndex(0).childAtIndex(0),
                                             Rational::Builder(2)))),
        Rational::Builder(1, 2));
    // reduce x^2
    sqrt.childAtIndex(0).childAtIndex(1).childAtIndex(1).shallowReduce(
        reductionContext);
    // reduce -1*x^2
    sqrt.childAtIndex(0).childAtIndex(1).shallowReduce(reductionContext);
    // reduce 1-1*x^2
    sqrt.childAtIndex(0).shallowReduce(reductionContext);
    e.replaceWithInPlace(sqrt);
    // reduce sqrt(1+(-1)*x^2)
    return sqrt.shallowReduce(reductionContext);
  }

  // Step 4. Look for an expression of type "cos(atan(x))" or "sin(atan(x))"
  // cos(atan(x)) --> 1/sqrt(1+x^2)
  // sin(atan(x)) --> x/sqrt(1+x^2)
  // These equalities are true on complexes
  if ((e.type() == ExpressionNode::Type::Cosine ||
       e.type() == ExpressionNode::Type::Sine) &&
      e.childAtIndex(0).type() == ExpressionNode::Type::ArcTangent) {
    Expression x = e.childAtIndex(0).childAtIndex(0);
    // Build 1/sqrt(1+x^2)
    Expression res = Power::Builder(
        Addition::Builder(
            Rational::Builder(1),
            Power::Builder(
                e.type() == ExpressionNode::Type::Cosine ? x : x.clone(),
                Rational::Builder(2))),
        Rational::Builder(-1, 2));

    // reduce x^2
    res.childAtIndex(0).childAtIndex(1).shallowReduce(reductionContext);
    // reduce 1+x^2
    res.childAtIndex(0).shallowReduce(reductionContext);
    if (e.type() == ExpressionNode::Type::Sine) {
      res = Multiplication::Builder(x, res);
      // reduce (1+x^2)^(-1/2)
      res.childAtIndex(1).shallowReduce(reductionContext);
    }
    e.replaceWithInPlace(res);
    // reduce (1+x^2)^(-1/2) or x*(1+x^2)^(-1/2)
    return res.shallowReduce(reductionContext);
  }

  // Step 5. Look for an expression of type "cos(-a)", return "+/-cos(a)"
  Expression positiveArg =
      e.childAtIndex(0).makePositiveAnyNegativeNumeralFactor(reductionContext);
  if (!positiveArg.isUninitialized()) {
    // The argument was of form cos(-a)
    if (e.type() == ExpressionNode::Type::Cosine) {
      // cos(-a) = cos(a)
      return e.shallowReduce(reductionContext);
    } else {
      // sin(-a) = -sin(a) or tan(-a) = -tan(a)
      Multiplication m = Multiplication::Builder(Rational::Builder(-1));
      e.replaceWithInPlace(m);
      m.addChildAtIndexInPlace(e, 1, 1);
      e.shallowReduce(reductionContext);
      return m.shallowReduce(reductionContext);
    }
  }

  /* Step 6. Look for an expression of type "cos(p/q * π)" in radians or
   * "cos(p/q)" in degrees, put the argument in [0, π/2[ or [0, 90[ and
   * multiply the cos/sin/tan by -1 if needed.
   * We know thanks to Step 3 that p/q > 0. */
  const Preferences::AngleUnit angleUnit = reductionContext.angleUnit();
  if ((angleUnit == Preferences::AngleUnit::Radian &&
       e.childAtIndex(0).type() == ExpressionNode::Type::Multiplication &&
       e.childAtIndex(0).numberOfChildren() == 2 &&
       e.childAtIndex(0).childAtIndex(1).type() ==
           ExpressionNode::Type::ConstantMaths &&
       e.childAtIndex(0).childAtIndex(1).convert<Constant>().isPi() &&
       e.childAtIndex(0).childAtIndex(0).type() ==
           ExpressionNode::Type::Rational) ||
      ((angleUnit == Preferences::AngleUnit::Degree ||
        angleUnit == Preferences::AngleUnit::Gradian) &&
       e.childAtIndex(0).type() == ExpressionNode::Type::Rational)) {
    Rational r = angleUnit == Preferences::AngleUnit::Radian
                     ? e.childAtIndex(0).childAtIndex(0).convert<Rational>()
                     : e.childAtIndex(0).convert<Rational>();
    /* Step 6.1. In radians:
     * We first check if p/q * π is already in the right quadrant:
     * p/q * π < π/2 => p/q < 2 => 2p < q */
    Integer dividand = Integer::Addition(r.unsignedIntegerNumerator(),
                                         r.unsignedIntegerNumerator());
    Integer divisor = Integer::Multiplication(r.integerDenominator(),
                                              Integer(PiDivisor(angleUnit)));
    if (divisor.isLowerThan(dividand)) {
      /* Step 6.2. p/q * π is not in the wanted trigonometrical quadrant.
       * We could subtract n*π to p/q with n an integer.
       * Given p/q = (q'*q+r')/q, we have
       * (p/q * π - q'*π) < π/2 => r'/q < 1/2 => 2*r'<q
       * (q' is the theoretical n).*/
      int unaryCoefficient = 1;  // store 1 or -1 for the final result.
      Integer piDivisor = Integer::Multiplication(
          r.integerDenominator(), Integer(PiDivisor(angleUnit)));
      IntegerDivision div =
          Integer::Division(r.unsignedIntegerNumerator(), piDivisor);
      dividand = Integer::Addition(div.remainder, div.remainder);
      if (divisor.isLowerThan(dividand)) {
        /* Step 6.3. r'/q * π is not in the wanted trigonometrical quadrant,
         * and because r'<q (as r' is the remainder of an euclidian division
         * by q), we know that r'/q*π is in [π/2; π[.
         * So we can take the new angle π - r'/q*π, which changes cosinus or
         * tangent, but not sinus. The new rational is 1-r'/q = (q-r')/q. */
        div.remainder = Integer::Subtraction(piDivisor, div.remainder);
        if (e.type() == ExpressionNode::Type::Cosine ||
            e.type() == ExpressionNode::Type::Tangent) {
          unaryCoefficient *= -1;
        }
      }
      if (div.remainder.isOverflow()) {
        return e;
      }
      // Step 6.5. Build the new result.
      Integer rDenominator = r.integerDenominator();
      Expression newR = Rational::Builder(div.remainder, rDenominator);
      Expression rationalParent =
          angleUnit == Preferences::AngleUnit::Radian ? e.childAtIndex(0) : e;
      rationalParent.replaceChildAtIndexInPlace(0, newR);
      newR.shallowReduce(reductionContext);
      if (angleUnit == Preferences::AngleUnit::Radian) {
        e.childAtIndex(0).shallowReduce(reductionContext);
      }
      if (Integer::Division(div.quotient, Integer(2)).remainder.isOne() &&
          e.type() != ExpressionNode::Type::Tangent) {
        /* Step 6.6. If we subtracted an odd number of π in 6.2, we need to
         * multiply the result by -1 (because cos((2k+1)π + x) = -cos(x) */
        unaryCoefficient *= -1;
      }
      Expression simplifiedCosine =
          e.shallowReduce(reductionContext);  // recursive
      Multiplication m =
          Multiplication::Builder(Rational::Builder(unaryCoefficient));
      simplifiedCosine.replaceWithInPlace(m);
      m.addChildAtIndexInPlace(simplifiedCosine, 1, 1);
      return m.shallowReduce(reductionContext);
    }
    assert(r.isPositive() == TrinaryBoolean::True);
  }
  return e;
}

Expression Trigonometry::shallowReduceInverseFunction(
    Expression& e, ReductionContext reductionContext) {
  assert(isInverseTrigonometryFunction(e));
  // Step 0. Map on list child if possible
  {
    Expression eReduced = SimplificationHelper::defaultShallowReduce(
        e, &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::BanUnits,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists);
    if (!eReduced.isUninitialized()) {
      return eReduced;
    }
  }

  const Preferences::AngleUnit angleUnit = reductionContext.angleUnit();
  float pi = PiInAngleUnit(angleUnit);

  // Step 1. Look for an expression of type "acos(cos(x))", return x
  Expression result;
  // special case for arctan(sin/cos)
  bool isArcTanOfSinCos = e.type() == ExpressionNode::Type::ArcTangent &&
                          ExpressionIsEquivalentToTangent(e.childAtIndex(0));
  if (isArcTanOfSinCos || AreInverseFunctions(e.childAtIndex(0), e)) {
    Expression result = isArcTanOfSinCos
                            ? e.childAtIndex(0).childAtIndex(0).childAtIndex(0)
                            : e.childAtIndex(0).childAtIndex(0);
    float x =
        result.node()
            ->approximate(float(), ApproximationContext(reductionContext, true))
            .toScalar();
    if (!(std::isinf(x) || std::isnan(x))) {
      // We translate the result within [-π,π] for acos(cos), [-π/2,π/2] for
      // asin(sin) and atan(tan)
      float k = (e.type() == ExpressionNode::Type::ArcCosine)
                    ? std::floor(x / pi)
                    : std::floor((x + pi / 2.0f) / pi);
      if (!std::isinf(k) && !std::isnan(k) &&
          std::fabs(k) <= static_cast<float>(INT_MAX)) {
        int kInt = static_cast<int>(k);
        Multiplication mult = Multiplication::Builder(
            Rational::Builder(-kInt),
            PiExpressionInAngleUnit(reductionContext.angleUnit()));
        result = Addition::Builder(result.clone(), mult);
        mult.shallowReduce(reductionContext);
        if ((e.type() == ExpressionNode::Type::ArcCosine) &&
            ((int)k % 2 == 1)) {
          Expression sub = Subtraction::Builder(
              PiExpressionInAngleUnit(reductionContext.angleUnit()), result);
          result.shallowReduce(reductionContext);
          result = sub;
        }
        if ((e.type() == ExpressionNode::Type::ArcSine) && ((int)k % 2 == 1)) {
          Expression add = result;
          result = Opposite::Builder(add);
          add.shallowReduce(reductionContext);
        }
      }
      e.replaceWithInPlace(result);
      return result.shallowReduce(reductionContext);
    }
  }

  // Step 2. Look for an expression of type atan(1/x) or atan(cos/sin), return
  // sign(x)*π/2-atan(x)
  bool isArcTanOfCosSin =
      e.type() == ExpressionNode::Type::ArcTangent &&
      ExpressionIsEquivalentToInverseOfTangent(e.childAtIndex(0));
  if (isArcTanOfCosSin ||
      (e.type() == ExpressionNode::Type::ArcTangent &&
       e.childAtIndex(0).type() == ExpressionNode::Type::Power &&
       e.childAtIndex(0).childAtIndex(1).type() ==
           ExpressionNode::Type::Rational &&
       e.childAtIndex(0).childAtIndex(1).convert<Rational>().isMinusOne())) {
    Expression x;
    if (isArcTanOfCosSin) {
      /* (cos(x)/sin(x))^-1 is turned to dep(tan(x),{1/sin(x)} so we don't
       * need to add a dependency. */
      x = Power::Builder(Undefined::Builder(), Rational::Builder(-1));
      x.replaceChildAtIndexInPlace(0, e.childAtIndex(0));
      x = x.shallowReduce(reductionContext);
    } else {
      // This equality is not true if x = 0. We thus add a dependency in 1/x
      Dependency dep =
          Dependency::Builder(Undefined::Builder(), List::Builder());
      dep.addDependency(e.childAtIndex(0).clone());
      dep.replaceChildAtIndexInPlace(0, e.childAtIndex(0).childAtIndex(0));
      x = dep.shallowReduce(reductionContext);
    }
    Expression sign = SignFunction::Builder(x.clone());
    Multiplication m0 = Multiplication::Builder(
        Rational::Builder(1, 2), sign, PiExpressionInAngleUnit(angleUnit));
    sign.shallowReduce(reductionContext);
    e.replaceChildAtIndexInPlace(0, x);
    Addition a = Addition::Builder(m0);
    m0.shallowReduce(reductionContext);
    e.replaceWithInPlace(a);
    Multiplication m1 = Multiplication::Builder(Rational::Builder(-1), e);
    e.shallowReduce(reductionContext);
    a.addChildAtIndexInPlace(m1, 1, 1);
    m1.shallowReduce(reductionContext);
    return a.shallowReduce(reductionContext);
  }

  // Step 3. Try finding an easy standard calculation reduction
  Expression lookup = TrigonometryCheatTable::Table()->simplify(
      e.childAtIndex(0), e.type(), reductionContext);
  if (!lookup.isUninitialized()) {
    e.replaceWithInPlace(lookup);
    return lookup;
  }

  /* We do not apply some rules if:
   * - the parent node is a cosine, a sine or a tangent. In this case there is a
   * simplication of form f(g(x)) with f cos, sin or tan and g acos, asin or
   * atan.
   * - the reduction is being BottomUp. In this case, we do not yet have any
   *   information on the parent which could later be a cosine, a sine or a
   * tangent.
   */
  Expression p = e.parent();
  bool letArcFunctionAtRoot =
      !p.isUninitialized() && isDirectTrigonometryFunction(p);
  /* Step 4. Handle opposite argument: acos(-x) = π-acos(x),
   * asin(-x) = -asin(x), atan(-x)= -atan(x) *
   */
  if (!letArcFunctionAtRoot) {
    Expression positiveArg =
        e.childAtIndex(0).makePositiveAnyNegativeNumeralFactor(
            reductionContext);
    if (!positiveArg.isUninitialized()) {
      // The argument was made positive
      // acos(-x) = π-acos(x)
      if (e.type() == ExpressionNode::Type::ArcCosine) {
        Expression pi = PiExpressionInAngleUnit(angleUnit);
        Subtraction s = Subtraction::Builder();
        e.replaceWithInPlace(s);
        s.replaceChildAtIndexInPlace(0, pi);
        s.replaceChildAtIndexInPlace(1, e);
        e.shallowReduce(reductionContext);
        return s.shallowReduce(reductionContext);
      } else {
        // asin(-x) = -asin(x) or atan(-x) = -atan(x)
        Multiplication m = Multiplication::Builder(Rational::Builder(-1));
        e.replaceWithInPlace(m);
        m.addChildAtIndexInPlace(e, 1, 1);
        e.shallowReduce(reductionContext);
        return m.shallowReduce(reductionContext);
      }
    }
  }

  return e;
}

Expression Trigonometry::shallowReduceAdvancedFunction(
    Expression& e, ReductionContext reductionContext) {
  /* Since the child always ends in a direct function, angle units are left
   * untouched here */
  assert(isAdvancedTrigonometryFunction(e));
  {
    Expression eReduced = SimplificationHelper::defaultShallowReduce(
        e, &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::KeepUnits);
    if (!eReduced.isUninitialized()) {
      return eReduced;
    }
  }
  // Step 0. Replace with inverse (^-1) of equivalent direct function.
  Expression result;
  switch (e.type()) {
    case ExpressionNode::Type::Secant:
      result = Cosine::Builder(e.childAtIndex(0));
      break;
    case ExpressionNode::Type::Cosecant:
      result = Sine::Builder(e.childAtIndex(0));
      break;
    default:
      assert(e.type() == ExpressionNode::Type::Cotangent);
      // Use cot(x)=cos(x)/sin(x) definition to handle cot(pi/2)=0
      Cosine c = Cosine::Builder(e.childAtIndex(0).clone());
      Sine s = Sine::Builder(e.childAtIndex(0));
      Division d = Division::Builder(c, s);
      e.replaceWithInPlace(d);
      c.shallowReduce(reductionContext);
      s.shallowReduce(reductionContext);
      return d.shallowReduce(reductionContext);
      break;
  }
  Power p = Power::Builder(result, Rational::Builder(-1));
  e.replaceWithInPlace(p);
  result.shallowReduce(reductionContext);
  return p.shallowReduce(reductionContext);
}

Expression Trigonometry::shallowReduceInverseAdvancedFunction(
    Expression& e, ReductionContext reductionContext) {
  assert(isInverseAdvancedTrigonometryFunction(e));
  {
    Expression eReduced = SimplificationHelper::defaultShallowReduce(
        e, &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::BanUnits);
    if (!eReduced.isUninitialized()) {
      return eReduced;
    }
  }
  Expression result;
  // Step 1. Manage specific cases for Arcotangent
  if (e.type() == ExpressionNode::Type::ArcCotangent) {
    TrinaryBoolean isNull =
        e.childAtIndex(0).isNull(reductionContext.context());
    // Step 1.1. Reduce ArcCotangent(0) to π/2
    if (isNull == TrinaryBoolean::True) {
      result = Multiplication::Builder(
          PiExpressionInAngleUnit(reductionContext.angleUnit()),
          Rational::Builder(1, 2));
      e.replaceWithInPlace(result);
      return result.shallowReduce(reductionContext);
    }
    // Step 1.2. Do not reduce ArcCotangent when we do not know if child is null
    // and target is not User
    if (isNull == TrinaryBoolean::Unknown &&
        reductionContext.target() != ReductionTarget::User) {
      return e;
    }
  }
  // Step 2. Replace with equivalent inverse function on inverse (^-1) argument
  Power p = Power::Builder(e.childAtIndex(0), Rational::Builder(-1));
  switch (e.type()) {
    case ExpressionNode::Type::ArcSecant:
      result = ArcCosine::Builder(p);
      break;
    case ExpressionNode::Type::ArcCosecant:
      result = ArcSine::Builder(p);
      break;
    default:
      assert(e.type() == ExpressionNode::Type::ArcCotangent);
      result = ArcTangent::Builder(p);
      break;
  }
  e.replaceWithInPlace(result);
  p.shallowReduce(reductionContext);
  return result.shallowReduce(reductionContext);
}

Expression Trigonometry::replaceWithAdvancedFunction(Expression& e,
                                                     Expression& denominator) {
  /* Replace direct trigonometric function with their advanced counterpart.
   * This function must be called within a denominator. */
  assert(e.type() == ExpressionNode::Type::Power &&
         !denominator.isUninitialized());
  assert(isDirectTrigonometryFunction(denominator));
  Expression result;
  switch (denominator.type()) {
    case ExpressionNode::Type::Cosine:
      result = Secant::Builder(denominator.childAtIndex(0));
      break;
    case ExpressionNode::Type::Sine:
      result = Cosecant::Builder(denominator.childAtIndex(0));
      break;
    default:
      assert(denominator.type() == ExpressionNode::Type::Tangent);
      result = Cotangent::Builder(denominator.childAtIndex(0));
      break;
  }
  e.replaceWithInPlace(result);
  return result;
}

template <typename T>
std::complex<T> Trigonometry::ConvertToRadian(
    const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  if (angleUnit != Preferences::AngleUnit::Radian) {
    return c *
           std::complex<T>((T)M_PI / (T)Trigonometry::PiInAngleUnit(angleUnit));
  }
  return c;
}

template <typename T>
std::complex<T> Trigonometry::ConvertRadianToAngleUnit(
    const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  if (angleUnit != Preferences::AngleUnit::Radian) {
    return c *
           std::complex<T>((T)Trigonometry::PiInAngleUnit(angleUnit) / (T)M_PI);
  }
  return c;
}

bool Trigonometry::DetectLinearPatternOfCosOrSin(
    const Expression& e, ReductionContext reductionContext, const char* symbol,
    bool acceptAddition, double* coefficientBeforeCos,
    double* coefficientBeforeSymbol, double* angle) {
  if (e.type() == ExpressionNode::Type::Multiplication ||
      (acceptAddition && e.type() == ExpressionNode::Type::Addition)) {
    /* Check if expression is a*b*cos(theta+constant) or if
     * expression is a*cos(theta+b)+constant */
    int nChildren = e.numberOfChildren();
    for (int i = 0; i < nChildren; i++) {
      Expression child = e.childAtIndex(i);
      if (!DetectLinearPatternOfCosOrSin(child, reductionContext, symbol,
                                         acceptAddition, coefficientBeforeCos,
                                         coefficientBeforeSymbol, angle)) {
        continue;
      }
      if (nChildren == 1) {
        return true;
      }
      Expression clone = e.clone();
      NAryExpression eWithoutCos = static_cast<NAryExpression&>(clone);
      eWithoutCos.removeChildAtIndexInPlace(i);
      if (eWithoutCos.polynomialDegree(reductionContext.context(), symbol) !=
          0) {
        return false;
      }
      if (coefficientBeforeCos &&
          e.type() == ExpressionNode::Type::Multiplication) {
        *coefficientBeforeCos *= eWithoutCos.approximateToScalar<double>(
            reductionContext.context(), reductionContext.complexFormat(),
            reductionContext.angleUnit());
      }
      return true;
    }
    return false;
  }

  if (coefficientBeforeCos) {
    *coefficientBeforeCos = 1.0;
  }

  // Check is expression is cos(theta+constant)
  if (e.type() != ExpressionNode::Type::Sine &&
      e.type() != ExpressionNode::Type::Cosine) {
    return false;
  }

  Expression child = e.childAtIndex(0);
  int thetaDeg = child.polynomialDegree(reductionContext.context(), symbol);
  if (thetaDeg != 1) {
    return false;
  }
  Expression coefficients[2];  // Only 2 coefficients since child has degree 1
  child.getPolynomialReducedCoefficients(
      symbol, coefficients, reductionContext.context(),
      reductionContext.complexFormat(), reductionContext.angleUnit(),
      reductionContext.unitFormat(), reductionContext.symbolicComputation());

  if (coefficients[0].isUninitialized() || coefficients[1].isUninitialized()) {
    // Reduction of coefficients failed
    return false;
  }

  if (angle) {
    *angle = coefficients[0].approximateToScalar<double>(
        reductionContext.context(), reductionContext.complexFormat(),
        reductionContext.angleUnit());
    if (e.type() == ExpressionNode::Type::Sine) {
      *angle -= M_PI_2;
    }
    *angle = std::fmod(*angle, 2 * M_PI);
  }

  if (coefficientBeforeSymbol) {
    *coefficientBeforeSymbol = coefficients[1].approximateToScalar<double>(
        reductionContext.context(), reductionContext.complexFormat(),
        reductionContext.angleUnit());
  }
  return true;
}

static Expression AddAngleUnitToDirectFunctionIfNeeded(
    Expression& e, Preferences::AngleUnit angleUnit) {
  assert(e.type() == ExpressionNode::Type::Cosine ||
         e.type() == ExpressionNode::Type::Sine ||
         e.type() == ExpressionNode::Type::Tangent ||
         e.type() == ExpressionNode::Type::Cosecant ||
         e.type() == ExpressionNode::Type::Secant ||
         e.type() == ExpressionNode::Type::Cotangent);

  assert(e.numberOfChildren() == 1 && !e.childAtIndex(0).isUninitialized());

  Expression child = e.childAtIndex(0);
  bool containsPi = false;
  bool containsOtherChildrenThanCombinationOfNumberAndPi =
      child.recursivelyMatches(
          [](const Expression e, Context* context, void* auxiliary) {
            if (e.type() == ExpressionNode::Type::ConstantMaths &&
                static_cast<const Constant&>(e).isPi()) {
              bool* containsPi = static_cast<bool*>(auxiliary);
              *containsPi = true;
              return TrinaryBoolean::False;
            }
            if (e.isNumber()) {
              return TrinaryBoolean::False;
            }
            if (e.type() == ExpressionNode::Type::Addition ||
                e.type() == ExpressionNode::Type::Subtraction ||
                e.type() == ExpressionNode::Type::Multiplication ||
                e.type() == ExpressionNode::Type::Division ||
                e.type() == ExpressionNode::Type::Power) {
              return TrinaryBoolean::Unknown;
            }
            // Stop search if the expression is not one of the above
            return TrinaryBoolean::True;
          },
          nullptr, SymbolicComputation::DoNotReplaceAnySymbol,
          static_cast<void*>(&containsPi));

  if (containsOtherChildrenThanCombinationOfNumberAndPi) {
    return e;
  }

  if (containsPi == (angleUnit == Preferences::AngleUnit::Radian)) {
    /* Do not add angle units if the child contains Pi and the angle is in Rad
     * or if the child does not contain Pi and the angle unit is other. */
    return e;
  }

  Unit unit = Unit::Builder(
      UnitNode::AngleRepresentative::DefaultRepresentativeForAngleUnit(
          angleUnit));
  if (child.type() == ExpressionNode::Type::Addition ||
      child.type() == ExpressionNode::Type::Subtraction) {
    child = Parenthesis::Builder(child);
  }
  Expression newChild = Multiplication::Builder(child, unit);
  e.replaceChildAtIndexInPlace(0, newChild);
  return e;
}

Expression Trigonometry::DeepAddAngleUnitToAmbiguousDirectFunctions(
    Expression& e, Preferences::AngleUnit angleUnit) {
  if (e.type() == ExpressionNode::Type::Cosine ||
      e.type() == ExpressionNode::Type::Sine ||
      e.type() == ExpressionNode::Type::Tangent ||
      e.type() == ExpressionNode::Type::Cosecant ||
      e.type() == ExpressionNode::Type::Secant ||
      e.type() == ExpressionNode::Type::Cotangent) {
    e = AddAngleUnitToDirectFunctionIfNeeded(e, angleUnit);
    return e;
  }
  int nChildren = e.numberOfChildren();
  for (int i = 0; i < nChildren; i++) {
    Expression child = e.childAtIndex(i);
    DeepAddAngleUnitToAmbiguousDirectFunctions(child, angleUnit);
  }
  return e;
}

template std::complex<float> Trigonometry::ConvertToRadian<float>(
    std::complex<float>, Preferences::AngleUnit);
template std::complex<double> Trigonometry::ConvertToRadian<double>(
    std::complex<double>, Preferences::AngleUnit);
template std::complex<float> Trigonometry::ConvertRadianToAngleUnit<float>(
    std::complex<float>, Preferences::AngleUnit);
template std::complex<double> Trigonometry::ConvertRadianToAngleUnit<double>(
    std::complex<double>, Preferences::AngleUnit);

}  // namespace Poincare
