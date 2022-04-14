#ifndef SHARED_RECORD_NAME_HELPER_H
#define SHARED_RECORD_NAME_HELPER_H

#include <ion/storage.h>
#include "double_pair_store.h"
#include "sequence_store.h"

namespace Shared {

class RecordNameHelper : public Ion::RecordNameHelper {
public:
  typedef struct {
    const char * const * namePrefixes;
    const char * extension;
    size_t prefixRepetitions;
  } ReservedExtension;

  constexpr static const char * const * sequencesReservedNames = SequenceStore::k_sequenceNames;
  constexpr static const char * regressionListsNamePrefixes[] = {"X", "Y"};
  constexpr static const char * statisticsListsNamePrefixes[] = {"V", "N"};
  constexpr static size_t k_numberOfSeriesInApps = DoublePairStore::k_numberOfSeries;

  constexpr static ReservedExtension reservedExtensions[] = {
    {sequencesReservedNames, Ion::Storage::seqExtension, 0},
    {regressionListsNamePrefixes, Ion::Storage::lisExtension, k_numberOfSeriesInApps},
    {statisticsListsNamePrefixes, Ion::Storage::lisExtension, k_numberOfSeriesInApps}
  };
  constexpr static size_t k_reservedExtensionsLength = sizeof(reservedExtensions) / sizeof(ReservedExtension);

  bool isNameReservedForAnotherExtension(const char * name, const char * extension) override;

};

}

#endif
