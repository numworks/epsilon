#include <poincare/name.h>
#include <string.h>
#include <assert.h>

bool Name::isAliasOf(const char * name, int nameLen) const {
  if (!hasAliases()) {
    return nameLen == strlen(m_formattedNamesList) && strncmp(name, m_formattedNamesList, nameLen) == 0;
  }
  const char * nameInList = parseNextName(m_formattedNamesList);
  while (nameInList != nullptr) {
    if (nameLen == strlen(nameInList) && strncmp(name, nameInList, nameLen) == 0) {
      return true;
    }
    nameInList = nameInList + strlen(nameInList) + 1; // skip the terminating 0
    nameInList = parseNextName(nameInList);
  }
  return false;
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