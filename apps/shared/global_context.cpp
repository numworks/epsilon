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
#include "function_name_helper.h"
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
  Context *context = Escher::App::app()->localContext();
  if (record.hasExtension(Ion::Storage::expressionExtension) ||
      record.hasExtension(Ion::Storage::listExtension) ||
      record.hasExtension(Ion::Storage::matrixExtension)) {
    return PoincareHelpers::CreateLayout(ExpressionForActualSymbol(record),
                                         context);
  } else if (record.hasExtension(Ion::Storage::functionExtension) ||
             record.hasExtension(Ion::Storage::parametricComponentExtension) ||
             record.hasExtension(Ion::Storage::regressionExtension)) {
    CodePoint symbol = UCodePointNull;
    if (record.hasExtension(Ion::Storage::functionExtension)) {
      symbol = GlobalContext::continuousFunctionStore->modelForRecord(record)
                   ->symbol();
    } else if (record.hasExtension(
                   Ion::Storage::parametricComponentExtension)) {
      symbol = ContinuousFunctionProperties::k_parametricSymbol;
    } else {
      assert(record.hasExtension(Ion::Storage::regressionExtension));
      symbol = ContinuousFunctionProperties::k_cartesianSymbol;
    }
    return PoincareHelpers::CreateLayout(
        ExpressionForFunction(Symbol::Builder(symbol), record), context);
  } else {
    assert(record.hasExtension(Ion::Storage::sequenceExtension));
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
  } else if (strcmp(extension, Ion::Storage::expressionExtension) == 0 ||
             strcmp(extension, Ion::Storage::matrixExtension) == 0) {
    return Context::SymbolAbstractType::Symbol;
  } else if (strcmp(extension, Ion::Storage::functionExtension) == 0 ||
             strcmp(extension, Ion::Storage::parametricComponentExtension) ==
                 0 ||
             strcmp(extension, Ion::Storage::regressionExtension) == 0) {
    return Context::SymbolAbstractType::Function;
  } else if (strcmp(extension, Ion::Storage::listExtension) == 0) {
    return Context::SymbolAbstractType::List;
  } else {
    assert(strcmp(extension, Ion::Storage::sequenceExtension) == 0);
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
    e = JuniorUndefined::Builder();
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
      static_cast<const Symbol &>(childSymbol), Symbol::SystemSymbol());
  SymbolAbstract symbolToStore = symbol;
  if (!(childSymbol.isIdenticalTo(
            Symbol::Builder(ContinuousFunction::k_cartesianSymbol)) ||
        childSymbol.isIdenticalTo(
            Symbol::Builder(ContinuousFunction::k_polarSymbol)) ||
        childSymbol.isIdenticalTo(
            Symbol::Builder(ContinuousFunction::k_parametricSymbol)))) {
    // Unsupported symbol. Fall back to the default cartesain function symbol
    Expression symbolInX = symbol.clone();
    symbolInX.replaceChildAtIndexInPlace(
        0, Symbol::Builder(ContinuousFunction::k_cartesianSymbol));
    symbolToStore = static_cast<const SymbolAbstract &>(symbolInX);
  }
  return setExpressionForFunction(finalExpression, symbolToStore, record) ==
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
  if (!r.hasExtension(Ion::Storage::expressionExtension) &&
      !r.hasExtension(Ion::Storage::listExtension) &&
      !r.hasExtension(Ion::Storage::matrixExtension)) {
    return Expression();
  }
  // An expression record value is the expression itself
  Ion::Storage::Record::Data d = r.value();
  return Expression::ExpressionFromAddress(d.buffer, d.size);
}

const Expression GlobalContext::ExpressionForFunction(
    const Expression &parameter, Ion::Storage::Record r) {
  Expression e;
  if (r.hasExtension(Ion::Storage::parametricComponentExtension) ||
      r.hasExtension(Ion::Storage::regressionExtension)) {
    // A regression record value is the expression itself
    Ion::Storage::Record::Data d = r.value();
    e = Expression::ExpressionFromAddress(d.buffer, d.size);
  } else if (r.hasExtension(Ion::Storage::functionExtension)) {
    /* A function record value has metadata before the expression. To get the
     * expression, use the function record handle. */
    e = ContinuousFunction(r).expressionClone();
  }
  if (!e.isUninitialized()) {
    e = e.replaceSymbolWithExpression(Symbol::SystemSymbol(), parameter);
  }
  return e;
}

const Expression GlobalContext::expressionForSequence(
    const SymbolAbstract &symbol, Ion::Storage::Record r, Context *ctx) {
  if (!r.hasExtension(Ion::Storage::sequenceExtension)) {
    return Expression();
  }
  /* An function record value has metadata before the expression. To get the
   * expression, use the function record handle. */
  Sequence seq(r);
  Expression rank = symbol.childAtIndex(0).clone();
  bool rankIsInteger = false;
  PoincareHelpers::CloneAndSimplify(
      &rank, ctx, {.target = ReductionTarget::SystemForApproximation});
  double rankValue = PoincareHelpers::ApproximateToScalar<double>(rank, ctx);
  if (rank.type() == ExpressionNode::Type::Rational) {
#if 0  // TODO_PCJ
    Rational n = static_cast<Rational &>(rank);
    rankIsInteger = n.isInteger();
#else
    assert(false);
    rankIsInteger = false;
#endif
  } else if (!std::isnan(rankValue)) {
    /* WARNING:
     * in some edge cases, because of quantification, we have
     * floor(x) = x while x is not integer.*/
    rankIsInteger = std::floor(rankValue) == rankValue;
  }
  if (rankIsInteger) {
    return JuniorFloat<double>::Builder(
        seq.evaluateXYAtParameter(rankValue, sequenceContext()).y());
  }
  return JuniorFloat<double>::Builder(NAN);
}

Ion::Storage::Record::ErrorStatus GlobalContext::setExpressionForActualSymbol(
    Expression &expression, const SymbolAbstract &symbol,
    Ion::Storage::Record previousRecord) {
  bool storeApproximation =
      ExpressionDisplayPermissions::NeverDisplayReductionOfInput(expression,
                                                                 this);
  PoincareHelpers::ReductionParameters params = {
      .symbolicComputation =
          SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined};
  PoincareHelpers::CloneAndSimplify(&expression, this, params);
  /* "approximateKeepingUnits" is called because the expression might contain
   * units, and juste calling "approximate" would return undef*/

#if 0  // TODO_PCJ
  Expression approximation = PoincareHelpers::ApproximateKeepingUnits<double>(
      expression, this, params);
#else
  Expression approximation =
      PoincareHelpers::Approximate<double>(expression, this);
#endif
  // Do not store exact derivative, etc.
  if (storeApproximation ||
      ExpressionDisplayPermissions::ShouldOnlyDisplayApproximation(
          Expression(), expression, approximation, this)) {
    expression = approximation;
  }
  ExpressionNode::Type type = expression.type();
  const char *extension;
  if (type == ExpressionNode::Type::List) {
    extension = Ion::Storage::listExtension;
  } else if (type == ExpressionNode::Type::Matrix) {
    extension = Ion::Storage::matrixExtension;
  } else {
    extension = Ion::Storage::expressionExtension;
  }
  /* If there is another record competing with this one for its name,
   * it is destroyed directly in Storage, through the record_name_verifier. */
  return Ion::Storage::FileSystem::sharedFileSystem->createRecordWithExtension(
      symbol.name(), extension, expression.addressInPool(), expression.size(),
      true);
}

Ion::Storage::Record::ErrorStatus GlobalContext::setExpressionForFunction(
    const Expression &expressionToStore, const SymbolAbstract &symbol,
    Ion::Storage::Record previousRecord) {
  Ion::Storage::Record recordToSet = previousRecord;
  Ion::Storage::Record::ErrorStatus error =
      Ion::Storage::Record::ErrorStatus::None;
  if (previousRecord.hasExtension(Ion::Storage::functionExtension)) {
    GlobalContext::DeleteParametricComponentsOfRecord(recordToSet);
  } else {
    // The previous record was not a function. Create a new model.
    ContinuousFunction newModel =
        continuousFunctionStore->newModel(symbol.name(), &error);
    if (error != Ion::Storage::Record::ErrorStatus::None) {
      return error;
    }
    recordToSet = newModel;
  }
  Poincare::Expression equation = Poincare::Comparison::Builder(
      symbol.clone(), ComparisonNode::OperatorType::Equal, expressionToStore);
  ExpiringPointer<ContinuousFunction> f =
      GlobalContext::continuousFunctionStore->modelForRecord(recordToSet);
  // TODO: factorise with ContinuousFunction::setContent
  bool wasCartesian = f->properties().isCartesian();
  error = f->setExpressionContent(equation);
  if (error == Ion::Storage::Record::ErrorStatus::None) {
    f->updateModel(this, wasCartesian);
  }
  GlobalContext::StoreParametricComponentsOfRecord(recordToSet);
  return error;
}

Ion::Storage::Record GlobalContext::SymbolAbstractRecordWithBaseName(
    const char *name) {
  return Ion::Storage::FileSystem::sharedFileSystem
      ->recordBaseNamedWithExtensions(name, k_extensions, k_numberOfExtensions);
}

void GlobalContext::tidyDownstreamPoolFrom(TreeNode *treePoolCursor) {
  sequenceStore->tidyDownstreamPoolFrom(treePoolCursor);
  continuousFunctionStore->tidyDownstreamPoolFrom(treePoolCursor);
}

void GlobalContext::prepareForNewApp() {
  sequenceStore->setStorageChangeFlag(false);
  continuousFunctionStore->setStorageChangeFlag(false);
}

void GlobalContext::reset() {
  sequenceStore->reset();
  continuousFunctionStore->reset();
}

// Parametric components

static void deleteParametricComponent(char *baseName, size_t baseNameLength,
                                      size_t bufferSize, bool first) {
  FunctionNameHelper::AddSuffixForParametricComponent(baseName, baseNameLength,
                                                      bufferSize, first);
  Ion::Storage::Record record =
      Ion::Storage::FileSystem::sharedFileSystem->recordBaseNamedWithExtension(
          baseName, Ion::Storage::parametricComponentExtension);
  record.destroy();
}

void GlobalContext::DeleteParametricComponentsWithBaseName(
    char *baseName, size_t baseNameLength, size_t bufferSize) {
  deleteParametricComponent(baseName, baseNameLength, bufferSize, true);
  deleteParametricComponent(baseName, baseNameLength, bufferSize, false);
}

void GlobalContext::DeleteParametricComponentsOfRecord(
    Ion::Storage::Record record) {
  ExpiringPointer<ContinuousFunction> f =
      GlobalContext::continuousFunctionStore->modelForRecord(record);
  if (!f->properties().isEnabledParametric()) {
    return;
  }
  constexpr size_t bufferSize = SymbolAbstractNode::k_maxNameSize;
  char buffer[bufferSize];
  size_t length = f->name(buffer, bufferSize);
  DeleteParametricComponentsWithBaseName(buffer, length, bufferSize);
}

static void storeParametricComponent(char *baseName, size_t baseNameLength,
                                     size_t bufferSize, const Expression &e,
                                     bool first) {
  assert(!e.isUninitialized() && e.type() == ExpressionNode::Type::Point &&
         e.numberOfChildren() == 2);
  Expression child = e.childAtIndex(first ? 0 : 1).clone();
  FunctionNameHelper::AddSuffixForParametricComponent(baseName, baseNameLength,
                                                      bufferSize, first);
  child.storeWithNameAndExtension(baseName,
                                  Ion::Storage::parametricComponentExtension);
}

void GlobalContext::StoreParametricComponentsOfRecord(
    Ion::Storage::Record record) {
  ExpiringPointer<ContinuousFunction> f =
      GlobalContext::continuousFunctionStore->modelForRecord(record);
  if (!f->properties().isEnabledParametric()) {
    return;
  }
  Expression e = f->expressionClone();
  if (e.type() != ExpressionNode::Type::Point) {
    // For example: g(t)=f'(t) or g(t)=diff(f(t),t,t)
    return;
  }
  constexpr size_t bufferSize = SymbolAbstractNode::k_maxNameSize;
  char buffer[bufferSize];
  size_t length = f->name(buffer, bufferSize);
  assert(FunctionNameHelper::ParametricComponentsNamesAreFree(buffer, length,
                                                              bufferSize));
  storeParametricComponent(buffer, length, bufferSize, e, true);
  storeParametricComponent(buffer, length, bufferSize, e, false);
}

}  // namespace Shared
