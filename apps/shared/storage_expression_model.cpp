#include "storage_expression_model.h"
#include "poincare_helpers.h"
#include <string.h>
#include <cmath>
#include <assert.h>

using namespace Poincare;

namespace Shared {

StorageExpressionModel::StorageExpressionModel(Ion::Storage::Record record) :
  m_record(record),
  m_expression(),
  m_layout()
{
}

void StorageExpressionModel::destroy() {
  m_record.destroy();
}

void StorageExpressionModel::text(char * buffer, size_t bufferSize) const {
  expression().serialize(buffer, bufferSize);
}

Expression StorageExpressionModel::expression() const {
  if (m_expression.isUninitialized()) {
    m_expression = Expression::ExpressionFromRecord(m_record);
  }
  return m_expression;
}

Expression StorageExpressionModel::reducedExpression(Poincare::Context * context) const {
  return expression().clone().deepReduce(*context, Preferences::AngleUnit::Degree, true);
}

Layout StorageExpressionModel::layout() {
  if (m_layout.isUninitialized()) {
    m_layout = PoincareHelpers::CreateLayout(expression());
  }
  return m_layout;
}

bool StorageExpressionModel::isDefined() {
  return !m_record.isNull();
}

bool StorageExpressionModel::isEmpty() {
  return m_record.isNull();
}

void StorageExpressionModel::setContent(const char * c) {
  Expression expressionToStore = Expression::parse(c);
  GlobalContext context;
  expressionToStore = expressionToStore.deepReduce(context, Preferences::AngleUnit::Degree, false);
  const char x[2] = {Symbol::SpecialSymbols::UnknownX, 0};
  Symbol xUnknown = Symbol(x, 1);
  expressionToStore = expressionToStore.replaceSymbolWithExpression(Symbol("x", 1), xUnknown);
  Ion::Storage::Record::Data newData = {.buffer = expressionToStore.addressInPool(), .size = expressionToStore.size()};
  m_record.setValue(newData);
  /* We cannot call tidy here because tidy is a virtual function and does not
   * do the same thing for all children class. And here we want to delete only
   * the m_layout and m_expression. */
  m_layout = Layout();
  m_expression = Expression();
}

void StorageExpressionModel::tidy() {
  m_layout = Layout();
  m_expression = Expression();
}

}
