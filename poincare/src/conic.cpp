#include <poincare/conic.h>
#include <poincare/polynomial.h>
#include <poincare/preferences.h>

namespace Poincare {

double smallestPositive(double a, double b, bool strictly = true) {
  /* Return smallest positive number between a and b, assuming at least one is
   * positive */
  if (a > b) {
    return smallestPositive(b, a, strictly);
  }
  // We have a <= b
  assert(b > 0.0 || (!strictly && b == 0.0));
  return (a > 0.0 || (!strictly && a == 0.0)) ? a : b;
}

double nonSmallestPositive(double a, double b, bool strictly = true) {
  /* Same as smallestPositive, returning the other value. */
  return smallestPositive(a, b, strictly) == a ? b : a;
}

Conic::Conic() :
    m_a(NAN),
    m_b(NAN),
    m_c(NAN),
    m_d(NAN),
    m_e(NAN),
    m_f(NAN),
    m_cx(NAN),
    m_cy(NAN),
    m_r(NAN),
    m_type(Type::Undefined) {
}

Conic::Conic(const Expression e, Context * context, const char * x, const char * y) :
    m_a(0.0),
    m_b(0.0),
    m_c(0.0),
    m_d(0.0),
    m_e(0.0),
    m_f(0.0),
    m_cx(0.0),
    m_cy(0.0),
    m_r(0.0),
    m_type(Type::Unknown) {
  // m_a*x^2 + m_b*x*y + m_c*y^2 + m_d*x + m_e*y + m_f = 0
  // TODO Hugo : Double check these parameters
  Preferences::ComplexFormat complexFormat =
      Preferences::ComplexFormat::Cartesian;
  Preferences::AngleUnit angleUnit =
      Preferences::sharedPreferences()->angleUnit();
  Preferences::UnitFormat unitFormat = Preferences::UnitFormat::Metric;
  ExpressionNode::SymbolicComputation symbolicComputation =
      ExpressionNode::SymbolicComputation::DoNotReplaceAnySymbol;

  Expression coefficientsY[Expression::k_maxNumberOfPolynomialCoefficients];
  int dy = e.getPolynomialReducedCoefficients(y, coefficientsY, context,
                                              complexFormat, angleUnit,
                                              unitFormat, symbolicComputation);
  if (dy > 2 || dy < 1) {
    m_type = Type::Undefined;
    return;
  }
  Expression coefficientsX[Expression::k_maxNumberOfPolynomialCoefficients];
  // C term
  if (dy == 2) {
    int dx = coefficientsY[2].getPolynomialReducedCoefficients(
        x, coefficientsX, context, complexFormat, angleUnit, unitFormat,
        symbolicComputation);
    if (dx != 0) {
      m_type = Type::Undefined;
      return;
    }
    m_c = coefficientsY[2].approximateToScalar<double>(context, complexFormat,
                                                       angleUnit);
  }
  // B and E terms
  int dx = coefficientsY[1].getPolynomialReducedCoefficients(
      x, coefficientsX, context, complexFormat, angleUnit, unitFormat,
      symbolicComputation);
  if (dx > 1 || dx < 0) {
    m_type = Type::Undefined;
    return;
  }
  if (dx == 1) {
    m_b = coefficientsX[1].approximateToScalar<double>(context, complexFormat,
                                                       angleUnit);
  }
  m_e = coefficientsX[0].approximateToScalar<double>(context, complexFormat,
                                                     angleUnit);
  // A, D and F terms
  dx = coefficientsY[0].getPolynomialReducedCoefficients(
      x, coefficientsX, context, complexFormat, angleUnit, unitFormat,
      symbolicComputation);
  if (dx > 2 || dx < 1 || (dx == 1 && dy == 1)) {
    m_type = Type::Undefined;
    return;
  }
  if (dx == 2) {
    m_a = coefficientsX[2].approximateToScalar<double>(context, complexFormat,
                                                       angleUnit);
  }
  m_d = coefficientsX[1].approximateToScalar<double>(context, complexFormat,
                                                     angleUnit);
  m_f = coefficientsX[0].approximateToScalar<double>(context, complexFormat,
                                                     angleUnit);
  assert(std::isfinite(m_a) && std::isfinite(m_b) && std::isfinite(m_c) &&
         std::isfinite(m_d) && std::isfinite(m_e) && std::isfinite(m_f));
}

Conic::Type Conic::getConicType() {
  if (m_type == Type::Unknown) {
    if ((m_a == 0.0 && m_c == 0.0) ||
        (m_b == 0.0 &&
         ((m_a == 0.0 && m_d == 0.0) || (m_c == 0.0 && m_e == 0.0)))) {
      // A conic must have at least 1 squared term, 1 x term and 1 y term.
      m_type = Type::Undefined;
    } else {
      double determinant = getDeterminant();
      if (determinant < 0.0) {
        m_type = (m_b == 0.0 && m_a == m_c) ? Type::Circle : Type::Ellipse;
      } else {
        m_type = (determinant == 0.0) ? Type::Parabola : Type::Hyperbola;
      }
    }
  }
  return m_type;
}

void Conic::rotateConic() {
  if (m_b == 0.0 && (m_a > 0.0 && (m_c <= 0.0 || m_a <= m_c) &&
                     m_a == smallestPositive(m_a, m_c))) {
    // Conic is already rotated.
    return;
  }
  assert(m_r == 0.0);
  double a = m_a;
  double b = m_b;
  double c = m_c;
  double d = m_d;
  double e = m_e;
  // If b is non null and a = c, there is a pi/4 rotation.
  double r = -((a == c) ? M_PI / 4 : std::atan(b / (a - c)) / 2.0);
  double cr, sr;
  for (int i = 0; i < 4; i++) {
    cr = std::cos(r);
    sr = std::sin(r);
    // replacing x with cr*x+sr*y and y with -sr*x+cr*y to cancel B
    m_a = ApproximateForParameter(a * cr * cr - b * cr * sr + c * sr * sr);
    m_b = ApproximateForParameter(2 * a * cr * sr + b * cr * cr - b * sr * sr -
                                  2 * c * sr * cr);
    m_c = ApproximateForParameter(a * sr * sr + b * cr * sr + c * cr * cr);
    assert(m_b == 0.0);
    /* Looking at each pi/2 rotations to find the most canonic form :
     * - A is strictly positive (y is the axis of symmetry)
     * - C is either :
     *     - null (Parabola)
     *     - negative (Hyperbola),
     *     - equal (Circle)
     *     - greater than A (Elipsis, major axis along x)
     */
    if (m_a == smallestPositive(m_a, m_c, true)) {
      // TODO Hugo : Remove this assert
      assert(m_a > 0.0 && (m_c <= 0.0 || m_a <= m_c));
      break;
    }
    assert(i < 3);
    r += M_PI / 2;
  }
  m_r = r;
  // Conic shall be rotated such that a is positive, non-null and smaller than c
  // if c is positive
  assert(m_a > 0.0);
  assert(m_c <= 0.0 || m_a <= m_c);
  assert(m_a == smallestPositive(m_a, m_c));

  m_d = ApproximateForParameter(d * cr - e * sr);
  m_e = ApproximateForParameter(d * sr + e * cr);
  assert(m_b == 0.0);
}

void Conic::centerConic() {
  rotateConic();
  // There should remain only one term in x and one term in y
  // If A or C is null, F should be null
  if (m_a * m_d == 0.0 && m_c * m_e == 0.0 &&
      ((m_a * m_c == 0.0) == (m_f == 0.0))) {
    return;
  }
  assert(m_cx == 0.0 && m_cy == 0.0);
  double a = m_a;
  double c = m_c;
  double d = m_d;
  double e = m_e;
  double f = m_f;
  double h;
  double k;
  // Replacing x with x-h and y with y-k in order to cancel :
  // - F and E if A is null : Cy^2 + Dx = 0
  // - D and F if C is null : Ax^2 + Ey = 0
  // - D and E otherwise    : Ax^2 + Cy^2 = F
  assert(a != 0.0 || c != 0.0);
  if (a != 0.0) {
    h = d / (2 * a);
  }
  if (c != 0.0) {
    k = e / (2 * c);
  } else {
    k = (f - a * h * h - d * h) / e;
  }
  if (a == 0.0) {
    h = (f - c * k * k - e * k) / d;
  }
  // A and C remain unchanged
  m_d = ApproximateForParameter(d - 2 * a * h);
  m_e = ApproximateForParameter(e - 2 * c * k);
  m_f = ApproximateForParameter(f + a * h * h + c * k * k - d * h - e * k);
  // Update center (taking previous rotation into account)
  double cr = std::cos(m_r);
  double sr = std::sin(m_r);
  m_cx = -(h * cr + k * sr);
  m_cy = -(h * (-sr) + k * cr);
  assert(m_a * m_d == 0.0 && m_c * m_e == 0.0 &&
         ((m_a * m_c == 0.0) == (m_f == 0.0)));
}

void Conic::canonize() {
  // Canonize the equation by rotating and centering it
  centerConic();
  /* Equation should be in either of these canonic forms :
   * - Circle, Ellipse, Hyperbola : Ax^2 + Cy^2 = 1, A > 0 and 0 < C or C >= A
   * - Parabola                   : x^2 + Ey = 0
   */
  double factor;
  // TODO Hugo : A positive m_f may threaten the canonic form for Hyperbolas
  if (m_f != 0.0) {
    factor = -m_f;
  } else {
    assert(m_a * m_c == 0.0 && m_d * m_e == 0.0);
    factor = m_a + m_c;
  }
  m_a /= factor;
  m_b /= factor;
  m_c /= factor;
  m_d /= factor;
  m_e /= factor;
  m_f /= factor;
  assert(m_f == -1.0 || m_a + m_c == 1.0);
  // TODO Hugo : Remove these asserts
  assert(m_f == -1.0 || m_a == 1.0);
  assert(m_a == smallestPositive(m_a, m_c, true));
}

double Conic::getDeterminant() const {
  return m_b * m_b - 4.0 * m_a * m_c;
}

void Conic::getCenter(double * cx, double * cy) {
  centerConic();
  *cx = m_cx;
  *cy = m_cy;
}

double Conic::getEccentricity() {
  assert(getConicType() != Type::Undefined);
  canonize();
  Type type = getConicType();
  double e = std::sqrt(1 - smallestPositive(m_a, m_c, false) /
                               nonSmallestPositive(m_a, m_c, false));
  if (type == Type::Circle) {
    assert(e == 0.0);
  } else if (type == Type::Parabola) {
    assert(e == 1.0);
  }
  return e;
}

double Conic::getSemiMajorAxis() {
  assert(getConicType() == Type::Ellipse || getConicType() == Type::Hyperbola);
  canonize();
  return std::sqrt(1 / smallestPositive(m_a, m_c));
}

double Conic::getSemiMinorAxis() {
  assert(getConicType() == Type::Ellipse || getConicType() == Type::Hyperbola);
  canonize();
  return std::sqrt(1 / std::abs(nonSmallestPositive(m_a, m_c)));
}

double Conic::getLinearEccentricity() {
  assert(getConicType() == Type::Ellipse || getConicType() == Type::Hyperbola);
  canonize();
  return std::sqrt(std::abs(1 / m_a - 1 / m_c));
}

double Conic::getParameter() {
  assert(getConicType() == Type::Parabola);
  canonize();
  return std::abs(m_d == 0.0 ? m_e : m_d) / 2;
}

void Conic::getSummit(double * sx, double * sy) {
  assert(getConicType() == Type::Parabola);
  // Parabola's summit is also it's center
  getCenter(sx, sy);
  assert(m_f == 0.0 && m_b == 0.0 && m_a * m_c == 0.0 && m_d * m_e == 0.0);
}

double Conic::getRadius() {
  assert(getConicType() == Type::Circle);
  canonize();
  return std::sqrt(1 / m_a);
}

}  // namespace Poincare
