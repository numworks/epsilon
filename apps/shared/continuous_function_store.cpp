#include "continuous_function_store.h"

#include <ion.h>
#include <poincare/serialization_helper.h>

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

ContinuousFunction ContinuousFunctionStore::newModel(const char* name,
                                                     ErrorStatus* error) {
  /* Choose the next color following the rule:
   * - no other function is the same color among the last
   * size(Palette::DataColor) - 1 ones in the list */
  constexpr int paletteSize = std::size(Palette::DataColor);

  int functionsCount = numberOfModels();

  // Find the first color in the palette that is different

  KDColor nextColor;

  /* Compare to the last paletteSize-1 functions. There will be at least 1 color
   * that is not taken. */
  int maxSize = paletteSize - 1;
  KDColor lastColorsTaken[maxSize];
  int size = 0;
  for (int i = functionsCount - 1; i >= 0; i--) {
    ExpiringPointer<ContinuousFunction> f = modelForRecord(recordAtIndex(i));
    KDColor color0 = f->color(0);
    lastColorsTaken[size++] = color0;
    if (size >= maxSize) {
      break;
    }
    if (f->canDisplayDerivative()) {
      KDColor color1 = f->color(1);
      if (color1 != color0) {
        lastColorsTaken[size++] = color1;
        if (size >= maxSize) {
          break;
        }
      }
      KDColor color2 = f->color(2);
      if (color2 != color0 && color2 != color1) {
        lastColorsTaken[size++] = color2;
        if (size >= maxSize) {
          break;
        }
      }
    }
  }

  // Among those non taken color, choose the first one in the palette.
  for (KDColor color : Palette::DataColor) {
    bool isCandidate = true;
    for (int i = 0; i < size; i++) {
      if (lastColorsTaken[i] == color) {
        isCandidate = false;
        break;
      }
    }
    if (isCandidate) {
      nextColor = color;
      break;
    }
  }

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
