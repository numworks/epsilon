#ifndef POINCARE_EXPRESSION_ALIASES_H
#define POINCARE_EXPRESSION_ALIASES_H

#include <assert.h>
#include <omg/utf8_decoder.h>
#include <poincare/src/layout/layout_span_decoder.h>
#include <string.h>

/* This class is used to handle name's aliases for reserved functions, units
 * and constants.
 *
 * === SIMPLE NAMES WITHOUT ALIAS ===
 * If a name has no alias, the Aliases object is equivalent to the string of
 * the name.
 *
 * === ALIASES LIST SYNTAX ===
 * If a name has aliases, they are all stored in one char list, with a special
 * syntax.
 * The list starts with \01 and ends with a double \00
 * Each string of the list is separated by a \00
 *
 * Example:
 * arccos and acos are aliases for the arccos function.
 * |\01|a|r|c|c|o|s|\00|a|c|o|s|\00|\00|
 *  ^start of list              ^end of alias2
 *                  ^end of alias1  ^end of aliases list
 *
 * === MAIN ALIAS ===
 * The main alias is the name output by the calculator. For example, the main
 * alias of "pi" is "π".
 * */

namespace Poincare::Internal {

class Aliases {
 public:
  constexpr Aliases(const char* formattedAliases)
      : m_formattedAliases(formattedAliases) {}
  constexpr operator const char*() const { return m_formattedAliases; }
  constexpr const char* mainAlias() const {
    return m_formattedAliases + hasMultipleAliases();
  }

  bool contains(const char* string) const {
    UTF8Decoder decoder(string);
    return maxDifferenceWith(&decoder) == 0;
  }

  bool contains(LayoutSpan span) const { return maxDifferenceWith(span) == 0; }

  bool isEquivalentTo(Aliases other) {
    return strcmp(mainAlias(), other.mainAlias()) == 0;
  }

  int maxDifferenceWith(UnicodeDecoder* decoder) const;

  /* Return 0 if name is alias of this,
   * else, return the max difference value between name and the aliases
   * of this. */
  int maxDifferenceWith(LayoutSpan span) const;

  /* You can iterate through the names list with syntax:
   * for (const char * alias : name ) {} */
  template <typename T>
  class Iterator {
   public:
    Iterator(T name, const char* firstAlias)
        : m_list(name), m_currentAlias(firstAlias) {}
    const char* operator*() { return m_currentAlias; }
    Iterator& operator++() {
      m_currentAlias = m_list.nextAlias(m_currentAlias);
      return *this;
    }
    bool operator!=(const Iterator& it) const {
      return (m_currentAlias != it.m_currentAlias);
    }

   protected:
    // We use a template otherwise Aliases would be incomplete here
    T m_list;
    const char* m_currentAlias;
  };

  Iterator<Aliases> begin() const {
    return Iterator<Aliases>(*this, mainAlias());
  }
  Iterator<Aliases> end() const { return Iterator<Aliases>(*this, nullptr); }

 private:
  constexpr static char k_listStart = '\01';

  constexpr bool hasMultipleAliases() const {
    return m_formattedAliases[0] == k_listStart;
  }
  // Returns nullptr if there is no next name
  const char* nextAlias(const char* currentPositionInAliasesList) const;

  const char* m_formattedAliases;
};

}  // namespace Poincare::Internal
#endif
