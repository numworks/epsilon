#include "expression_model.h"
#include "global_context.h"
#include "poincare_helpers.h"
#include <poincare/horizontal_layout.h>
#include <poincare/undefined.h>
#include <string.h>
#include <cmath>
#include <assert.h>

using namespace Ion;
using namespace Poincare;

namespace Shared {

static inline int maxInt(int x, int y) { return x > y ? x : y; }

ExpressionModel::ExpressionModel() :
  m_expression(),
  m_layout(),
  m_circular(-1)
{
}

void ExpressionModel::text(const Storage::Record * record, char * buffer, size_t bufferSize, CodePoint symbol) const {
  Expression e = expressionClone(record);
  if (e.isUninitialized() && bufferSize > 0) {
    buffer[0] = 0;
  } else {
    if (symbol != 0 && !e.isUninitialized()) {
      e = e.replaceSymbolWithExpression(Symbol::Builder(UCodePointUnknownX), Symbol::Builder(symbol));
    }
    e.serialize(buffer, bufferSize);
  }
}

bool ExpressionModel::isCircularlyDefined(const Storage::Record * record, Poincare::Context * context) const {
  if (m_circular == -1) {
    m_circular = Expression::ExpressionWithoutSymbols(expressionClone(record), context).isUninitialized();
  }
  return m_circular;
}

Expression ExpressionModel::expressionReduced(const Storage::Record * record, Poincare::Context * context) const {
  if (m_expression.isUninitialized()) {
    assert(record->fullName() != nullptr);
    if (isCircularlyDefined(record, context)) {
      m_expression = Undefined::Builder();
    } else {
      m_expression = Expression::ExpressionFromAddress(expressionAddress(record), expressionSize(record));
      PoincareHelpers::Simplify(&m_expression, context, ExpressionNode::ReductionTarget::SystemForApproximation);
      // simplify might return an uninitialized Expression if interrupted
      if (m_expression.isUninitialized()) {
        m_expression = Expression::ExpressionFromAddress(expressionAddress(record), expressionSize(record));
      }
    }
  }
  return m_expression;
}

Expression ExpressionModel::expressionClone(const Storage::Record * record) const {
  assert(record->fullName() != nullptr);
  /* A new Expression has to be created at each call (because it might be tempered with after calling) */
  return Expression::ExpressionFromAddress(expressionAddress(record), expressionSize(record));
}

Layout ExpressionModel::layout(const Storage::Record * record, CodePoint symbol) const {
  if (m_layout.isUninitialized()) {
    Expression clone = expressionClone(record);
    if (!clone.isUninitialized() && symbol != 0) {
      clone = clone.replaceSymbolWithExpression(Symbol::Builder(UCodePointUnknownX), Symbol::Builder(symbol));
    }
    m_layout = PoincareHelpers::CreateLayout(clone);
    if (m_layout.isUninitialized()) {
      m_layout = HorizontalLayout::Builder();
    }
  }
  return m_layout;
}

Ion::Storage::Record::ErrorStatus ExpressionModel::setContent(Ion::Storage::Record * record, const char * c, CodePoint symbol) {
  Expression e = ExpressionModel::BuildExpressionFromText(c, symbol);
  return setExpressionContent(record, e);
}

Ion::Storage::Record::ErrorStatus ExpressionModel::setExpressionContent(Ion::Storage::Record * record, const Expression & newExpression) {
  assert(record->fullName() != nullptr);
  // Prepare the new data to be stored
  Ion::Storage::Record::Data newData = record->value();
  size_t previousExpressionSize = expressionSize(record);
  size_t newExpressionSize = newExpression.isUninitialized() ? 0 : newExpression.size();
  size_t previousDataSize = newData.size;
  size_t newDataSize = previousDataSize - previousExpressionSize + newExpressionSize;
  void * expAddress = expressionAddress(record);
  // Update size of record to maximal size between previous and new data
  newData.size = maxInt(previousDataSize, newDataSize);
  Ion::Storage::Record::ErrorStatus error = record->setValue(newData);
  if (error != Ion::Storage::Record::ErrorStatus::None) {
    assert(error == Ion::Storage::Record::ErrorStatus::NotEnoughSpaceAvailable);
    return error;
  }
  // Prepare the new data content
  /* WARNING: expressionAddress() cannot be used while the metadata is invalid
   * (as it is sometimes computed from metadata). Thus, the expression address
   * is given as a parameter to updateNewDataWithExpression. */
  updateNewDataWithExpression(record, newExpression, expAddress, newExpressionSize, previousExpressionSize);
  // Set the data with the right size
  newData.size = newDataSize;
  error = record->setValue(newData);
  // Any error would have occured at the first call to setValue
  assert(error == Ion::Storage::Record::ErrorStatus::None);

  /* Here we delete only the elements relative to the expression model kept in
   * this handle. */
  tidy();
  return error;
}

void ExpressionModel::updateNewDataWithExpression(Ion::Storage::Record * record, const Expression & expressionToStore, void * expressionAddress, size_t expressionToStoreSize, size_t previousExpressionSize) {
  if (!expressionToStore.isUninitialized()) {
    memmove(expressionAddress, expressionToStore.addressInPool(), expressionToStoreSize);
  }
}

void ExpressionModel::tidy() const {
  m_layout = Layout();
  m_expression = Expression();
  m_circular = 0;
}

Poincare::Expression ExpressionModel::BuildExpressionFromText(const char * c, CodePoint symbol) {
  Expression expressionToStore;
  // if c = "", we want to reinit the Expression
  if (c && *c != 0) {
    // Compute the expression to store, without replacing symbols
    expressionToStore = Expression::Parse(c);
    if (!expressionToStore.isUninitialized() && symbol != 0) {
      expressionToStore = expressionToStore.replaceSymbolWithExpression(Symbol::Builder(symbol), Symbol::Builder(UCodePointUnknownX));
    }
  }
  return expressionToStore;
}

}
