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
  Sequence& operator=(const Sequence& other);
  Sequence& operator=(Sequence&& other) = delete;
  Sequence(const Sequence& other) = delete;
  Sequence(Sequence&& other) = delete;
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
  bool isEmpty() override;
  float evaluateAtAbscissa(float x, Poincare::Context * context) const override;
  float sumOfTermsBetweenAbscissa(float start, float end, Poincare::Context * context);
private:
  constexpr static int k_maxRecurrentRank = 10000;
  constexpr static float k_maxNumberOfTermsInSum = 100000.0f;
  char symbol() const override;
  Type m_type;
  char m_firstInitialConditionText[TextField::maxBufferSize()];
  char m_secondInitialConditionText[TextField::maxBufferSize()];
  Poincare::Expression * m_firstInitialConditionExpression;
  Poincare::Expression * m_secondInitialConditionExpression;
  Poincare::ExpressionLayout * m_firstInitialConditionLayout;
  Poincare::ExpressionLayout * m_secondInitialConditionLayout;
  Poincare::ExpressionLayout * m_nameLayout;
  Poincare::ExpressionLayout * m_definitionName;
  Poincare::ExpressionLayout * m_firstInitialConditionName;
  Poincare::ExpressionLayout * m_secondInitialConditionName;
  /* In order to accelerate the computation of values of recurrent sequences,
   * we memoize the last computed values of the sequence and their associated
   * ranks (n and n+1 for instance). Thereby, when another evaluation at a
   * superior rank k > n+1 is called, we avoid iterating from 0 but can start
   * from n. */
  constexpr static int k_maxRecurrenceDepth = 2;
  mutable int m_indexBuffer[k_maxRecurrenceDepth];
  mutable float m_buffer[k_maxRecurrenceDepth];
};

}

#endif
