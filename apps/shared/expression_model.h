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
  void text(const Ion::Storage::Record* record, char* buffer, size_t bufferSize,
            CodePoint symbol = 0) const;
  virtual Poincare::Expression expressionReduced(
      const Ion::Storage::Record* record, Poincare::Context* context) const;
  virtual Poincare::Expression expressionClone(
      const Ion::Storage::Record* record) const;
  Poincare::Layout layout(const Ion::Storage::Record* record,
                          CodePoint symbol = 0) const;

  // Setters
  Ion::Storage::Record::ErrorStatus setContent(Ion::Storage::Record* record,
                                               const char* c,
                                               Poincare::Context* context,
                                               CodePoint symbol = 0);
  Ion::Storage::Record::ErrorStatus setExpressionContent(
      Ion::Storage::Record* record, const Poincare::Expression& newExpression);

  virtual void tidyDownstreamPoolFrom(char* treePoolCursor = nullptr) const;

  Poincare::Preferences::ComplexFormat complexFormat(
      const Ion::Storage::Record* record, Poincare::Context* context) const;

 protected:
  // Setters helper
  virtual Poincare::Expression buildExpressionFromText(
      const char* c, CodePoint symbol = 0,
      Poincare::Context* context = nullptr) const;
  static Poincare::Expression ReplaceSymbolWithUnknown(Poincare::Expression e,
                                                       CodePoint symbol);

  bool isCircularlyDefined(const Ion::Storage::Record* record,
                           Poincare::Context* context) const;
  virtual void updateNewDataWithExpression(
      Ion::Storage::Record* record,
      const Poincare::Expression& expressionToStore, void* expressionAddress,
      size_t expressionToStoreSize, size_t previousExpressionSize);

  mutable Poincare::Expression m_expression;
  mutable Poincare::Layout m_layout;

 private:
  virtual void* expressionAddress(const Ion::Storage::Record* record) const = 0;
  virtual size_t expressionSize(const Ion::Storage::Record* record) const = 0;

  mutable int8_t m_circular;

  enum class MemoizedComplexFormat : int8_t {
    NotMemoized = -1,
    Complex = 0,
    Any = 1,
  };

  /* Memoize complex format and angle unit to avoid recomputing them each time
   * the model is approximated (when it's plotted for example). */
  mutable MemoizedComplexFormat m_expressionComplexFormat;
};

}  // namespace Shared

#endif
