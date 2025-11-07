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
    m_selectedRecordIndex = 0;
    IntersectionGraphController::viewWillAppear();
  }

 private:
  /* Unlike IntersectionGraphController's implementation, we only look at
   * intersections between inequalities, and also include intersections between
   * other inequalities than the selected one. */
  Poincare::Coordinate2D<double> computeNewPointOfInterest(
      double start, double max, bool stretch) override;

  int m_selectedRecordIndex = 0;
};

}  // namespace Graph
