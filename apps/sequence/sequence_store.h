#ifndef SEQUENCE_SEQUENCE_STORE_H
#define SEQUENCE_SEQUENCE_STORE_H

#include "../shared/function_store.h"
#include "sequence.h"
#include <stdint.h>
#include <escher.h>

namespace Sequence {

class SequenceStore : public Shared::FunctionStore {
public:
  using Shared::FunctionStore::FunctionStore;
  uint32_t storeChecksum() override;
  Sequence * functionAtIndex(int i) override;
  Sequence * activeFunctionAtIndex(int i) override;
  Sequence * definedFunctionAtIndex(int i) override;
  Sequence * addEmptyFunction() override;
  /* WARNING: after calling removeFunction or removeAll, the sequence context
   * need to invalidate its cache as the sequences evaluations might have
   * changed */
  void removeFunction(Shared::Function * f) override;
  void removeAll() override;
  int maxNumberOfFunctions() override;
  const char * firstAvailableName() override;
  char symbol() const override;
  static constexpr const char * k_sequenceNames[MaxNumberOfSequences] = {
    "u", "v"//, "w"
  };
private:
  const KDColor firstAvailableColor() override;
  static constexpr KDColor k_defaultColors[MaxNumberOfSequences] = {
    Palette::Red, Palette::Blue//, Palette::YellowDark
  };
  Sequence m_sequences[MaxNumberOfSequences];
};

}

#endif
