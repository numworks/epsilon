#include "sequence_context.h"

#include <apps/shared/global_store.h>
#include <apps/shared/poincare_helpers.h>
#include <omg/signaling_nan.h>

#include <array>
#include <cmath>

#include "sequence_store.h"

using namespace Poincare;

namespace Shared {

SequenceContext::SequenceContext(const Context* parentContext,
                                 const SequenceStore* sequenceStore)
    : ContextWithParent(parentContext), m_sequenceStore(sequenceStore) {}

Context::UserNamedType SequenceContext::expressionTypeForIdentifier(
    std::string_view identifier) const {
  constexpr int numberOfSequencesNames =
      std::size(SequenceHelper::k_sequenceNames);
  for (int i = 0; i < numberOfSequencesNames; i++) {
    if (identifier == std::string_view(SequenceHelper::k_sequenceNames[i])) {
      return Context::UserNamedType::Sequence;
    }
  }
  return ContextWithParent::expressionTypeForIdentifier(identifier);
}

const Sequence* SequenceContext::sequenceAtNameIndex(int sequenceIndex) const {
  assert(0 <= sequenceIndex && sequenceIndex < k_numberOfSequences);
  Ion::Storage::Record record =
      m_sequenceStore->recordAtNameIndex(sequenceIndex);
  if (record.isNull()) {
    return nullptr;
  }
  const Sequence* s = m_sequenceStore->modelForRecord(record);
  return s;
}

bool SequenceContext::sequenceIsNotComputable(int sequenceIndex) const {
  return cache()->sequenceIsNotComputable(sequenceIndex);
}

double SequenceContext::approximateSequenceAtRank(const char* identifier,
                                                  int rank) const {
  int index = SequenceStore::SequenceIndexForName(identifier[0]);
  const Sequence* sequence = sequenceAtNameIndex(index);
  if (sequence == nullptr) {
    return NAN;
  }
  double result = cache()->storedValueOfSequenceAtRank(index, rank);
  if (OMG::IsSignalingNan(result)) {
    // compute value if not in cache
    result = sequence->approximateAtRank(rank, cache());
  }
  return result;
}

SequenceCache* SequenceContext::cache() const {
  return &GlobalContextAccessor::SequenceCache();
}

}  // namespace Shared
