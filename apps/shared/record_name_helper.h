#ifndef SHARED_RECORD_NAME_HELPER_H
#define SHARED_RECORD_NAME_HELPER_H

#include <ion/storage.h>
#include "double_pair_store.h"
#include "sequence_store.h"
#include "../regression/store.h"
#include "../statistics/store.h"

namespace Shared {

class RecordNameHelper : public Ion::RecordNameHelper {
public:
  typedef struct {
    const char * const * namePrefixes;
    const char * extension;
    size_t prefixRepetitions;
  } ReservedExtension;

  constexpr static ReservedExtension k_reservedExtensions[] = {
    {SequenceStore::k_sequenceNames, Ion::Storage::seqExtension, 0},
    {Regression::Store::k_columnNames, Ion::Storage::lisExtension, DoublePairStore::k_numberOfSeries},
    {Statistics::Store::k_columnNames, Ion::Storage::lisExtension, DoublePairStore::k_numberOfSeries}
  };
  constexpr static size_t k_reservedExtensionsLength = sizeof(k_reservedExtensions) / sizeof(ReservedExtension);

  bool isNameReservedForAnotherExtension(const char * name, const char * extension) override;

};

}

#endif
