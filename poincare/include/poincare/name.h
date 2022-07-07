#ifndef POINCARE_NAME_H
#define POINCARE_NAME_H

#include <poincare/preferences.h>
#include <assert.h>

/* This class is used to handle name's aliases for reserved functions, units
 * and constants.
 *
 * === ACCESS NAME AND ALIASES ===
 * To access the main name, use Name::mainName
 * To know if a name is an alias of another, use Name::isAliasOf
 * To iterate through aliases, use for (const char * alias : Name)
 *
 * === SIMPLE NAMES WITHOUT ALIAS ===
 * If a name has no alias, the Name object is equivalent to the string of
 * the name.
 *
 * === ALIASES LIST SYNTAX ===
 * If a name has aliases, they are all stored in one char list, with a special
 * syntax.
 * It is composed of a header, and a list of strings.
 * The header starts with the char \01
 * The strings start with the char \02
 * The strings end with the classic char \00
 *
 * Example:
 * arccos and acos are aliases for the arccos function.
 * name = "\01\02arccos\00\02acos\00"
 * (here the header is empty).
 *
 * === MAIN NAME ===
 * The main name is the name outputted by the calculator. For example, the main
 * name of "pi" is "π".
 * By default, the main name is the first of the list, but in some cases, the
 * main name can be specific to a country (ex: "sen" for "sin" in PT).
 * The header contains information on what is the main name for each country.
 *
 * === HEADER ===
 * If the header is empty or if there is no information for the current
 * country, the main name is the first of the list.
 *
 * To specify an other main name for a country, the header must contain
 * 1 char of the country identifier + 1 char for the main name index in
 * the list. These country identifiers are stored in the constexpr
 * k_identifiersForNamingConvention.
 *
 * Example:
 * The header "P1" means that the portugal main name is at index 1 of the list.
 * (the index starts at 0)
 * So the name "\01P1\02sin\00\02sen\00" means that the main name
 * for every country is "sin" except for portugal for which it's "sen".
 * WARNING: This implementation does not allow for an index > 9.
 * */

namespace Poincare {

class Name {
public:
  constexpr Name(const char * formattedNamesList) : m_formattedNamesList(formattedNamesList) {}
  constexpr operator const char *() const { return m_formattedNamesList; }

  const char * mainName() const { return mainName(Preferences::sharedPreferences()->namingConvention()); }
  bool isAliasOf(const char * name, int nameLen) const { return comparedWith(name, nameLen) == 0; }
  bool isSameNameAs(Name otherName) { return strcmp(mainName(), otherName.mainName()) == 0; }

  /* Return 0 if name is alias of this,
   * else, return the max difference value between name and the aliases
   * of this. */
  int comparedWith(const char * name, int nameLen) const;

  /* You can iterate through the names list with syntax:
   * for (const char * alias : name ) {} */
  template <typename T>
  class Iterator {
  public:
    Iterator(T name, const char * firstName) : m_name(name), m_currentName(firstName) {}
    const char * operator*() { return m_currentName; }
    Iterator & operator++() {
      m_currentName = m_name.nextName(m_currentName);
      return *this;
    }
    bool operator!=(const Iterator& it) const { return (m_currentName != it.m_currentName); }
  protected:
    T m_name;
    const char * m_currentName;
  };

  Iterator<Name> begin() const {
    return Iterator<Name>(*this, firstName());
  }
  Iterator<Name> end() const {
    return Iterator<Name>(*this, nullptr);
  }

private:
  constexpr static char k_headerStart = '\01';
  constexpr static char k_stringStart = '\02';
  constexpr static struct { Preferences::NamingConvention namingConvention; char identifier; } k_identifiersForNamingConvention[] = {
    { Preferences::NamingConvention::Portugal, 'P' }
  };
  constexpr static int k_numberOfNamingConvention = sizeof(k_identifiersForNamingConvention) / sizeof(k_identifiersForNamingConvention[0]);
  // + 1 for WorldWide which does not need an identifier
  static_assert(k_numberOfNamingConvention + 1 == static_cast<int>(Preferences::NamingConvention::NumberOfNamingConvention), "Number of naming convention and their identifiers mismatch in name.h");
  static char IdentifierForNamingConvention(Poincare::Preferences::NamingConvention namingConvention);

  bool hasAliases() const { return m_formattedNamesList[0] == k_headerStart; }
  const char * firstName() const;
  // Returns nullptr if there is no next name
  const char * nextName(const char * currentPositionInNamesList) const;
  const char * mainName(Preferences::NamingConvention namingConvention) const;
  int indexOfMain(Preferences::NamingConvention namingConventionr) const;

  const char * m_formattedNamesList;
};

namespace NamesWithAlias {
  // Special identifiers
  constexpr static Name k_ansName = "\01\02Ans\00\02ans\00";
  // Constants
  constexpr static Name k_piName = "\01\02π\00\02pi\00";
  // Units
  constexpr static Name k_litersName = "\01\02L\00\02l\00";
  // Trigo
  constexpr static Name k_sinName = "\01P1\02sin\00\02sen\00";
  constexpr static Name k_tanName = "\01P1\02tan\00\02tg\00";
  // Inverse trigo
  constexpr static Name k_acosName = "\01\02arccos\00\02acos\00\02bgcos\00";
  constexpr static Name k_asinName = "\01P1\02arcsin\00\02arcsen\00\02asin\00\02bgsin\00";
  constexpr static Name k_atanName = "\01P1\02arctan\00\02arctg\00\02atan\00\02bgtan\00";
  // Advanced trigo
  constexpr static Name k_cosecName = "\01\02cosec\00\02csc\00";
  constexpr static Name k_cotName = "\01\02cot\00\02cotg\00";
  // Inverse advanced trigo
  constexpr static Name k_acosecName = "\01\02arccosec\00\02acosec\00\02bgcosec\00\02arccsc\00\02acsc\00";
  constexpr static Name k_asecName = "\01\02arcsec\00\02asec\00\02bgsec\00";
  constexpr static Name k_acotName = "\01\02arccot\00\02acot\00\02bgcot\00";
  // Hyperbolic trigo
  constexpr static Name k_sinhName = "\01P1\02sinh\00\02senh\00";
  // Inverse hyperbolic trigo
  constexpr static Name k_acoshName = "\01\02arcosh\00\02acosh\00";
  constexpr static Name k_asinhName = "\01P1\02arsinh\00\02arsenh\00\02asinh\00";
  constexpr static Name k_atanhName = "\01\02artanh\00\02atanh\00";
}

}
#endif