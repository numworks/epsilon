#include <poincare/name.h>
#include <string.h>
#include <assert.h>

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
  const char * nameInList = parseNextName(m_formattedNamesList);
  assert(nameInList != nullptr);
  while (nameInList != nullptr) {
    int tempValueOfComparison = compareNonNullTerminatedStringWithNullTerminated(name, nameLen, nameInList);
    if (tempValueOfComparison == 0) {
      return 0;
    }
    if (maxValueOfComparison < tempValueOfComparison || maxValueOfComparison == 0) {
      maxValueOfComparison = tempValueOfComparison;
    }
    nameInList = nameInList + strlen(nameInList) + 1; // skip the terminating 0
    nameInList = parseNextName(nameInList);
  }
  return maxValueOfComparison;
}

const char * Name::parseNextName(const char * currentPositionInNamesList) const {
  if (currentPositionInNamesList[0] == 0) { // End of list
    return nullptr;
  }
  assert(currentPositionInNamesList[0] == k_headerStart);
  const char * result = currentPositionInNamesList;
  while (result[0] != k_stringStart && result[0] != 0) { // Skip header for now
    result++;
  }
  assert(result[0] == k_stringStart); // Wrong formatting of namesList
  result++; // skip k_stringStart char
  return result;
}