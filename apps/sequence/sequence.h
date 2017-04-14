#ifndef SEQUENCE_SEQUENCE_H
#define SEQUENCE_SEQUENCE_H

#include "../shared/function.h"

namespace Sequence {

class Sequence : public Shared::Function {
public:
  enum class Type {
    Explicite = 0,
    SingleRecurrence = 1,
    DoubleRecurrence = 2
  };
  Sequence(const char * text = nullptr, KDColor color = KDColorBlack);
  ~Sequence();
  Type type();
  void setType(Type type);
  const char * firstInitialConditionText();
  const char * secondInitialConditionText();
  Poincare::Expression * firstInitialConditionExpression();
  Poincare::Expression * secondInitialConditionExpression();
  Poincare::ExpressionLayout * firstInitialConditionLayout();
  Poincare::ExpressionLayout * secondInitialConditionLayout();
  void setContent(const char * c) override;
  void setFirstInitialConditionContent(const char * c);
  void setSecondInitialConditionContent(const char * c);
  int numberOfElements();
  Poincare::ExpressionLayout * nameLayout();
  Poincare::ExpressionLayout * definitionName();
  Poincare::ExpressionLayout * firstInitialConditionName();
  Poincare::ExpressionLayout * secondInitialConditionName();
  bool isDefined() override;
  float evaluateAtAbscissa(float x, Poincare::Context * context) const override;
  float sumOfTermsBetweenAbscissa(float start, float end, Poincare::Context * context);
private:
  constexpr static int k_maxRecurrentRank = 10000;
  char symbol() const override;
  Type m_type;
  char m_firstInitialConditionText[Shared::Function::k_bodyLength];
  char m_secondInitialConditionText[Shared::Function::k_bodyLength];
  Poincare::Expression * m_firstInitialConditionExpression;
  Poincare::Expression * m_secondInitialConditionExpression;
  Poincare::ExpressionLayout * m_firstInitialConditionLayout;
  Poincare::ExpressionLayout * m_secondInitialConditionLayout;
  Poincare::ExpressionLayout * m_nameLayout;
  Poincare::ExpressionLayout * m_definitionName;
  Poincare::ExpressionLayout * m_firstInitialConditionName;
  Poincare::ExpressionLayout * m_secondInitialConditionName;
  mutable int m_indexBuffer1;
  mutable int m_indexBuffer2;
  mutable float m_buffer1;
  mutable float m_buffer2;
};

}

#endif
