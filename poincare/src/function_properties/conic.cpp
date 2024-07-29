#include <poincare/function_properties/conic.h>
#include <poincare/function_properties/function_type.h>
#include <poincare/src/expression/degree.h>
#include <poincare/src/expression/division.h>
#include <poincare/src/expression/polynomial.h>
#include <poincare/src/expression/trigonometry.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/memory/tree.h>

#include <algorithm>

/* Double comparison is extensively used in Conics's methods for performances.
 * To limit the approximation errors that may rise from these comparisons, we
 * round the values of coefficients to 0 (or 1) before and after manipulating
 * them. */

using namespace Poincare::Internal;

namespace Poincare {

// Return smallest positive number between x and y, assuming at least one is > 0
double smallestPositive(double x, double y) {
  if (x > y) {
    return smallestPositive(y, x);
  }
  // We have x < y. Assert y is strictly positive.
  assert(y > 0.0);
  return (x > 0.0) ? x : y;
}

CartesianConic::CartesianConic(const SystemExpression& analyzedExpression,
                               const char* x, const char* y)
    : Conic(),
      m_a(0.0),
      m_b(0.0),
      m_c(0.0),
      m_d(0.0),
      m_e(0.0),
      m_f(0.0),
      m_cx(0.0),
      m_cy(0.0),
      m_r(0.0) {
  /* Expression analyzedExpression represents an equation of the form:
   * a·x^2 + b·x·y + c·y^2 + d·x + e·y + f = 0
   * In this constructor, we extract the coefficients parameters.
   * We then compute the conic's type and canonize the coefficients. */

  const Tree* e = analyzedExpression.tree();

  int dy = Degree::Get(e, y);
  if (dy < 1 || dy > 2) {
    m_shape = Shape::Undefined;
    return;
  }
  Tree* coefListY = PolynomialParser::GetReducedCoefficients(e, y);
  assert(coefListY && dy == coefListY->numberOfChildren() - 1);

  // Extract c
  if (dy == 2) {
    Tree* c = coefListY->child(2);
    // Ensure coefficient c does not depend on x
    if (Degree::Get(c, x) != 0) {
      m_shape = Shape::Undefined;
      coefListY->removeTree();
      return;
    }
    m_c = Approximation::To<double>(c);
  }

  // Extract b and e
  Tree* be = coefListY->child(1);
  int dx = Degree::Get(be, x);
  if (dx < 0 || dx > 1) {
    m_shape = Shape::Undefined;
    coefListY->removeTree();
    return;
  }
  Tree* coefListX = PolynomialParser::GetReducedCoefficients(be, x);
  assert(coefListX && dx == coefListX->numberOfChildren() - 1);
  if (dx == 1) {
    m_b = Approximation::To<double>(coefListX->child(1));
  }
  m_e = Approximation::To<double>(coefListX->child(0));
  coefListX->removeTree();

  // Extract a, d and f
  Tree* adf = coefListY->child(0);
  dx = Degree::Get(adf, x);
  if (dx > 2 || dx < 0 || (dx < 2 && dy < 2 && m_b == 0.0) ||
      (dx == 0 && m_b == 0.0)) {
    // A conic must have at least 1 squared term, 1 x term and 1 y term.
    m_shape = Shape::Undefined;
    coefListY->removeTree();
    return;
  }
  coefListX = PolynomialParser::GetReducedCoefficients(adf, x);
  assert(coefListX && dx == coefListX->numberOfChildren() - 1);
  if (dx == 2) {
    m_a = Approximation::To<double>(coefListX->child(2));
  }
  if (dx >= 1) {
    m_d = Approximation::To<double>(coefListX->child(1));
  }
  m_f = Approximation::To<double>(coefListX->child(0));
  coefListX->removeTree();
  coefListY->removeTree();

  // Round the coefficients to 0 if they are negligible against the other ones
  roundCoefficientsIfNegligible();
  if (!isConic()) {
    m_shape = Shape::Undefined;
    return;
  }
  // Setting type from a canonic conic is safer.
  canonize();
  updateConicType();
}

void CartesianConic::updateConicType() {
  if (m_shape == Shape::Unknown) {
    if (!isConic()) {
      // A conic must have at least 1 squared term, 1 x term and 1 y term.
      m_shape = Shape::Undefined;
    } else {
      double determinant = getDeterminant();
      if (determinant < 0.0) {
        m_shape = (m_b == 0.0 && m_a == m_c) ? Shape::Circle : Shape::Ellipse;
      } else {
        m_shape = (determinant == 0.0) ? Shape::Parabola : Shape::Hyperbola;
      }
    }
  }
}

double CartesianConic::roundIfNegligible(double value, double target,
                                         double amplitude) const {
  return (std::abs(value - target) < k_tolerance * std::abs(amplitude)) ? target
                                                                        : value;
}

void CartesianConic::roundCoefficientsIfNegligible() {
  double amplitude = std::max({std::fabs(m_a), std::fabs(m_b), std::fabs(m_c),
                               std::fabs(m_d), std::fabs(m_e), std::fabs(m_f)});
  m_a = roundIfNegligible(m_a, 0.0, amplitude);
  m_b = roundIfNegligible(m_b, 0.0, amplitude);
  m_c = roundIfNegligible(m_c, 0.0, amplitude);
  m_d = roundIfNegligible(m_d, 0.0, amplitude);
  m_e = roundIfNegligible(m_e, 0.0, amplitude);
  m_f = roundIfNegligible(m_f, 0.0, amplitude);
}

void CartesianConic::multiplyCoefficients(double factor) {
  m_a *= factor;
  m_b *= factor;
  m_c *= factor;
  m_d *= factor;
  m_e *= factor;
  m_f *= factor;
}

bool CartesianConic::isConic() const {
  /* Constraints are :
   * - Coefficients are all finite
   * - There is at least one "squared" (A, B or C) coefficient
   * - There is at least one x (B, A or D) and one y (B, C or E) coefficient */
  return std::isfinite(m_a) && std::isfinite(m_b) && std::isfinite(m_c) &&
         std::isfinite(m_d) && std::isfinite(m_e) && std::isfinite(m_f) &&
         (m_a != 0.0 || m_b != 0.0 || m_c != 0.0) &&
         (m_b != 0.0 ||
          ((m_a != 0.0 || m_d != 0.0) && (m_c != 0.0 || m_e != 0.0)));
}

bool CartesianConic::isCanonicallyRotated() const {
  /* For the conic to be rotated, coefficients must be of one of these forms :
   * A*x^2 + C*y^2 + D*x + E*y + F = 0 with A > 0 and either C <= 0 or C >= A */
  assert((m_a > 0.0 || m_c > 0.0));
  return m_b == 0.0 && m_a == smallestPositive(m_a, m_c);
}

void CartesianConic::rotateConic() {
  /* Rotate the conic by first aligning it along the x and y axes. Then, rotate
   * it up to three times by π/2 to find a unique canonic orientation. */
  if (m_a <= 0.0 && m_c <= 0.0) {
    // This ensures that at least on of the two coefficients is always positive
    multiplyCoefficients(-1.0);
  }
  if ((m_a > 0.0 || m_c > 0.0) && isCanonicallyRotated()) {
    // Conic is already rotated in canonic form.
    return;
  }
  double a = m_a;
  double b = m_b;
  double c = m_c;
  double d = m_d;
  double e = m_e;
  double amplitude = std::max({std::fabs(a), std::fabs(b), std::fabs(c)});
  // If B is non null and A = C, the first rotation must be of π/4
  double r =
      (b == 0.0 ? 0.0 : -((a == c) ? M_PI / 4 : std::atan(b / (a - c)) / 2.0));
  double cr, sr;
  for (int i = 0; i < 4; i++) {
    cr = std::cos(r);
    sr = std::sin(r);
    // Replacing x with cr*x+sr*y and y with -sr*x+cr*y to cancel B coefficient
    m_a = roundIfNegligible(a * cr * cr - b * cr * sr + c * sr * sr, 0.0,
                            amplitude);
    m_b = roundIfNegligible(
        2 * a * cr * sr + b * cr * cr - b * sr * sr - 2 * c * sr * cr, 0.0,
        amplitude);
    m_c = roundIfNegligible(a * sr * sr + b * cr * sr + c * cr * cr, 0.0,
                            amplitude);
    assert(m_b == 0.0);
    /* Looking at each π/2 rotations to find the most canonic form :
     * - A is strictly positive (y is the axis of symmetry)
     * - C is either :
     *     - null (Parabola)
     *     - negative (Hyperbola),
     *     - equal (Circle)
     *     - greater than A (Elipsis, major axis along x)
     */
    if ((m_a > 0.0 || m_c > 0.0) && isCanonicallyRotated()) {
      break;
    }
    assert(i < 3);
    r += M_PI / 2;
  }
  /* If rotated for the second time, the conic has been centered, and the
   * rotation might not be simple to compute anymore. Set it to NAN as we won't
   * use it anyway. */
  m_r = (m_r == 0.0) ? r : NAN;
  // Apply the rotation to D and E (F remains unchanged)
  amplitude = std::max(std::fabs(d), std::fabs(e));
  m_d = roundIfNegligible(d * cr - e * sr, 0.0, amplitude);
  m_e = roundIfNegligible(d * sr + e * cr, 0.0, amplitude);
  // Assert the conic is rotated in a canonic for.
  assert(isCanonicallyRotated());
}

bool CartesianConic::isCanonicallyCentered() const {
  /* For the conic to be centered, coefficients must be of one of these forms :
   * Ax^2 + Ey = 0 with A > 0 and E != 0
   * Ax^2 + Cy^2 + F = 0 with A > 0 and either C <= 0 or C >= A
   * Calling this method on already rotated conics ensures B is null, A > 0,
   * and either C <= 0 or C >= A. */
  assert(isCanonicallyRotated());
  return m_d == 0.0 && (m_e == 0.0 || (m_c == 0.0 && m_e != 0.0 && m_f == 0.0));
}

void CartesianConic::centerConic() {
  /* Rotate and center the conic so that here remains only one term in x and one
   * term in y. If A or C is null, F should be null as well. */
  rotateConic();
  if (isCanonicallyCentered()) {
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
  /* Replacing x with x-h and y with y-k in order to cancel :
   *  - D and F if C is null : Ax^2 + Ey = 0
   *  - D and E otherwise    : Ax^2 + Cy^2 = F */
  assert(a != 0.0);
  h = d / (2 * a);
  if (c != 0.0) {
    k = e / (2 * c);
  } else if (e != 0.0) {
    k = (f + a * h * h - d * h) / e;
  } else {
    k = 0.0;
  }
  // A and C remain unchanged
  m_d = roundIfNegligible(d - 2 * a * h, 0.0,
                          std::max(std::fabs(d), std::fabs(2 * a * h)));
  m_e = roundIfNegligible(e - 2 * c * k, 0.0,
                          std::max(std::fabs(e), std::fabs(2 * c * k)));
  double fAmplitude =
      std::max({std::fabs(f), std::fabs(a * h * h), std::fabs(c * k * k),
                std::fabs(d * h), std::fabs(e * k)});
  m_f = roundIfNegligible(f + a * h * h + c * k * k - d * h - e * k, 0.0,
                          fAmplitude);
  // Update center (taking previous rotation into account)
  assert(!std::isnan(m_r));
  double cr = std::cos(m_r);
  double sr = std::sin(m_r);
  m_cx = -(h * cr + k * sr);
  m_cy = -(h * (-sr) + k * cr);
  assert(isCanonicallyCentered());
}

bool CartesianConic::isCanonical() const {
  /* For the conic to be canonic, coefficients must be of one of these forms :
   * x^2 + Ey = 0 with E != 0
   * Ax^2 + Cy^2 - 1 = 0 with A > 0 and either C < 0 or C >= A
   * Calling this method on canonically centered conics to simplify check */
  assert(isCanonicallyCentered());
  return (m_f == -1.0 && m_c != 0.0) ||
         (m_f == 0.0 && m_a == 1.0 && m_e != 0.0);
}

void CartesianConic::canonize() {
  /* Rotate, center the conic and ensure that coefficients are unique :
   * - Circle, Ellipse, Hyperbola : Ax^2 + Cy^2 = 1, A > 0 and (C < 0 or C >= A)
   * - Parabola                   : x^2 + Ey = 0 with E != 0
   */
  centerConic();
  if (isCanonical()) {
    return;
  }
  if (m_e == 0.0 && (m_f == 0.0 || m_c == 0.0)) {
    /* The equation is of one of the forms :
     * Ax^2 + Cy^2 = 0 with A > 0 and C <= 0 or C >= A
     * Ax^2 = F with A > 0 and F >= 0
     * This can be a set of two lines or points, and isn't considered as a conic
     * here. */
    m_shape = Shape::Undefined;
    return;
  }
  // Canonize either F or A
  if (m_f != 0.0) {
    multiplyCoefficients(-1 / m_f);
    m_f = roundIfNegligible(m_f, -1.0, std::abs(m_f));
    assert(m_f == -1.0);
  } else {
    multiplyCoefficients(1 / m_a);
    m_a = roundIfNegligible(m_a, 1.0, std::abs(m_a));
    assert(m_a == 1.0);
  }
  /* The sign of F may sometimes change after centering. In that case, the
   * conic needs to be rotated again to ensure the canonic form. */
  if ((m_a <= 0.0 && m_c <= 0.0)) {
    // A, B and F are all negative the equation has no solution.
    m_shape = Shape::Undefined;
    return;
  }

  if (!isCanonicallyRotated()) {
    /* We must rotate the conic once more. It isn't an issue for the center
     * metrics as it is already centered well. However, m_r is expected to only
     * rotate by  */
    rotateConic();
  }
  assert(isCanonical());
}

double CartesianConic::getDeterminant() const {
  // Only the sign matters, there is no need for a canonic equation here.
  return m_b * m_b - 4.0 * m_a * m_c;
}

void CartesianConic::getCenter(double* cx, double* cy) const {
  // Conic must be centered
  assert(isCanonicallyCentered());
  *cx = m_cx;
  *cy = m_cy;
}

double CartesianConic::getEccentricity() const {
  // Conic must be canonically rotated
  assert(isCanonicallyRotated());
  assert(m_shape != Shape::Undefined);
  if (m_shape == Shape::Parabola) {
    assert(m_a == 0.0 || m_c == 0.0);
    return 1.0;
  }
  // A being the smallest positive number we can assert that
  assert(1 - m_a / m_c >= 0.0);
  double e = std::sqrt(1 - m_a / m_c);
  assert(m_shape != Shape::Circle || e == 0.0);
  return e;
}

double CartesianConic::getSemiMajorAxis() const {
  // Conic must be rotated, F must be -1.0
  assert(isCanonical());
  assert(m_shape == Shape::Ellipse || m_shape == Shape::Hyperbola);
  return std::sqrt(1 / m_a);
}

double CartesianConic::getSemiMinorAxis() const {
  // Conic must be rotated, F must be -1.0
  assert(isCanonical());
  assert(m_shape == Shape::Ellipse || m_shape == Shape::Hyperbola);
  return std::sqrt(1 / std::abs(m_c));
}

double CartesianConic::getLinearEccentricity() const {
  // Conic must be rotated, F must be -1.0
  assert(isCanonical());
  assert(m_shape == Shape::Ellipse || m_shape == Shape::Hyperbola);
  return std::sqrt(std::abs(1 / m_a - 1 / m_c));
}

double CartesianConic::getParameter() const {
  // Conic must be canonically rotated, and A must be 1.0
  assert(isCanonical());
  assert(m_shape == Shape::Parabola);
  double defaultParameter = std::abs(m_e) / 2;
  return Preferences::SharedPreferences()->parabolaParameter() ==
                 Preferences::ParabolaParameter::FocalLength
             ? defaultParameter / 2
             : defaultParameter;
}

void CartesianConic::getSummit(double* sx, double* sy) const {
  // Conic must be centered
  assert(isCanonicallyCentered());
  assert(m_shape == Shape::Parabola);
  // Parabola's summit is also it's center
  getCenter(sx, sy);
}

double CartesianConic::getRadius() const {
  // F must be -1.0
  assert(isCanonical());
  assert(m_shape == Shape::Circle);
  return std::sqrt(1 / m_a);
}

PolarConic::PolarConic(const SystemExpression& analyzedExpression,
                       const char* symbol) {
  const Tree* e = analyzedExpression.tree();

  // Detect the pattern r = a
  int deg = Degree::Get(e, symbol);
  if (deg == 0) {
    m_shape = Shape::Circle;
    return;
  }
  if (deg > 0) {
    m_shape = Shape::Undefined;
    return;
  }

  /* Detect the pattern r = a·cosOrSin(θ+c)
   * TODO: Detect r=cos(θ)+2sin(θ) */
  double a, b, c;
  if (FunctionPropertiesHelper::DetectLinearPatternOfTrig(e, symbol, &a, &b, &c,
                                                          false) &&
      b == 1.0) {
    m_shape = Shape::Circle;
    return;
  }

  // Detect the pattern (d·e)/(1±e·cos(θ+c)) where e is the eccentricity
  TreeRef numerator, denominator;
  Division::GetNumeratorAndDenominator(e, numerator, denominator);
  assert(numerator && denominator);
  bool ok = Degree::Get(numerator, symbol) == 0 && denominator->isAdd() &&
            FunctionPropertiesHelper::DetectLinearPatternOfTrig(
                denominator, symbol, &a, &b, &c, true) &&
            b == 1.0;
  numerator->removeTree();
  if (!ok) {
    m_shape = Shape::Undefined;
    denominator->removeTree();
    return;
  }

  // Remove the cos term of the denominator
  int nChildren = denominator->numberOfChildren();
  Tree* child = denominator->child(0);
  int nRemoved = 0;
  for (int i = 0; i < nChildren; i++) {
    if (Degree::Get(child, symbol) != 0) {
      child->removeTree();
      nRemoved++;
    } else {
      child = child->nextTree();
    }
  }
  assert(0 < nRemoved && nRemoved < nChildren);
  NAry::SetNumberOfChildren(denominator, nChildren - nRemoved);
  double k = Approximation::To<double>(denominator);
  denominator->removeTree();

  // Turn a·cos(θ+c)+k into (a/k)·cos(θ+c)+1
  double absValueCoefficient = std::fabs(a / k);
  if (absValueCoefficient < 1.0) {
    m_shape = Shape::Ellipse;
  } else if (absValueCoefficient > 1.0) {
    m_shape = Shape::Hyperbola;
  } else if (absValueCoefficient == 1.0) {
    m_shape = Shape::Parabola;
  } else {
    assert(std::isnan(absValueCoefficient));
    m_shape = Shape::Undefined;
  }
}

ParametricConic::ParametricConic(const SystemExpression& analyzedExpression,
                                 const char* symbol) {
  assert(analyzedExpression.type() == ExpressionNode::Type::Point);

  const Tree* xOfT = analyzedExpression.tree()->child(0);
  const Tree* yOfT = xOfT->nextTree();
  int degOfTinX = Degree::Get(xOfT, symbol);
  int degOfTinY = Degree::Get(yOfT, symbol);

  if ((degOfTinX == 1 && degOfTinY == 2) ||
      (degOfTinX == 2 && degOfTinY == 1)) {
    m_shape = Shape::Parabola;
    return;
  }

  // Detect parabola (x, y) = (a·f(t)+c, b·f(t)^2+d)
  Tree* variableX = xOfT->cloneTree();
  FunctionPropertiesHelper::RemoveConstantTermsInAddition(variableX, symbol);
  Tree* variableY = yOfT->cloneTree();
  FunctionPropertiesHelper::RemoveConstantTermsInAddition(variableY, symbol);

  Tree* quotient = PatternMatching::CreateSimplify(
      KMult(KPow(KA, 2_e), KPow(KB, -1_e)), {.KA = variableX, .KB = variableY});
  bool parabola = Degree::Get(quotient, symbol) == 0;
  if (!parabola) {
    quotient->moveTreeOverTree(
        PatternMatching::CreateSimplify(KMult(KPow(KA, 2_e), KPow(KB, -1_e)),
                                        {.KA = variableY, .KB = variableX}));
    parabola = Degree::Get(quotient, symbol) == 0;
  }
  quotient->removeTree();
  variableY->removeTree();
  variableX->removeTree();
  if (parabola) {
    m_shape = Shape::Parabola;
    return;
  }

  if (degOfTinX >= 0 || degOfTinY >= 0) {
    m_shape = Shape::Undefined;
    return;
  }

  /* Detect other conics:
   * Circle: (x, y) = (a·cos(b·t+c)+k , a·sin(b·t+c)+l)
   * Ellipse: (x, y) = (a·cos(b·t+c)+k , d·cos(b·t+e)+l) with c != e
   * TODO: Hyperbola: (x, y) = (a·sec(b·t+c)+k , g·tan(c·t+e)+l)
   */

  // Detect if x(t) = a·cos(b·t+c)+k, same for y(t)
  double aX, bX, cX, aY, bY, cY;
  if (!FunctionPropertiesHelper::DetectLinearPatternOfTrig(xOfT, symbol, &aX,
                                                           &bX, &cX, true) ||
      !FunctionPropertiesHelper::DetectLinearPatternOfTrig(yOfT, symbol, &aY,
                                                           &bY, &cY, true)) {
    m_shape = Shape::Undefined;
    return;
  }
  if (bY == bX) {
    if (std::fabs(aY) == std::fabs(aX) && std::fabs(cX - cY) == M_PI_2) {
      m_shape = Shape::Circle;
      return;
    }
    if (cX != cY) {
      m_shape = Shape::Ellipse;
      return;
    }
  }
  m_shape = Shape::Undefined;
}

}  // namespace Poincare
