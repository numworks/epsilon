#include "expression_model.h"
#include "global_context.h"
#include "poincare_helpers.h"
#include "function.h"
#include <ion/unicode/utf8_helper.h>
#include <poincare/function.h>
#include <poincare/symbol.h>
#include <apps/apps_container.h>
#include <poincare/horizontal_layout.h>
#include <poincare/undefined.h>
#include <string.h>
#include <cmath>
#include <assert.h>
#include <algorithm>

using namespace Ion;
using namespace Poincare;

namespace Shared {

ExpressionModel::ExpressionModel() :
  m_circular(-1)
{
}

void ExpressionModel::text(const Storage::Record * record, char * buffer, size_t bufferSize, CodePoint symbol) const {
  assert(record->fullName() != nullptr);
  Expression e = ExpressionModel::expressionClone(record);
  if (e.isUninitialized()) {
    if (bufferSize > 0) {
      buffer[0] = 0;
    }
    return;
  }
  if (symbol != 0) {
    e = e.replaceSymbolWithExpression(Symbol::Builder(UCodePointUnknown), Symbol::Builder(symbol));
  }
  int serializedSize = e.serialize(buffer, bufferSize);
  if (serializedSize >= (int)bufferSize - 1) {
    // It is very likely that the buffer is overflowed
    buffer[0] = 0;
  }
}

bool ExpressionModel::isCircularlyDefined(const Storage::Record * record, Poincare::Context * context) const {
  if (m_circular == -1) {
    Expression e = expressionClone(record);
    m_circular = Expression::ExpressionWithoutSymbols(e, context).isUninitialized();
  }
  return m_circular;
}

Expression ExpressionModel::expressionReduced(const Storage::Record * record, Poincare::Context * context) const {
  /* TODO
   * By calling isCircularlyDefined and then Simplify, the expression tree is
   * browsed twice. Note that Simplify does ALMOST the job of
   * isCircularlyDefined, since Simplify reduces the expression, replaces the
   * symbols it encounters (see SymbolAbstract::Expand). isCircularlyDefined
   * should probably be removed. The difficulty relies in the ambiguous
   * conventions about the values returned or set when a symbol has no proper
   * expression: for example,
   *  - GlobalContext::expressionForSymbolAbstract returns an uninitialized
   *    expression,
   *  - so do Expression::ExpressionWithoutSymbols and SymbolAbstract::Expand,
   *  - Symbol::shallowReduce and Function::shallowReduce and
   *    Expression::deepReplaceReplaceableSymbols return Undefined or
   *    the expression unaltered according to symbolic-computation setting,
   *  - expressionReduced returns Undefined if the expression
   *    isCircularlyDefined but leaves the expression unchanged if Simplify
   *    returns an uninitialized expression...
   */
  if (m_expression.isUninitialized()) {
    assert(record->fullName() != nullptr);
    if (isCircularlyDefined(record, context)) {
      m_expression = Undefined::Builder();
    } else {
      m_expression = Expression::ExpressionFromAddress(expressionAddress(record), expressionSize(record));
      /* 'Simplify' routine might need to call expressionReduced on the very
       * same function. So we need to keep a valid m_expression while executing
       * 'Simplify'. Thus, we use a temporary expression. */
      PoincareHelpers::CloneAndSimplify(&m_expression, context, ExpressionNode::ReductionTarget::SystemForApproximation);
    }
  }
  return m_expression;
}

Expression ExpressionModel::expressionClone(const Storage::Record * record) const {
  assert(record->fullName() != nullptr);
  /* A new Expression has to be created at each call (because it might be tempered with after calling) */
  return Expression::ExpressionFromAddress(expressionAddress(record), expressionSize(record));
  /* TODO
   * The substitution of UCodePointUnknown back and forth is done in the
   * methods text, setContent (through buildExpressionFromText), layout and
   * also in GlobalContext::expressionForSymbolAbstract and
   * GlobalContext::setExpressionForSymbolAbstract. When getting the
   * expression, the substitutions may probably be gathered here.
   */
}

Layout ExpressionModel::layout(const Storage::Record * record, CodePoint symbol) const {
  if (m_layout.isUninitialized()) {
    assert(record->fullName() != nullptr);
    Expression clone = ExpressionModel::expressionClone(record);
    if (!clone.isUninitialized() && symbol != 0) {
      clone = clone.replaceSymbolWithExpression(Symbol::Builder(UCodePointUnknown), Symbol::Builder(symbol));
    }
    m_layout = PoincareHelpers::CreateLayout(clone, AppsContainer::activeApp()->localContext());
    if (m_layout.isUninitialized()) {
      m_layout = HorizontalLayout::Builder();
    }
  }
  return m_layout;
}

Ion::Storage::Record::ErrorStatus ExpressionModel::setContent(Ion::Storage::Record * record, const char * c, Context * context, CodePoint symbol) {
  Expression e = buildExpressionFromText(c, symbol, context);
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
  newData.size = std::max(previousDataSize, newDataSize);
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
  tidyDownstreamPoolFrom();
  return error;
}

void ExpressionModel::updateNewDataWithExpression(Ion::Storage::Record * record, const Expression & expressionToStore, void * expressionAddress, size_t expressionToStoreSize, size_t previousExpressionSize) {
  if (!expressionToStore.isUninitialized()) {
    memmove(expressionAddress, expressionToStore.addressInPool(), expressionToStoreSize);
  }
}

void ExpressionModel::tidyDownstreamPoolFrom(char * treePoolCursor) const {
  if (treePoolCursor == nullptr || m_expression.isDownstreamOf(treePoolCursor)) {
    assert(treePoolCursor == nullptr || m_layout.isUninitialized() || m_layout.isDownstreamOf(treePoolCursor));
    m_layout = Layout();
    m_expression = Expression();
    m_circular = -1;
  }
}

Poincare::Expression ExpressionModel::buildExpressionFromText(const char * c, CodePoint symbol, Poincare::Context * context) const {
  Expression expressionToStore;
  // if c = "", we want to reinit the Expression
  if (c && *c != 0) {
    // Compute the expression to store, without replacing symbols
    expressionToStore = Expression::Parse(c, context);
    expressionToStore = ReplaceSymbolWithUnknown(expressionToStore, symbol);
  }
  return expressionToStore;
}

Poincare::Expression ExpressionModel::ReplaceSymbolWithUnknown(Poincare::Expression e, CodePoint symbol) {
  if (!e.isUninitialized() && symbol != 0) {
    return e.replaceSymbolWithExpression(Symbol::Builder(symbol), Symbol::Builder(UCodePointUnknown));
  }
  return e;
}

}