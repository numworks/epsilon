#include "global_context.h"

#include <apps/apps_container.h>
#include <assert.h>
#include <poincare/function.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/symbol.h>
#include <poincare/undefined.h>

#include "continuous_function.h"
#include "continuous_function_store.h"
#include "expression_display_permissions.h"
#include "poincare_helpers.h"
#include "sequence.h"
#include "sequence_context.h"

using namespace Poincare;

namespace Shared {

constexpr const char *GlobalContext::k_extensions[];

OMG::GlobalBox<SequenceStore> GlobalContext::sequenceStore;
OMG::GlobalBox<ContinuousFunctionStore> GlobalContext::continuousFunctionStore;

void GlobalContext::storageDidChangeForRecord(Ion::Storage::Record record) {
  m_sequenceContext.resetCache();
  GlobalContext::sequenceStore->storageDidChangeForRecord(record);
  GlobalContext::continuousFunctionStore->storageDidChangeForRecord(record);
}

bool GlobalContext::SymbolAbstractNameIsFree(const char *baseName) {
  return SymbolAbstractRecordWithBaseName(baseName).isNull();
}

const Layout GlobalContext::LayoutForRecord(Ion::Storage::Record record) {
  assert(!record.isNull());
  if (record.hasExtension(Ion::Storage::expExtension) ||
      record.hasExtension(Ion::Storage::lisExtension) ||
      record.hasExtension(Ion::Storage::matExtension)) {
    return PoincareHelpers::CreateLayout(
        ExpressionForActualSymbol(record),
        AppsContainer::activeApp()->localContext());
  } else if (record.hasExtension(Ion::Storage::funcExtension)) {
    return PoincareHelpers::CreateLayout(
        ExpressionForFunction(
            Symbol::Builder(ContinuousFunction(record).symbol()), record),
        AppsContainer::activeApp()->localContext());
  } else if (record.hasExtension(Ion::Storage::regExtension)) {
    return PoincareHelpers::CreateLayout(
        ExpressionForFunction(Symbol::Builder('x'), record),
        AppsContainer::activeApp()->localContext());
  } else {
    assert(record.hasExtension(Ion::Storage::seqExtension));
    return Sequence(record).layout();
  }
}

void GlobalContext::DestroyRecordsBaseNamedWithoutExtension(
    const char *baseName, const char *extension) {
  for (int i = 0; i < k_numberOfExtensions; i++) {
    if (strcmp(k_extensions[i], extension) != 0) {
      Ion::Storage::FileSystem::sharedFileSystem
          ->recordBaseNamedWithExtension(baseName, k_extensions[i])
          .destroy();
    }
  }
}

Context::SymbolAbstractType GlobalContext::expressionTypeForIdentifier(
    const char *identifier, int length) {
  const char *extension =
      Ion::Storage::FileSystem::sharedFileSystem
          ->extensionOfRecordBaseNamedWithExtensions(
              identifier, length, k_extensions, k_numberOfExtensions);
  if (extension == nullptr) {
    return Context::SymbolAbstractType::None;
  } else if (strcmp(extension, Ion::Storage::expExtension) == 0 ||
             strcmp(extension, Ion::Storage::matExtension) == 0) {
    return Context::SymbolAbstractType::Symbol;
  } else if (strcmp(extension, Ion::Storage::funcExtension) == 0 ||
             strcmp(extension, Ion::Storage::regExtension) == 0) {
    return Context::SymbolAbstractType::Function;
  } else if (strcmp(extension, Ion::Storage::lisExtension) == 0) {
    return Context::SymbolAbstractType::List;
  } else {
    assert(strcmp(extension, Ion::Storage::seqExtension) == 0);
    return Context::SymbolAbstractType::Sequence;
  }
}

const Expression GlobalContext::protectedExpressionForSymbolAbstract(
    const Poincare::SymbolAbstract &symbol, bool clone,
    Poincare::ContextWithParent *lastDescendantContext) {
  Ion::Storage::Record r = SymbolAbstractRecordWithBaseName(symbol.name());
  return expressionForSymbolAndRecord(
      symbol, r,
      lastDescendantContext ? static_cast<Context *>(lastDescendantContext)
                            : static_cast<Context *>(this));
}

bool GlobalContext::setExpressionForSymbolAbstract(
    const Expression &expression, const SymbolAbstract &symbol) {
  /* If the new expression contains the symbol, replace it because it will be
   * destroyed afterwards (to be able to do A+2->A) */
  Ion::Storage::Record record = SymbolAbstractRecordWithBaseName(symbol.name());
  Expression e = expressionForSymbolAndRecord(symbol, record, this);
  if (e.isUninitialized()) {
    e = Undefined::Builder();
  }
  Expression finalExpression =
      expression.clone().replaceSymbolWithExpression(symbol, e);

  // Set the expression in the storage depending on the symbol type
  if (symbol.type() == ExpressionNode::Type::Symbol) {
    return setExpressionForActualSymbol(finalExpression, symbol, record) ==
           Ion::Storage::Record::ErrorStatus::None;
  }
  assert(symbol.type() == ExpressionNode::Type::Function &&
         symbol.childAtIndex(0).type() == ExpressionNode::Type::Symbol);
  Expression childSymbol = symbol.childAtIndex(0);
  finalExpression = finalExpression.replaceSymbolWithExpression(
      static_cast<const Symbol &>(childSymbol),
      Symbol::Builder(UCodePointUnknown));
  if (!(childSymbol.isIdenticalTo(
            Symbol::Builder(ContinuousFunction::k_cartesianSymbol)) ||
        childSymbol.isIdenticalTo(
            Symbol::Builder(ContinuousFunction::k_parametricSymbol)))) {
    // Unsupported symbol. Fall back to the default cartesain function symbol
    Expression symbolInX = symbol.clone();
    symbolInX.replaceChildAtIndexInPlace(
        0, Symbol::Builder(ContinuousFunction::k_cartesianSymbol));
    return setExpressionForFunction(
               finalExpression, static_cast<const SymbolAbstract &>(symbolInX),
               record) == Ion::Storage::Record::ErrorStatus::None;
  }
  return setExpressionForFunction(finalExpression, symbol, record) ==
         Ion::Storage::Record::ErrorStatus::None;
}

const Expression GlobalContext::expressionForSymbolAndRecord(
    const SymbolAbstract &symbol, Ion::Storage::Record r, Context *ctx) {
  if (symbol.type() == ExpressionNode::Type::Symbol) {
    return ExpressionForActualSymbol(r);
  } else if (symbol.type() == ExpressionNode::Type::Function) {
    return ExpressionForFunction(symbol.childAtIndex(0), r);
  }
  assert(symbol.type() == ExpressionNode::Type::Sequence);
  return expressionForSequence(symbol, r, ctx);
}

const Expression GlobalContext::ExpressionForActualSymbol(
    Ion::Storage::Record r) {
  if (!r.hasExtension(Ion::Storage::expExtension) &&
      !r.hasExtension(Ion::Storage::lisExtension) &&
      !r.hasExtension(Ion::Storage::matExtension)) {
    return Expression();
  }
  // An expression record value is the expression itself
  Ion::Storage::Record::Data d = r.value();
  return Expression::ExpressionFromAddress(d.buffer, d.size);
}

const Expression GlobalContext::ExpressionForFunction(
    const Expression &parameter, Ion::Storage::Record r) {
  Expression e;
  if (r.hasExtension(Ion::Storage::regExtension)) {
    // A regression record value is the expression itself
    Ion::Storage::Record::Data d = r.value();
    e = Expression::ExpressionFromAddress(d.buffer, d.size);
  } else if (r.hasExtension(Ion::Storage::funcExtension)) {
    /* A function record value has metadata before the expression. To get the
     * expression, use the function record handle. */
    e = ContinuousFunction(r).expressionClone();
  }
  if (!e.isUninitialized()) {
    e = e.replaceSymbolWithExpression(Symbol::Builder(UCodePointUnknown),
                                      parameter);
  }
  return e;
}

const Expression GlobalContext::expressionForSequence(
    const SymbolAbstract &symbol, Ion::Storage::Record r, Context *ctx) {
  if (!r.hasExtension(Ion::Storage::seqExtension)) {
    return Expression();
  }
  /* An function record value has metadata before the expression. To get the
   * expression, use the function record handle. */
  Sequence seq(r);
  Expression rank = symbol.childAtIndex(0).clone();
  bool rankIsInteger = false;
  PoincareHelpers::CloneAndSimplify(&rank, ctx,
                                    ReductionTarget::SystemForApproximation);
  double rankValue = PoincareHelpers::ApproximateToScalar<double>(rank, ctx);
  if (rank.type() == ExpressionNode::Type::Rational) {
    Rational n = static_cast<Rational &>(rank);
    rankIsInteger = n.isInteger();
  } else if (!std::isnan(rankValue)) {
    /* WARNING:
     * in some edge cases, because of quantification, we have
     * floor(x) = x while x is not integer.*/
    rankIsInteger = std::floor(rankValue) == rankValue;
  }
  if (rankIsInteger) {
    return Float<double>::Builder(
        seq.evaluateXYAtParameter(rankValue, sequenceContext()).x2());
  }
  return Float<double>::Builder(NAN);
}

Ion::Storage::Record::ErrorStatus GlobalContext::setExpressionForActualSymbol(
    const Expression &expression, const SymbolAbstract &symbol,
    Ion::Storage::Record previousRecord) {
  Expression expressionToStore = expression;
  PoincareHelpers::CloneAndSimplify(
      &expressionToStore, this, ReductionTarget::User,
      SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined);
  /* "approximateKeepingUnits" is called because the expression might contain
   * units, and juste calling "approximate" would return undef
   */
  Poincare::Preferences *preferences = Poincare::Preferences::sharedPreferences;
  Poincare::Preferences::ComplexFormat complexFormat =
      Poincare::Preferences::UpdatedComplexFormatWithExpressionInput(
          preferences->complexFormat(), expressionToStore, this);
  Expression approximation = expressionToStore.approximateKeepingUnits<double>(
      Poincare::ReductionContext(
          this, complexFormat, preferences->angleUnit(),
          GlobalPreferences::sharedGlobalPreferences->unitFormat(),
          ReductionTarget::User,
          SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined,
          Poincare::UnitConversion::Default));
  // Do not store exact derivative, etc.
  if (ExpressionDisplayPermissions::ShouldOnlyDisplayApproximation(
          expression, expressionToStore, approximation, this)) {
    expressionToStore = approximation;
  }
  ExpressionNode::Type type = expressionToStore.type();
  const char *extension;
  if (type == ExpressionNode::Type::List) {
    extension = Ion::Storage::lisExtension;
  } else if (type == ExpressionNode::Type::Matrix) {
    extension = Ion::Storage::matExtension;
  } else {
    extension = Ion::Storage::expExtension;
  }
  /* If there is another record competing with this one for its name,
   * it is destroyed directly in Storage, through the record_name_verifier. */
  return Ion::Storage::FileSystem::sharedFileSystem->createRecordWithExtension(
      symbol.name(), extension, expressionToStore.addressInPool(),
      expressionToStore.size(), true);
}

Ion::Storage::Record::ErrorStatus GlobalContext::setExpressionForFunction(
    const Expression &expressionToStore, const SymbolAbstract &symbol,
    Ion::Storage::Record previousRecord) {
  Ion::Storage::Record recordToSet = previousRecord;
  Ion::Storage::Record::ErrorStatus error =
      Ion::Storage::Record::ErrorStatus::None;
  if (!previousRecord.hasExtension(Ion::Storage::funcExtension)) {
    // The previous record was not a function. Create a new model.
    ContinuousFunction newModel =
        ContinuousFunction::NewModel(&error, symbol.name());
    if (error != Ion::Storage::Record::ErrorStatus::None) {
      return error;
    }
    recordToSet = newModel;
  }
  Poincare::Expression equation = Poincare::Comparison::Builder(
      symbol.clone(), ComparisonNode::OperatorType::Equal, expressionToStore);
  ContinuousFunction model = ContinuousFunction(recordToSet);
  bool wasCartesian = model.properties().isCartesian();
  error = model.setExpressionContent(equation);
  if (error == Ion::Storage::Record::ErrorStatus::None) {
    model.updateModel(this, wasCartesian);
  }
  return error;
}

Ion::Storage::Record GlobalContext::SymbolAbstractRecordWithBaseName(
    const char *name) {
  return Ion::Storage::FileSystem::sharedFileSystem
      ->recordBaseNamedWithExtensions(name, k_extensions, k_numberOfExtensions);
}

void GlobalContext::tidyDownstreamPoolFrom(char *treePoolCursor) {
  sequenceStore->tidyDownstreamPoolFrom(treePoolCursor);
}

}  // namespace Shared
