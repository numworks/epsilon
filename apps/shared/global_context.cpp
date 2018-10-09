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
  Expression finalExpression = expression.clone().replaceSymbolWithExpression(symbol, e);
  // Delete any record with same name (as it is going to be overriden)
  record.destroy();

  Ion::Storage::Record::ErrorStatus err = Ion::Storage::sharedStorage()->createRecordWithExtension(symbol.name(), ExtensionForExpression(symbol), finalExpression.addressInPool(), finalExpression.size());
  if (err != Ion::Storage::Record::ErrorStatus::None) {
    assert(err == Ion::Storage::Record::ErrorStatus::NotEnoughSpaceAvailable);
    sStorageMemoryFull = true;
  }
}

Ion::Storage::Record GlobalContext::RecordWithName(const char * name) {
  const char * extensions[2] = {expExtension, funcExtension/*, seqExtension*/};
  return Ion::Storage::sharedStorage()->recordBaseNamedWithExtensions(name, extensions, 2);
}

const char * GlobalContext::ExtensionForExpression(const Expression & exp) {
  // TODO Once we have matrix exact reducing, add a .mat extension and check here if the root is a matrix
  // TODO .seq
  if (exp.type() == ExpressionNode::Type::Function) {
    return funcExtension;
  }
  return expExtension;
}

const Expression GlobalContext::expressionForSymbolAndRecord(const SymbolAbstract & symbol, Ion::Storage::Record r) {
  if (symbol.type() == ExpressionNode::Type::Symbol) {
    expressionForActualSymbol(symbol, r);
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

}
