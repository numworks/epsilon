#ifndef SHARED_EXPRESSION_MODEL_HANDLE_H
#define SHARED_EXPRESSION_MODEL_HANDLE_H

#include <poincare/context.h>
#include <poincare/expression.h>
#include <poincare/layout.h>
#include <ion/storage.h>

namespace Shared {

// ExpressionModelHandle is a handle to Ion::Record

class ExpressionModelHandle : public Ion::Storage::Record {
  // TODO find better name (once we remove ExpressionModel?)
public:
  ExpressionModelHandle(Ion::Storage::Record record);
  virtual char symbol() const { return 0; };
  void text(char * buffer, size_t bufferSize) const;
  Poincare::Expression expressionReduced(Poincare::Context * context) const;
  Poincare::Expression expressionClone() const;
  Poincare::Layout layout();
  /* TODO This comment will be true when Sequence inherits from this class
   * Here, isDefined is the exact contrary of isEmpty. However, for Sequence
   * inheriting from ExpressionModel, isEmpty and isDefined have not exactly
   * opposite meaning. For instance, u(n+1)=u(n) & u(0) = ... is not empty and
   * not defined. We thus have to keep both methods. */
  virtual bool isDefined();
  virtual bool isEmpty();
  virtual bool shouldBeClearedBeforeRemove() { return !isEmpty(); }
  /* tidy is responsible to tidy the whole model whereas tidyExpressionModel
   * tidies only the members associated with the ExpressionModel. In
   * ExpressionModelHandle, tidy and tidyExpressionModel trigger the same
   * behaviour but it is not true for its child classes (for example, in
   * Sequence). */
  virtual void tidy();
  virtual void tidyExpressionModel();
  virtual Ion::Storage::Record::ErrorStatus setContent(const char * c);
  Ion::Storage::Record::ErrorStatus setExpressionContent(Poincare::Expression & e);
protected:
  virtual size_t metaDataSize() const = 0;
  Ion::Storage::Record record() const;
  bool isCircularlyDefined(Poincare::Context * context) const;
  mutable Poincare::Expression m_expression;
  mutable Poincare::Layout m_layout;
private:
  void * expressionAddressForValue(Ion::Storage::Record::Data recordData) const;
  size_t expressionSizeForValue(Ion::Storage::Record::Data recordData) const;
  mutable int m_circular;
};

}

#endif
