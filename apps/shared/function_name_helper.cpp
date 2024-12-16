#include "function_name_helper.h"

#include <apps/shared/global_context.h>
#include <omg/utf8_helper.h>
#include <poincare/code_points.h>
#include <poincare/helpers/symbol.h>

using namespace Poincare;

namespace Shared {

namespace FunctionNameHelper {

size_t AddSuffixForParametricComponent(char* baseName, size_t baseNameLength,
                                       size_t bufferSize, bool first) {
  return UTF8Helper::WriteCodePoint(baseName + baseNameLength,
                                    bufferSize - baseNameLength,
                                    first ? 'x' : 'y');
}

size_t ParametricComponentNameWithArgument(Shared::ContinuousFunction* f,
                                           char* buffer, size_t bufferSize,
                                           bool first, int derivationOrder) {
  size_t length = f->name(buffer, bufferSize);
  length += FunctionNameHelper::AddSuffixForParametricComponent(
      buffer, length, bufferSize, first);
  if (derivationOrder > 0) {
    assert(derivationOrder == 1 || derivationOrder == 2);
    const CodePoint derivative = derivationOrder == 1 ? '\'' : '\"';
    length += UTF8Helper::WriteCodePoint(buffer + length, bufferSize - length,
                                         derivative);
  }
  length += Shared::Function::WithArgument(
      CodePoints::k_parametricSymbol, buffer + length, bufferSize - length);
  return length;
}

static bool parametricComponentNameIsFree(char* baseName, size_t baseNameLength,
                                          size_t bufferSize, bool first) {
  AddSuffixForParametricComponent(baseName, baseNameLength, bufferSize, first);
  bool isFree = GlobalContext::UserNameIsFree(baseName);
  baseName[baseNameLength] = 0;  // Remove suffix
  return isFree;
}

bool ParametricComponentsNamesAreFree(char* baseName, size_t baseNameLength,
                                      size_t bufferSize) {
  return parametricComponentNameIsFree(baseName, baseNameLength, bufferSize,
                                       true) &&
         parametricComponentNameIsFree(baseName, baseNameLength, bufferSize,
                                       false);
}

static bool functionNameIsFree(char* buffer, size_t bufferSize,
                               CodePoint symbol) {
  size_t length = strlen(buffer);
  return GlobalContext::UserNameIsFree(buffer) &&
         (symbol != CodePoints::k_parametricSymbol ||
          ParametricComponentsNamesAreFree(buffer, length, bufferSize));
}

int DefaultName(char* buffer, size_t bufferSize, CodePoint symbol) {
  constexpr int k_maxNumberOfDefaultLetterNames = 4;
  constexpr char k_defaultLetterNames[k_maxNumberOfDefaultLetterNames] = {
      'f', 'g', 'h', 'p'};
  /* First default names the first of theses names f, g, h, p and then f1, f2,
   * that does not exist yet in the storage. */
  size_t length = 0;
  if (symbol == CodePoints::k_polarSymbol) {
    // Try r1, r2, ...
    length = UTF8Helper::WriteCodePoint(buffer, bufferSize,
                                        CodePoints::k_radiusSymbol);
  } else {
    // Find the next available name
    for (size_t i = 0; i < k_maxNumberOfDefaultLetterNames; i++) {
      length = UTF8Helper::WriteCodePoint(buffer, bufferSize,
                                          k_defaultLetterNames[i]);
      if (functionNameIsFree(buffer, bufferSize, symbol)) {
        return length;
      }
    }
    // f, g, h and p are already taken. Try f1, f2, ...
    length =
        UTF8Helper::WriteCodePoint(buffer, bufferSize, k_defaultLetterNames[0]);
  }
  assert(bufferSize >= ContinuousFunction::k_maxDefaultNameSize);
  return Ion::Storage::FileSystem::sharedFileSystem
      ->firstAvailableNameFromPrefix(
          buffer, length, bufferSize,
          [](char* buffer, size_t bufferSize, void* auxiliary) {
            CodePoint* symbol = static_cast<CodePoint*>(auxiliary);
            return functionNameIsFree(buffer, bufferSize, *symbol);
          },
          &symbol);
}

bool ParametricComponentsNameError(UserExpression expression,
                                   ContinuousFunction* f) {
  /* Check that parametric components name are free if we are defining a
   * parametric function. */
  if (!ContinuousFunction::IsFunctionAssignment(expression)) {
    // The user is not defining a function
    return false;
  }
  const UserExpression function = expression.cloneChildAtIndex(0);
  const UserExpression functionSymbol = function.cloneChildAtIndex(0);
  const UserExpression point = expression.cloneChildAtIndex(1);
  if (!SymbolHelper::IsSymbol(functionSymbol, CodePoints::k_parametricSymbol) ||
      !point.isPoint()) {
    // The user is not defining a parametric function
    return false;
  }
  constexpr size_t bufferSize = SymbolHelper::k_maxNameSize;
  char functionName[bufferSize];
  assert(function.isUserFunction());
  strlcpy(functionName, SymbolHelper::GetName(function), bufferSize);
  size_t functionNameLength = strlen(functionName);
  assert(f->fullName() != nullptr);
  bool willDefineNewParametricComponents =
      GlobalContext::UserNameIsFree(functionName) ||
      (strncmp(f->fullName(), functionName, functionNameLength) == 0 &&
       !f->properties().isEnabledParametric());
  if (willDefineNewParametricComponents &&
      !FunctionNameHelper::ParametricComponentsNamesAreFree(
          functionName, functionNameLength, bufferSize)) {
    // Parametric components names are not free
    return true;
  }
  // Parametric components names are free
  return false;
}

}  // namespace FunctionNameHelper

}  // namespace Shared
