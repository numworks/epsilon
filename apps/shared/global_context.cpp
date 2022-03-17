#include "global_context.h"
#include "continuous_function.h"
#include "sequence.h"
#include "poincare_helpers.h"
#include <poincare/rational.h>
#include <poincare/equal.h>
#include <poincare/serialization_helper.h>
#include <poincare/symbol.h>
#include <poincare/function.h>
#include <poincare/undefined.h>
#include <assert.h>

using namespace Poincare;

namespace Shared {

constexpr const char * GlobalContext::k_extensions[];

SequenceStore * GlobalContext::sequenceStore() {
  static SequenceStore sequenceStore;
  return &sequenceStore;
}

bool GlobalContext::SymbolAbstractNameIsFree(const char * baseName) {
  return SymbolAbstractRecordWithBaseName(baseName).isNull();
}

const Layout GlobalContext::LayoutForRecord(Ion::Storage::Record record) {
  assert(!record.isNull());
  if (Ion::Storage::FullNameHasExtension(record.fullName(), Ion::Storage::expExtension, strlen(Ion::Storage::expExtension))
   || Ion::Storage::FullNameHasExtension(record.fullName(), Ion::Storage::lisExtension, strlen(Ion::Storage::expExtension)))
  {
    return PoincareHelpers::CreateLayout(ExpressionForActualSymbol(record));
  } else if (Ion::Storage::FullNameHasExtension(record.fullName(), Ion::Storage::funcExtension, strlen(Ion::Storage::funcExtension))) {
    return PoincareHelpers::CreateLayout(ExpressionForFunction(Symbol::Builder(ContinuousFunction(record).symbol()), record));
  } else {
    assert(Ion::Storage::FullNameHasExtension(record.fullName(), Ion::Storage::seqExtension, strlen(Ion::Storage::seqExtension)));
    return Sequence(record).layout();
  }
}

void GlobalContext::DestroyRecordsBaseNamedWithoutExtension(const char * baseName, const char * extension) {
  for (int i = 0; i < k_numberOfExtensions; i++) {
    if (strcmp(k_extensions[i], extension) != 0) {
      Ion::Storage::sharedStorage()->destroyRecordWithBaseNameAndExtension(baseName, k_extensions[i]);
    }
  }
}

Context::SymbolAbstractType GlobalContext::expressionTypeForIdentifier(const char * identifier, int length) {
  const char * extension = Ion::Storage::sharedStorage()->extensionOfRecordBaseNamedWithExtensions(identifier, length, k_extensions, k_numberOfExtensions);
  if (extension == nullptr) {
    return Context::SymbolAbstractType::None;
  } else if (extension == Ion::Storage::expExtension) {
    return Context::SymbolAbstractType::Symbol;
  } else if (extension == Ion::Storage::funcExtension) {
    return Context::SymbolAbstractType::Function;
  } else if (extension == Ion::Storage::lisExtension) {
    return Context::SymbolAbstractType::List;
  } else {
    assert(extension == Ion::Storage::seqExtension);
    return Context::SymbolAbstractType::Sequence;
  }
}

const Expression GlobalContext::expressionForSymbolAbstract(const Poincare::SymbolAbstract & symbol, bool clone, float unknownSymbolValue ) {
  Ion::Storage::Record r = SymbolAbstractRecordWithBaseName(symbol.name());
  return ExpressionForSymbolAndRecord(symbol, r, this, unknownSymbolValue);
}

void GlobalContext::setExpressionForSymbolAbstract(const Expression & expression, const SymbolAbstract & symbol) {
  /* If the new expression contains the symbol, replace it because it will be
   * destroyed afterwards (to be able to do A+2->A) */
  Ion::Storage::Record record = SymbolAbstractRecordWithBaseName(symbol.name());
  Expression e = ExpressionForSymbolAndRecord(symbol, record, this);
  if (e.isUninitialized()) {
    e = Undefined::Builder();
  }
  Expression finalExpression = expression.clone().replaceSymbolWithExpression(symbol, e);

  // Set the expression in the storage depending on the symbol type
  if (symbol.type() == ExpressionNode::Type::Symbol) {
    SetExpressionForActualSymbol(finalExpression, symbol, record, this);
  } else {
    assert(symbol.type() == ExpressionNode::Type::Function && symbol.childAtIndex(0).type() == ExpressionNode::Type::Symbol);
    Expression childSymbol = symbol.childAtIndex(0);
    finalExpression = finalExpression.replaceSymbolWithExpression(static_cast<const Symbol &>(childSymbol), Symbol::Builder(UCodePointUnknown));
    if (!(childSymbol.isIdenticalTo(Symbol::Builder(ContinuousFunction::k_cartesianSymbol)) || childSymbol.isIdenticalTo(Symbol::Builder(ContinuousFunction::k_parametricSymbol)) || childSymbol.isIdenticalTo(Symbol::Builder(ContinuousFunction::k_polarSymbol)))) {
      // Unsupported symbol. Fall back to the default cartesain function symbol
      Expression symbolInX = symbol.clone();
      symbolInX.replaceChildAtIndexInPlace(0, Symbol::Builder(ContinuousFunction::k_cartesianSymbol));
      setExpressionForFunction(finalExpression, static_cast<const SymbolAbstract&>(symbolInX), record);
    } else {
      setExpressionForFunction(finalExpression, symbol, record);
    }
  }
}

const Expression GlobalContext::ExpressionForSymbolAndRecord(const SymbolAbstract & symbol, Ion::Storage::Record r, Context * ctx, float unknownSymbolValue ) {
  if (symbol.type() == ExpressionNode::Type::Symbol) {
    return ExpressionForActualSymbol(r);
  } else if (symbol.type() == ExpressionNode::Type::Function) {
    return ExpressionForFunction(symbol.childAtIndex(0), r);
  }
  assert(symbol.type() == ExpressionNode::Type::Sequence);
  return ExpressionForSequence(symbol, r, ctx, unknownSymbolValue);
}

const Expression GlobalContext::ExpressionForActualSymbol(Ion::Storage::Record r) {
  if (!Ion::Storage::FullNameHasExtension(r.fullName(), Ion::Storage::expExtension, strlen(Ion::Storage::expExtension))
   && !Ion::Storage::FullNameHasExtension(r.fullName(), Ion::Storage::lisExtension, strlen(Ion::Storage::expExtension)))
  {
    return Expression();
  }
  // An expression record value is the expression itself
  Ion::Storage::Record::Data d = r.value();
  return Expression::ExpressionFromAddress(d.buffer, d.size);
}

const Expression GlobalContext::ExpressionForFunction(const Expression & parameter, Ion::Storage::Record r) {
  if (!Ion::Storage::FullNameHasExtension(r.fullName(), Ion::Storage::funcExtension, strlen(Ion::Storage::funcExtension))) {
    return Expression();
  }
  /* An function record value has metadata before the expression. To get the
   * expression, use the function record handle. */
  Expression e = ContinuousFunction(r).expressionClone();
  if (!e.isUninitialized()) {
    e = e.replaceSymbolWithExpression(Symbol::Builder(UCodePointUnknown), parameter);
  }
  return e;
}

const Expression GlobalContext::ExpressionForSequence(const SymbolAbstract & symbol, Ion::Storage::Record r, Context * ctx, float unknownSymbolValue) {
  if (!Ion::Storage::FullNameHasExtension(r.fullName(), Ion::Storage::seqExtension, strlen(Ion::Storage::seqExtension))) {
    return Expression();
  }
  /* An function record value has metadata before the expression. To get the
   * expression, use the function record handle. */
  Sequence seq(r);
  Expression rank = symbol.childAtIndex(0).clone();
  rank = rank.replaceSymbolWithExpression(Symbol::Builder(UCodePointUnknown), Float<float>::Builder(unknownSymbolValue));
  rank = rank.cloneAndSimplify(ExpressionNode::ReductionContext(ctx, Poincare::Preferences::sharedPreferences()->complexFormat(), Poincare::Preferences::sharedPreferences()->angleUnit(), GlobalPreferences::sharedGlobalPreferences()->unitFormat(), ExpressionNode::ReductionTarget::SystemForApproximation));
  bool rankIsInteger = false;
  double rankValue = rank.approximateToScalar<double>(ctx, Poincare::Preferences::sharedPreferences()->complexFormat(), Poincare::Preferences::sharedPreferences()->angleUnit());
  if (rank.type() == ExpressionNode::Type::Rational) {
    Rational n = static_cast<Rational &>(rank);
    rankIsInteger = n.isInteger();
  } else if (!std::isnan(unknownSymbolValue)) {
    /* If unknownSymbolValue is not nan, then we are in the graph app. In order
     * to allow functions like f(x) = u(x+0.5) to be ploted, we need to
     * approximate the rank and check if it is an integer. Unfortunatly this
     * leads to some edge cases were, because of quantification, we have
     * floor(x) = x while x is not integer.*/
  rankIsInteger = std::floor(rankValue) == rankValue;
  }
  if (rankIsInteger) {
    SequenceContext sqctx(ctx, sequenceStore());
    return Float<double>::Builder(seq.evaluateXYAtParameter(rankValue, &sqctx).x2());
  }
  return Float<double>::Builder(NAN);
}

Ion::Storage::Record::ErrorStatus GlobalContext::SetExpressionForActualSymbol(const Expression & expression, const SymbolAbstract & symbol, Ion::Storage::Record previousRecord, Context * context) {
  if (!previousRecord.isNull() && Ion::Storage::FullNameHasExtension(previousRecord.fullName(), Ion::Storage::funcExtension, strlen(Ion::Storage::funcExtension))) {
    /* A function can overwrite a variable, but a variable cannot be created if
     * it has the same name as an existing function. */
    // TODO Pop up "Name taken for a function"
    return Ion::Storage::Record::ErrorStatus::NameTaken;
  }
  // Delete any record with same name (as it is going to be overriden)
  previousRecord.destroy();
  const char * extension = expression.deepIsList(context) ? Ion::Storage::lisExtension : Ion::Storage::expExtension;
  return Ion::Storage::sharedStorage()->createRecordWithExtension(symbol.name(), extension, expression.addressInPool(), expression.size());
}

Ion::Storage::Record::ErrorStatus GlobalContext::setExpressionForFunction(const Expression & expressionToStore, const SymbolAbstract & symbol, Ion::Storage::Record previousRecord) {
  Ion::Storage::Record recordToSet = previousRecord;
  Ion::Storage::Record::ErrorStatus error = Ion::Storage::Record::ErrorStatus::None;
  if (!Ion::Storage::FullNameHasExtension(previousRecord.fullName(), Ion::Storage::funcExtension, strlen(Ion::Storage::funcExtension))) {
    // The previous record was not a function. Destroy it and create the new record.
    previousRecord.destroy();
    ContinuousFunction newModel = ContinuousFunction::NewModel(&error, symbol.name());
    if (error != Ion::Storage::Record::ErrorStatus::None) {
      return error;
    }
    recordToSet = newModel;
  }
  Poincare::Expression equation = Poincare::Equal::Builder(symbol.clone(), expressionToStore);
  ContinuousFunction model = ContinuousFunction(recordToSet);
  error = model.setExpressionContent(equation);
  model.udpateModel(this);
  return error;
}

Ion::Storage::Record GlobalContext::SymbolAbstractRecordWithBaseName(const char * name) {
  return Ion::Storage::sharedStorage()->recordBaseNamedWithExtensions(name, k_extensions, k_numberOfExtensions);
}

void GlobalContext::tidyDownstreamPoolFrom(char * treePoolCursor) {
  sequenceStore()->tidyDownstreamPoolFrom(treePoolCursor);
}

}
