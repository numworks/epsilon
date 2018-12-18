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
  m_layout(),
  m_circular(-1)
{
}

void StorageExpressionModel::text(char * buffer, size_t bufferSize) const {
  Expression e = expressionClone();
  if (e.isUninitialized() && bufferSize > 0) {
    buffer[0] = 0;
  } else {
    e.serialize(buffer, bufferSize);
  }
}

bool StorageExpressionModel::isCircularlyDefined(Poincare::Context * context) const {
  if (m_circular == -1) {
    m_circular = Expression::ExpressionWithoutSymbols(expressionClone(), *context).isUninitialized();
  }
  return m_circular;
}

Expression StorageExpressionModel::expressionReduced(Poincare::Context * context) const {
  if (m_expression.isUninitialized()) {
    assert(!isNull());
    Ion::Storage::Record::Data recordData = value();
    m_expression = Expression::ExpressionFromAddress(expressionAddressForValue(recordData), expressionSizeForValue(recordData)).reduce(*context, Preferences::sharedPreferences()->angleUnit(), true);
  }
  return m_expression;
}

Expression StorageExpressionModel::expressionClone() const {
  assert(!isNull());
  Ion::Storage::Record::Data recordData = value();
  /* A new Expression has to be created at each call (because it might be tempered with after calling) */
  return Expression::ExpressionFromAddress(expressionAddressForValue(recordData), expressionSizeForValue(recordData));
}

Layout StorageExpressionModel::layout() {
  if (m_layout.isUninitialized()) {
    m_layout = PoincareHelpers::CreateLayout(expressionClone());
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
  return value().size <= metaDataSize();
}

void StorageExpressionModel::tidy() {
  m_layout = Layout();
  m_expression = Expression();
  m_circular = 0;
}

Ion::Storage::Record::ErrorStatus StorageExpressionModel::setContent(const char * c) {
  Expression expressionToStore;
  // if c = "", we want to reinit the Expression
  if (c && *c != 0) {
    // Compute the expression to store, without replacing symbols
    expressionToStore = Expression::Parse(c);
    if (!expressionToStore.isUninitialized()) {
      expressionToStore = expressionToStore.replaceUnknown(Symbol('x')); //TODO Beware of non x unknowns! (for instance whe sequences are in the storage)
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
  Ion::Storage::Record::ErrorStatus error = setValue(newData);
  if (error != Ion::Storage::Record::ErrorStatus::None) {
    assert(error == Ion::Storage::Record::ErrorStatus::NotEnoughSpaceAvailable);
    return error;
  }

  // Copy the expression if needed
  if (!expressionToStore.isUninitialized()) {
    memcpy(expressionAddressForValue(value()), expressionToStore.addressInPool(), expressionToStore.size());
  }
  /* We cannot call tidy here because tidy is a virtual function and does not
   * do the same thing for all children class. And here we want to delete only
   * the m_layout and m_expression. */
  m_layout = Layout();
  m_expression = Expression();
  m_circular = 0;
  return error;
}

void * StorageExpressionModel::expressionAddressForValue(Ion::Storage::Record::Data recordData) const {
  return (char *)recordData.buffer+metaDataSize();
}

size_t StorageExpressionModel::expressionSizeForValue(Ion::Storage::Record::Data recordData) const {
  return recordData.size-metaDataSize();
}

}
