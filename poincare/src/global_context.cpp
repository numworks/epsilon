#include <poincare/global_context.h>
#include <poincare/undefined.h>
#include <poincare/preferences.h>
#include <assert.h>
#include <cmath>
#include <ion.h>

namespace Poincare {

GlobalContext::FileName GlobalContext::fileNameForSymbol(const Symbol & s) const {
  FileName fn;
  fn.nameWithExtension[0] = s.name();
  constexpr char extension[] = ".exp";
  strlcpy(fn.nameWithExtension+1, extension, k_extensionSize);
  return fn;
}

const Expression GlobalContext::expressionForSymbol(const Symbol & symbol) {
  // Constant symbols
  if (symbol.name() == Ion::Charset::SmallPi) {
    return Float<double>(M_PI);
  }
  if (symbol.name() == Ion::Charset::Exponential) {
    return Float<double>(M_E);
  }
  // Look up the file system for symbol
  FileName symbolFileName = fileNameForSymbol(symbol);
  Ion::Storage::Record record = Ion::Storage::sharedStorage()->recordNamed(symbolFileName.nameWithExtension);
  if (record.isNull())  {
    // Default A-Z value: 0
    if (Symbol::isScalarSymbol(symbol.name())) {
      return Decimal(Integer(0), 0);
    }
    // No other default value
    return Expression(); // unitialized Expression
  }
  // Build Expression in the Tree Pool
  return Expression(static_cast<ExpressionNode *>(TreePool::sharedPool()->copyTreeFromAddress(record.value().buffer, record.value().size)));
}

void GlobalContext::setExpressionForSymbolName(const Expression & expression, const Symbol & symbol, Context & context) {
  // Initiate Record features
  FileName symbolFileName = fileNameForSymbol(symbol);

  // evaluate before deleting anything (to be able to evaluate A+2->A)
  Preferences * preferences = Preferences::sharedPreferences();
  Expression evaluation = expression.isUninitialized() ? Expression() : expression.approximate<double>(context, preferences->angleUnit(), preferences->complexFormat());
  // Delete any record with same name (as it is going to be override)
  Ion::Storage::sharedStorage()->recordNamed(symbolFileName.nameWithExtension).destroy();

 if (Symbol::isMatrixSymbol(symbol.name())) {
   // Matrix symbol: force evaluation to be a Matrix
    if (!evaluation.isUninitialized() && evaluation.type() != ExpressionNode::Type::Matrix) {
      evaluation = Matrix(evaluation);
    }
  } else if (Symbol::isScalarSymbol(symbol.name())) {
    // Scalar symbol: force evaluation not to be a Matrix
    if (!evaluation.isUninitialized() && evaluation.type() == ExpressionNode::Type::Matrix) {
      evaluation = Undefined();
    }
  } else {
    // Other symbols than A-Z or M0-M9 are not stored yet TODO
    evaluation = Expression();
  }
  if (!evaluation.isUninitialized()) {
    Ion::Storage::Record::ErrorStatus err = Ion::Storage::sharedStorage()->createRecord(symbolFileName.nameWithExtension, evaluation.addressInPool(), evaluation.size());
    if (err != Ion::Storage::Record::ErrorStatus::None) {
      assert(err == Ion::Storage::Record::ErrorStatus::NotEnoughSpaceAvailable);
      // TODO: return false to set flag that the file system is full?
    }
  }
}

}
