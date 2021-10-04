#include "helper.h"

namespace Graph {

ContinuousFunction * addFunction(const char * definition, Shared::ContinuousFunction::PlotType type, ContinuousFunctionStore * store, Context * context) {
  Ion::Storage::Record::ErrorStatus err = store->addEmptyModel();
  quiz_assert(err == Ion::Storage::Record::ErrorStatus::None);
  Ion::Storage::Record record = store->recordAtIndex(store->numberOfModels() - 1);
  Shared::ContinuousFunction * f = static_cast<Shared::ContinuousFunction *>(store->modelForRecord(record).operator->());
  err = f->setContent(definition, context);
  quiz_assert(err == Ion::Storage::Record::ErrorStatus::None);
  // Setting content might have changed the records name.
  // TODO Hugo : See ContinuousFunction::setContent TODO
  record = store->recordAtIndex(store->numberOfModels() - 1);
  f = static_cast<Shared::ContinuousFunction *>(store->modelForRecord(record).operator->());
  quiz_assert(f->plotType() == type);
  return f;
}

}
