#include "global_context.h"
#include "cartesian_function.h"
#include "poincare_helpers.h"
#include <poincare/undefined.h>
#include <assert.h>

using namespace Poincare;

namespace Shared {

constexpr const char * GlobalContext::k_extensions[];

bool GlobalContext::SymbolAbstractNameIsFree(const char * baseName) {
  return SymbolAbstractRecordWithBaseName(baseName).isNull();
}

Poincare::Expression GlobalContext::ExpressionFromRecord(Ion::Storage::Record record) {
  if (record.isNull() || record.value().size == 0) {
    return Expression();
  }
  if (Ion::Storage::FullNameHasExtension(record.fullName(), Ion::Storage::expExtension, strlen(Ion::Storage::expExtension))) {
    return ExpressionFromSymbolRecord(record);
  }
  assert(Ion::Storage::FullNameHasExtension(record.fullName(), Ion::Storage::funcExtension, strlen(Ion::Storage::funcExtension)));
  return ExpressionFromFunctionRecord(record);
}

Poincare::Expression GlobalContext::ExpressionFromSymbolRecord(Ion::Storage::Record record) {
  if (record.isNull() || record.value().size == 0) {
    return Expression();
  }
  assert(Ion::Storage::FullNameHasExtension(record.fullName(), Ion::Storage::expExtension, strlen(Ion::Storage::expExtension)));
  // An expression record value is the expression itself
  Ion::Storage::Record::Data d = record.value();
  return Expression::ExpressionFromAddress(d.buffer, d.size);
}
Poincare::Expression GlobalContext::ExpressionFromFunctionRecord(Ion::Storage::Record record) {
  if (record.isNull() || record.value().size == 0) {
    return Expression();
  }
  if (!Ion::Storage::FullNameHasExtension(record.fullName(), Ion::Storage::funcExtension, strlen(Ion::Storage::funcExtension))) {
    return Expression();
  }
  /* An function record value has metadata before the expression. To get the
   * expression, use the function record handle. */
  CartesianFunction f = CartesianFunction(record);
  return f.expressionClone();
}

const Layout GlobalContext::LayoutForRecord(Ion::Storage::Record record) {
  assert(!record.isNull());
  if (Ion::Storage::FullNameHasExtension(record.fullName(), Ion::Storage::expExtension, strlen(Ion::Storage::expExtension))) {
    return PoincareHelpers::CreateLayout(ExpressionFromSymbolRecord(record));
  } else {
    assert(Ion::Storage::FullNameHasExtension(record.fullName(), Ion::Storage::funcExtension, strlen(Ion::Storage::funcExtension)));
    return CartesianFunction(record).layout();
  }
}

void GlobalContext::DestroyRecordsBaseNamedWithoutExtension(const char * baseName, const char * extension) {
  for (int i = 0; i < k_numberOfExtensions; i++) {
    if (strcmp(k_extensions[i], extension) != 0) {
      Ion::Storage::sharedStorage()->destroyRecordWithBaseNameAndExtension(baseName, k_extensions[i]);
    }
  }
}

const Expression GlobalContext::expressionForSymbolAbstract(const SymbolAbstract & symbol, bool clone) {
  Ion::Storage::Record r = SymbolAbstractRecordWithBaseName(symbol.name());
  return ExpressionForSymbolAndRecord(symbol, r);
}

void GlobalContext::setExpressionForSymbolAbstract(const Expression & expression, const SymbolAbstract & symbol, Context * context) {
  /* If the new expression contains the symbol, replace it because it will be
   * destroyed afterwards (to be able to do A+2->A) */
  Ion::Storage::Record record = SymbolAbstractRecordWithBaseName(symbol.name());
  Expression e = ExpressionFromRecord(record);
  if (e.isUninitialized()) {
    e = Undefined::Builder();
  }
  Expression finalExpression = expression.clone().replaceSymbolWithExpression(symbol, e);

  // Set the expression in the storage depending on the symbol type
  if (symbol.type() == ExpressionNode::Type::Symbol) {
    SetExpressionForActualSymbol(finalExpression, symbol, record);
  } else {
    assert(symbol.type() == ExpressionNode::Type::Function);
    SetExpressionForFunction(finalExpression, symbol, record);
  }
}

const Expression GlobalContext::ExpressionForSymbolAndRecord(const SymbolAbstract & symbol, Ion::Storage::Record r) {
  if (symbol.type() == ExpressionNode::Type::Symbol) {
    return ExpressionForActualSymbol(symbol, r);
  }
  assert(symbol.type() == ExpressionNode::Type::Function);
  return ExpressionForFunction(symbol, r);
}

const Expression GlobalContext::ExpressionForActualSymbol(const SymbolAbstract & symbol, Ion::Storage::Record r) {
  assert(symbol.type() == ExpressionNode::Type::Symbol);
  if (r.isNull() || !Ion::Storage::FullNameHasExtension(r.fullName(), Ion::Storage::expExtension, strlen(Ion::Storage::expExtension))) {
    return Expression();
  }
  // Look up the file system for symbol
  return ExpressionFromSymbolRecord(r);
}

Ion::Storage::Record::ErrorStatus GlobalContext::SetExpressionForActualSymbol(const Expression & expression, const SymbolAbstract & symbol, Ion::Storage::Record previousRecord) {
  if (!previousRecord.isNull() && Ion::Storage::FullNameHasExtension(previousRecord.fullName(), Ion::Storage::funcExtension, strlen(Ion::Storage::funcExtension))) {
    /* A function can overwrite a variable, but a variable cannot be created if
     * it has the same name as an existing function. */
    // TODO Pop up "Name taken for a function"
    return Ion::Storage::Record::ErrorStatus::NameTaken;
  }
  // Delete any record with same name (as it is going to be overriden)
  previousRecord.destroy();
  return Ion::Storage::sharedStorage()->createRecordWithExtension(symbol.name(), Ion::Storage::expExtension, expression.addressInPool(), expression.size());
}

Ion::Storage::Record::ErrorStatus GlobalContext::SetExpressionForFunction(const Expression & expressionToStore, const SymbolAbstract & symbol, Ion::Storage::Record previousRecord) {
  Ion::Storage::Record recordToSet = previousRecord;
  Ion::Storage::Record::ErrorStatus error = Ion::Storage::Record::ErrorStatus::None;
  if (!Ion::Storage::FullNameHasExtension(previousRecord.fullName(), Ion::Storage::funcExtension, strlen(Ion::Storage::funcExtension))) {
    // The previous record was not a function. Destroy it and create the new record.
    previousRecord.destroy();
    CartesianFunction newModel = CartesianFunction::NewModel(&error, symbol.name());
    if (error != Ion::Storage::Record::ErrorStatus::None) {
      return error;
    }
    recordToSet = newModel;
  }
  error = CartesianFunction(recordToSet).setExpressionContent(expressionToStore);
  return error;
}

Ion::Storage::Record GlobalContext::SymbolAbstractRecordWithBaseName(const char * name) {
  return Ion::Storage::sharedStorage()->recordBaseNamedWithExtensions(name, k_extensions, k_numberOfExtensions);
}

}
