#ifndef GRAPH_CARTESIAN_FUNCTION_H
#define GRAPH_CARTESIAN_FUNCTION_H

#include "../shared/function.h"

namespace Graph {

class CartesianFunction : public Shared::Function {
public:
  static constexpr const char * Parameter = "(x)";
  using Shared::Function::Function;
  CartesianFunction(const char * text = nullptr, KDColor color = KDColorBlack);
  bool displayDerivative();
  void setDisplayDerivative(bool display);
  float approximateDerivative(float x, Poincare::Context * context) const;
  char symbol() const override;
private:
  bool m_displayDerivative;
};

}

#endif
