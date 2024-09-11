#include "continuous_function_store.h"

#include <escher/palette.h>
#include <ion.h>
#include <poincare/old/serialization_helper.h>

using namespace Escher;

namespace Shared {

bool ContinuousFunctionStore::displaysOnlyCartesianFunctions(
    int* nbActiveFunctions) const {
  int nActive = numberOfActiveFunctions();
  if (nbActiveFunctions != nullptr) {
    *nbActiveFunctions = nActive;
  }
  return numberOfActiveFunctionsWithProperty(
             &ContinuousFunctionProperties::isCartesian) == nActive;
}

bool ContinuousFunctionStore::displaysFunctionsToNormalize(
    int* nbActiveFunctions) const {
  if (nbActiveFunctions != nullptr) {
    *nbActiveFunctions = numberOfActiveFunctions();
  }
  return numberOfActiveFunctionsWithProperty(
             &ContinuousFunctionProperties::enforcePlotNormalization) != 0;
}

int ContinuousFunctionStore::numberOfActiveFunctions() const {
  uint32_t checksum = Ion::Storage::FileSystem::sharedFileSystem->checksum();
  if (m_memoizedNumberOfActiveFunctions < 0 || checksum != m_storageCheckSum) {
    m_storageCheckSum = checksum;
    m_memoizedNumberOfActiveFunctions =
        FunctionStore::numberOfActiveFunctions();
  }
  return m_memoizedNumberOfActiveFunctions;
}

using ErrorStatus = Ion::Storage::Record::ErrorStatus;

static int indexInArray(KDColor value, const KDColor* array, size_t arraySize) {
  for (size_t i = 0; i < arraySize - 1; i++) {
    if (value == array[i]) {
      return i;
    }
  }
  assert(value == array[arraySize - 1]);
  return arraySize - 1;
}

void ContinuousFunctionStore::fillLastFreeColors(bool* colorIsFree) const {
  constexpr int paletteSize = std::size(Palette::DataColor);
  int nFree = paletteSize;
  for (int c = 0; c < paletteSize; c++) {
    // Initialize at true
    colorIsFree[c] = true;
  }

  int i = numberOfModels() - 1;
  while (i >= 0) {
    ExpiringPointer<ContinuousFunction> f = modelForRecord(recordAtIndex(i));
    int nDerivatives = 1 + 2 * f->canDisplayDerivative();
    for (int d = 0; d < nDerivatives; d++) {
      int c = indexInArray(f->color(d), Palette::DataColor, paletteSize);
      if (colorIsFree[c]) {
        colorIsFree[c] = false;
        nFree--;
      }
      // Stop when there is at most 1 free color
      if (nFree <= 1) {
        return;
      }
    }
    i--;
  }
}

KDColor ContinuousFunctionStore::colorForNewModel() const {
  // Check first last functions colors until there is at most 1 free color
  constexpr int paletteSize = std::size(Palette::DataColor);
  bool colorIsFree[paletteSize];
  fillLastFreeColors(colorIsFree);

  // Take first free color
  for (int c = 0; c < paletteSize - 1; c++) {
    if (colorIsFree[c]) {
      return Palette::DataColor[c];
    }
  }
  assert(colorIsFree[paletteSize - 1]);
  return Palette::DataColor[paletteSize - 1];
}

ContinuousFunction ContinuousFunctionStore::newModel(const char* name,
                                                     ErrorStatus* error) {
  KDColor nextColor = colorForNewModel();
  return ContinuousFunction::NewModel(error, name, nextColor);
}

Ion::Storage::Record::ErrorStatus ContinuousFunctionStore::addEmptyModel() {
  constexpr size_t bufferSize = ContinuousFunction::k_maxDefaultNameSize;
  char name[bufferSize];
  size_t length = Poincare::SerializationHelper::CodePoint(
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
