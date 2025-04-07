#include "bounds.h"

#include <float.h>
#include <poincare/src/expression/rational.h>

#include "poincare/src/expression/float_helper.h"

namespace Poincare::Internal {

Sign Bounds::Sign(const Tree* e) {
  Bounds bounds = Compute(e);
  Poincare::Sign sign = Sign::Unknown();
  if (bounds.hasKnownStrictSign()) {
    sign = Poincare::Sign(bounds.m_lower == 0, bounds.isStrictlyPositive(),
                          bounds.isStrictlyNegative(), true, false);
  }
  return sign;
}

Bounds Bounds::ComputeNumber(const Tree* e) {
  unsigned int ulp = 1;
  double value = NAN;
  if (e->isRational()) {
    value = Rational::To<double>(e);
    if (e->isOfType(
            {Type::Zero, Type::One, Type::Two, Type::Half, Type::MinusOne})) {
      ulp = 0;
    }
  } else if (e->isMathematicalConstant()) {
    value = e->isPi() ? M_PI : M_E;
  } else if (e->isFloat()) {
    value = FloatHelper::To(e);
  }
  assert(!std::isnan(value));
  return Bounds(value, value, ulp);
}

Bounds Bounds::Compute(const Tree* e) {
  if (e->isNumber()) {
    return Bounds::ComputeNumber(e);
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
  return bounds;
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

void Bounds::applyMonotoneFunction(double (*f)(double), bool decreasing,
                                   uint8_t ulp_precision) {
  m_lower = f(m_lower);
  m_upper = f(m_upper);
  if (decreasing) {
    flip();
  }
  spread(ulp_precision);
}

Bounds Bounds::Pow(const Tree* e) {
  Bounds base = Bounds::Compute(e->child(0));
  Bounds exp = Bounds::Compute(e->child(1));
  if (base.hasKnownStrictSign() && exp.hasKnownStrictSign()) {
    if (base.isStrictlyPositive()) {
      Bounds res = Bounds(std::pow(base.m_lower, exp.m_lower),
                          std::pow(base.m_upper, exp.m_upper), 0);
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

void Bounds::flip() {
  double tmp = m_lower;
  m_lower = m_upper;
  m_upper = tmp;
}

}  // namespace Poincare::Internal
