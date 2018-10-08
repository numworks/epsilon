#include <poincare/global_context.h>
#include <poincare/helpers.h>
#include <poincare/undefined.h>
#include <poincare/preferences.h>
#include <assert.h>
#include <cmath>
#include <ion.h>

namespace Poincare {

constexpr char GlobalContext::expExtension[];
constexpr char GlobalContext::funcExtension[];
//constexpr char GlobalContext::seqExtension[];

/* Storage memory full */

static bool sStorageMemoryFull = false;

bool GlobalContext::storageMemoryFull() {
  return sStorageMemoryFull;
}

/**/

const Expression GlobalContext::expressionForSymbol(const SymbolAbstract & symbol) {
  if (symbol.type() == ExpressionNode::Type::Symbol) {
    // Constant symbols
    Symbol s = static_cast<const Symbol &>(symbol);
    if (s.isPi()) {
      return Float<double>(M_PI);
    }
    if (s.isExponential()) {
      return Float<double>(M_E);
    }
  }
  // Look up the file system for symbol
  return Expression::ExpressionFromRecord(RecordWithName(symbol.name()));
}

void GlobalContext::setExpressionForSymbol(const Expression & expression, const SymbolAbstract & symbol, Context & context) {
  sStorageMemoryFull = false;
  /* If the new expression contains the symbol, replace it because it will be
   * destroyed afterwards (to be able to do A+2->A) */
  Ion::Storage::Record record = RecordWithName(symbol.name());
  Expression e = Expression::ExpressionFromRecord(record);
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

}
