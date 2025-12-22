#include "global_store.h"

#include <apps/apps_container.h>
#include <assert.h>
#include <omg/utf8_helper.h>
#include <poincare/cas.h>
#include <poincare/code_points.h>
#include <poincare/helpers/symbol_pool.h>
#include <poincare/k_tree.h>
#include <poincare/user_expression.h>

#include "continuous_function.h"
#include "continuous_function_store.h"
#include "function_name_helper.h"
#include "poincare_helpers.h"
#include "sequence.h"
#include "sequence_context.h"

using namespace Poincare;

namespace Shared {

constexpr const char* GlobalStore::k_extensions[];

OMG::GlobalBox<SequenceStore> GlobalStore::s_sequenceStore;
OMG::GlobalBox<SequenceCache> GlobalStore::s_sequenceCache;
OMG::GlobalBox<ContinuousFunctionStore> GlobalStore::s_continuousFunctionStore;

void GlobalStore::Init() {
  s_sequenceStore.init();
  s_sequenceCache.init(s_sequenceStore.get());
  s_continuousFunctionStore.init();
}

void GlobalStore::storageDidChangeForRecord(Ion::Storage::Record record) {
  m_sequenceContext.resetCache();
  GlobalStore::s_sequenceStore->storageDidChangeForRecord(record);
  GlobalStore::s_continuousFunctionStore->storageDidChangeForRecord(record);
}

bool GlobalStore::UserNameIsFree(const char* baseName) {
  return UserNamedRecordWithBaseName(baseName).isNull();
}

Layout GlobalStore::LayoutForRecord(Ion::Storage::Record record) {
  assert(!record.isNull());
  if (record.hasExtension(Ion::Storage::expressionExtension) ||
      record.hasExtension(Ion::Storage::listExtension) ||
      record.hasExtension(Ion::Storage::matrixExtension)) {
    return PoincareHelpers::CreateLayout(
        UserExpression::Builder(ExpressionForUserSymbol(record)),
        Escher::App::app()->localContext());
  } else if (record.hasExtension(Ion::Storage::functionExtension) ||
             record.hasExtension(Ion::Storage::parametricComponentExtension) ||
             record.hasExtension(Ion::Storage::regressionExtension)) {
    CodePoint symbol = UCodePointNull;
    if (record.hasExtension(Ion::Storage::functionExtension)) {
      symbol = GlobalStore::s_continuousFunctionStore->modelForRecord(record)
                   ->symbol();
    } else if (record.hasExtension(
                   Ion::Storage::parametricComponentExtension)) {
      symbol = CodePoints::k_parametricSymbol;
    } else {
      assert(record.hasExtension(Ion::Storage::regressionExtension));
      symbol = CodePoints::k_cartesianSymbol;
    }
    UserExpression expression =
        UserExpression::Builder(ExpressionForUserFunction(record))
            .cloneAndReplaceUnknownWithSymbol(symbol);
    return PoincareHelpers::CreateLayout(expression,
                                         Escher::App::app()->localContext());
  } else {
    assert(record.hasExtension(Ion::Storage::sequenceExtension));
    return Sequence(record).layout();
  }
}

void GlobalStore::DestroyRecordsBaseNamedWithoutExtension(
    const char* baseName, const char* extension) {
  for (int i = 0; i < k_numberOfExtensions; i++) {
    if (strcmp(k_extensions[i], extension) != 0) {
      Ion::Storage::FileSystem::sharedFileSystem
          ->recordBaseNamedWithExtension(baseName, k_extensions[i])
          .destroy();
    }
  }
}

SymbolContext::UserNamedType GlobalStore::expressionTypeForIdentifier(
    std::string_view identifier) const {
  const char* extension = Ion::Storage::FileSystem::sharedFileSystem
                              ->extensionOfRecordBaseNamedWithExtensions(
                                  identifier.data(), identifier.length(),
                                  k_extensions, k_numberOfExtensions);
  if (extension == nullptr) {
    return SymbolContext::UserNamedType::None;
  } else if (strcmp(extension, Ion::Storage::expressionExtension) == 0 ||
             strcmp(extension, Ion::Storage::matrixExtension) == 0) {
    return SymbolContext::UserNamedType::Symbol;
  } else if (strcmp(extension, Ion::Storage::functionExtension) == 0 ||
             strcmp(extension, Ion::Storage::parametricComponentExtension) ==
                 0 ||
             strcmp(extension, Ion::Storage::regressionExtension) == 0) {
    return SymbolContext::UserNamedType::Function;
  } else if (strcmp(extension, Ion::Storage::listExtension) == 0) {
    return SymbolContext::UserNamedType::List;
  } else {
    assert(strcmp(extension, Ion::Storage::sequenceExtension) == 0);
    return SymbolContext::UserNamedType::Sequence;
  }
}

const Internal::Tree* GlobalStore::expressionForUserNamed(
    const Internal::Tree* symbol) const {
  assert(symbol->isUserSymbol() || symbol->isUserFunction());
  Ion::Storage::Record r =
      UserNamedRecordWithBaseName(SymbolHelper::GetName(symbol));
  return expressionForSymbolAndRecord(symbol, r);
}

bool GlobalStore::setExpressionForUserNamed(
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
    e = UserExpression::Undefined();
  }
  UserExpression finalExpression =
      expression.cloneAndReplaceSymbolWithExpression(symbol, e);

  // Set the expression in the storage depending on the symbol type
  if (symbol.isUserSymbol()) {
    return setExpressionForUserSymbol(finalExpression,
                                      SymbolHelper::GetName(symbol), record) ==
           Ion::Storage::Record::ErrorStatus::None;
  }
  const UserExpression childSymbol = symbol.cloneChildAtIndex(0);
  assert(symbol.isUserFunction() && childSymbol.isUserSymbol());
  finalExpression =
      finalExpression.cloneAndReplaceSymbolWithUnknown(childSymbol);
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

const Internal::Tree* GlobalStore::expressionForSymbolAndRecord(
    const Internal::Tree* symbol, Ion::Storage::Record r) const {
  assert(symbol->isUserSymbol() || symbol->isUserFunction());
  return symbol->isUserSymbol() ? ExpressionForUserSymbol(r)
                                : ExpressionForUserFunction(r);
}

const Internal::Tree* GlobalStore::ExpressionForUserSymbol(
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

const Internal::Tree* GlobalStore::ExpressionForUserFunction(
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

Ion::Storage::Record::ErrorStatus GlobalStore::setExpressionForUserSymbol(
    UserExpression& expression, const char* name,
    Ion::Storage::Record previousRecord) {
  bool storeApproximation =
      CAS::NeverDisplayReductionOfInput(expression, *this);
  ProjectionContext projectionContext =
      PoincareHelpers::ProjectionContextForPreferences(expression, *this);
  projectionContext.m_symbolic = SymbolicComputation::ReplaceAllSymbols;
  UserExpression approximation;
  UserExpression simplifiedExpression;
  bool reductionFailure = expression.cloneAndSimplifyAndApproximate(
      &simplifiedExpression, &approximation, projectionContext);
  // Do not store exact derivative, etc.
  if (reductionFailure || storeApproximation ||
      CAS::ShouldOnlyDisplayApproximation(
          UserExpression(), simplifiedExpression, approximation, *this)) {
    simplifiedExpression = approximation;
  }
  const char* extension;
  if (simplifiedExpression.isList()) {
    extension = Ion::Storage::listExtension;
  } else if (simplifiedExpression.isMatrix()) {
    extension = Ion::Storage::matrixExtension;
  } else {
    extension = Ion::Storage::expressionExtension;
  }
  /* If there is another record competing with this one for its name,
   * it is destroyed directly in Storage, through the record_name_verifier. */
  return Ion::Storage::FileSystem::sharedFileSystem->createRecordWithExtension(
      name, extension, simplifiedExpression.addressInPool(),
      simplifiedExpression.size(), true);
}

Ion::Storage::Record::ErrorStatus GlobalStore::setExpressionForUserFunction(
    const UserExpression& expressionToStore, const UserExpression& symbol,
    Ion::Storage::Record previousRecord) {
  assert(symbol.isUserFunction());
  Ion::Storage::Record recordToSet = previousRecord;
  Ion::Storage::Record::ErrorStatus error =
      Ion::Storage::Record::ErrorStatus::None;
  if (previousRecord.hasExtension(Ion::Storage::functionExtension)) {
    GlobalStore::DeleteParametricComponentsOfRecord(recordToSet);
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
  OMG::ExpiringPointer<ContinuousFunction> f =
      GlobalStore::s_continuousFunctionStore->modelForRecord(recordToSet);
  // TODO: factorize with ContinuousFunction::setContent
  bool wasCartesian = f->properties().isCartesian();
  error = f->setExpressionContent(equation);
  if (error == Ion::Storage::Record::ErrorStatus::None) {
    f->updateModel(wasCartesian);
  }
  GlobalStore::StoreParametricComponentsOfRecord(recordToSet);
  return error;
}

Ion::Storage::Record GlobalStore::UserNamedRecordWithBaseName(
    const char* name) {
  return Ion::Storage::FileSystem::sharedFileSystem
      ->recordBaseNamedWithExtensions(name, k_extensions, k_numberOfExtensions);
}

void GlobalStore::tidyStores() {
  s_sequenceStore->tidyDownstreamPoolFrom();
  s_continuousFunctionStore->tidyCaches();
  s_continuousFunctionStore->tidyDownstreamPoolFrom();
}

void GlobalStore::prepareForNewApp() {
  s_sequenceStore->setStorageChangeFlag(false);
  s_continuousFunctionStore->setStorageChangeFlag(false);
}

void GlobalStore::reset() {
  s_sequenceStore->reset();
  s_continuousFunctionStore->reset();
}

void GlobalStore::resetAll() {
  reset();
  s_sequenceStore->tidyDownstreamPoolFrom();
  s_continuousFunctionStore->tidyDownstreamPoolFrom();
  Ion::Storage::FileSystem::sharedFileSystem->destroyAllRecords();
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

void GlobalStore::DeleteParametricComponentsWithBaseName(char* baseName,
                                                         size_t baseNameLength,
                                                         size_t bufferSize) {
  deleteParametricComponent(baseName, baseNameLength, bufferSize, true);
  deleteParametricComponent(baseName, baseNameLength, bufferSize, false);
}

void GlobalStore::DeleteParametricComponentsOfRecord(
    Ion::Storage::Record record) {
  OMG::ExpiringPointer<ContinuousFunction> f =
      GlobalStore::s_continuousFunctionStore->modelForRecord(record);
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

void GlobalStore::StoreParametricComponentsOfRecord(
    Ion::Storage::Record record) {
  OMG::ExpiringPointer<ContinuousFunction> f =
      GlobalStore::s_continuousFunctionStore->modelForRecord(record);
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

}  // namespace Shared
