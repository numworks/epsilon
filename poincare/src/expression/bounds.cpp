#include "bounds.h"

#include <float.h>
#include <poincare/src/expression/rational.h>

#include "number.h"

namespace Poincare::Internal {

struct Interval {
  double lower;
  double upper;
};

struct IntervalData {
  Interval interval;
  double width;
  double middle;
};

Interval LeftHalf(const IntervalData& intervalData) {
  return Interval{intervalData.interval.lower, intervalData.middle};
}

Interval RightHalf(const IntervalData& intervalData) {
  return Interval{intervalData.middle, intervalData.interval.upper};
}

double MapToInterval(double value, const IntervalData& intervalData) {
  double scaled = (value - intervalData.interval.lower) / intervalData.width;
  return (scaled - std::floor(scaled)) * intervalData.width +
         intervalData.interval.lower;
}

bool IsInside(double value, Interval interval) {
  return value >= interval.lower && value <= interval.upper;
}

Sign Bounds::Sign(const Tree* e) {
  Bounds bounds = Compute(e);
  Poincare::Sign sign = Sign::Unknown();
  if (bounds.hasKnownStrictSign()) {
    sign = Poincare::Sign(bounds.m_lower == 0, bounds.isStrictlyPositive(),
                          bounds.isStrictlyNegative(), true, false);
  }
  return sign;
}

Bounds Bounds::Compute(const Tree* e) {
  if (e->isNumber()) {
    unsigned int ulp =
        e->isOfType({Type::Zero, Type::One, Type::Two, Type::Half,
                     Type::MinusOne, Type::IntegerNegShort,
                     Type::IntegerPosShort})
            ? 0
            : 1;
    double value = Number::To<double>(e);
    return Bounds(value, value, ulp);
  }
  switch (e->type()) {
    case Type::Mult:
      return Mult(e);
    case Type::Add:
      return Add(e);
    case Type::PowReal:
    case Type::Pow:
      return Pow(e);
    case Type::Ln: {
      Bounds b = Bounds::Compute(e->child(0));
      if (b.exists()) {
        b.applyMonotoneFunction(std::log);
      }
      return b;
    }
    case Type::Exp: {
      Bounds b = Bounds::Compute(e->child(0));
      if (b.exists()) {
        b.applyMonotoneFunction(std::exp);
      }
      return b;
    }
    case Type::Trig: {
      Bounds b = Bounds::Compute(e->child(0));
      if (!b.exists()) {
        return b;
      }

      /* If the angle is "too big", the precision of std::cos and std::sin is
       * lost. In this case the bounds should not be propagated through the sin
       * or cos function. */
      constexpr double angleLimitForPrecision = 1000.0;
      if (std::max(std::abs(b.lower()), std::abs(b.upper())) >=
          angleLimitForPrecision) {
        return Invalid();
      }

      bool isCos = e->child(1)->isZero();
      IntervalData principalInterval =
          isCos
              ? IntervalData{Interval{-M_PI, M_PI}, 2 * M_PI, 0.0}
              : IntervalData{Interval{-M_PI_2, 3.0 * M_PI_2}, 2 * M_PI, M_PI_2};
      double principalAngleLower = MapToInterval(b.lower(), principalInterval);
      double angleUpper = principalAngleLower + b.upper() - b.lower();

      /* The MapToInterval function performs some operations that each create
       * a small precision loss. To account for this precision loss, we expand
       * the mapped bounds by a certain factor. */
      Bounds expandedBounds = Bounds(principalAngleLower, angleUpper, 100);

      if (IsInside(expandedBounds.lower(), LeftHalf(principalInterval)) &&
          IsInside(expandedBounds.upper(), LeftHalf(principalInterval))) {
        if (isCos) {
          // cos is strictly ascending between -π and 0
          expandedBounds.applyMonotoneFunction(std::cos);
        } else {
          // sin is strictly ascending between -π/2 and π/2
          expandedBounds.applyMonotoneFunction(std::sin);
        }
        return expandedBounds;
      }
      if (IsInside(expandedBounds.lower(), RightHalf(principalInterval)) &&
          IsInside(expandedBounds.upper(), RightHalf(principalInterval))) {
        if (isCos) {
          // cos is strictly decreasing between 0 and π
          expandedBounds.applyMonotoneFunction(std::cos, true);
        } else {
          // sin is strictly ascending between π/2 and 3π/2
          expandedBounds.applyMonotoneFunction(std::sin, true);
        }
        return expandedBounds;
      }
      return b;
    }
    default:
      return Invalid();
  }
}

Bounds Bounds::Add(const Tree* e) {
  assert(e->numberOfChildren() > 0);
  Bounds bounds = Bounds(0., 0., 0);
  for (const Tree* child : e->children()) {
    Bounds childBounds = Compute(child);
    if (!childBounds.exists()) {
      return Invalid();
    }
    bounds.m_lower += childBounds.m_lower;
    bounds.m_upper += childBounds.m_upper;
  }
  if (!bounds.exists()) {
    return Invalid();
  }
  bounds.spread(e->numberOfChildren() - 1);
  assert(bounds.lower() <= bounds.upper());
  return bounds;
}

Bounds Bounds::Mult(const Tree* e) {
  assert(e->numberOfChildren() > 0);
  Bounds bounds = Bounds(1., 1., 0);
  for (const Tree* child : e->children()) {
    Bounds childBounds = Compute(child);
    if (!childBounds.exists()) {
      return Invalid();
    }
    bounds.m_lower *= childBounds.m_lower;
    bounds.m_upper *= childBounds.m_upper;
    // Cannot spread after each operation because we ignore the final sign yet
  }
  if (!bounds.exists()) {
    return Invalid();
  }
  /* A negative multiplication can cause the bounds to be flipped */
  if (bounds.m_upper < bounds.m_lower) {
    bounds.flip();
  }
  bounds.spread(e->numberOfChildren() - 1);
  assert(bounds.lower() <= bounds.upper());
  return bounds;
}

Bounds Bounds::Pow(const Tree* e) {
  Bounds base = Bounds::Compute(e->child(0));
  Bounds exp = Bounds::Compute(e->child(1));
  if (base.exists() && exp.hasKnownStrictSign()) {
    if (base.isStrictlyPositive()) {
      /* 1. 0 < e
       *    1 < b       => b-^e- < b+^e+    2^2 < 10^3
       *    b- < 1 < b+ => b-^e- < b+^e+   .5^2 < 10^3
       *    b < 1       => b-^e+ < b+^e-   .1^3 < .5^2
       * 2. e < 0
       *    1 < b       => b+^e- < b-^e+   10^-3 <  2^-2
       *    b- < 1 < b+ => b+^e+ < b-^e-   10^-2 < .5^-3
       *    b < 1       => b+^e+ < b-^e-   .5^-2 < .1^-3
       * */
      if (exp.isStrictlyPositive() && base.m_upper <= 1) {
        exp.flip();
      } else if (exp.isStrictlyNegative()) {
        base.flip();
        if (base.m_lower <= 1) {
          exp.flip();
        }
      }
      Bounds res(std::pow(base.m_lower, exp.m_lower),
                 std::pow(base.m_upper, exp.m_upper), 0);
      assert(res.m_lower <= res.m_upper);
      /* OpenBSD pow become less precise on large values.
       * The doc states around 2ulp of error for moderate magnitude and below
       * 300ulp otherwise.
       * We set an arbitrary cut-off for moderate magnitude at 2**30, and a safe
       * margin of 5ulp instead of 2. */
      res.spread(res.m_upper < std::pow(2., 30.) ? 5 : 300);
      return res;
    }
    // TODO: handle base < 0 if we could preserve "int-ness" of exp
    // To handle cases like (-1/2)^(-3), (-pi)^2
  }
  return Invalid();
}

void Bounds::applyMonotoneFunction(double (*f)(double), bool decreasing,
                                   uint8_t ulp_precision) {
  m_lower = f(m_lower);
  m_upper = f(m_upper);
  if (decreasing) {
    flip();
  }
  spread(ulp_precision);
  assert(m_lower <= m_upper);
}

static void nthNextafter(double& value, const double& spreadDirection,
                         const unsigned int nth) {
  for (unsigned int i = 0; i < nth; ++i) {
    value = nextafter(value, spreadDirection);
  }
}

void Bounds::spread(unsigned int ulp) {
  /* OpenBSD doc: https://man.openbsd.org/exp.3#ERRORS_(due_to_Roundoff_etc.)
   * exp(x), log(x), expm1(x) and log1p(x) are accurate to within an ulp, and
   * log10(x) to within about 2 ulps; an ulp is one Unit in the Last Place. The
   * error in pow(x, y) is below about 2 ulps when its magnitude is moderate,
   * but increases as pow(x, y) approaches the over/underflow thresholds until
   * almost as many bits could be lost as are occupied by the floating-point
   * format's exponent field; that is 11 bits for IEEE 754 Double. No such
   * drastic loss has been exposed by testing; the worst errors observed have
   * been below 300 ulps for IEEE 754 Double. Moderate values of pow() are
   * accurate enough that pow(integer, integer) is exact until it is bigger than
   * 2**53 for IEEE 754. */
  assert(m_lower <= m_upper);
  nthNextafter(m_lower, -DBL_MAX, ulp);
  nthNextafter(m_upper, DBL_MAX, ulp);
}

}  // namespace Poincare::Internal
