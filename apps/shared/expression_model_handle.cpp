#include "expression_model_handle.h"
#include "global_context.h"
#include "poincare_helpers.h"
#include <poincare/horizontal_layout.h>
#include <string.h>
#include <cmath>
#include <assert.h>

using namespace Ion;
using namespace Poincare;

namespace Shared {

static inline int maxInt(int x, int y) { return x > y ? x : y; }

ExpressionModelHandle::ExpressionModelHandle() :
  m_expression(),
  m_layout(),
  m_circular(-1)
{
}

void ExpressionModelHandle::text(const Storage::Record * record, char * buffer, size_t bufferSize) const {
  Expression e = expressionClone(record);
  if (e.isUninitialized() && bufferSize > 0) {
    buffer[0] = 0;
  } else {
    e.serialize(buffer, bufferSize);
  }
}

bool ExpressionModelHandle::isCircularlyDefined(const Storage::Record * record, Poincare::Context * context) const {
  if (m_circular == -1) {
    m_circular = Expression::ExpressionWithoutSymbols(expressionClone(record), *context).isUninitialized();
  }
  return m_circular;
}

Expression ExpressionModelHandle::expressionReduced(const Storage::Record * record, Poincare::Context * context) const {
  if (m_expression.isUninitialized()) {
    assert(record->fullName() != nullptr);
    m_expression = Expression::ExpressionFromAddress(expressionAddress(record), expressionSize(record));
    PoincareHelpers::Simplify(&m_expression, *context);
    // simplify might return an uninitialized Expression if interrupted
    if (m_expression.isUninitialized()) {
      m_expression = Expression::ExpressionFromAddress(expressionAddress(record), expressionSize(record));
    }
  }
  return m_expression;
}

Expression ExpressionModelHandle::expressionClone(const Storage::Record * record) const {
  assert(record->fullName() != nullptr);
  /* A new Expression has to be created at each call (because it might be tempered with after calling) */
  return Expression::ExpressionFromAddress(expressionAddress(record), expressionSize(record));
}

Layout ExpressionModelHandle::layout(const Storage::Record * record) const {
  if (m_layout.isUninitialized()) {
    m_layout = PoincareHelpers::CreateLayout(expressionClone(record));
    if (m_layout.isUninitialized()) {
      m_layout = HorizontalLayout::Builder();
    }
  }
  return m_layout;
}

Ion::Storage::Record::ErrorStatus ExpressionModelHandle::setContent(Ion::Storage::Record * record, const char * c, char symbol) {
  Expression e = ExpressionModelHandle::BuildExpressionFromText(c, symbol);
  return setExpressionContent(record, e);
}

Ion::Storage::Record::ErrorStatus ExpressionModelHandle::setExpressionContent(Ion::Storage::Record * record, Expression & newExpression) {
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

void ExpressionModelHandle::updateNewDataWithExpression(Ion::Storage::Record * record, Expression & expressionToStore, void * expressionAddress, size_t expressionToStoreSize, size_t previousExpressionSize) {
  if (!expressionToStore.isUninitialized()) {
    memmove(expressionAddress, expressionToStore.addressInPool(), expressionToStoreSize);
  }
}

void ExpressionModelHandle::tidy() const {
  m_layout = Layout();
  m_expression = Expression();
  m_circular = 0;
}

Poincare::Expression ExpressionModelHandle::BuildExpressionFromText(const char * c, char symbol) {
  Expression expressionToStore;
  // if c = "", we want to reinit the Expression
  if (c && *c != 0) {
    // Compute the expression to store, without replacing symbols
    expressionToStore = Expression::Parse(c);
    if (!expressionToStore.isUninitialized() && symbol != 0) {
      expressionToStore = expressionToStore.replaceUnknown(Symbol::Builder(symbol));
    }
  }
  return expressionToStore;
}

}
