#ifndef SHARED_PLOT_VIEW_AXES_H
#define SHARED_PLOT_VIEW_AXES_H

#include "plot_view.h"
#include <poincare/print_float.h>

namespace Shared {
namespace PlotPolicy {

template<class CGrid, class CAxisX, class CAxisY>
class Axes : public CGrid {
public:
  /* We inline these method to avoid having to write explicit template
   * specializations. */
  Axes() {
    m_xAxis.setOtherAxis(m_yAxis.isAxis());
    m_yAxis.setOtherAxis(m_xAxis.isAxis());
  }

  void drawAxesAndGrid(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const {
    CGrid::drawGrid(plotView, ctx, rect);
    m_xAxis.drawAxis(plotView, ctx, rect, AbstractPlotView::Axis::Horizontal);
    m_yAxis.drawAxis(plotView, ctx, rect, AbstractPlotView::Axis::Vertical);
  }
  void reloadAxes(AbstractPlotView * plotView) {
    m_xAxis.reloadAxis(plotView, AbstractPlotView::Axis::Horizontal);
    m_yAxis.reloadAxis(plotView, AbstractPlotView::Axis::Vertical);
  }

protected:
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

  void drawGridLines(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect, AbstractPlotView::Axis parallel, bool boldGrid) const;
};

class NoAxis {
public:
  void drawAxis(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect, AbstractPlotView::Axis) const {}
  void reloadAxis(AbstractPlotView *, AbstractPlotView::Axis) {}
  bool isAxis() const { return false; }
  void setOtherAxis(bool other) {}
};

class SimpleAxis {
public:
  void drawAxis(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect, AbstractPlotView::Axis axis) const;
  virtual void reloadAxis(AbstractPlotView *, AbstractPlotView::Axis axis) {}
  bool isAxis() const { return true; }
  virtual void setOtherAxis(bool other) {}

protected:
  constexpr static KDColor k_color = KDColorBlack;

  virtual float tickPosition(int i, const AbstractPlotView * plotView, AbstractPlotView::Axis axis) const;
  virtual float tickStep(const AbstractPlotView * plotView, AbstractPlotView::Axis axis) const;
  virtual void drawLabel(int i, float t, const AbstractPlotView * plotView, KDContext * ctx, KDRect rect, AbstractPlotView::Axis axis, KDColor color = k_color) const {}
};

/* Abstract class describing an axis with evenly spaced out labels. */
class AbstractLabeledAxis : public SimpleAxis {
public:
  constexpr static int k_numberSignificantDigits = Poincare::Preferences::LargeNumberOfSignificantDigits;
  constexpr static int k_labelBufferMaxSize = Poincare::PrintFloat::charSizeForFloatsWithPrecision(k_numberSignificantDigits);
  constexpr static int k_labelBufferMaxGlyphLength = Poincare::PrintFloat::glyphLengthForFloatWithPrecision(k_numberSignificantDigits);
  constexpr static int k_maxNumberOfXLabels = CurveViewRange::k_maxNumberOfXGridUnits;
  constexpr static int k_maxNumberOfYLabels = CurveViewRange::k_maxNumberOfYGridUnits;

  AbstractLabeledAxis() : m_hidden(false) {}

  void reloadAxis(AbstractPlotView * plotView, AbstractPlotView::Axis axis) override;
  void setOtherAxis(bool other) override { m_otherAxis = other; }
  void setHidden(bool hide) { m_hidden = hide; }

protected:
  virtual size_t numberOfLabels() const = 0;
  virtual char * mutableLabel(int i) = 0;
  const char * label(int i) const { return const_cast<AbstractLabeledAxis *>(this)->mutableLabel(i); }
  virtual int computeLabel(int i, const AbstractPlotView * plotView, AbstractPlotView::Axis axis);
  virtual bool labelWillBeDisplayed(int i, KDRect rect) const { return true; }
  void drawLabel(int i, float t, const AbstractPlotView * plotView, KDContext * ctx, KDRect rect, AbstractPlotView::Axis axis, KDColor color = k_color) const override;
  void computeLabelsRelativePosition(const AbstractPlotView * plotView, AbstractPlotView::Axis axis) const;

  mutable float m_labelsPosition;
  mutable AbstractPlotView::RelativePosition m_relativePosition : 2;
  bool m_hidden : 1;
  bool m_otherAxis : 1;
};

template<size_t N>
class LabeledAxis : public AbstractLabeledAxis {
public:
  LabeledAxis() {
    for (size_t i = 0; i < N; i++) {
      m_labels[i][0] = '\0';
    }
  }

protected:
  size_t numberOfLabels() const override { return N; }
  char * mutableLabel(int i) override { assert(i < N); return m_labels[i]; }
  char m_labels[N][k_labelBufferMaxSize];
};

typedef LabeledAxis<AbstractLabeledAxis::k_maxNumberOfXLabels> HorizontalLabeledAxis;
typedef LabeledAxis<AbstractLabeledAxis::k_maxNumberOfYLabels> VerticalLabeledAxis;

/* The following classes are intended to be used as template arguments for
 * PlotView */

typedef Axes<NoGrid, NoAxis, NoAxis> NoAxes;
typedef Axes<WithGrid, SimpleAxis, SimpleAxis> TwoUnlabeledAxes;
typedef Axes<NoGrid, HorizontalLabeledAxis, NoAxis> LabeledXAxis;
typedef Axes<WithGrid, HorizontalLabeledAxis, VerticalLabeledAxis> TwoLabeledAxes;

}
}

#endif
