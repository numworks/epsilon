#include "expression_model_handle.h"
#include "global_context.h"
#include "poincare_helpers.h"
#include <poincare/horizontal_layout.h>
#include <string.h>
#include <cmath>
#include <assert.h>

using namespace Ion;
using namespace Poincare;

namespace Shared {

ExpressionModelHandle::ExpressionModelHandle(Storage::Record record) :
  Storage::Record(record)
{
}

bool ExpressionModelHandle::isDefined() {
  return !isEmpty();
}

bool ExpressionModelHandle::isEmpty() {
  return value().size <= metaDataSize();
}

}
