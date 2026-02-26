#pragma once

#include <float.h>
#include <poincare/analysis_expression.h>

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
    NumberOfShapes
  };

  enum class CoordinateType : uint8_t { Cartesian = 0, Polar, Parametric };

  struct Type {
    CoordinateType coordinateType;
    Shape shape;
  };

  Conic() : m_shape(Shape::Unknown) {}
  Type conicType() const { return {coordinateType(), m_shape}; }

 protected:
  virtual CoordinateType coordinateType() const = 0;
  Shape m_shape;
};

class CartesianConic : public Conic {
 public:
  // Extract A,B,C,D,E,F parameters
  CartesianConic(const AnalysisExpression& analyzedExpression,
                 const char* x = "x", const char* y = "y");

  // Conic Eccentricity
  double getEccentricity() const;
  // Conic Center
  void getCenter(double* cx, double* cy) const;
  // Ellipse or Hyperbola's semi major axis
  double getSemiMajorAxis() const;
  // Ellipse or Hyperbola's linear eccentricity
  double getLinearEccentricity() const;
  // Ellipse or Hyperbola's semi minor axis
  double getSemiMinorAxis() const;
  // Parabola's parameter
  double getParameter() const;
  // Coordinates of Parabola's summit (relative to parameters' center)
  void getSummit(double* sx, double* sy) const;
  // Circle's radius
  double getRadius() const;

 private:
  CoordinateType coordinateType() const override {
    return CoordinateType::Cartesian;
  }

  // Threshold under which a parameter is considered null
  constexpr static double k_tolerance = 10.0 * DBL_EPSILON;
  // Return target if |target-value| is negligible compared to |amplitude|
  double roundIfNegligible(double value, double target, double amplitude) const;
  // Round all coefficients to 0 if negligible against the others
  void roundCoefficientsIfNegligible();
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
  PolarConic(const AnalysisExpression& analyzedExpression,
             const char* symbol = "θ");

 private:
  CoordinateType coordinateType() const override {
    return CoordinateType::Polar;
  }
};

class ParametricConic : public Conic {
 public:
  ParametricConic(const AnalysisExpression& analyzedExpression,
                  const char* symbol = "t");

 private:
  CoordinateType coordinateType() const override {
    return CoordinateType::Parametric;
  }
};

}  // namespace Poincare
