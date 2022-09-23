#ifndef SHARED_PLOT_VIEW_H
#define SHARED_PLOT_VIEW_H

#include "banner_view.h"
#include "cursor_view.h"
#include "plot_range.h"
#include <poincare/coordinate_2D.h>

/* AbstractPlotView maps a range in R² to the screen and provides methods for
 * drawing basic geometry in R².
 * To instanciate a view, one can use or derive the
 * PlotView<CAXes, CPlot, CBanner, CCursor> template. */

namespace Shared {

class AbstractPlotView : public Escher::View {
public:
  enum class Axis : uint8_t {
    Horizontal = 0,
    Vertical,
  };
  enum class RelativePostion : uint8_t {
    Before,
    There,
    After,
  };

  constexpr static Axis OtherAxis(Axis axis) { return static_cast<Axis>(1 - static_cast<uint8_t>(axis)); }

  AbstractPlotView(PlotRange * range) : m_range(range), m_stampDashIndex(k_stampIndexNoDash) {}

  // Escher::View
  void drawRect(KDContext * ctx, KDRect rect) const final;

  float pixelWidth() const { return (m_range->xMax() - m_range->xMin()) / (bounds().width() - 1); }
  float pixelHeight() const { return (m_range->yMax() - m_range->yMin()) / (bounds().height() - 1); }
  float floatToPixel(Axis axis, float f) const;
  float pixelToFloat(Axis axis, KDCoordinate c) const;
  Poincare::Coordinate2D<float> floatToPixel2D(Poincare::Coordinate2D<float> p) const { return Poincare::Coordinate2D<float>(floatToPixel(Axis::Horizontal, p.x1()), floatToPixel(Axis::Vertical, p.x2())); }

  // Methods for drawing basic geometry
  /* FIXME These methods are public as they need to be accessible to helper
   * classes used as template parameters to PlotView. A better solution might
   * be to private them and befriend the helpers. */
  void drawStraightSegment(KDContext * ctx, KDRect rect, Axis parallel, float position, float min, float max, KDColor color, KDCoordinate thickness = 1, KDCoordinate dashSize = 0) const;
  void drawSegment(KDContext * ctx, KDRect rect, Poincare::Coordinate2D<float> a, Poincare::Coordinate2D<float> b, KDColor color, bool thick = true) const;
  void drawLabel(KDContext * ctx, KDRect rect, const char * label, Poincare::Coordinate2D<float> xy, RelativePostion xPosition, RelativePostion yPosition, KDColor color) const;
  void straightJoinDots(KDContext * ctx, KDRect rect, Poincare::Coordinate2D<float> pixelA, Poincare::Coordinate2D<float> pixelB, KDColor color, bool thick) const;
  void stamp(KDContext * ctx, KDRect rect, Poincare::Coordinate2D<float> p, KDColor color, bool thick) const;

private:
  constexpr static KDCoordinate k_stampDashSize = 5;
  constexpr static KDCoordinate k_stampIndexNoDash = -1;
  constexpr static KDFont::Size k_font = KDFont::Size::Small;

  // Escher::View
  int numberOfSubviews() const { return (bannerView() != nullptr) + (cursorView() != nullptr); }
  Escher::View * subviewAtIndex(int i);
  void layoutSubviews(bool force = false);

  virtual void drawBackground(KDContext * ctx, KDRect rect) const { ctx->fillRect(rect, KDColorWhite); }
  virtual void drawAxes(KDContext * ctx, KDRect rect) const = 0;
  virtual void drawPlot(KDContext * ctx, KDRect rect) const = 0;
  virtual BannerView * bannerView() const = 0;
  virtual KDRect bannerFrame() = 0;
  virtual CursorView * cursorView() const = 0;
  virtual KDRect cursorFrame() = 0;

  PlotRange * m_range;
  mutable KDCoordinate m_stampDashIndex;
};

template<class CAxes, class CPlot, class CBanner, class CCursor>
class PlotView : public AbstractPlotView,  CAxes,  CPlot,  CBanner, CCursor {
public:
  using AbstractPlotView::AbstractPlotView;

private:
  using CAxes::drawAxes;
  void drawAxes(KDContext * ctx, KDRect rect) const override { drawAxes(this, ctx, rect); }

  using CPlot::drawPlot;
  void drawPlot(KDContext * ctx, KDRect rect) const override { drawPlot(this, ctx, rect); }

  using CBanner::bannerView;
  BannerView * bannerView() const override { return bannerView(this); }
  using CBanner::bannerFrame;
  KDRect bannerFrame() override { return bannerFrame(this); }

  using CCursor::cursorView;
  CursorView * cursorView() const override { return cursorView(this); }
  using CCursor::cursorFrame;
  KDRect cursorFrame() override { return cursorFrame(this); }
};

}

#endif
