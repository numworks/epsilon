#pragma once

#include <apps/i18n.h>

#include "intersection_graph_controller.h"

namespace Graph {

class IntersectionRegionGraphController : public IntersectionGraphController {
 public:
  using IntersectionGraphController::IntersectionGraphController;
  const char* title() const override {
    return I18n::translate(I18n::Message::RegionOfIntersectionTitle);
  }

  void viewWillAppear() override {
    m_graphView->setIntersectionRegionDisplay(true);
    IntersectionGraphController::viewWillAppear();
  }
};

}  // namespace Graph
