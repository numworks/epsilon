#ifndef SEQUENCE_SEQUENCE_STORE_H
#define SEQUENCE_SEQUENCE_STORE_H

#include "sequence.h"
#include "../shared/function_store.h"
#include <stdint.h>

namespace Sequence {

class SequenceStore : public Shared::FunctionStore {
public:
  using Shared::FunctionStore::FunctionStore;
  uint32_t storeChecksum() override;
  Sequence * functionAtIndex(int i) override;
  Sequence * activeFunctionAtIndex(int i) override;
  Sequence * definedFunctionAtIndex(int i) override;
  Sequence * addEmptyFunction() override;
  void removeFunction(Shared::Function * f) override;
  int maxNumberOfFunctions() override;
  static constexpr int k_maxNumberOfSequences = 3;
private:
  const char * firstAvailableName() override;
  const KDColor firstAvailableColor() override;
  static constexpr KDColor k_defaultColors[k_maxNumberOfSequences] = {
    KDColor::RGB24(0xbe2727), KDColor::RGB24(0x3e6f3c), KDColor::RGB24(0x656975)
  };
  static constexpr const char * k_sequenceNames[k_maxNumberOfSequences] = {
    "u", "v", "w"
  };
  Sequence m_sequences[k_maxNumberOfSequences];
};

}

#endif
