#ifndef POINCARE_CONIC_H
#define POINCARE_CONIC_H

#include <poincare/expression.h>
#include <float.h>

namespace Poincare {

class Conic {
public:

  enum class Shape : uint8_t {
    Hyperbola = 0,
    Parabola,
    Ellipse,
    Circle,
    Undefined,
    Unknown,
  };

  enum class CoordinateType : uint8_t {
    Cartesian = 0,
    Polar,
    Parametric
  };

  typedef struct {
    CoordinateType coordinateType;
    Shape shape;
  } Type;

  Conic() : m_shape(Shape::Unknown) {}
  Type conicType() const { return {coordinateType(), m_shape}; }

  /* TODO: Make these methods virtual pure and mplement them for PolarConics
   * and ParametricConics when DetailsParameterController will need to display
   * details for all types of conic. */

  // Conic Eccentricity
  virtual double getEccentricity() const { assert(false); return 0.0; }
  // Conic Center
  virtual void getCenter(double * cx, double * cy) const { assert(false);}
  // Ellipse or Hyperbola's semi major axis
  virtual double getSemiMajorAxis() const { assert(false); return 0.0; }
  // Ellipse or Hyperbola's linear eccentricity
  virtual double getLinearEccentricity() const { assert(false); return 0.0; }
  // Ellipse or Hyperbola's semi minor axis
  virtual double getSemiMinorAxis() const { assert(false); return 0.0; }
  // Parabola's parameter
  virtual double getParameter() const { assert(false); return 0.0; }
  // Coordinates of Parabola's summit (relative to parameters' center)
  virtual void getSummit(double * sx, double * sy) const { assert(false); }
  // Circle's radius
  virtual double getRadius() const { assert(false); return 0.0; }

protected:
  virtual CoordinateType coordinateType() const = 0;
  Shape m_shape;
};

class CartesianConic : public Conic {
public:
  // Extract A,B,C,D,E,F parameters
  CartesianConic(const Expression e, Context * context, const char * x = "x", const char * y = "y");
  double getEccentricity() const override;
  void getCenter(double * cx, double * cy) const override;
  double getSemiMajorAxis() const override;
  double getLinearEccentricity() const override;
  double getSemiMinorAxis() const override;
  double getParameter() const override;
  void getSummit(double * sx, double * sy) const override;
  double getRadius() const override;

private:
  CoordinateType coordinateType() const override { return CoordinateType::Cartesian; }

  // Thereshold under which a parameter is considered null
  constexpr static double k_tolerance = 10.0*DBL_EPSILON;
  // Return target if |target-value| is neglectable compared to |amplitude|
  double roundIfNeglectable(double value, double target, double amplitude) const;
  // Round all coefficients to 0 if neglectable against the others
  void roundCoefficientsIfNeglectable();
  // Multiply all coefficients by a factor
  void multiplyCoefficients(double factor);
  // Update conic type from parameters
  void updateConicType();
  // Return true if coefficients are those of a conic
  bool isConic() const;
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
};

class PolarConic : public Conic {
public:
  constexpr static const char * k_tetha = "θ";
  PolarConic(const Expression& e, Context * context);
private:
  CoordinateType coordinateType() const override { return CoordinateType::Polar; }
};

class ParametricConic : public Conic {
public:
  constexpr static const char * k_t = "t";
  ParametricConic(const Expression& e, Context * context);
private:
  CoordinateType coordinateType() const override { return CoordinateType::Parametric; }
};

}  // namespace Poincare

#endif
