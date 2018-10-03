#include "storage_expression_model.h"
#include "poincare_helpers.h"
#include <string.h>
#include <cmath>
#include <assert.h>

using namespace Poincare;

namespace Shared {

StorageExpressionModel::StorageExpressionModel() :
  m_record(),
  m_expression(),
  m_layout()
{
}

void StorageExpressionModel::destroy() {
  m_record.destroy();
}

const char * StorageExpressionModel::text() const {
  return static_cast<const char *>(m_record.value().buffer);
}

Poincare::Expression StorageExpressionModel::expression(Poincare::Context * context) const {
  if (m_expression.isUninitialized()) {
    m_expression = PoincareHelpers::ParseAndSimplify(text(), *context);
  }
  return m_expression;
}

Layout StorageExpressionModel::layout() {
  if (m_layout.isUninitialized()) {
    Expression nonSimplifiedExpression = Expression::parse(text());
    m_layout = PoincareHelpers::CreateLayout(nonSimplifiedExpression);
  }
  return m_layout;
}

bool StorageExpressionModel::isDefined() {
  return m_record.isNull();
}

bool StorageExpressionModel::isEmpty() {
  return m_record.isNull();
}

void StorageExpressionModel::setContent(const char * c) {
  Ion::Storage::Record::Data newData = {.buffer = c, .size = strlen(c) + 1};
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
