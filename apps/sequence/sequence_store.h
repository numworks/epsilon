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
  Sequence * modelAtIndex(int i) override {
    assert(i>=0 && i<m_numberOfModels);
    return &m_sequences[i];
  }
  Sequence * activeFunctionAtIndex(int i) override { return (Sequence *)Shared::FunctionStore::activeFunctionAtIndex(i); }
  Sequence * definedFunctionAtIndex(int i) override { return (Sequence *)Shared::FunctionStore::definedFunctionAtIndex(i); }
  /* WARNING: after calling removeModel or removeAll, the sequence context
   * need to invalidate its cache as the sequences evaluations might have
   * changed */
  int maxNumberOfModels() const override { return MaxNumberOfSequences; }
  char symbol() const override;
  static constexpr const char * k_sequenceNames[MaxNumberOfSequences] = {
    "u", "v"//, "w"
  };
  const char * firstAvailableName() override {
    return firstAvailableAttribute(k_sequenceNames, FunctionStore::name);
  }
private:
  Sequence * emptyModel() override;
  Sequence * nullModel() override;
  void setModelAtIndex(Shared::ExpressionModel * f, int i) override;
  static constexpr KDColor k_defaultColors[MaxNumberOfSequences] = {
    Palette::Red, Palette::Blue//, Palette::YellowDark
  };
  const KDColor firstAvailableColor() override {
    return firstAvailableAttribute(k_defaultColors, FunctionStore::color);
  }
  Sequence m_sequences[MaxNumberOfSequences];
};

}

#endif
