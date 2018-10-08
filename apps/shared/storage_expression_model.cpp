#include "storage_expression_model.h"
#include "global_context.h"
#include "poincare_helpers.h"
#include <poincare/horizontal_layout.h>
#include <string.h>
#include <cmath>
#include <assert.h>

using namespace Poincare;

namespace Shared {

StorageExpressionModel::StorageExpressionModel(const char * name) :
  m_name(name),
  m_expression(),
  m_layout(),
  m_record()
{
}

StorageExpressionModel::StorageExpressionModel(Ion::Storage::Record record) :
  m_name(record.fullName()),
  m_expression(),
  m_layout(),
  m_record(record)
{
}

void StorageExpressionModel::destroy() {
  record().destroy();
}

void StorageExpressionModel::text(char * buffer, size_t bufferSize) const {
  Expression e = expression();
  if (e.isUninitialized() && bufferSize > 0) {
    buffer[0] = 0;
  } else {
    e.serialize(buffer, bufferSize);
  }
}

Expression StorageExpressionModel::expression() const {
  if (m_expression.isUninitialized()) {
    m_expression = Expression::ExpressionFromRecord(record());
  }
  return m_expression;
}

Expression StorageExpressionModel::reducedExpression(Poincare::Context * context) const {
  return expression().clone().deepReduce(*context, Preferences::AngleUnit::Degree, true);
}

Layout StorageExpressionModel::layout() {
  if (m_layout.isUninitialized()) {
    m_layout = PoincareHelpers::CreateLayout(expression());
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
  return record().isNull() || (record().value().size == 0);
}

void StorageExpressionModel::setContent(const char * c) {
  setContentExpression(expressionToStoreFromString(c));
}

void StorageExpressionModel::tidy() {
  m_layout = Layout();
  m_expression = Expression();
}

Expression StorageExpressionModel::expressionToStoreFromString(const char * c) {
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
  return expressionToStore;
}

void StorageExpressionModel::setContentExpression(Expression expressionToStore) {
  Ion::Storage::Record::Data newData;
  if (expressionToStore.isUninitialized()) {
    newData = {.buffer = nullptr, .size = 0};
  } else {
    newData = {.buffer = expressionToStore.addressInPool(), .size = expressionToStore.size()};
  }
  Ion::Storage::Record::ErrorStatus error = record().setValue(newData);
  assert(error == Ion::Storage::Record::ErrorStatus::None);
  /* We cannot call tidy here because tidy is a virtual function and does not
   * do the same thing for all children class. And here we want to delete only
   * the m_layout and m_expression. */
  m_layout = Layout();
  m_expression = Expression();
}


Ion::Storage::Record StorageExpressionModel::record() const {
  if (m_record.isNull()) {
    m_record = Ion::Storage::sharedStorage()->recordNamed(m_name);
  }
  return m_record;
}

}
