#include <assert.h>
#include <float.h>
#include <ion.h>
#include <limits.h>
#include <omg/float.h>
#include <poincare/expression.h>
#include <poincare/k_tree.h>
#include <poincare/old/addition.h>
#include <poincare/old/arc_cosine.h>
#include <poincare/old/arc_sine.h>
#include <poincare/old/arc_tangent.h>
#include <poincare/old/constant.h>
#include <poincare/old/cosecant.h>
#include <poincare/old/cosine.h>
#include <poincare/old/cotangent.h>
#include <poincare/old/decimal.h>
#include <poincare/old/dependency.h>
#include <poincare/old/derivative.h>
#include <poincare/old/division.h>
#include <poincare/old/multiplication.h>
#include <poincare/old/opposite.h>
#include <poincare/old/parenthesis.h>
#include <poincare/old/power.h>
#include <poincare/old/rational.h>
#include <poincare/old/secant.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/sign_function.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/sine.h>
#include <poincare/old/subtraction.h>
#include <poincare/old/symbol.h>
#include <poincare/old/tangent.h>
#include <poincare/old/trigonometry.h>
#include <poincare/old/trigonometry_cheat_table.h>
#include <poincare/old/undefined.h>
#include <poincare/old/unit.h>
#include <poincare/preferences.h>
#include <poincare/src/expression/unit_representatives.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/memory/tree.h>

#include <cmath>

namespace Poincare {

/* The values must be in the order defined in poincare/preferences:
 * Radians / Degrees / Gradians */

UserExpression Trigonometry::PiExpressionInAngleUnit(
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

UserExpression Trigonometry::AnglePeriodInAngleUnit(
    Preferences::AngleUnit angleUnit) {
  return Multiplication::Builder(
      Rational::Builder(2), Trigonometry::PiExpressionInAngleUnit(angleUnit));
}

bool Trigonometry::IsDirectTrigonometryFunction(const UserExpression& e) {
  return e.isOfType({ExpressionNode::Type::Cosine, ExpressionNode::Type::Sine,
                     ExpressionNode::Type::Tangent});
}

bool Trigonometry::IsInverseTrigonometryFunction(const UserExpression& e) {
  return e.isOfType({ExpressionNode::Type::ArcCosine,
                     ExpressionNode::Type::ArcSine,
                     ExpressionNode::Type::ArcTangent});
}

bool Trigonometry::IsAdvancedTrigonometryFunction(const UserExpression& e) {
  return e.isOfType({ExpressionNode::Type::Secant,
                     ExpressionNode::Type::Cosecant,
                     ExpressionNode::Type::Cotangent});
}

bool Trigonometry::IsInverseAdvancedTrigonometryFunction(
    const UserExpression& e) {
  return e.isOfType({ExpressionNode::Type::ArcSecant,
                     ExpressionNode::Type::ArcCosecant,
                     ExpressionNode::Type::ArcCotangent});
}

bool Trigonometry::AreInverseFunctions(const UserExpression& directFunction,
                                       const UserExpression& inverseFunction) {
  if (!IsDirectTrigonometryFunction(directFunction)) {
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

UserExpression Trigonometry::UnitConversionFactor(
    Preferences::AngleUnit fromUnit, Preferences::AngleUnit toUnit) {
  if (fromUnit == toUnit) {
    // Just an optimisation to gain some time at reduction
    return Rational::Builder(1);
  }
  return Division::Builder(PiExpressionInAngleUnit(toUnit),
                           PiExpressionInAngleUnit(fromUnit));
}

bool Trigonometry::ExpressionIsTangentOrInverseOfTangent(
    const UserExpression& e, bool inverse) {
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
      e.childAtIndex(denominatorIndex).childAtIndex(1).isMinusOne() &&
      e.childAtIndex(numeratorIndex)
          .childAtIndex(0)
          .isIdenticalTo(e.childAtIndex(denominatorIndex)
                             .childAtIndex(0)
                             .childAtIndex(0))) {
    return true;
  }
  return false;
}

bool Trigonometry::ExpressionIsEquivalentToTangent(const UserExpression& e) {
  return ExpressionIsTangentOrInverseOfTangent(e, false);
}

bool Trigonometry::ExpressionIsEquivalentToInverseOfTangent(
    const UserExpression& e) {
  return ExpressionIsTangentOrInverseOfTangent(e, true);
}

// TODO_PCJ: Delete these method
#if 0
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

Expression Trigonometry::ShallowReduceDirectFunction(
    Expression& e, ReductionContext reductionContext) {
  assert(IsDirectTrigonometryFunction(e));

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
    if (unit.type() != ExpressionNode::Type::OUnit) {
      return e.replaceWithUndefinedInPlace();
    }
    OUnit unitRef = static_cast<OUnit&>(unit);
    if (unitRef.representative()->dimensionVector() !=
        OUnit::AngleRepresentative::Default().dimensionVector()) {
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

  // Step 2. Look for an expression of type "cos(acos(x))", return x.
  if (AreInverseFunctions(e, e.childAtIndex(0))) {
    Expression result = e.childAtIndex(0).childAtIndex(0);
    // Only real functions asin and acos have a domain of definition
    if (reductionContext.complexFormat() == Preferences::ComplexFormat::Real &&
        e.type() != ExpressionNode::Type::Tangent) {
      OList listOfDependencies = OList::Builder();
      listOfDependencies.addChildAtIndexInPlace(e.childAtIndex(0).clone(), 0,
                                                0);
      result = Dependency::Builder(result, listOfDependencies);
      result = result.shallowReduce(reductionContext);
    }
    e.replaceWithInPlace(result);
    return result;
  }

  /* Step 3. Look for an expression of type "cos(asin(x))" or "sin(acos(x)),
   * return sqrt(1-x^2) These equalities are true on complexes */
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

  /* Step 4. Look for an expression of type "cos(atan(x))" or "sin(atan(x))"
   * cos(atan(x)) --> 1/sqrt(1+x^2)
   * sin(atan(x)) --> x/sqrt(1+x^2)
   * These equalities are true on complexes */
  if ((e.isOfType(
          {ExpressionNode::Type::Cosine, ExpressionNode::Type::Sine})) &&
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
        if (e.isOfType({ExpressionNode::Type::Cosine,
                        ExpressionNode::Type::Tangent})) {
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
    assert(r.isPositive() == OMG::Troolean::True);
  }
  return e;
}

Expression Trigonometry::ShallowReduceInverseFunction(
    Expression& e, ReductionContext reductionContext) {
  assert(IsInverseTrigonometryFunction(e));
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
  double pi = PiInAngleUnit(angleUnit);

  // Step 1. Look for an expression of type "acos(cos(x))", return x
  Expression result;
  // special case for arctan(sin/cos)
  bool isArcTanOfSinCos = e.type() == ExpressionNode::Type::ArcTangent &&
                          ExpressionIsEquivalentToTangent(e.childAtIndex(0));
  if (isArcTanOfSinCos || AreInverseFunctions(e.childAtIndex(0), e)) {
    bool isArccos = e.type() == ExpressionNode::Type::ArcCosine;
    Expression result = isArcTanOfSinCos
                            ? e.childAtIndex(0).childAtIndex(0).childAtIndex(0)
                            : e.childAtIndex(0).childAtIndex(0);
    double x = result.approximateToScalar<double>(
        ApproximationContext(reductionContext, true));
    if (std::isfinite(x)) {
      /* We translate the result within [-π,π] for acos(cos), [-π/2,π/2] for
       * asin(sin) and atan(tan) */
      double k =
          isArccos ? std::floor(x / pi) : std::floor((x + pi / 2.0f) / pi);
      assert(std::isfinite(k));
      if (std::fabs(k) <= static_cast<double>(INT_MAX)) {
        int kInt = static_cast<int>(k);
        Multiplication mult = Multiplication::Builder(
            Rational::Builder(-kInt),
            PiExpressionInAngleUnit(reductionContext.angleUnit()));
        result = Addition::Builder(result.clone(), mult);
        mult.shallowReduce(reductionContext);
        if (isArccos && kInt % 2 == 1) {
          Expression sub = Subtraction::Builder(
              PiExpressionInAngleUnit(reductionContext.angleUnit()), result);
          result.shallowReduce(reductionContext);
          result = sub;
        }
        if (e.type() == ExpressionNode::Type::ArcSine && kInt % 2 == 1) {
          Expression add = result;
          result = Opposite::Builder(add);
          add.shallowReduce(reductionContext);
        }
        result = result.shallowReduce(reductionContext);
        double approxResult = result.approximateToScalar<double>(
            ApproximationContext(reductionContext, true));
        double intervalBound =
            (pi / (isArccos ? 1. : 2.)) + OMG::Float::EpsilonLax<double>();
        if (approxResult > intervalBound || approxResult < -intervalBound) {
          /* The approximation of x and k was too imprecise and made the
           * translation fail. Let approximation handle this. */
          return e;
        }
        e.replaceWithInPlace(result);
        return result;
      }
    }
  }

  // Step 2. Reduce atan(1/x) in sign(x)*π/2-atan(x)
  if (e.type() == ExpressionNode::Type::ArcTangent &&
      e.childAtIndex(0).type() == ExpressionNode::Type::Power &&
      e.childAtIndex(0).childAtIndex(1).isMinusOne()) {
    // We add a dependency in 1/x
    Dependency dep =
        Dependency::Builder(Undefined::Builder(), OList::Builder());
    dep.addDependency(e.childAtIndex(0).clone());
    dep.replaceChildAtIndexInPlace(0, e.childAtIndex(0).childAtIndex(0));
    Expression x = dep.shallowReduce(reductionContext);

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
      !p.isUninitialized() && IsDirectTrigonometryFunction(p);
  /* Step 4. Handle opposite argument: acos(-x) = π-acos(x),
   * asin(-x) = -asin(x), atan(-x)= -atan(x) *
   */
  if (!letArcFunctionAtRoot) {
    Expression positiveArg =
        e.childAtIndex(0).makePositiveAnyNegativeNumeralFactor(
            reductionContext);
    if (!positiveArg.isUninitialized()) {
      /* The argument was made positive
       * acos(-x) = π-acos(x) */
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

Expression Trigonometry::ShallowReduceAdvancedFunction(
    Expression& e, ReductionContext reductionContext) {
  /* Since the child always ends in a direct function, angle units are left
   * untouched here */
  assert(IsAdvancedTrigonometryFunction(e));
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

Expression Trigonometry::ShallowReduceInverseAdvancedFunction(
    Expression& e, ReductionContext reductionContext) {
  assert(IsInverseAdvancedTrigonometryFunction(e));
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
    OMG::Troolean isNull = e.childAtIndex(0).isNull(reductionContext.context());
    // Step 1.1. Reduce ArcCotangent(0) to π/2
    if (isNull == OMG::Troolean::True) {
      result = Multiplication::Builder(
          PiExpressionInAngleUnit(reductionContext.angleUnit()),
          Rational::Builder(1, 2));
      e.replaceWithInPlace(result);
      return result.shallowReduce(reductionContext);
    }
    /* Step 1.2. Do not reduce ArcCotangent when we do not know if child is null
     * and target is not User. */
    if (isNull == OMG::Troolean::Unknown &&
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

Expression Trigonometry::ReplaceWithAdvancedFunction(Expression& e,
                                                     Expression& denominator) {
  /* Replace direct trigonometric function with their advanced counterpart.
   * This function must be called within a denominator. */
  assert(e.type() == ExpressionNode::Type::Power &&
         !denominator.isUninitialized());
  assert(IsDirectTrigonometryFunction(denominator));
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
#endif

static void AddAngleUnitToDirectFunctionIfNeeded(
    Internal::Tree* e, Preferences::AngleUnit angleUnit) {
  assert(e->isDirectTrigonometryFunction() ||
         e->isAdvancedTrigonometryFunction());

  Internal::Tree* child = e->child(0);

  if (child->isZero()) {
    return;
  }

  if (child->hasDescendantSatisfying([](const Internal::Tree* e) {
        switch (e->type()) {
          case Internal::Type::Pi:
          case Internal::Type::Add:
          case Internal::Type::Sub:
          case Internal::Type::Mult:
          case Internal::Type::Div:
          case Internal::Type::Pow:
            return false;
          default:
            return !e->isNumber();
        }
      })) {
    return;
  }

  if ((angleUnit == Preferences::AngleUnit::Radian) ==
      child->hasDescendantSatisfying(
          [](const Internal::Tree* e) { return e->isPi(); })) {
    /* Do not add angle units if the child contains Pi and the angle is in Rad
     * or if the child does not contain Pi and the angle unit is other. */
    return;
  }

  if (child->isAdd() || child->isSub()) {
    child->cloneNodeAtNode(KParentheses);
  }

  Internal::TreeRef unit = Internal::Units::Unit::Push(
      angleUnit == Preferences::AngleUnit::Radian
          ? &Internal::Units::Angle::representatives.radian
      : angleUnit == Preferences::AngleUnit::Degree
          ? &Internal::Units::Angle::representatives.degree
          : &Internal::Units::Angle::representatives.gradian,
      Internal::Units::Prefix::EmptyPrefix());

  child->moveTreeOverTree(Internal::PatternMatching::Create(
      KMult(KA, KB), {.KA = child, .KB = unit}));
  unit->removeTree();
}

void PrivateDeepAddAngleUnitToAmbiguousDirectFunctions(
    Internal::Tree* e, Preferences::AngleUnit angleUnit) {
  if (e->isDirectTrigonometryFunction() ||
      e->isDirectAdvancedTrigonometryFunction()) {
    return AddAngleUnitToDirectFunctionIfNeeded(e, angleUnit);
  }
  for (Internal::Tree* child : e->children()) {
    PrivateDeepAddAngleUnitToAmbiguousDirectFunctions(child, angleUnit);
  }
}

void Trigonometry::DeepAddAngleUnitToAmbiguousDirectFunctions(
    UserExpression& e, Preferences::AngleUnit angleUnit) {
  Internal::Tree* clone = e.tree()->cloneTree();
  PrivateDeepAddAngleUnitToAmbiguousDirectFunctions(clone, angleUnit);
  e = UserExpression::Builder(clone);
  return;
}

}  // namespace Poincare
