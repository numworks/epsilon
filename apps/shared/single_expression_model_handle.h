#ifndef SHARED_SINGLE_EXPRESSION_MODEL_HANDLE_H
#define SHARED_SINGLE_EXPRESSION_MODEL_HANDLE_H

#include "expression_model_handle.h"

namespace Shared {


class SingleExpressionModelHandle : public Ion::Storage::Record {
public:
  SingleExpressionModelHandle(Ion::Storage::Record record);

  // Property
  virtual char symbol() const { return 0; };
  void text(char * buffer, size_t bufferSize) const { return handle()->text(this, buffer, bufferSize); }
  Poincare::Expression expressionReduced(Poincare::Context * context) const { return handle()->expressionReduced(this, context); }
  Poincare::Expression expressionClone() const { return handle()->expressionClone(this); }
  Poincare::Layout layout() { return handle()->layout(this); }
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
  virtual void tidy() { handle()->tidy(); }
  virtual Ion::Storage::Record::ErrorStatus setContent(const char * c) { return editableHandle()->setContent(this, c, symbol()); }
  Ion::Storage::Record::ErrorStatus setExpressionContent(Poincare::Expression & e) { return editableHandle()->setExpressionContent(this, e); }
protected:
  bool isCircularlyDefined(Poincare::Context * context) const { return handle()->isCircularlyDefined(this, context); }
  ExpressionModelHandle * editableHandle() { return const_cast<ExpressionModelHandle *>(handle()); }
  virtual const ExpressionModelHandle * handle() const = 0;
  virtual size_t metaDataSize() const = 0;
};

}

#endif
