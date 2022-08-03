#include <poincare/aliases_list.h>
#include <ion/unicode/utf8_helper.h>

namespace Poincare {

int AliasesList::maxDifferenceWith(const char * alias, int aliasLen) const {
  if (!hasMultipleAliases()) {
    return UTF8Helper::CompareNonNullTerminatedStringWithNullTerminated(alias, aliasLen, m_formattedAliasesList);
  }
  int maxValueOfComparison = 0;
  for (const char * aliasInList : *this) {
    int tempValueOfComparison = UTF8Helper::CompareNonNullTerminatedStringWithNullTerminated(alias, aliasLen, aliasInList);
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
    assert(beginningOfNextAlias[0] == 0);
    return nullptr; // End of list
  }
  return beginningOfNextAlias + 1; // Skip string start char
}

const char * AliasesList::mainAlias(Poincare::Preferences::NamingConventionForAliases namingConventionForAliases) const {
  if (!hasMultipleAliases()) {
    return m_formattedAliasesList;
  }
  int mainIndex = mainAliasIndex(namingConventionForAliases);
  const char * result; // skip header
  int currentIndex = 0;
  for (const char * currentAlias : *this) {
    result = currentAlias;
    currentIndex++;
    if (currentIndex > mainIndex) {
      break;
    }
  }
  assert(result != nullptr);
  assert(strlen(result) != 0);
  return result;
}

int AliasesList::mainAliasIndex(Poincare::Preferences::NamingConventionForAliases namingConventionForAliases) const {
  assert(hasMultipleAliases());
  if (namingConventionForAliases == Preferences::NamingConventionForAliases::WorldWide) {
    return 0;
  }
  char conventionIdentifier = IdentifierForNamingConventionForAliases(namingConventionForAliases);
  const char * currentHeaderPosition = m_formattedAliasesList + 1;
  while (currentHeaderPosition[0] != k_stringStart) {
    if (currentHeaderPosition[0] == conventionIdentifier) {
      return currentHeaderPosition[1] - '0'; // The index follows the identifier
    }
    currentHeaderPosition += 2;
  }
  return 0;
}

char AliasesList::IdentifierForNamingConventionForAliases(Poincare::Preferences::NamingConventionForAliases namingConventionForAliases) {
  for (int i = 0; i < k_numberOfNamingConventionForAliases; i++) {
    if (k_identifiersForNamingConventionForAliases[i].namingConventionForAliases == namingConventionForAliases) {
      return k_identifiersForNamingConventionForAliases[i].identifier;
    }
  }
  assert(false);
  return k_identifiersForNamingConventionForAliases[0].identifier; // silence compiler
}

}