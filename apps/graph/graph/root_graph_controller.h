#ifndef GRAPH_ROOT_GRAPH_CONTROLLER_H
#define GRAPH_ROOT_GRAPH_CONTROLLER_H

#include "calculation_graph_controller.h"
#include "../../i18n.h"

namespace Graph {

class RootGraphController final : public CalculationGraphController {
public:
  RootGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, Shared::CurveViewCursor * cursor) :
    CalculationGraphController(parentResponder, graphView, bannerView, curveViewRange, cursor, I18n::Message::NoZeroFound) {}
  const char * title() override;
private:
  CartesianFunction::Point computeNewPointOfInterest(double start, double step, double max, Poincare::Context * context) override;
};

}

#endif
