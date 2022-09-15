#include "calculation_graph_controller.h"
#include <apps/i18n.h>

namespace Graph {

class PreimageGraphController : public CalculationGraphController {
public:
  PreimageGraphController(
    Escher::Responder * parentResponder,
    GraphView * graphView,
    BannerView * bannerView,
    Shared::InteractiveCurveViewRange * curveViewRange,
    Shared::CurveViewCursor * cursor
  );
  const char * title() override { return I18n::translate(I18n::Message::Preimage); }
  double image() { return m_image; }
  void setImage(double value) { m_image = value; }
private:
  Poincare::Coordinate2D<double> computeNewPointOfInterest(double start, double max, Poincare::Context * context) override;
  double m_image;
};

}
