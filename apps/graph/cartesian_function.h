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
  Poincare::Expression::Coordinate2D nextMinimumFrom(double start, double step, double max, Poincare::Context * context) const;
  Poincare::Expression::Coordinate2D nextMaximumFrom(double start, double step, double max, Poincare::Context * context) const;
  double nextRootFrom(double start, double step, double max, Poincare::Context * context) const;
  Poincare::Expression::Coordinate2D nextIntersectionFrom(double start, double step, double max, Poincare::Context * context, const Shared::Function * function) const;
  char symbol() const override;
private:
  bool m_displayDerivative;
};

}

#endif
