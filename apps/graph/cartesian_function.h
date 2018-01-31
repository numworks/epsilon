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
  Point nextMinimumFrom(double start, double step, double max, Poincare::Context * context) const;
  Point nextMaximumFrom(double start, double step, double max, Poincare::Context * context) const;
  double nextRootFrom(double start, double step, double max, Poincare::Context * context) const;
  Point nextIntersectionFrom(double start, double step, double max, Poincare::Context * context, const Shared::Function * function) const;
  char symbol() const override;
private:
  constexpr static double k_precision = 1.0E-5;
  constexpr static double k_sqrtEps = 1.4901161193847656E-8; // sqrt(DBL_EPSILON)
  constexpr static double k_goldenRatio = 0.381966011250105151795413165634361882279690820194237137864; // (3-sqrt(5))/2
  typedef double (*Evaluation)(double abscissa, Poincare::Context * context, const Shared::Function * function0, const Shared::Function * function1);
  Point nextMinimumOfFunction(double start, double step, double max, Evaluation evaluation, Poincare::Context * context, const Shared::Function * function = nullptr, bool lookForRootMinimum = false) const;
  void bracketMinimum(double start, double step, double max, double result[3], Evaluation evaluation, Poincare::Context * context, const Shared::Function * function= nullptr) const;
  Point brentMinimum(double ax, double bx, Evaluation evaluation, Poincare::Context * context, const Shared::Function * function = nullptr) const;
  double nextIntersectionWithFunction(double start, double step, double max, Evaluation evaluation, Poincare::Context * context, const Shared::Function * function) const;
  void bracketRoot(double start, double step, double max, double result[2], Evaluation evaluation, Poincare::Context * context, const Shared::Function * function) const;
  double brentRoot(double ax, double bx, double precision, Evaluation evaluation, Poincare::Context * context, const Shared::Function * function) const;
  bool m_displayDerivative;
};

}

#endif
