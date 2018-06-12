#ifndef SHARED_EXPRESSION_MODEL_H
#define SHARED_EXPRESSION_MODEL_H

#include <poincare.h>
#include <kandinsky.h>
#include <escher.h>

namespace Shared {

class ExpressionModel {
public:
  ExpressionModel();
  virtual ~ExpressionModel(); // Delete expression and layout, if needed
  ExpressionModel& operator=(const ExpressionModel& other);
  ExpressionModel& operator=(ExpressionModel&& other) = delete;
  ExpressionModel(const ExpressionModel& other) = delete;
  ExpressionModel(ExpressionModel&& other) = delete;
  const char * text() const;
  Poincare::Expression * expression(Poincare::Context * context) const;
  Poincare::ExpressionLayout * layout();
  virtual bool isDefined();
  virtual bool isEmpty();
  virtual bool shouldBeClearedBeforeRemove() {
    return !isEmpty();
  }
  virtual void setContent(const char * c);
  virtual void tidy();
  constexpr static int k_expressionBufferSize = TextField::maxBufferSize();
private:
  constexpr static size_t k_dataLengthInBytes = (TextField::maxBufferSize())*sizeof(char);
  static_assert((k_dataLengthInBytes & 0x3) == 0, "The expression model data size is not a multiple of 4 bytes (cannot compute crc)"); // Assert that dataLengthInBytes is a multiple of 4
  char m_text[k_expressionBufferSize];
  mutable Poincare::Expression * m_expression;
  mutable Poincare::ExpressionLayout * m_layout;
};

}

#endif
