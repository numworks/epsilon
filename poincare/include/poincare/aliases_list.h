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
 * It is composed of a header, and a list of strings.
 * The header starts with the char \01
 * The strings start with the char \02
 * The strings end with the classic char \00
 * The aliases list ends with an other \00
 *
 * Example:
 * arccos and acos are aliases for the arccos function.
 * |\01|…|\02|a|r|c|c|o|s|\00|\02|a|c|o|s|\00|\00|
 *  ^header start         ^end of alias 1
 *        ^start of alias 1                   ^end of aliases list
 *
 * === MAIN ALIAS ===
 * The main alias is the name outputted by the calculator. For example, the main
 * alias of "pi" is "π".
 *
 * === HEADER ===
 * By default, the main alias is the first of the list, but in some cases, the
 * main alias can be specific to a country (ex: "sen" for "sin" in PT).
 * The header contains information on what is the main alias for each country.
 *
 * To specify an other main alias for a country, the header must contain
 * 1 char of the country identifier + 1 char for the main alias index in
 * the list. These country identifiers are stored in the constexpr
 * k_identifiersForNamingConventionForAliases.
 *
 * Example:
 * The header "P1" means that the portugal main alias is at index 1 of the list.
 * (the index starts at 0)
 * So the name "\01P1\02sin\00\02sen\00" means that the main alias
 * for every country is "sin" except for portugal for which it's "sen".
 * WARNING: This implementation does not allow for an index > 9.
 * */

namespace Poincare {

class AliasesList {
public:
  constexpr AliasesList(const char * formattedAliasesList) : m_formattedAliasesList(formattedAliasesList) {}
  constexpr operator const char *() const { return m_formattedAliasesList; }

  const char * mainAlias() const { return mainAlias(Preferences::sharedPreferences()->namingConventionForAliases()); }
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
    return Iterator<AliasesList>(*this, firstAlias());
  }
  Iterator<AliasesList> end() const {
    return Iterator<AliasesList>(*this, nullptr);
  }

private:
  constexpr static char k_headerStart = '\01';
  constexpr static char k_stringStart = '\02';
  constexpr static struct { Preferences::NamingConventionForAliases namingConventionForAliases; char identifier; } k_identifiersForNamingConventionForAliases[] = {
    { Preferences::NamingConventionForAliases::Portugal, 'P' }
  };
  constexpr static int k_numberOfNamingConventionForAliases = sizeof(k_identifiersForNamingConventionForAliases) / sizeof(k_identifiersForNamingConventionForAliases[0]);
  // + 1 for WorldWide which does not need an identifier
  static_assert(k_numberOfNamingConventionForAliases + 1 == static_cast<int>(Preferences::NamingConventionForAliases::NumberOfNamingConventionsForAliases), "Number of naming convention and their identifiers mismatch in aliases_list.h");
  static char IdentifierForNamingConventionForAliases(Poincare::Preferences::NamingConventionForAliases namingConventionForAliases);

  bool hasMultipleAliases() const { return m_formattedAliasesList[0] == k_headerStart; }
  const char * firstAlias() const;
  // Returns nullptr if there is no next name
  const char * nextAlias(const char * currentPositionInAliasesList) const;
  const char * mainAlias(Preferences::NamingConventionForAliases namingConventionForAliases) const;
  int mainAliasIndex(Preferences::NamingConventionForAliases namingConventionForAliases) const;

  const char * m_formattedAliasesList;
};

namespace AliasesLists {
  // Special identifiers
  constexpr static AliasesList k_ansAliases = "\01\02Ans\00\02ans\00";
  // Constants
  constexpr static AliasesList k_piAliases = "\01\02π\00\02pi\00";
  // Units
  constexpr static AliasesList k_litersAliases = "\01\02L\00\02l\00";
  // Inverse trigo
  constexpr static AliasesList k_acosAliases = "\01\02arccos\00\02acos\00";
  constexpr static AliasesList k_asinAliases = "\01\02arcsin\00\02asin\00";
  constexpr static AliasesList k_atanAliases = "\01\02arctan\00\02atan\00";
}

}
#endif