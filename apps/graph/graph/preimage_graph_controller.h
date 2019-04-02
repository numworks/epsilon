#include "calculation_graph_controller.h"

namespace Graph {

class PreimageGraphController : public CalculationGraphController {
public:
  PreimageGraphController(
    Responder * parentResponder,
    GraphView * graphView,
    BannerView * bannerView,
    Shared::InteractiveCurveViewRange * curveViewRange,
    Shared::CurveViewCursor * cursor
  );
  const char * title() override { return I18n::translate(I18n::Message::Preimage); }
  double image() { return m_image; }
  void setImage(double value) { m_image = value; }
private:
  Poincare::Expression::Coordinate2D computeNewPointOfInterest(double start, double step, double max, Poincare::Context * context) override;
  double m_image;
};

}
