#ifndef SHARED_EXPRESSION_MODEL_H
#define SHARED_EXPRESSION_MODEL_H

#include <apps/constant.h>
#include <poincare/expression.h>
#include <poincare/context.h>
#include <poincare/layout.h>
#include <kandinsky.h>
#include <escher.h>

namespace Shared {

class ExpressionModel {
public:
  ExpressionModel();
  const char * text() const;
  Poincare::Expression expression(Poincare::Context * context) const;
  Poincare::Layout layout();
  /* Here, isDefined is the exact contrary of isEmpty. However, for Sequence
   * inheriting from ExpressionModel, isEmpty and isDefined have not exactly
   * opposite meaning. For instance, u(n+1)=u(n) & u(0) = ... is not empty and
   * not defined. We thus have to keep both methods. */
  virtual bool isDefined();
  virtual bool isEmpty();
  virtual bool shouldBeClearedBeforeRemove() {
    return !isEmpty();
  }
  virtual void setContent(const char * c);
  virtual void tidy();
  constexpr static int k_expressionBufferSize = Constant::MaxSerializedExpressionSize;
private:
  constexpr static size_t k_dataLengthInBytes = k_expressionBufferSize*sizeof(char);
  static_assert((k_dataLengthInBytes & 0x3) == 0, "The expression model data size is not a multiple of 4 bytes (cannot compute crc)"); // Assert that dataLengthInBytes is a multiple of 4
  char m_text[k_expressionBufferSize];
  mutable Poincare::Expression m_expression;
  mutable Poincare::Layout m_layout;
};

}

#endif
