#include "expression_model_handle.h"

namespace Shared {

ExpressionModelHandle::ExpressionModelHandle(Ion::Storage::Record record)
    : Ion::Storage::Record(record) {}

bool ExpressionModelHandle::isDefined() const { return !isEmpty(); }

bool ExpressionModelHandle::isEmpty() const {
  return value().size <= metaDataSize();
}

}  // namespace Shared
