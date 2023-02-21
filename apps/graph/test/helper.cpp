#include "helper.h"

namespace Graph {

ContinuousFunction *addFunction(const char *definition,
                                Shared::ContinuousFunctionStore *store,
                                Context *context) {
  Ion::Storage::Record::ErrorStatus err = store->addEmptyModel();
  quiz_assert(err == Ion::Storage::Record::ErrorStatus::None);
  Shared::ContinuousFunction *f;
  {
    Ion::Storage::Record record =
        store->recordAtIndex(store->numberOfModels() - 1);
    f = static_cast<Shared::ContinuousFunction *>(
        store->modelForRecord(record).operator->());
    // Prevent further use of record as it may get invalidated by setContent.
  }
  err = f->setContent(definition, context);
  quiz_assert(err == Ion::Storage::Record::ErrorStatus::None);
  (void)err;  // Silence compilation warning.

  return f;
}

}  // namespace Graph
