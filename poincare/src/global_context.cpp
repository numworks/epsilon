#include <poincare/global_context.h>
#include <poincare/undefined.h>
#include <poincare/preferences.h>
#include <assert.h>
#include <cmath>
#include <ion.h>

namespace Poincare {

constexpr char GlobalContext::expExtension[];
constexpr char GlobalContext::funcExtension[];
//constexpr char GlobalContext::seqExtension[];

const Expression GlobalContext::expressionForSymbol(const Symbol & symbol) {
  // Constant symbols
  if (symbol.isPi()) {
    return Float<double>(M_PI);
  }
  if (symbol.isExponential()) {
    return Float<double>(M_E);
  }
  // Look up the file system for symbol
  return ExpressionForRecord(RecordWithName(symbol.name()));
}

void GlobalContext::setExpressionForSymbolName(const Expression & expression, const char * symbolName, Context & context) {
  /* If the new expression contains the symbol, replace it because it will be
   * destroyed afterwards (to be able to do A+2->A) */
  Ion::Storage::Record record = RecordWithName(symbolName);
  Expression e = ExpressionForRecord(record);
  Expression finalExpression = expression.clone().replaceSymbolWithExpression(symbolName, e);
  // Delete any record with same name (as it is going to be overriden)
  record.destroy();

  Ion::Storage::Record::ErrorStatus err = Ion::Storage::sharedStorage()->createRecordWithExtension(symbolName, ExtensionForExpression(finalExpression), finalExpression.addressInPool(), finalExpression.size());
  if (err != Ion::Storage::Record::ErrorStatus::None) {
    assert(err == Ion::Storage::Record::ErrorStatus::NotEnoughSpaceAvailable);
    // TODO: return false to set flag that the file system is full?
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

const Expression GlobalContext::ExpressionForRecord(const Ion::Storage::Record & record) {
  if (record.isNull()) {
    return Undefined();
  }
  // Build the Expression in the Tree Pool
  return Expression(static_cast<ExpressionNode *>(TreePool::sharedPool()->copyTreeFromAddress(record.value().buffer, record.value().size)));
}

}
