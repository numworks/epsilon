#include "global_context.h"

#include <apps/apps_container.h>
#include <assert.h>
#include <omg/utf8_helper.h>
#include <poincare/cas.h>
#include <poincare/code_points.h>
#include <poincare/expression.h>
#include <poincare/helpers/symbol.h>
#include <poincare/k_tree.h>
#include <poincare/src/expression/symbol.h>

#include "continuous_function.h"
#include "continuous_function_store.h"
#include "function_name_helper.h"
#include "poincare_helpers.h"
#include "sequence.h"
#include "sequence_context.h"

using namespace Poincare;

namespace Shared {

constexpr const char* GlobalContext::k_extensions[];

OMG::GlobalBox<SequenceStore> GlobalContext::s_sequenceStore;
OMG::GlobalBox<SequenceCache> GlobalContext::s_sequenceCache;
OMG::GlobalBox<ContinuousFunctionStore>
    GlobalContext::s_continuousFunctionStore;

void GlobalContext::storageDidChangeForRecord(Ion::Storage::Record record) {
  m_sequenceContext.resetCache();
  GlobalContext::s_sequenceStore->storageDidChangeForRecord(record);
  GlobalContext::s_continuousFunctionStore->storageDidChangeForRecord(record);
}

bool GlobalContext::UserNameIsFree(const char* baseName) {
  return UserNamedRecordWithBaseName(baseName).isNull();
}

const Layout GlobalContext::LayoutForRecord(Ion::Storage::Record record) {
  assert(!record.isNull());
  Context* context = Escher::App::app()->localContext();
  if (record.hasExtension(Ion::Storage::expressionExtension) ||
      record.hasExtension(Ion::Storage::listExtension) ||
      record.hasExtension(Ion::Storage::matrixExtension)) {
    return PoincareHelpers::CreateLayout(
        Expression::Builder(ExpressionForUserSymbol(record)), context);
  } else if (record.hasExtension(Ion::Storage::functionExtension) ||
             record.hasExtension(Ion::Storage::parametricComponentExtension) ||
             record.hasExtension(Ion::Storage::regressionExtension)) {
    CodePoint symbol = UCodePointNull;
    if (record.hasExtension(Ion::Storage::functionExtension)) {
      symbol = GlobalContext::s_continuousFunctionStore->modelForRecord(record)
                   ->symbol();
    } else if (record.hasExtension(
                   Ion::Storage::parametricComponentExtension)) {
      symbol = CodePoints::k_parametricSymbol;
    } else {
      assert(record.hasExtension(Ion::Storage::regressionExtension));
      symbol = CodePoints::k_cartesianSymbol;
    }
    UserExpression expression =
        Expression::Builder(ExpressionForUserFunction(record));
    expression.replaceUnknownWithSymbol(symbol);
    return PoincareHelpers::CreateLayout(expression, context);
  } else {
    assert(record.hasExtension(Ion::Storage::sequenceExtension));
    return Sequence(record).layout();
  }
}

void GlobalContext::DestroyRecordsBaseNamedWithoutExtension(
    const char* baseName, const char* extension) {
  for (int i = 0; i < k_numberOfExtensions; i++) {
    if (strcmp(k_extensions[i], extension) != 0) {
      Ion::Storage::FileSystem::sharedFileSystem
          ->recordBaseNamedWithExtension(baseName, k_extensions[i])
          .destroy();
    }
  }
}

Context::UserNamedType GlobalContext::expressionTypeForIdentifier(
    const char* identifier, int length) {
  const char* extension =
      Ion::Storage::FileSystem::sharedFileSystem
          ->extensionOfRecordBaseNamedWithExtensions(
              identifier, length, k_extensions, k_numberOfExtensions);
  if (extension == nullptr) {
    return Context::UserNamedType::None;
  } else if (strcmp(extension, Ion::Storage::expressionExtension) == 0 ||
             strcmp(extension, Ion::Storage::matrixExtension) == 0) {
    return Context::UserNamedType::Symbol;
  } else if (strcmp(extension, Ion::Storage::functionExtension) == 0 ||
             strcmp(extension, Ion::Storage::parametricComponentExtension) ==
                 0 ||
             strcmp(extension, Ion::Storage::regressionExtension) == 0) {
    return Context::UserNamedType::Function;
  } else if (strcmp(extension, Ion::Storage::listExtension) == 0) {
    return Context::UserNamedType::List;
  } else {
    assert(strcmp(extension, Ion::Storage::sequenceExtension) == 0);
    return Context::UserNamedType::Sequence;
  }
}

const Internal::Tree* GlobalContext::expressionForUserNamed(
    const Internal::Tree* symbol) {
  assert(symbol->isUserSymbol() || symbol->isUserFunction());
  Ion::Storage::Record r =
      UserNamedRecordWithBaseName(Internal::Symbol::GetName(symbol));
  return expressionForSymbolAndRecord(symbol, r);
}

bool GlobalContext::setExpressionForUserNamed(
    const Internal::Tree* expressionTree, const Internal::Tree* symbolTree) {
  assert(symbolTree->isUserNamed());
  UserExpression expression = UserExpression::Builder(expressionTree);
  UserExpression symbol = UserExpression::Builder(symbolTree);
  /* If the new expression contains the symbol, replace it because it will be
   * destroyed afterwards (to be able to do A+2->A) */
  Ion::Storage::Record record =
      UserNamedRecordWithBaseName(SymbolHelper::GetName(symbol));
  UserExpression e =
      UserExpression::Builder(expressionForSymbolAndRecord(symbolTree, record));
  if (e.isUninitialized()) {
    e = Undefined::Builder();
  }
  UserExpression finalExpression = expression.clone();
  finalExpression.replaceSymbolWithExpression(symbol, e);

  // Set the expression in the storage depending on the symbol type
  if (symbol.isUserSymbol()) {
    return setExpressionForUserSymbol(finalExpression,
                                      SymbolHelper::GetName(symbol), record) ==
           Ion::Storage::Record::ErrorStatus::None;
  }
  const UserExpression childSymbol = symbol.cloneChildAtIndex(0);
  assert(symbol.isUserFunction() && childSymbol.isUserSymbol());
  finalExpression.replaceSymbolWithUnknown(childSymbol);
  UserExpression symbolToStore = symbol;
  if (!(SymbolHelper::IsSymbol(childSymbol, CodePoints::k_cartesianSymbol) ||
        SymbolHelper::IsSymbol(childSymbol, CodePoints::k_polarSymbol) ||
        SymbolHelper::IsSymbol(childSymbol, CodePoints::k_parametricSymbol))) {
    // Unsupported symbol. Fall back to the default cartesian function symbol
    symbolToStore = Poincare::SymbolHelper::BuildFunction(
        SymbolHelper::GetName(symbolToStore),
        SymbolHelper::BuildSymbol(CodePoints::k_cartesianSymbol));
  }
  return setExpressionForUserFunction(finalExpression, symbolToStore, record) ==
         Ion::Storage::Record::ErrorStatus::None;
}

const Internal::Tree* GlobalContext::expressionForSymbolAndRecord(
    const Internal::Tree* symbol, Ion::Storage::Record r) {
  assert(symbol->isUserSymbol() || symbol->isUserFunction());
  return symbol->isUserSymbol() ? ExpressionForUserSymbol(r)
                                : ExpressionForUserFunction(r);
}

const Internal::Tree* GlobalContext::ExpressionForUserSymbol(
    Ion::Storage::Record r) {
  if (!r.hasExtension(Ion::Storage::expressionExtension) &&
      !r.hasExtension(Ion::Storage::listExtension) &&
      !r.hasExtension(Ion::Storage::matrixExtension)) {
    return UserExpression();
  }
  // An expression record value is the expression itself
  Ion::Storage::Record::Data d = r.value();
  return UserExpression::TreeFromAddress(d.buffer);
}

const Internal::Tree* GlobalContext::ExpressionForUserFunction(
    Ion::Storage::Record r) {
  if (r.hasExtension(Ion::Storage::parametricComponentExtension) ||
      r.hasExtension(Ion::Storage::regressionExtension)) {
    // A regression record value is the expression itself
    Ion::Storage::Record::Data d = r.value();
    return UserExpression::TreeFromAddress(d.buffer);
  } else if (r.hasExtension(Ion::Storage::functionExtension)) {
    /* A function record value has metadata before the expression. To get the
     * expression, use the function record handle. */
    return ContinuousFunction(r).expressionTree();
  }
  return nullptr;
}

Ion::Storage::Record::ErrorStatus GlobalContext::setExpressionForUserSymbol(
    UserExpression& expression, const char* name,
    Ion::Storage::Record previousRecord) {
  bool storeApproximation = CAS::NeverDisplayReductionOfInput(expression, this);
  PoincareHelpers::ReductionParameters params = {
      .symbolicComputation = SymbolicComputation::ReplaceAllSymbols};
  bool reductionFailure = false;
  PoincareHelpers::CloneAndSimplify(&expression, this, params,
                                    &reductionFailure);
  /* "approximateKeepingUnits" is called because the expression might contain
   * units, and just calling "approximate" would return undef */
  UserExpression approximation =
      PoincareHelpers::ApproximateKeepingUnits<double>(expression, this,
                                                       params);
  // Do not store exact derivative, etc.
  if (reductionFailure || storeApproximation ||
      CAS::ShouldOnlyDisplayApproximation(UserExpression(), expression,
                                          approximation, this)) {
    expression = approximation;
  }
  const char* extension;
  if (expression.isList()) {
    extension = Ion::Storage::listExtension;
  } else if (expression.isMatrix()) {
    extension = Ion::Storage::matrixExtension;
  } else {
    extension = Ion::Storage::expressionExtension;
  }
  /* If there is another record competing with this one for its name,
   * it is destroyed directly in Storage, through the record_name_verifier. */
  return Ion::Storage::FileSystem::sharedFileSystem->createRecordWithExtension(
      name, extension, expression.addressInPool(), expression.size(), true);
}

Ion::Storage::Record::ErrorStatus GlobalContext::setExpressionForUserFunction(
    const UserExpression& expressionToStore, const UserExpression& symbol,
    Ion::Storage::Record previousRecord) {
  assert(symbol.isUserFunction());
  Ion::Storage::Record recordToSet = previousRecord;
  Ion::Storage::Record::ErrorStatus error =
      Ion::Storage::Record::ErrorStatus::None;
  if (previousRecord.hasExtension(Ion::Storage::functionExtension)) {
    GlobalContext::DeleteParametricComponentsOfRecord(recordToSet);
  } else {
    // The previous record was not a function. Create a new model.
    ContinuousFunction newModel = s_continuousFunctionStore->newModel(
        SymbolHelper::GetName(symbol), &error);
    if (error != Ion::Storage::Record::ErrorStatus::None) {
      return error;
    }
    recordToSet = newModel;
  }
  Poincare::UserExpression equation = Poincare::UserExpression::Create(
      KEqual(KA, KB), {.KA = symbol, .KB = expressionToStore});
  ExpiringPointer<ContinuousFunction> f =
      GlobalContext::s_continuousFunctionStore->modelForRecord(recordToSet);
  // TODO: factorize with ContinuousFunction::setContent
  bool wasCartesian = f->properties().isCartesian();
  error = f->setExpressionContent(equation);
  if (error == Ion::Storage::Record::ErrorStatus::None) {
    f->updateModel(this, wasCartesian);
  }
  GlobalContext::StoreParametricComponentsOfRecord(recordToSet);
  return error;
}

Ion::Storage::Record GlobalContext::UserNamedRecordWithBaseName(
    const char* name) {
  return Ion::Storage::FileSystem::sharedFileSystem
      ->recordBaseNamedWithExtensions(name, k_extensions, k_numberOfExtensions);
}

void GlobalContext::tidyStores() {
  s_sequenceStore->tidyDownstreamPoolFrom();
  s_continuousFunctionStore->tidyDownstreamPoolFrom();
}

void GlobalContext::prepareForNewApp() {
  s_sequenceStore->setStorageChangeFlag(false);
  s_continuousFunctionStore->setStorageChangeFlag(false);
}

void GlobalContext::reset() {
  s_sequenceStore->reset();
  s_continuousFunctionStore->reset();
}

// Parametric components

static void deleteParametricComponent(char* baseName, size_t baseNameLength,
                                      size_t bufferSize, bool first) {
  FunctionNameHelper::AddSuffixForParametricComponent(baseName, baseNameLength,
                                                      bufferSize, first);
  Ion::Storage::Record record =
      Ion::Storage::FileSystem::sharedFileSystem->recordBaseNamedWithExtension(
          baseName, Ion::Storage::parametricComponentExtension);
  record.destroy();
}

void GlobalContext::DeleteParametricComponentsWithBaseName(
    char* baseName, size_t baseNameLength, size_t bufferSize) {
  deleteParametricComponent(baseName, baseNameLength, bufferSize, true);
  deleteParametricComponent(baseName, baseNameLength, bufferSize, false);
}

void GlobalContext::DeleteParametricComponentsOfRecord(
    Ion::Storage::Record record) {
  ExpiringPointer<ContinuousFunction> f =
      GlobalContext::s_continuousFunctionStore->modelForRecord(record);
  if (!f->properties().isEnabledParametric()) {
    return;
  }
  constexpr size_t bufferSize = SymbolHelper::k_maxNameSize;
  char buffer[bufferSize];
  size_t length = f->name(buffer, bufferSize);
  DeleteParametricComponentsWithBaseName(buffer, length, bufferSize);
}

static void storeParametricComponent(char* baseName, size_t baseNameLength,
                                     size_t bufferSize, const UserExpression& e,
                                     bool first) {
  assert(!e.isUninitialized() && e.isPoint());
  UserExpression child = e.cloneChildAtIndex(first ? 0 : 1);
  FunctionNameHelper::AddSuffixForParametricComponent(baseName, baseNameLength,
                                                      bufferSize, first);
  Ion::Storage::FileSystem::sharedFileSystem->createRecordWithExtension(
      baseName, Ion::Storage::parametricComponentExtension,
      child.addressInPool(), child.size(), true);
}

void GlobalContext::StoreParametricComponentsOfRecord(
    Ion::Storage::Record record) {
  ExpiringPointer<ContinuousFunction> f =
      GlobalContext::s_continuousFunctionStore->modelForRecord(record);
  if (!f->properties().isEnabledParametric()) {
    return;
  }
  UserExpression e = f->expressionClone();
  if (!e.isPoint()) {
    // For example: g(t)=f'(t) or g(t)=diff(f(t),t,t)
    return;
  }
  constexpr size_t bufferSize = SymbolHelper::k_maxNameSize;
  char buffer[bufferSize];
  size_t length = f->name(buffer, bufferSize);
  assert(FunctionNameHelper::ParametricComponentsNamesAreFree(buffer, length,
                                                              bufferSize));
  storeParametricComponent(buffer, length, bufferSize, e, true);
  storeParametricComponent(buffer, length, bufferSize, e, false);
}

double GlobalContext::approximateSequenceAtRank(const char* identifier,
                                                int rank) const {
  int index = s_sequenceStore->SequenceIndexForName(identifier[0]);
  Sequence* sequence = m_sequenceContext.sequenceAtNameIndex(index);
  if (sequence == nullptr) {
    return NAN;
  }
  double result = s_sequenceCache->storedValueOfSequenceAtRank(index, rank);
  if (OMG::IsSignalingNan(result)) {
    // compute value if not in cache
    result = sequence->approximateAtRank(
        rank, s_sequenceCache,
        const_cast<SequenceContext*>(&m_sequenceContext));
  }
  return result;
}

}  // namespace Shared
