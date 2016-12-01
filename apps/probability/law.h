#ifndef PROBABILITE_LAW_H
#define PROBABILITE_LAW_H

#include "../graph/evaluate_context.h"

namespace Probability {

class Law {
public:
  enum class Type {
    NoType,
    Binomial,
    Uniform,
    Exponential,
    Normal,
    Poisson
  };
  Law();
  int numberOfParameter();
  float parameterAtIndex(int index);
  void setParameterAtIndex(float f, int index);
  float evaluateAtAbscissa(float x, Graph::EvaluateContext * context) const;
  Type type() const;
  void setType(Type type);
  bool isContinuous();
private:
  Type m_type;
  float m_parameter1;
  float m_parameter2;
  Expression * m_expression;
  float m_xMin;
  float m_xMax;

};

}

#endif
