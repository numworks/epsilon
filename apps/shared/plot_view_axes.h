#ifndef SHARED_PLOT_VIEW_AXES_H
#define SHARED_PLOT_VIEW_AXES_H

#include "plot_view.h"
#include <poincare/print_float.h>

namespace Shared {
namespace PlotPolicy {

template<class CGrid, class CAxisX, class CAxisY>
class Axes : CGrid {
protected:
  void drawAxes(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const {
    /* We inline this method to avoid having to write explicit template
     * specializations. */
    CGrid::drawGrid(plotView, ctx, rect);
    m_xAxis.drawAxis(plotView, ctx, rect, AbstractPlotView::Axis::Horizontal);
    m_yAxis.drawAxis(plotView, ctx, rect, AbstractPlotView::Axis::Vertical);
  }

  /* Compose instead of inheriting, as one cannot inherit twice from the same
   * class. */
  CAxisX m_xAxis;
  CAxisY m_yAxis;
};

class NoGrid {
protected:
  void drawGrid(const AbstractPlotView *, KDContext *, KDRect) const {}
};

class WithGrid {
protected:
  void drawGrid(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const;

private:
  constexpr static KDColor k_boldColor = Escher::Palette::GrayMiddle;
  constexpr static KDColor k_lightColor = Escher::Palette::GrayWhite;

  void drawGridLines(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect, AbstractPlotView::Axis parallel) const;
};

class NoAxis {
public:
  void drawAxis(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect, AbstractPlotView::Axis) const {}
};

class SimpleAxis {
public:
  void drawAxis(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect, AbstractPlotView::Axis axis) const;

protected:
  float labelPosition(int i, const AbstractPlotView * plotView, AbstractPlotView::Axis axis) const;
  virtual const char * labelText(int i, const AbstractPlotView * plotView, AbstractPlotView::Axis axis) const { return nullptr; }

private:
  constexpr static KDCoordinate k_labelGraduationHalfLength = 3;
  constexpr static KDColor k_color = KDColorBlack;

  virtual float labelStep(const AbstractPlotView * plotView, AbstractPlotView::Axis axis) const;
};

class LabeledAxis : public SimpleAxis {
protected:
  const char * labelText(int i, const AbstractPlotView * plotView, AbstractPlotView::Axis axis) const override;

private:
  constexpr static int k_numberSignificantDigits = Poincare::Preferences::LargeNumberOfSignificantDigits;
  constexpr static int k_labelBufferMaxSize = 1 + k_numberSignificantDigits + 1 + Poincare::PrintFloat::k_specialECodePointByteLength + 1 + 3 + 1; // '-' + significant digits + '.' + "E" + '-' + 3 digits + null-terminating char
  constexpr static int k_labelBufferMaxGlyphLength = 1 + k_numberSignificantDigits + 3 + 3; // '-' + significant digits + ".E-" + 3 digits
  /* FIXME Y axis needs less labels than X axis */
  constexpr static int k_maxNumberOfLabels = PlotRange::k_maxNumberOfXGridUnits > PlotRange::k_maxNumberOfYGridUnits ? PlotRange::k_maxNumberOfXGridUnits : PlotRange::k_maxNumberOfYGridUnits;

  virtual int computeLabel(int i, const AbstractPlotView * plotView, AbstractPlotView::Axis axis) const;

  mutable char k_labels[k_maxNumberOfLabels][k_labelBufferMaxSize];
};

/* The following classes are intended to be used as template arguments for
 * PlotView */

typedef Axes<NoGrid, NoAxis, NoAxis> NoAxes;
typedef Axes<WithGrid, SimpleAxis, SimpleAxis> TwoUnlabeledAxes;
typedef Axes<NoGrid, LabeledAxis, NoAxis> LabeledXAxis;
typedef Axes<WithGrid, LabeledAxis, LabeledAxis> TwoLabeledAxes;

}
}

#endif
