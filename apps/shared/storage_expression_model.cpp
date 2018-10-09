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
  if (m_expression.isUninitialized() && !m_record.isNull()) {
    m_expression = Expression::ExpressionFromAddress(expressionAddress(), expressionSize());
  }
  return m_expression;
}

Expression StorageExpressionModel::reducedExpression(Poincare::Context * context) const {
  return expression().clone().deepReduce(*context, Preferences::AngleUnit::Degree, true);
}

Layout StorageExpressionModel::layout() {
  if (m_layout.isUninitialized() && !m_record.isNull()) {
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

void StorageExpressionModel::didSetContentData() {
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
