#ifndef GRAPH_FUNCTION_H
#define GRAPH_FUNCTION_H

#include "../shared/function.h"

namespace Graph {

class Function : public Shared::Function {
public:
  static constexpr const char * Parameter = "(x)";
  using Shared::Function::Function;
  Function(const char * text = nullptr, KDColor color = KDColorBlack);
  bool displayDerivative();
  void setDisplayDerivative(bool display);
  float approximateDerivative(float x, Poincare::Context * context) const;
  char symbol() const override;
private:
  bool m_displayDerivative;
};

}

#endif
