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
  void setFirstInitialConditionContent(const char * c);
  void setSecondInitialConditionContent(const char * c);
  char symbol() const override;
  int numberOfElements();
  Poincare::ExpressionLayout * definitionName();
  Poincare::ExpressionLayout * firstInitialConditionName();
  Poincare::ExpressionLayout * secondInitialConditionName();
private:
  Type m_type;
  char m_firstInitialConditionText[Shared::Function::k_bodyLength];
  char m_secondInitialConditionText[Shared::Function::k_bodyLength];
  Poincare::Expression * m_firstInitialConditionExpression;
  Poincare::Expression * m_secondInitialConditionExpression;
  Poincare::ExpressionLayout * m_firstInitialConditionLayout;
  Poincare::ExpressionLayout * m_secondInitialConditionLayout;
  Poincare::ExpressionLayout * m_definitionName;
  Poincare::ExpressionLayout * m_firstInitialConditionName;
  Poincare::ExpressionLayout * m_secondInitialConditionName;
};

}

#endif
