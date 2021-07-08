#include "conic.h"

namespace Poincare {

// Ax^2+Bxy+Cy^2+Dx+Ey+F=0

// Extract A,B,C,D,E,F parameters
Conic::Conic(Expression e) :
  m_a(0.0), m_b(0.0), m_c(0.0), m_d(0.0), m_e(0.0), m_f(0.0), m_cx(0.0), m_cy(0.0), m_r(0.0), m_type(Type::Unknown) {
  // TODO : Extract coefficients from expression
  assert(m_a != 0.0 || m_c != 0.0);
}

// Return conic type from parameters
Conic::Type Conic::getConicType() {
  if (m_type == Type::Unknown) {
    double determinant = getDeterminant();
    if (determinant < 0.0) {
      m_type = (m_b == 0.0 && m_a == m_c) ? Type::Circle : Type::Ellipse;
    } else {
      m_type = (determinant == 0.0) ? Type::Parabola : Type::Hyperbola;
    }
  }
  return m_type;
}

// Remove rotation from the parameters (B = 0)
void Conic::rotateConic() {
  if (m_b == 0.0) {
    return;
  }
  double a = m_a;
  double b = m_b;
  double c = m_c;
  // If b is non null and a = c, there is a pi/2 rotation.
  m_r = (a == c) ? M_PI/2 : std::atan(b/(a-c))/2.0;
  double cr = std::cos(m_r);
  double sr = std::sin(m_r);
  // replacing x with cr*x+sr*y and y with sr*x-cr*y to cancel B
  m_a = a*cr*cr + b*cr*sr + c*sr*sr;
  m_b = 2*a*cr*sr - b*cr*cr + b*sr*sr - 2*c*sr*cr;
  m_c = a*sr*sr - b*cr*sr + c*cr*cr;
  assert(m_b == 0.0);
}

// Remove both rotation and off-centering from the parameters
void Conic::centerConic() {
  rotateConic();
  // There should remain only one term in x and one term in y
  // If A or C is null, F should be null
  if (m_a*m_d == 0.0 && m_c*m_e == 0.0 && ((m_a*m_d == 0.0) == (m_f == 0.0))) {
    return;
  }
  double a = m_a;
  double c = m_c;
  double d = m_d;
  double e = m_e;
  double f = m_f;
  double h;
  double k;
  // Replacing x with x-h and y with y-k to cancel :
  // - F and E if A is null : Cy^2 + Dx = 0
  // - D and F if C is null : Ax^2 + Ey = 0
  // - D and E otherwise    : Ax^2 + Cy^2 = F
  assert(a != 0.0 || c != 0);
  if (a != 0.0) {
    h = d / (2*a);
  }
  if (c != 0) {
    k = e / (2*c);
  } else {
    k = (f - a*h*h - d*h)/e;
  }
  if (a == 0) {
    h = (f - c*k*k - e*k)/d;
  }
  // Update center
  m_cx = h;
  m_cy = k;
  // A and C remain unchanged
  m_d = d - 2*a*h;
  m_e = e - 2*c*k;
  m_f = f + a*h*h + c*k*k - d*h - e*k;
  assert(m_a*m_d == 0.0 && m_c*m_e == 0.0 && ((m_a*m_d == 0.0) == (m_f == 0.0)));
}

// Make it so that F parameter is -1 or 0
void Conic::canonize() {
  centerConic();
  double factor;
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
}

// Conic determinant, allow identification of the type
double Conic::getDeterminant() const {
  return m_b * m_b - 4.0 * m_a * m_c;
}

// Conic Center
void Conic::getCenter(double * cx, double * cy) {
  centerConic();
  *cx = m_cx;
  *cy = m_cy;
}

// Conic Eccentricity
double Conic::getEccentricity() {
  canonize();
  Type type = getConicType();
  double e = std::sqrt(1 - std::min(m_a,m_c) / std::max(m_a,m_c));
  if (type == Type::Circle) {
    assert(e == 0.0);
  } else if (type == Type::Parabola) {
    assert(e == 1.0);
  }
  return e;
}

// Ellipse or Hyperbola's semi major axis
double Conic::getSemiMajorAxis() {
  assert(getConicType() == Type::Ellipse || getConicType() == Type::Hyperbola);
  canonize();
  if (getConicType() == Type::Ellipse) {
    return std::sqrt(1/std::min(m_a,m_c));
  } else {
    return std::sqrt(1/std::max(m_a,m_c));
  }
}

// Ellipse or Hyperbola's semi major axis
double Conic::getSemiMinorAxis() {
  assert(getConicType() == Type::Ellipse);
  canonize();
  if (getConicType() == Type::Ellipse) {
    return std::sqrt(1/std::max(m_a,m_c));
  } else {
    return std::sqrt(1/std::min(m_a,m_c));
  }
}

// Ellipse or Hyperbola's linear eccentricity
double Conic::getLinearEccentricity() {
  assert(getConicType() == Type::Ellipse || getConicType() == Type::Hyperbola);
  canonize();
  return std::sqrt(1/std::max(m_a,m_c) - 1/std::min(m_a,m_c));
}

// Coordinates of Parabola's summit
void Conic::getSummit( double * sx, double * sy) {
  assert(getConicType() == Type::Parabola);
  getCenter(sx,sy);
  // Parabola's summit is also it's center
  assert(m_f == 0.0 && m_b == 0.0 && m_a * m_c == 0.0 && m_d * m_e == 0.0);
}

// Circle's radius
double Conic::getRadius() {
  assert(getConicType() == Type::Circle);
  return std::sqrt(std::abs(m_f/m_a));
}

} // namespace Poincare
