#include <poincare/aliases_list.h>
#include <ion/unicode/utf8_helper.h>

namespace Poincare {

const char * AliasesList::mainAlias() const {
  if (!hasMultipleAliases()) {
    return m_formattedAliasesList;
  }
  assert(m_formattedAliasesList[0] == k_stringStart);
  return m_formattedAliasesList + 1;
}

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

}