#include "sequence_context.h"

#include <apps/shared/global_context.h>
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
    const char* identifier, int length) const {
  constexpr int numberOfSequencesNames =
      std::size(SequenceHelper::k_sequenceNames);
  for (int i = 0; i < numberOfSequencesNames; i++) {
    if (strncmp(identifier, SequenceHelper::k_sequenceNames[i], length) == 0) {
      return Context::UserNamedType::Sequence;
    }
  }
  return ContextWithParent::expressionTypeForIdentifier(identifier, length);
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

bool SequenceContext::sequenceIsNotComputable(const Context& context,
                                              int sequenceIndex) const {
  return cache()->sequenceIsNotComputable(context, sequenceIndex);
}

SequenceCache* SequenceContext::cache() const {
  return GlobalContext::s_sequenceCache;
}

}  // namespace Shared
