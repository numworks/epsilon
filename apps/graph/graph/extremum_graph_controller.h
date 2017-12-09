#ifndef GRAPH_EXTREMUM_GRAPH_CONTROLLER_H
#define GRAPH_EXTREMUM_GRAPH_CONTROLLER_H

#include "calculation_graph_controller.h"
#include "../../i18n.h"

namespace Graph {

class MinimumGraphController final : public CalculationGraphController {
public:
  MinimumGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, Shared::CurveViewCursor * cursor) :
    CalculationGraphController(parentResponder, graphView, bannerView, curveViewRange, cursor, I18n::Message::NoMinimumFound) {}
  const char * title() override;
private:
  CartesianFunction::Point computeNewPointOfInterest(double start, double step, double max, Poincare::Context * context) override;
};

class MaximumGraphController final : public CalculationGraphController {
public:
  MaximumGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, Shared::CurveViewCursor * cursor) :
    CalculationGraphController(parentResponder, graphView, bannerView, curveViewRange, cursor, I18n::Message::NoMaximumFound) {}
  const char * title() override;
private:
  CartesianFunction::Point computeNewPointOfInterest(double start, double step, double max, Poincare::Context * context) override;
};

}

#endif
