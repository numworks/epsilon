#ifndef GRAPH_CARTESIAN_FUNCTION_H
#define GRAPH_CARTESIAN_FUNCTION_H

#include "../shared/function.h"

namespace Graph {

class CartesianFunction : public Shared::Function {
public:
  using Shared::Function::Function;
  CartesianFunction(const char * text = nullptr, KDColor color = KDColorBlack);
  bool displayDerivative();
  void setDisplayDerivative(bool display);
  double approximateDerivative(double x, Poincare::Context * context) const;
  double sumBetweenBounds(double start, double end, Poincare::Context * context) const override;
  struct Point {
    double abscissa;
    double value;
  };
  Point mininimumBetweenBounds(double start, double end, Poincare::Context * context) const;
  char symbol() const override;
private:
  constexpr static double k_sqrtEps = 1.4901161193847656E-8; // sqrt(DBL_EPSILON)
  constexpr static double k_goldenRatio = 0.381966011250105151795413165634361882279690820194237137864; // (3-sqrt(5))/2
  double bracketFunction(double x, double z, Poincare::Context * context);
  Point brentAlgorithm(double ax, double bx, Poincare::Context * context) const;
  bool m_displayDerivative;
};

}

#endif
