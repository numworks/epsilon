#ifndef ION_RECORD_NAME_VERIFIER_H
#define ION_RECORD_NAME_VERIFIER_H

#include <assert.h>
#include "record.h"

/* This is used by the storage to know if it can store a record with
 * a specific name. Two main reasons can prevent a record from being created:
 *  - Restrictive extensions
 *  - Reserved names
 *
 * == Restrictive extensions ==
 * Some records extensions are restrictive, which means that two record with
 * the same base name and one of these extensions can't exist at the same
 * time in the storage.
 * If a record's extension is not a restrictive extension, it means that it can
 * coexist with any record of a same base name and a different extension.
 * For instance, test.py can coexist with test.func, but test.exp can't
 * coexist with test.func.
 * The precedence score of an extension indicates if it can override
 * an other extension with the same base name.
 *
 * If precendeceScore1 <= precedenceScore2 record1 can override record2
 * If precedenceScore1 > precedenceScore2, record1 cannot override record2
 *
 * The restrictive extensions are registered by the AppsContainer in Apps.
 *
 * == Reserved names ==
 * Some names are reserved for specific extensions, such as V1, X1, or N1
 * which are reserved for the list extension.
 * This means that a record can be name V1 only in two cases :
 * If it is a list OR if no list named V1 exists.
 * To do so, the list extension is considered having a precedence score lower
 * to any other score if it is reserved for the base name.
 * So if X1.func exists in the storage, X1.lis will override it, even if
 * the list extension has usually an higher precedence score.
 *
 * Since we want V1, V2 and V3 to be reserved, we store "V" in prefixes and
 * 3 in prefixRepetitions.
 * For u, v, and w, reserved for sequence, prefixRepetitions = 0
 *
 * The reserved names are registerd by each app, when the snapshot is instiated
 * (Sequence registers u, v and w, Regression registers X1, Y1, etc.)
 * */


namespace Ion {

namespace Storage {

class RecordNameVerifier {
public:
  RecordNameVerifier() : m_numberOfRestrictiveExtensions(0), m_numberOfReservedNamesArrays(0) {}
  // Register a new restrictive extension
  void registerRestrictiveExtensionWithPrecedence(const char * extension, int precedenceScore);
  void unregisterAllRestrictiveExtensions() { m_numberOfRestrictiveExtensions = 0; }

  // Getters
  const char * const * restrictiveExtensions() const { return m_restrictiveExtensions; }
  int numberOfRestrictiveExtensions() const { return m_numberOfRestrictiveExtensions; }
  bool extensionCanOverrideItself(const char * extension) const {
    // Restrictive extension override themselves.
    return precedenceScoreOfExtension(extension) > 0;
  }

  // Register a new array of reserved names
  void registerArrayOfReservedNames(const char * const * namePrefixes, const char* extension, int prefixRepetitions, int numberOfElements);
  void unregisterAllReservedNames() { m_numberOfReservedNamesArrays = 0; }

  enum class OverrideStatus {
    Forbidden = 0,
    Allowed,
    CanCoexist
  };

  /* This method indicates if a record can be overwritten with another which
   * has the same base name but a new extension.
   * The return values are:
   * - OverrideStatus::Forbidden : if the newExtension has a higher
   *   precedence score than the previous, OR if the previousRecord name is
   *   reserved for its extension (see below).
   *
   * - OverrideStatus::Allowed : if the newExtension has the same or a lower
   *   precedence score than the previous, OR it the previousRecord name is
   *   reserved for the newExtension.
   *
   * - OverrideStatus::CanCoexist : if at least one of the two extensions is
   *   not a restrictive extension. */
  OverrideStatus shouldRecordBeOverridenWithNewExtension(Record previousRecord, const char * newExtension) const;

private:
  typedef struct {
    const char * const * namePrefixes;
    const char * extension;
    int prefixRepetitions;
    int numberOfElements;
  } ReservedNamesArray;

  constexpr static int k_maxNumberOfRestrictiveExtensions = 5; // This can be changed if you need more restrictive extensions
  constexpr static int k_maxNumberOfReservedNamesArrays = 3; // This can be changed if you need more reserved names arrays

  int precedenceScoreOfExtension(const char * extension) const;
  bool isNameReservedForExtension(const char * name, int nameLength, const char * extension) const;

  const char * m_restrictiveExtensions[k_maxNumberOfRestrictiveExtensions];
  int m_restrictiveExtensionsPrecedenceScore[k_maxNumberOfRestrictiveExtensions];
  int m_numberOfRestrictiveExtensions;
  ReservedNamesArray m_reservedNamesArrays[k_maxNumberOfReservedNamesArrays];
  int m_numberOfReservedNamesArrays;
};

}

}

#endif

