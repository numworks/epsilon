#ifndef SHARED_EXPRESSION_MODEL_HANDLE_H
#define SHARED_EXPRESSION_MODEL_HANDLE_H

#include "expression_model.h"

namespace Shared {

//ExpressionModelHandle is a handle for Ion::Record

class ExpressionModelHandle : public Ion::Storage::Record {
public:
  ExpressionModelHandle(Ion::Storage::Record record = Ion::Storage::Record());

  // Property
  void text(char * buffer, size_t bufferSize) const { return handle()->text(this, buffer, bufferSize); }
  Poincare::Expression expressionReduced(Poincare::Context * context) const { return handle()->expressionReduced(this, context); }
  Poincare::Expression expressionClone() const { return handle()->expressionClone(this); }
  Poincare::Layout layout() { return handle()->layout(this); }
   /* Here, isDefined is the exact contrary of isEmpty. However, for Sequence
   * inheriting from ExpressionModelHandle, isEmpty and isDefined have not exactly
   * opposite meaning. For instance, u(n+1)=u(n) & u(0) = ... is not empty and
   * not defined. We thus have to keep both methods. */
  virtual bool isDefined();
  virtual bool isEmpty();
  virtual bool shouldBeClearedBeforeRemove() { return !isEmpty(); }
  /* tidy is responsible to tidy the whole model whereas tidyExpressionModel
   * tidies only the members associated with the ExpressionModel. In
   * ExpressionModel, tidy and tidyExpressionModel trigger the same
   * behaviour but it is not true for its child classes (for example, in
   * Sequence). */
  virtual void tidy() { handle()->tidy(); }
  virtual Ion::Storage::Record::ErrorStatus setContent(const char * c) { return editableHandle()->setContent(this, c); }
  Ion::Storage::Record::ErrorStatus setExpressionContent(Poincare::Expression & e) { return editableHandle()->setExpressionContent(this, e); }
protected:
  bool isCircularlyDefined(Poincare::Context * context) const { return handle()->isCircularlyDefined(this, context); }
  ExpressionModel * editableHandle() { return const_cast<ExpressionModel *>(handle()); }
  virtual const ExpressionModel * handle() const = 0;
  virtual size_t metaDataSize() const = 0;
};

}

#endif
