#ifndef SHARED_EXPRESSION_MODEL_HANDLE_H
#define SHARED_EXPRESSION_MODEL_HANDLE_H

#include "expression_model.h"

namespace Shared {

//ExpressionModelHandle is a handle for Ion::Record

class ExpressionModelHandle : public Ion::Storage::Record {
public:
  ExpressionModelHandle(Ion::Storage::Record record = Ion::Storage::Record());
  virtual CodePoint symbol() const { return 0; }

  // Property
  void text(char * buffer, size_t bufferSize) const { return model()->text(this, buffer, bufferSize, symbol()); }
  virtual Poincare::Expression expressionReduced(Poincare::Context * context) const { return model()->expressionReduced(this, context); }
  Poincare::Expression expressionClone() const { return model()->expressionClone(this); }
  Poincare::Layout layout() { return model()->layout(this, symbol()); }
   /* Here, isDefined is the exact contrary of isEmpty. However, for Sequence
   * inheriting from ExpressionModelHandle, isEmpty and isDefined have not exactly
   * opposite meaning. For instance, u(n+1)=u(n) & u(0) = ... is not empty and
   * not defined. We thus have to keep both methods. */
  virtual bool isDefined();
  virtual bool isEmpty();
  /* tidy is responsible to tidy the whole model whereas tidyExpressionModel
   * tidies only the members associated with the ExpressionModel. In
   * ExpressionModel, tidy and tidyExpressionModel trigger the same
   * behaviour but it is not true for its child classes (for example, in
   * Sequence). */
  virtual void tidy() { model()->tidy(); }
  virtual Ion::Storage::Record::ErrorStatus setContent(const char * c, Poincare::Context * context) { return editableModel()->setContent(this, c, context, symbol()); }
  Ion::Storage::Record::ErrorStatus setExpressionContent(const Poincare::Expression & e) { return editableModel()->setExpressionContent(this, e); }
protected:
  ExpressionModel * editableModel() { return const_cast<ExpressionModel *>(model()); }
  virtual const ExpressionModel * model() const = 0;
  virtual size_t metaDataSize() const = 0;
};

}

#endif
