#ifndef SHARED_EXPRESSION_MODEL_H
#define SHARED_EXPRESSION_MODEL_H

#include <poincare/context.h>
#include <poincare/expression.h>
#include <poincare/layout.h>

namespace Shared {

class ExpressionModel {
public:
  ExpressionModel();

  // Getters
  void text(const Ion::Storage::Record * record, char * buffer, size_t bufferSize, CodePoint symbol = 0) const;
  Poincare::Expression expressionReduced(const Ion::Storage::Record * record, Poincare::Context * context) const;
  Poincare::Expression expressionClone(const Ion::Storage::Record * record) const;
  Poincare::Layout layout(const Ion::Storage::Record * record, CodePoint symbol = 0) const;

  // Setters
  Ion::Storage::Record::ErrorStatus setContent(Ion::Storage::Record * record, const char * c, Poincare::Context * context, CodePoint symbol = 0);
  Ion::Storage::Record::ErrorStatus setExpressionContent(Ion::Storage::Record * record, const Poincare::Expression & newExpression);

  virtual void tidy() const;
protected:
  // Setters helper
  static Poincare::Expression BuildExpressionFromText(const char * c, CodePoint symbol = 0, Poincare::Context * context = nullptr);
  mutable Poincare::Expression m_expression;
  mutable Poincare::Layout m_layout;
private:
  virtual void updateNewDataWithExpression(Ion::Storage::Record * record, const Poincare::Expression & expressionToStore, void * expressionAddress, size_t expressionToStoreSize, size_t previousExpressionSize);
  virtual void * expressionAddress(const Ion::Storage::Record * record) const = 0;
  virtual size_t expressionSize(const Ion::Storage::Record * record) const = 0;
  bool isCircularlyDefined(const Ion::Storage::Record * record, Poincare::Context * context) const;
  mutable int8_t m_circular;
};

}

#endif
