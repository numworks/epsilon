#pragma once

#include "abstract_color_parameter_controller.h"
#include "double_pair_store.h"

namespace Shared {

class DatasetColorParameterController
    : public AbstractColorParameterController {
 public:
  DatasetColorParameterController(Escher::Responder* parentResponder)
      : AbstractColorParameterController(parentResponder),
        m_store(nullptr),
        m_series(0) {}

  void setStore(DoublePairStore* store) { m_store = store; }
  void setSeries(int series) { m_series = series; }

 protected:
  int currentColorIndex() const override {
    assert(m_store != nullptr);
    return m_store->colorIndexForSeries(m_series);
  }
  void setColorIndex(int colorIndex) override {
    assert(m_store != nullptr);
    m_store->setColorIndexForSeries(m_series, static_cast<uint8_t>(colorIndex));
  }

  // popAfterColorSelection uses base default: stack->pop()

 private:
  DoublePairStore* m_store;
  int m_series;
};

}  // namespace Shared
