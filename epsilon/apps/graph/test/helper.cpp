#include "helper.h"

#include <quiz.h>
#include <shared/global_store.h>

namespace Graph {

Ion::Storage::Record AddFunction(const char* definition) {
  Shared::ContinuousFunctionStore& functionStore =
      Shared::GlobalContextAccessor::ContinuousFunctionStore();
  Ion::Storage::Record::ErrorStatus err = functionStore.addEmptyModel();
  quiz_assert(err == Ion::Storage::Record::ErrorStatus::None);
  Shared::ContinuousFunction* f;
  {
    Ion::Storage::Record record =
        functionStore.recordAtIndex(functionStore.numberOfModels() - 1);
    f = static_cast<Shared::ContinuousFunction*>(
        functionStore.modelForRecord(record).operator->());
    // Prevent further use of record as it may get invalidated by setContent.
  }
  err = f->setContent(Poincare::Layout::Parse(definition),
                      Shared::GlobalContextAccessor::Context());
  quiz_assert(err == Ion::Storage::Record::ErrorStatus::None);
  (void)err;
  return *f;
}

}  // namespace Graph
