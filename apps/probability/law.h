#ifndef PROBABILITE_LAW_H
#define PROBABILITE_LAW_H

#include "evaluate_context.h"

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
  enum CalculationType : uint8_t {
    LeftIntegral = 0,
    FiniteIntegral = 1,
    RightIntegral = 2,
  };
  Law(EvaluateContext * evaluateContext);
  ~Law();
  EvaluateContext * evaluateContext();
  void setType(Type type);
  Type type() const;
  void setCalculationType(CalculationType calculationType);
  CalculationType calculationType() const;
  Expression * expression();
  bool isContinuous();
  float xMin();
  float yMin();
  float xMax();
  float yMax();
  float scale();
  float calculationElementAtIndex(int index);
  void setCalculationElementAtIndex(float f, int index);
  int numberOfParameter();
  float parameterValueAtIndex(int index);
  const char * parameterNameAtIndex(int index);
  const char * parameterDefinitionAtIndex(int index);
  void setParameterAtIndex(float f, int index);
  float evaluateAtAbscissa(float x, EvaluateContext * context) const;
private:
  void setWindow();
  void computeScale();
  void computeCalculation(int indexKnownElement);
  void initCalculationElements();
  Type m_type;
  CalculationType m_calculationType;
  float m_parameter1;
  float m_parameter2;
  Expression * m_expression;
  float m_xMin;
  float m_xMax;
  float m_yMin;
  float m_yMax;
  float m_scale;
  float m_calculationElement1;
  float m_calculationElement2;
  float m_calculationElement3;
  EvaluateContext * m_evaluateContext;
};

}

#endif
