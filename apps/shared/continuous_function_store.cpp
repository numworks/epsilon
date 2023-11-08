#include "continuous_function_store.h"

#include <ion.h>

using namespace Escher;

namespace Shared {

bool ContinuousFunctionStore::displaysNonCartesianFunctions(
    int* nbActiveFunctions) const {
  int nActive = numberOfActiveFunctions();
  if (nbActiveFunctions != nullptr) {
    *nbActiveFunctions = nActive;
  }
  return numberOfActiveFunctionsWithProperty(
             &ContinuousFunctionProperties::isCartesian) < nActive;
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

  for (KDColor color : Palette::DataColor) {
    bool isCandidate = true;

    for (int i = std::max(0, functionsCount - (paletteSize - 1));
         i < functionsCount; i++) {
      if (colorForRecord(recordAtIndex(i)) == color) {
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
  char name[ContinuousFunction::k_maxDefaultNameSize];
  const char* const extensions[1] = {modelExtension()};
  name[0] = ContinuousFunction::k_unnamedRecordFirstChar;
  Ion::Storage::FileSystem::sharedFileSystem->firstAvailableNameFromPrefix(
      name, 1, ContinuousFunction::k_maxDefaultNameSize, extensions, 1, 99);
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
