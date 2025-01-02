#include "sequence_context.h"

#include <apps/shared/global_context.h>
#include <apps/shared/poincare_helpers.h>
#include <omg/signaling_nan.h>
#include <poincare/expression.h>
#include <poincare/src/expression/symbol.h>

#include <array>
#include <cmath>

#include "sequence_store.h"

using namespace Poincare;

namespace Shared {

SequenceContext::SequenceContext(Context* parentContext,
                                 SequenceStore* sequenceStore)
    : ContextWithParent(parentContext), m_sequenceStore(sequenceStore) {}

Context::UserNamedType SequenceContext::expressionTypeForIdentifier(
    const char* identifier, int length) {
  constexpr int numberOfSequencesNames =
      std::size(SequenceStore::k_sequenceNames);
  for (int i = 0; i < numberOfSequencesNames; i++) {
    if (strncmp(identifier, SequenceStore::k_sequenceNames[i], length) == 0) {
      return Context::UserNamedType::Sequence;
    }
  }
  return ContextWithParent::expressionTypeForIdentifier(identifier, length);
}

Sequence* SequenceContext::sequenceAtNameIndex(int sequenceIndex) const {
  assert(0 <= sequenceIndex && sequenceIndex < k_numberOfSequences);
  Ion::Storage::Record record =
      m_sequenceStore->recordAtNameIndex(sequenceIndex);
  if (record.isNull()) {
    return nullptr;
  }
  Sequence* s = m_sequenceStore->modelForRecord(record);
  return s;
}

bool SequenceContext::sequenceIsNotComputable(int sequenceIndex) {
  return cache()->sequenceIsNotComputable(sequenceIndex);
}

SequenceCache* SequenceContext::cache() {
  return GlobalContext::s_sequenceCache;
}

}  // namespace Shared
