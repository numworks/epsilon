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

/* Storage memory full */

static bool sStorageMemoryFull = false;

bool GlobalContext::storageMemoryFull() {
  return sStorageMemoryFull;
}

const Expression GlobalContext::expressionForSymbol(const SymbolAbstract & symbol) {
  Ion::Storage::Record r = RecordWithName(symbol.name());
  return expressionForSymbolAndRecord(symbol, r);
}

void GlobalContext::setExpressionForSymbol(const Expression & expression, const SymbolAbstract & symbol, Context & context) {
  sStorageMemoryFull = false;
  /* If the new expression contains the symbol, replace it because it will be
   * destroyed afterwards (to be able to do A+2->A) */
  Ion::Storage::Record record = RecordWithName(symbol.name());
  Expression e = expressionForSymbolAndRecord(symbol, record);
  if (e.isUninitialized()) {
    e = Undefined();
  }
  Expression finalExpression = expression.clone().replaceSymbolWithExpression(symbol, e);

  Ion::Storage::Record::ErrorStatus err = Ion::Storage::Record::ErrorStatus::None;
  if (symbol.type() == ExpressionNode::Type::Symbol) {
    err = setExpressionForActualSymbol(finalExpression, symbol, record);
  } else {
    assert(symbol.type() == ExpressionNode::Type::Function);
    err = setExpressionForFunction(finalExpression, symbol, record);
  }

  if (err != Ion::Storage::Record::ErrorStatus::None) {
    assert(err == Ion::Storage::Record::ErrorStatus::NotEnoughSpaceAvailable);
    sStorageMemoryFull = true;
  }
}

Ion::Storage::Record GlobalContext::RecordWithName(const char * name) {
  const char * extensions[2] = {expExtension, funcExtension/*, seqExtension*/};
  return Ion::Storage::sharedStorage()->recordBaseNamedWithExtensions(name, extensions, 2);
}

const Expression GlobalContext::expressionForSymbolAndRecord(const SymbolAbstract & symbol, Ion::Storage::Record r) {
  if (r.isNull()) {
    return Expression();
  }
  if (symbol.type() == ExpressionNode::Type::Symbol) {
    return expressionForActualSymbol(symbol, r);
  }
  assert(symbol.type() == ExpressionNode::Type::Function);
  return expressionForFunction(symbol, r);
}

const Expression GlobalContext::expressionForActualSymbol(const SymbolAbstract & symbol, Ion::Storage::Record r) {
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
  return Expression::ExpressionFromRecord(r);
}

const Expression GlobalContext::expressionForFunction(const SymbolAbstract & symbol, Ion::Storage::Record r) {
  assert(symbol.type() == ExpressionNode::Type::Function);
  StorageCartesianFunction f = StorageCartesianFunction(r);
  return f.expression();
}

Ion::Storage::Record::ErrorStatus GlobalContext::setExpressionForActualSymbol(const Expression & expression, const SymbolAbstract & symbol, Ion::Storage::Record previousRecord) {
  // Delete any record with same name (as it is going to be overriden)
  previousRecord.destroy();
  return Ion::Storage::sharedStorage()->createRecordWithExtension(symbol.name(), expExtension, expression.addressInPool(), expression.size());
}

Ion::Storage::Record::ErrorStatus GlobalContext::setExpressionForFunction(const Expression & expressionToStore, const SymbolAbstract & symbol, Ion::Storage::Record previousRecord) {
  size_t expressionToStoreSize = expressionToStore.isUninitialized() ? 0 : expressionToStore.size();
  size_t newDataSize = sizeof(StorageCartesianFunction::CartesianFunctionRecordData) + expressionToStoreSize;
  Ion::Storage::Record::ErrorStatus error = Ion::Storage::Record::ErrorStatus::None;
  if (Ion::Storage::FullNameHasExtension(previousRecord.fullName(), funcExtension, strlen(funcExtension))) {
    // The previous record was also a function: we want to keep its metadata
    // Prepare the new data to store
    Ion::Storage::Record::Data newData = previousRecord.value();
    newData.size = newDataSize;
    // Set the data
    error = previousRecord.setValue(newData);
  } else {
    // The previous record was not a function. Destroy it and create the new record.
    previousRecord.destroy();
    // Prepare the new data to store
    StorageCartesianFunction::CartesianFunctionRecordData newData;
    // Create the record
    error = Ion::Storage::sharedStorage()->createRecordWithExtension(symbol.name(), funcExtension, &newData, newDataSize);
  }
  if (error == Ion::Storage::Record::ErrorStatus::None && !expressionToStore.isUninitialized()) {
    void * newDataExpressionAddress = StorageCartesianFunction(Ion::Storage::sharedStorage()->recordBaseNamedWithExtension(symbol.name(), funcExtension)).expressionAddress();
    memcpy(newDataExpressionAddress, expressionToStore.addressInPool(), expressionToStore.size());
  }
  return error;
}

}
