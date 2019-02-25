#include "single_expression_model_handle.h"
#include "global_context.h"
#include "poincare_helpers.h"
#include <poincare/horizontal_layout.h>
#include <string.h>
#include <cmath>
#include <assert.h>

using namespace Ion;
using namespace Poincare;

namespace Shared {

SingleExpressionModelHandle::SingleExpressionModelHandle(Storage::Record record) :
  Storage::Record(record)
{
}

bool SingleExpressionModelHandle::isDefined() {
  return !isEmpty();
}

bool SingleExpressionModelHandle::isEmpty() {
  return value().size <= metaDataSize();
}

}
