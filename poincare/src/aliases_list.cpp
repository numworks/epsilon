#include <poincare/aliases_list.h>

namespace Poincare {

int compareNonNullTerminatedStringWithNullTerminated(const char * nonNullTerminatedName, size_t nonNullTerminatedNameLength, const char * nullTerminatedName) {
  /* Compare similarly to strcmp */
  int diff = strncmp(nonNullTerminatedName, nullTerminatedName, nonNullTerminatedNameLength);
  return (diff != 0) ? diff : strcmp("", nullTerminatedName + nonNullTerminatedNameLength);
}

int AliasesList::maxDifferenceWith(const char * name, int nameLen) const {
  if (!hasMultipleNames()) {
    return compareNonNullTerminatedStringWithNullTerminated(name, nameLen, m_formattedAliasesList);
  }
  int maxValueOfComparison = 0;
  for (const char * nameInList : *this) {
    int tempValueOfComparison = compareNonNullTerminatedStringWithNullTerminated(name, nameLen, nameInList);
    if (tempValueOfComparison == 0) {
      return 0;
    }
    if (maxValueOfComparison < tempValueOfComparison || maxValueOfComparison == 0) {
      maxValueOfComparison = tempValueOfComparison;
    }
  }
  return maxValueOfComparison;
}

const char * AliasesList::firstName() const {
  if (!hasMultipleNames()) {
    return m_formattedAliasesList;
  }
  const char * result = m_formattedAliasesList;
  while (result[0] != k_stringStart) {
    result++;
  }
  return result + 1;
}

const char * AliasesList::nextName(const char * currentPositionInNamesList) const {
  if (!hasMultipleNames()) {
    return nullptr;
  }
  assert(strlen(currentPositionInNamesList) != 0);
  const char * beginningOfNextName = currentPositionInNamesList + strlen(currentPositionInNamesList) + 1;
  if (beginningOfNextName[0] != k_stringStart) {
    return nullptr; // End of list
  }
  return beginningOfNextName + 1; // Skip string start char
}

const char * AliasesList::mainName(Poincare::Preferences::NamingConvention namingConvention) const {
  if (!hasMultipleNames()) {
    return m_formattedAliasesList;
  }
  int mainIndex = indexOfMain(namingConvention);
  const char * result; // skip header
  int currentIndex = 0;
  for (const char * currentName : *this) {
    result = currentName;
    currentIndex++;
    if (currentIndex >= mainIndex) {
      break;
    }
  }
  assert(result != nullptr);
  assert(strlen(result) != 0);
  return result;
}

int AliasesList::indexOfMain(Poincare::Preferences::NamingConvention namingConvention) const {
  assert(hasMultipleNames());
  if (namingConvention == Preferences::NamingConvention::WorldWide) {
    return 0;
  }
  char conventionIdentifier = IdentifierForNamingConvention(namingConvention);
  const char * currentHeaderPosition = m_formattedAliasesList + 1;
  while (currentHeaderPosition[0] != k_stringStart) {
    if (currentHeaderPosition[0] == conventionIdentifier) {
      return currentHeaderPosition[1] - '0'; // The index follows the identifier
    }
    currentHeaderPosition += 2;
  }
  return 0;
}

char AliasesList::IdentifierForNamingConvention(Poincare::Preferences::NamingConvention namingConvention) {
  for (int i = 0; i < k_numberOfNamingConvention; i++) {
    if (k_identifiersForNamingConvention[i].namingConvention == namingConvention) {
      return k_identifiersForNamingConvention[i].identifier;
    }
  }
  assert(false);
  return k_identifiersForNamingConvention[0].identifier; // silence compiler
}

}