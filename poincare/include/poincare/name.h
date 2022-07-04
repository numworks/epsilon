#ifndef POINCARE_NAME_H
#define POINCARE_NAME_H

#include <assert.h>
class Name {
public:
  constexpr Name(const char * formattedNamesList) : m_formattedNamesList(formattedNamesList) {}
  constexpr operator const char *() const { return m_formattedNamesList; }

  const char * mainName() const { return hasAliases() ? parseMainName() : m_formattedNamesList; }
  bool isAliasOf(const char * name, int nameLen) const;

  constexpr static char k_headerStart = '\01';
  constexpr static char k_stringStart = '\02';

private:
  bool hasAliases() const { return m_formattedNamesList[0] == k_headerStart; }
  const char * parseNextName(const char * currentPositionInNamesList) const;
  const char * parseMainName() const { return parseNextName(m_formattedNamesList); }

  const char * m_formattedNamesList;
};

#endif