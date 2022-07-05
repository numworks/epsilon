#include <poincare/name.h>
#include <string.h>
#include <assert.h>

namespace Poincare {

int compareNonNullTerminatedStringWithNullTerminated(const char * nonNullTerminatedName, size_t nonNullTerminatedNameLength, const char * nullTerminatedName) {
  /* Compare similarly to strcmp */
  int diff = strncmp(nonNullTerminatedName, nullTerminatedName, nonNullTerminatedNameLength);
  return (diff != 0) ? diff : strcmp("", nullTerminatedName + nonNullTerminatedNameLength);
}

int Name::comparedWith(const char * name, int nameLen) const {
  if (!hasAliases()) {
    return compareNonNullTerminatedStringWithNullTerminated(name, nameLen, m_formattedNamesList);
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

const char * Name::firstName() const {
  if (!hasAliases()) {
    return m_formattedNamesList;
  }
  const char * result = m_formattedNamesList;
  while (result[0] != k_stringStart) {
    result++;
  }
  return result + 1;
}

const char * Name::nextName(const char * currentPositionInNamesList) const {
  if (!hasAliases()) {
    return nullptr;
  }
  assert(strlen(currentPositionInNamesList) != 0);
  const char * beginningOfNextName = currentPositionInNamesList + strlen(currentPositionInNamesList) + 1;
  if (beginningOfNextName[0] != k_stringStart) {
    return nullptr; // End of list
  }
  return beginningOfNextName + 1; // Skip string start char
}

const char * Name::mainName(Poincare::Preferences::NamingConvention namingConvention) const {
  if (!hasAliases()) {
    return m_formattedNamesList;
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

int Name::indexOfMain(Poincare::Preferences::NamingConvention namingConvention) const {
  assert(hasAliases());
  if (namingConvention == Preferences::NamingConvention::WorldWide) {
    return 0;
  }
  char conventionIdentifier = IdentifierForNamingConvention(namingConvention);
  const char * currentHeaderPosition = m_formattedNamesList + 1;
  while (currentHeaderPosition[0] != k_stringStart) {
    if (currentHeaderPosition[0] == conventionIdentifier) {
      return currentHeaderPosition[1] - '0'; // The index follows the identifier
    }
    currentHeaderPosition += 2;
  }
  return 0;
}

char Name::IdentifierForNamingConvention(Poincare::Preferences::NamingConvention namingConvention) {
  for (int i = 0; i < k_numberOfNamingConvention; i++) {
    if (k_identifiersForNamingConvention[i].namingConvention == namingConvention) {
      return k_identifiersForNamingConvention[i].identifier;
    }
  }
  assert(false);
  return k_identifiersForNamingConvention[0].identifier; // silence compiler
}

}