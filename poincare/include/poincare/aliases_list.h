#ifndef POINCARE_ALIASES_LIST_H
#define POINCARE_ALIASES_LIST_H

#include <poincare/preferences.h>
#include <assert.h>
#include <string.h>

/* This class is used to handle name's aliases for reserved functions, units
 * and constants.
 *
 * === SIMPLE NAMES WITHOUT ALIAS ===
 * If a name has no alias, the AliasesList object is equivalent to the string of
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

namespace Poincare {

class AliasesList {
public:
  constexpr AliasesList(const char * formattedAliasesList) : m_formattedAliasesList(formattedAliasesList) {}
  constexpr operator const char *() const { return m_formattedAliasesList; }

  constexpr const char * mainAlias() const { return m_formattedAliasesList + hasMultipleAliases(); }

  bool contains(const char * alias, int aliasLen = -1) const { return maxDifferenceWith(alias, aliasLen > -1 ? aliasLen : strlen(alias)) == 0; }
  bool isEquivalentTo(AliasesList otherList) { return strcmp(mainAlias(), otherList.mainAlias()) == 0; }

  /* Return 0 if name is alias of this,
   * else, return the max difference value between name and the aliases
   * of this. */
  int maxDifferenceWith(const char * alias, int aliasLen) const;

  /* You can iterate through the names list with syntax:
   * for (const char * alias : name ) {} */
  template <typename T>
  class Iterator {
  public:
    Iterator(T name, const char * firstAlias) : m_list(name), m_currentAlias(firstAlias) {}
    const char * operator*() { return m_currentAlias; }
    Iterator & operator++() {
      m_currentAlias = m_list.nextAlias(m_currentAlias);
      return *this;
    }
    bool operator!=(const Iterator& it) const { return (m_currentAlias != it.m_currentAlias); }
  protected:
    T m_list;
    const char * m_currentAlias;
  };

  Iterator<AliasesList> begin() const {
    return Iterator<AliasesList>(*this, mainAlias());
  }
  Iterator<AliasesList> end() const {
    return Iterator<AliasesList>(*this, nullptr);
  }

private:
  constexpr static char k_listStart = '\01';

  constexpr bool hasMultipleAliases() const { return m_formattedAliasesList[0] == k_listStart; }
  // Returns nullptr if there is no next name
  const char * nextAlias(const char * currentPositionInAliasesList) const;

  const char * m_formattedAliasesList;
};

namespace AliasesLists {
  // Special identifiers
  constexpr static AliasesList k_ansAliases = "\01Ans\00ans\00";
  constexpr static AliasesList k_trueAliases = "\01True\00true\00";
  constexpr static AliasesList k_falseAliases = "\01False\00false\00";
  constexpr static AliasesList k_infinityAliases = "\01∞\00inf\00";
  // Constants
  constexpr static AliasesList k_piAliases = "\01π\00pi\00";
  // Symbols
  constexpr static AliasesList k_thetaAliases = "\01θ\00theta\00";
  // Units
  constexpr static AliasesList k_litersAliases = "\01L\00l\00";
  // Inverse trigo
  constexpr static AliasesList k_acosAliases = "\01arccos\00acos\00";
  constexpr static AliasesList k_asinAliases = "\01arcsin\00asin\00";
  constexpr static AliasesList k_atanAliases = "\01arctan\00atan\00";
  // Other functions
  constexpr static AliasesList k_squareRootAliases = "\01√\00sqrt\00";
}

}
#endif