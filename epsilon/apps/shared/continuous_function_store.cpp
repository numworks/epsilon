#include "continuous_function_store.h"

#include <escher/palette.h>
#include <escher/theme/data_colors.h>
#include <ion.h>
#include <omg/utf8_helper.h>

using namespace Escher;

namespace Shared {

void ContinuousFunctionStore::removeAll() {
  FunctionStore::removeAll();
  m_memoizedNumberOfActiveFunctions = 0;
  for (int i = 0; i < k_maxNumberOfMemoizedModels; i++) {
    m_functions[i] = ContinuousFunction();
  }
  /* m_storageCheckSum must be updated too, otherwise, if removed functions are
   * added again, m_memoizedNumberOfActiveFunctions will not be updated. */
  m_storageCheckSum = Ion::Storage::FileSystem::sharedFileSystem->checksum();
}

bool ContinuousFunctionStore::displaysOnlyCartesianFunctions(
    int* nbActiveFunctions) const {
  int nActive = numberOfActiveFunctions();
  if (nbActiveFunctions != nullptr) {
    *nbActiveFunctions = nActive;
  }
  return numberOfActiveFunctionsWithProperty(
             &ContinuousFunctionProperties::isCartesian) == nActive;
}

int ContinuousFunctionStore::numberOfActiveFunctions() const {
  uint32_t checksum = Ion::Storage::FileSystem::sharedFileSystem->checksum();
  if (m_memoizedNumberOfActiveFunctions < 0 || checksum != m_storageCheckSum) {
    m_storageCheckSum = checksum;
    m_memoizedNumberOfActiveFunctions =
        FunctionStore::numberOfActiveFunctions();
  } else {
    assert(m_memoizedNumberOfActiveFunctions ==
           FunctionStore::numberOfActiveFunctions());
  }
  return m_memoizedNumberOfActiveFunctions;
}

using ErrorStatus = Ion::Storage::Record::ErrorStatus;

void ContinuousFunctionStore::fillLastFreeColors(bool* colorIsFree) const {
  constexpr int paletteSize = Palette::k_numberOfDataColors;
  int nFree = paletteSize;
  for (int c = 0; c < paletteSize; c++) {
    colorIsFree[c] = true;
  }

  // Loop over existing function models starting from the most recent one
  int i = numberOfModels() - 1;
  while (i >= 0) {
    OMG::ExpiringPointer<const ContinuousFunction> f =
        modelForRecord(recordAtIndex(i));
    int nDerivatives = 1 + 2 * f->canDisplayDerivative();
    for (int d = 0; d < nDerivatives && nFree > 1; d++) {
      int colorIndex = static_cast<uint8_t>(f->color(d)) -
                       static_cast<uint8_t>(DataColorComponent(0));
      assert(colorIndex >= 0 && colorIndex < k_numberOfDataColors);
      if (colorIsFree[colorIndex]) {
        colorIsFree[colorIndex] = false;
        nFree--;
      }
      // Stop when there is only one free color
      if (nFree <= 1) {
        return;
      }
    }
    i--;
  }
}

void ContinuousFunctionStore::tidyCaches() const {
  for (int i = 0; i < maxNumberOfMemoizedModels(); i++) {
    memoizedModelAtIndex(i)->tidyCache();
  }
}

ThemeComponent ContinuousFunctionStore::colorForNewModel() const {
  // Check first last functions colors until there is at most 1 free color
  constexpr int paletteSize = Palette::k_numberOfDataColors;
  bool colorIsFree[paletteSize];
  fillLastFreeColors(colorIsFree);
  // Take first free color
  for (int c = 0; c < paletteSize; c++) {
    if (colorIsFree[c]) {
      return DataColorComponent(c);
    }
  }
  OMG_UNREACHABLE
}

ContinuousFunction ContinuousFunctionStore::newModel(const char* name,
                                                     ErrorStatus* error) {
  return ContinuousFunction::NewModel(error, name, colorForNewModel());
}

Ion::Storage::Record::ErrorStatus ContinuousFunctionStore::addEmptyModel() {
  constexpr size_t bufferSize = ContinuousFunction::k_maxDefaultNameSize;
  char name[bufferSize];
  size_t length = UTF8Helper::WriteCodePoint(
      name, bufferSize, ContinuousFunction::k_unnamedRecordFirstChar);
  Ion::Storage::FileSystem::sharedFileSystem->firstAvailableNameFromPrefix(
      name, length, bufferSize, modelExtension());
  Ion::Storage::Record::ErrorStatus error =
      Ion::Storage::Record::ErrorStatus::RecordDoesNotExist;

  newModel(name, &error);
  return error;
}

ExpressionModelHandle* ContinuousFunctionStore::setMemoizedModelAtIndex(
    int cacheIndex, Ion::Storage::Record record) const {
  assert(cacheIndex >= 0 && cacheIndex < maxNumberOfMemoizedModels());
  m_functions[cacheIndex] = ContinuousFunction(record);
  return &m_functions[cacheIndex];
}

ExpressionModelHandle* ContinuousFunctionStore::memoizedModelAtIndex(
    int cacheIndex) const {
  assert(cacheIndex >= 0 && cacheIndex < maxNumberOfMemoizedModels());
  return &m_functions[cacheIndex];
}

}  // namespace Shared
