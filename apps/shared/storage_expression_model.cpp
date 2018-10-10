#include "storage_expression_model.h"
#include "global_context.h"
#include "poincare_helpers.h"
#include <poincare/horizontal_layout.h>
#include <string.h>
#include <cmath>
#include <assert.h>

using namespace Ion;
using namespace Poincare;

namespace Shared {

StorageExpressionModel::StorageExpressionModel(Storage::Record record) :
  Storage::Record(record),
  m_expression(),
  m_layout()
{
}

/*void StorageExpressionModel::text(char * buffer, size_t bufferSize) const {
  Expression e = expression();
  if (e.isUninitialized() && bufferSize > 0) {
    buffer[0] = 0;
  } else {
    e.serialize(buffer, bufferSize);
  }
}*/

Expression StorageExpressionModel::expression(Poincare::Context * context) const {
  if (m_expression.isUninitialized()) {
    m_expression = Expression::ExpressionFromAddress(expressionAddress(), expressionSize()).deepReduce(*context, Preferences::AngleUnit::Degree, true);
  }
  return m_expression;
}


Layout StorageExpressionModel::layout() {
  if (m_layout.isUninitialized()) {
    m_layout = PoincareHelpers::CreateLayout(Expression::ExpressionFromAddress(expressionAddress(), expressionSize()));
    if (m_layout.isUninitialized()) {
      m_layout = HorizontalLayout();
    }
  }
  return m_layout;
}

bool StorageExpressionModel::isDefined() {
  return !isEmpty();
}

bool StorageExpressionModel::isEmpty() {
  return record().value().size == 0;
}

void StorageExpressionModel::tidy() {
  m_layout = Layout();
  m_expression = Expression();
}

Ion::Storage::Record::ErrorStatus StorageExpressionModel::setContent(const char * c) {
  // Compute the expression to store
  Expression expressionToStore = Expression::parse(c);
  if (!expressionToStore.isUninitialized()) {
    GlobalContext context;
    expressionToStore = expressionToStore.deepReduce(context, Preferences::AngleUnit::Degree, false);
    if (!expressionToStore.isUninitialized()) {
      const char x[2] = {Symbol::SpecialSymbols::UnknownX, 0};
      Symbol xUnknown = Symbol(x, 1);
      expressionToStore = expressionToStore.replaceSymbolWithExpression(Symbol("x", 1), xUnknown);
    }
  }
  return setExpressionContent(expressionToStore);
}

Ion::Storage::Record::ErrorStatus StorageExpressionModel::setExpressionContent(Expression & expressionToStore) {
  assert(!isNull());
  // Prepare the new data to store
  Ion::Storage::Record::Data newData = value();
  size_t expressionToStoreSize = expressionToStore.isUninitialized() ? 0 : expressionToStore.size();
  newData.size = metaDataSize() + expressionToStoreSize;

  // Set the data
  Ion::Storage::Record::ErrorStatus error = record().setValue(newData);
  if (error != Ion::Storage::Record::ErrorStatus::None) {
    assert(error == Ion::Storage::Record::ErrorStatus::NotEnoughSpaceAvailable);
    return error;
  }

  // Copy the expression if needed
  if (!expressionToStore.isUninitialized()) {
    memcpy(expressionAddress(), expressionToStore.addressInPool(), expressionToStore.size());
  }
  /* We cannot call tidy here because tidy is a virtual function and does not
   * do the same thing for all children class. And here we want to delete only
   * the m_layout and m_expression. */
  m_layout = Layout();
  m_expression = Expression();
  return error;
}

void * StorageExpressionModel::expressionAddress() const {
  assert(!isNull());
  return (char *)value().buffer+metaDataSize();
}

size_t StorageExpressionModel::expressionSize() const {
  assert(!isNull());
  return value().size-metaDataSize();
}

}
