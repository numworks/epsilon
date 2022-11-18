#include "vector_graph_cell.h"
#include "kandinsky/color.h"
#include "vector_model.h"
#include "escher/palette.h"
#include <cmath>

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Calculation {

void VectorGraphPolicy::drawPlot(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const {
  assert(m_model && plotView);

  float x = m_model->vectorX();
  float y = m_model->vectorY();
  float length = std::sqrt(std::pow(x, 2.f) + std::pow(y, 2.f));
  float angle = y >= 0.f ? std::acos(x / length) : 2 * M_PI - std::acos(x / length);
  assert(std::isfinite(x) && std::isfinite(y) && std::isfinite(length) && std::isfinite(angle));

  // - Draw label
  constexpr float smallAngle = M_PI / 30.f; // Ad hoc
  bool labelTooCloseFromArrow = std::abs(angle) < smallAngle || std::abs(M_PI - angle) < smallAngle;
  constexpr float labelDistanceInPixels = k_arcRadiusInPixels + KDFont::GlyphWidth(KDFont::Size::Small);
  float labelDistance = plotView->pixelWidth() * labelDistanceInPixels;
  Coordinate2D<float> labelPosition(labelDistance * std::cos(0.5f * angle), labelDistance * std::sin(0.5f * angle));
  plotView->drawLabel(ctx, rect, "θ", labelPosition, AbstractPlotView::RelativePosition::There, labelTooCloseFromArrow ? AbstractPlotView::RelativePosition::Before : AbstractPlotView::RelativePosition::There, Palette::GrayDark);

  // - Draw arc
  drawArcOfEllipse(plotView, ctx, rect, Coordinate2D<float>(0.f, 0.f), k_arcRadiusInPixels * plotView->pixelWidth(), k_arcRadiusInPixels * plotView->pixelHeight(), 0.f, angle, Palette::GrayDark);

  // - Draw arrow
  plotView->drawSegment(ctx, rect, Coordinate2D<float>(0.f, 0.f), Coordinate2D<float>(x, y), Palette::Red);
  plotView->drawArrowhead(ctx, rect, Coordinate2D<float>(x, y), Coordinate2D<float>(x, y), k_arrowSizeInPixels, Palette::Red);

}

VectorGraphView::VectorGraphView(VectorModel * model) :
  PlotView(model)
{
  m_model = model;
}

}
