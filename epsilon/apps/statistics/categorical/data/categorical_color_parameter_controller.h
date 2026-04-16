#pragma once

#include <apps/shared/abstract_color_parameter_controller.h>

#include "store.h"

namespace Statistics::Categorical {

class CategoricalColorParameterController
    : public Shared::AbstractColorParameterController {
 public:
  CategoricalColorParameterController(Escher::Responder* parentResponder)
      : AbstractColorParameterController(parentResponder),
        m_store(nullptr),
        m_column(-1) {}

  void setStore(Store* store) { m_store = store; }
  void setColumn(int col) { m_column = col; }

 protected:
  int currentColorIndex() override {
    assert(m_store && m_column >= 0);
    return m_store->colorIndexForGroup(m_column);
  }
  void setColorIndex(int colorIndex) override {
    assert(m_store && m_column >= 0);
    m_store->setColorIndexForGroup(m_column, static_cast<uint8_t>(colorIndex));
  }
  // popAfterColorSelection uses base default: stack->pop()

 private:
  Store* m_store;
  int m_column;
};

}  // namespace Statistics::Categorical
