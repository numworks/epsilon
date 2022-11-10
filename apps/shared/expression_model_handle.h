#ifndef SHARED_EXPRESSION_MODEL_HANDLE_H
#define SHARED_EXPRESSION_MODEL_HANDLE_H

#include "expression_model.h"

namespace Shared {

//ExpressionModelHandle is a handle for Ion::Record

class ExpressionModelHandle : public Ion::Storage::Record {
public:
  ExpressionModelHandle(Ion::Storage::Record record = Ion::Storage::Record());
  virtual CodePoint symbol() const = 0;

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
  virtual bool shouldBeClearedBeforeRemove() { return !isEmpty(); }
  /* tidyDownstreamPoolFrom tidy the model if its members are located downstream
   * in Poincare pool of the node given as arguments. */
  virtual void tidyDownstreamPoolFrom(char * treePoolCursor = nullptr) const { model()->tidyDownstreamPoolFrom(treePoolCursor); }
  virtual Ion::Storage::Record::ErrorStatus setContent(const char * c, Poincare::Context * context) { return editableModel()->setContent(this, c, context, symbol()); }
  Ion::Storage::Record::ErrorStatus setExpressionContent(const Poincare::Expression & e) { return editableModel()->setExpressionContent(this, e); }

  Poincare::Preferences::ComplexFormat complexFormat(Poincare::Context * context) const {
    return model()->complexFormat(this, context);
  }
protected:
  ExpressionModel * editableModel() { return const_cast<ExpressionModel *>(model()); }
  virtual const ExpressionModel * model() const = 0;
  virtual size_t metaDataSize() const = 0;
};

}

#endif
