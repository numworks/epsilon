#ifndef SHARED_STORAGE_EXPRESSION_MODEL_H
#define SHARED_STORAGE_EXPRESSION_MODEL_H

#include <poincare/context.h>
#include <poincare/expression.h>
#include <poincare/layout.h>
#include <ion/storage.h>

namespace Shared {

class StorageExpressionModel {
  // TODO find better name (once we remove ExpressionModel?)
public:
  StorageExpressionModel(const char * name);
  StorageExpressionModel(Ion::Storage::Record record);
  void destroy();
  const char * name() const { return m_name; }
  void text(char * buffer, size_t bufferSize) const;
  Poincare::Expression expression() const;
  Poincare::Expression reducedExpression(Poincare::Context * context) const;
  Poincare::Layout layout();
  /* TODO This comment will be true when Sequence inherits from this class
   * Here, isDefined is the exact contrary of isEmpty. However, for Sequence
   * inheriting from ExpressionModel, isEmpty and isDefined have not exactly
   * opposite meaning. For instance, u(n+1)=u(n) & u(0) = ... is not empty and
   * not defined. We thus have to keep both methods. */
  virtual bool isDefined();
  virtual bool isEmpty();
  virtual bool shouldBeClearedBeforeRemove() { return !isEmpty(); }
  virtual void setContent(const char * c);
  virtual void tidy();
  virtual void * expressionAddress() const = 0;
  virtual size_t expressionSize() const = 0;
protected:
  Poincare::Expression expressionToStoreFromString(const char * c);
  void setContentExpression(Poincare::Expression expressionToStore);
  Ion::Storage::Record record() const;
  const char * m_name;
  mutable Poincare::Expression m_expression;
  mutable Poincare::Layout m_layout;
private:
  mutable Ion::Storage::Record m_record;
};

}

#endif
