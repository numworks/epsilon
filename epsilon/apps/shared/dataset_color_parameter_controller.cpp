#include "dataset_color_parameter_controller.h"

#include <assert.h>

namespace Shared {

int DatasetColorParameterController::currentColorIndex() {
  assert(m_store != nullptr);
  return m_store->colorIndexForSeries(m_series);
}

void DatasetColorParameterController::setColorIndex(int colorIndex) {
  assert(m_store != nullptr);
  m_store->setColorIndexForSeries(m_series, static_cast<uint8_t>(colorIndex));
}

}  // namespace Shared
