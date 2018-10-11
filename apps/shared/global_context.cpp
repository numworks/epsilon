#include "global_context.h"
#include "storage_cartesian_function.h"
#include <poincare/helpers.h>
#include <poincare/undefined.h>
#include <poincare/preferences.h>
#include <assert.h>
#include <cmath>
#include <ion.h>

using namespace Poincare;

namespace Shared {

constexpr char GlobalContext::expExtension[];
constexpr char GlobalContext::funcExtension[];
//constexpr char GlobalContext::seqExtension[];

static bool sStorageMemoryFull = false;

bool GlobalContext::RecordBaseNameIsFree(const char * baseName) {
  return RecordWithBaseName(baseName).isNull();
}

bool GlobalContext::storageMemoryFull() {
  return sStorageMemoryFull;
}

Poincare::Expression GlobalContext::ExpressionFromRecord(Ion::Storage::Record record) {
  if (record.isNull() || record.value().size == 0) {
    return Expression();
  }
  if (Ion::Storage::FullNameHasExtension(record.fullName(), expExtension, strlen(expExtension))) {
    return ExpressionFromSymbolRecord(record);
  }
  assert(Ion::Storage::FullNameHasExtension(record.fullName(), funcExtension, strlen(funcExtension)));
  return ExpressionFromFunctionRecord(record);
}

Poincare::Expression GlobalContext::ExpressionFromSymbolRecord(Ion::Storage::Record record) {
  if (record.isNull() || record.value().size == 0) {
    return Expression();
  }
  assert(Ion::Storage::FullNameHasExtension(record.fullName(), expExtension, strlen(expExtension)));
  // An expression record value is the expression itself
  Ion::Storage::Record::Data d = record.value();
  return Expression::ExpressionFromAddress(d.buffer, d.size);
}
Poincare::Expression GlobalContext::ExpressionFromFunctionRecord(Ion::Storage::Record record) {
  if (record.isNull() || record.value().size == 0) {
    return Expression();
  }
  assert(Ion::Storage::FullNameHasExtension(record.fullName(), funcExtension, strlen(funcExtension)));
  /* An function record value has metadata before the expression. To get the
   * expression, use the funciton record handle. */
  StorageCartesianFunction f = StorageCartesianFunction(record);
  return f.expressionWithSymbol();
}

const Expression GlobalContext::expressionForSymbol(const SymbolAbstract & symbol) {
  Ion::Storage::Record r = RecordWithBaseName(symbol.name());
  return ExpressionForSymbolAndRecord(symbol, r);
}

void GlobalContext::setExpressionForSymbol(const Expression & expression, const SymbolAbstract & symbol, Context & context) {
  sStorageMemoryFull = false;
  /* If the new expression contains the symbol, replace it because it will be
   * destroyed afterwards (to be able to do A+2->A) */
  Ion::Storage::Record record = RecordWithBaseName(symbol.name());
  Expression e = ExpressionFromRecord(record);
  if (e.isUninitialized()) {
    e = Undefined();
  }
  Expression finalExpression = expression.clone().replaceSymbolWithExpression(symbol, e);

  // Set the expression in the storage depending on the symbol type
  Ion::Storage::Record::ErrorStatus err = Ion::Storage::Record::ErrorStatus::None;
  if (symbol.type() == ExpressionNode::Type::Symbol) {
    err = SetExpressionForActualSymbol(finalExpression, symbol, record);
  } else {
    assert(symbol.type() == ExpressionNode::Type::Function);
    err = SetExpressionForFunction(finalExpression, symbol, record);
  }

  // Handle a storage error
  if (err != Ion::Storage::Record::ErrorStatus::None) {
    assert(err == Ion::Storage::Record::ErrorStatus::NotEnoughSpaceAvailable);
    sStorageMemoryFull = true;
  }
}

const Expression GlobalContext::ExpressionForSymbolAndRecord(const SymbolAbstract & symbol, Ion::Storage::Record r) {
  if (r.isNull()) {
    return Expression();
  }
  if (symbol.type() == ExpressionNode::Type::Symbol) {
    return ExpressionForActualSymbol(symbol, r);
  }
  assert(symbol.type() == ExpressionNode::Type::Function);
  return ExpressionForFunction(symbol, r);
}

const Expression GlobalContext::ExpressionForActualSymbol(const SymbolAbstract & symbol, Ion::Storage::Record r) {
  assert(symbol.type() == ExpressionNode::Type::Symbol);
  // Constant symbols
  Symbol s = static_cast<const Symbol &>(symbol);
  if (s.isPi()) {
    return Float<double>(M_PI);
  }
  if (s.isExponential()) {
    return Float<double>(M_E);
  }
  // Look up the file system for symbol
  return ExpressionFromSymbolRecord(r);
}

Ion::Storage::Record::ErrorStatus GlobalContext::SetExpressionForActualSymbol(const Expression & expression, const SymbolAbstract & symbol, Ion::Storage::Record previousRecord) {
  // Delete any record with same name (as it is going to be overriden)
  previousRecord.destroy();
  return Ion::Storage::sharedStorage()->createRecordWithExtension(symbol.name(), expExtension, expression.addressInPool(), expression.size());
}

Ion::Storage::Record::ErrorStatus GlobalContext::SetExpressionForFunctionRecord(Expression expressionToStore, const char * baseName, Ion::Storage::Record previousRecord) {
  Ion::Storage::Record recordToSet = previousRecord;
  Ion::Storage::Record::ErrorStatus error = Ion::Storage::Record::ErrorStatus::None;
  if (!Ion::Storage::FullNameHasExtension(previousRecord.fullName(), funcExtension, strlen(funcExtension))) {
    // The previous record was not a function. Destroy it and create the new record.
    previousRecord.destroy();
    StorageCartesianFunction newModel = StorageCartesianFunction::NewModel(&error);
    if (error == Ion::Storage::Record::ErrorStatus::None) {
      return error;
    }
    recordToSet = newModel.record();
  }
  error = StorageCartesianFunction(recordToSet).setExpressionContent(expressionToStore);
  return error;
}

Ion::Storage::Record::ErrorStatus GlobalContext::SetExpressionForFunction(const Expression & expressionToStore, const SymbolAbstract & symbol, Ion::Storage::Record previousRecord) {
  return SetExpressionForFunctionRecord(expressionToStore, symbol.name(), previousRecord);
}

Ion::Storage::Record GlobalContext::RecordWithBaseName(const char * name) {
  const char * extensions[2] = {expExtension, funcExtension/*, seqExtension*/};
  return Ion::Storage::sharedStorage()->recordBaseNamedWithExtensions(name, extensions, 2);
}

}
