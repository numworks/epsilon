#ifndef SHARED_RECORD_NAME_HELPER_H
#define SHARED_RECORD_NAME_HELPER_H

#include <assert.h>
#include <ion/storage.h>
#include "double_pair_store.h"
#include "sequence_store.h"
#include "../regression/store.h"
#include "../statistics/store.h"

namespace Shared {

class RecordDelegate : public Ion::RecordDelegate {
public:
  /* Some records extensions are restrictive, which means that two record with
   * the same base name and one of these extensions can't exist at the same
   * time in the storage.
   * If a record's extension is not a restrictive extension, it means that it can
   * coexist with any record of a same base name and a different extension.
   * For instance, test.py can coexist with test.func, but test.exp can't
   * coexist with test.func */
  constexpr static const char * k_restrictiveExtensions[] = {
    Ion::Storage::funcExtension,
    Ion::Storage::seqExtension,
    Ion::Storage::expExtension,
    Ion::Storage::lisExtension,
    Ion::Storage::matExtension
  };
  constexpr static int k_numberOfRestrictiveExtensions = sizeof(k_restrictiveExtensions) / sizeof(char *);

  /* The precedence score of an extension indicates if it can override
   * an other extension with the same base name.
   * WARNING : Each of these scores correspond to the extension at the same
   * index in the k_restrictiveExtensions table.
   * These are not in the same table because we need the k_restrictiveExtensions
   * table to be passed to some storage functions.
   *
   * If precendeceScore1 <= precedenceScore2 record1 can override record2
   * If precedenceScore1 > precedenceScore2, record1 cannot override record2
   * */
  constexpr static size_t k_restrictiveExtensionsPrecedenceScore[] = {
    1,
    1,
    2,
    2,
    2
  };
  static_assert(k_numberOfRestrictiveExtensions == sizeof(k_restrictiveExtensionsPrecedenceScore) / sizeof(char *), "Number of precedence scores and number of restrictive extensions don't match.");

  const char * const * restrictiveExtensions() override { return k_restrictiveExtensions; }
  int numberOfRestrictiveExtensions() override { return k_numberOfRestrictiveExtensions; }
  bool extensionCanOverrideItself(const char * extension) override {
    // Restrictive extension override themselves.
    return precedenceScoreOfExtension(extension) > 0;
  }

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
  Ion::RecordDelegate::OverrideStatus shouldRecordBeOverridenWithNewExtension(Ion::Storage::Record previousRecord, const char * newExtension) override;

private:
  size_t precedenceScoreOfExtension(const char * extension);
  /* Some names are reserved for specific extensions, such as V1, X1, or N1
   * which are reserved for the list extension.
   * This means that a record can be name V1 only in two cases :
   * If it is a list OR if no list named V1 exists.
   * To do so, the list extension has a precedence score lower to any other
   * score if it is reserved for the base name.
   * So if X1.func exists in the storage, X1.lis will override it, even if
   * the list extension has usually an higher precedence score.
   *
   * Since we want V1, V2 and V3 to be reserved, we store "V" in prefixes and
   * 3 in prefixRepetitions.
   * For u, v, and w, reserved for sequence, prefixRepetitions = 0 */
  typedef struct {
    const char * const * namePrefixes;
    const char * extension;
    int prefixRepetitions;
    int numberOfElements;
  } ReservedExtension;

  constexpr static ReservedExtension k_reservedExtensions[] = {
    {SequenceStore::k_sequenceNames, Ion::Storage::seqExtension, 0, sizeof(SequenceStore::k_sequenceNames) / sizeof(char *)},
    {Regression::Store::k_columnNames, Ion::Storage::lisExtension, DoublePairStore::k_numberOfSeries, sizeof(Regression::Store::k_columnNames) / sizeof(char *)},
    {Statistics::Store::k_columnNames, Ion::Storage::lisExtension, DoublePairStore::k_numberOfSeries, sizeof(Statistics::Store::k_columnNames) / sizeof(char *)}
  };
  constexpr static int k_reservedExtensionsLength = sizeof(k_reservedExtensions) / sizeof(ReservedExtension);

  /* Returns true if the name is reserved for an other extension than the one
  * given in parameter. Return false if the name is reserved for this extension
  * or not reserved at all. */
  bool isNameReservedForAnotherExtension(const char * name, int nameLength, const char * extension);
};

}

#endif
