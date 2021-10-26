#ifndef POINCARE_CONIC_H
#define POINCARE_CONIC_H

#include <poincare/expression.h>
#include <float.h>

namespace Poincare {

class Conic {
public:
  enum class Type : uint8_t {
    Hyperbola = 0,
    Parabola,
    Ellipse,
    Circle,
    Undefined,
    Unknown,
  };
  // Extract A,B,C,D,E,F parameters
  Conic(const Expression e, Context * context, const char * x = "x", const char * y = "y");
  // Return conic type from parameters
  Type getConicType() const { return m_type; }
  // Conic Eccentricity
  double getEccentricity() const;
  // Conic Center
  void getCenter(double * cx, double * cy) const;
  // Ellipse or Hyperbola's semi major axis
  double getSemiMajorAxis() const;
  // Ellipse or Hyperbola's linear eccentricity
  double getLinearEccentricity() const;
  // Ellipse or Hyperbola's semi minor axis
  double getSemiMinorAxis() const;
  // Parabola's parameter
  double getParameter() const;
  // Coordinates of Parabola's summit (relative to parameters' center)
  void getSummit(double * sx, double * sy) const;
  // Circle's radius
  double getRadius() const;

private:
  // Thereshold under which a parameter is considered null
  static constexpr double k_tolerance = 10.0*DBL_EPSILON;
  // Return target if |target-value| is neglectable compared to |amplitude|
  double roundIfNeglectable(double value, double target, double amplitude) const;
  // Round all coefficients to 0 if neglectable against the others
  void roundCoefficientsIfNeglectable();
  // Multiply all coefficients by a factor
  void multiplyCoefficients(double factor);
  // Update conic type from parameters
  void updateConicType();
  // Return true if conic's coefficients are canonically rotated
  bool isCanonicallyRotated() const;
  // Remove rotation from the parameters (B = 0)
  void rotateConic();
  // Return true if conic's coefficients are canonically centered
  bool isCanonicallyCentered() const;
  // Remove both rotation and off-centering from the parameters
  void centerConic();
  // Return true if conic's coefficients are canonical
  bool isCanonical() const;
  // Make it so that F parameter is -1 or 0
  void canonize();
  // Conic determinant, allow identification of the type
  double getDeterminant() const;
  // Ax^2+Bxy+Cy^2+Dx+Ey+F=0
  double m_a;
  double m_b;
  double m_c;
  double m_d;
  double m_e;
  double m_f;
  double m_cx;
  double m_cy;
  double m_r;
  Type m_type;
};

}  // namespace Poincare

#endif
