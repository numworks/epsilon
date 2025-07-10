#include "bounds.h"

#include <float.h>
#include <poincare/src/expression/rational.h>

#include "number.h"

namespace Poincare::Internal {

constexpr static bool HaveSameSign(double x, double y) { return x * y >= 0; }

constexpr static double TrigDerivative(double x, bool isCos) {
  return isCos ? -std::sin(x) : std::cos(x);
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
    case Type::Trig:
      return Trig(e);
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

Bounds Bounds::Trig(const Tree* e) {
  Bounds b = Bounds::Compute(e->child(0));
  if (!b.exists()) {
    return Invalid();
  }

  const Bounds trigonometricDefaultBounds = Bounds(-1.0, 1.0);

  if (b.width() >= M_PI) {
    /* The image interval of the bounds through the sin or cos function can
     * potientially be [-1, 1]. */
    return trigonometricDefaultBounds;
  }

  /* If the angle is "too big", the precision of std::cos and std::sin is
   * lost. In this case the bounds should not be propagated through the
   * sin or cos function. */
  constexpr double k_angleLimitForPrecision = 1000.0;
  if (std::max(std::abs(b.lower()), std::abs(b.upper())) >=
      k_angleLimitForPrecision) {
    return trigonometricDefaultBounds;
  }

  /* We check whether the function derivative taken at b.lower() and at
   * b.upper() has the same sign. If this is the case, knowing that the
   * distance between b.lower() and b.upper() is at most π, it means that
   * the bounds are both comprised in an interval where the function is
   * either strictly increasing or strictly decreasing. In that case, the
   * bounds can safely be spread. */
  bool isCos = e->child(1)->isZero();
  if (!HaveSameSign(TrigDerivative(b.lower(), isCos),
                    TrigDerivative(b.upper(), isCos))) {
    return trigonometricDefaultBounds;
  }
  /* We currently don't handle the case where the lower bound is just a
   * little below a multiple of π/2, and the upper bound is just a little
   * above this same multiple of π/2.
   * This case can be detected by looking at the sign of the function
   * derivative at the lower and upper bound.
   * The returned value would be Bounds(std::min(std::sin(bounds.lower()),
   * std::sin(bounds.upper())), 1) or Bounds(-1,
   * std::max(std::sin(bounds.lower()), std::sin(bounds.upper()))),
   * depending on whether the bounds are around a maximum or a minimum of
   * sin/cos. */

  if (isCos) {
    b.applyMonotoneFunction(std::cos);
  } else {
    b.applyMonotoneFunction(std::sin);
  }
  return b;
}

void Bounds::applyMonotoneFunction(double (*f)(double), uint8_t ulp_precision) {
  m_lower = f(m_lower);
  m_upper = f(m_upper);
  if (m_upper < m_lower) {
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
