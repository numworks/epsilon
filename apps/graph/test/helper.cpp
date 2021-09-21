#include "helper.h"

namespace Graph {

ContinuousFunction * addFunction(const char * definition, Shared::ContinuousFunction::PlotType type, ContinuousFunctionStore * store, Context * context) {
  Ion::Storage::Record::ErrorStatus err = store->addEmptyModel();
  assert(err == Ion::Storage::Record::ErrorStatus::None);
  (void) err; // Silence compilation warning about unused variable.
  Ion::Storage::Record record = store->recordAtIndex(store->numberOfModels() - 1);
  Shared::ContinuousFunction * f = static_cast<Shared::ContinuousFunction *>(store->modelForRecord(record).operator->());
  f->setPlotType(type, Poincare::Preferences::AngleUnit::Degree, context);
  f->setContent(definition, context);
  return f;
}

}
