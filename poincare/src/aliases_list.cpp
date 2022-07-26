#include <poincare/aliases_list.h>

namespace Poincare {

int compareNonNullTerminatedStringWithNullTerminated(const char * nonNullTerminatedAlias, size_t nonNullTerminatedAliasLength, const char * nullTerminatedAlias) {
  /* Compare similarly to strcmp */
  int diff = strncmp(nonNullTerminatedAlias, nullTerminatedAlias, nonNullTerminatedAliasLength);
  return (diff != 0) ? diff : strcmp("", nullTerminatedAlias + nonNullTerminatedAliasLength);
}

int AliasesList::maxDifferenceWith(const char * alias, int aliasLen) const {
  if (!hasMultipleAliases()) {
    return compareNonNullTerminatedStringWithNullTerminated(alias, aliasLen, m_formattedAliasesList);
  }
  int maxValueOfComparison = 0;
  for (const char * aliasInList : *this) {
    int tempValueOfComparison = compareNonNullTerminatedStringWithNullTerminated(alias, aliasLen, aliasInList);
    if (tempValueOfComparison == 0) {
      return 0;
    }
    if (maxValueOfComparison < tempValueOfComparison || maxValueOfComparison == 0) {
      maxValueOfComparison = tempValueOfComparison;
    }
  }
  return maxValueOfComparison;
}

const char * AliasesList::firstAlias() const {
  if (!hasMultipleAliases()) {
    return m_formattedAliasesList;
  }
  const char * result = m_formattedAliasesList;
  while (result[0] != k_stringStart) {
    result++;
  }
  return result + 1;
}

const char * AliasesList::nextAlias(const char * currentPositionInAliasesList) const {
  if (!hasMultipleAliases()) {
    return nullptr;
  }
  assert(strlen(currentPositionInAliasesList) != 0);
  const char * beginningOfNextAlias = currentPositionInAliasesList + strlen(currentPositionInAliasesList) + 1;
  if (beginningOfNextAlias[0] != k_stringStart) {
    return nullptr; // End of list
  }
  return beginningOfNextAlias + 1; // Skip string start char
}

const char * AliasesList::mainAlias(Poincare::Preferences::NamingConvention namingConvention) const {
  if (!hasMultipleAliases()) {
    return m_formattedAliasesList;
  }
  int mainIndex = mainAliasIndex(namingConvention);
  const char * result; // skip header
  int currentIndex = 0;
  for (const char * currentAlias : *this) {
    result = currentAlias;
    currentIndex++;
    if (currentIndex >= mainIndex) {
      break;
    }
  }
  assert(result != nullptr);
  assert(strlen(result) != 0);
  return result;
}

int AliasesList::mainAliasIndex(Poincare::Preferences::NamingConvention namingConvention) const {
  assert(hasMultipleAliases());
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