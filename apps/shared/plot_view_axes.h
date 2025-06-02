#ifndef SHARED_PLOT_VIEW_AXES_H
#define SHARED_PLOT_VIEW_AXES_H

#include <poincare/print_float.h>

#include "plot_view.h"
#include "poincare/expression_or_float.h"

namespace Graph {
class GraphView;
}

namespace Shared {
namespace PlotPolicy {

template <class CGrid, class CAxisX, class CAxisY>
class Axes : public CGrid {
 public:
  /* We inline these method to avoid having to write explicit template
   * specializations. */
  Axes() {
    m_xAxis.setOtherAxis(m_yAxis.isAxis());
    m_yAxis.setOtherAxis(m_xAxis.isAxis());
  }

  void drawAxes(const AbstractPlotView* plotView, KDContext* ctx,
                KDRect rect) const {
    m_xAxis.drawAxis(plotView, ctx, rect, OMG::Axis::Horizontal);
    m_yAxis.drawAxis(plotView, ctx, rect, OMG::Axis::Vertical);
  }

  void drawAxesAndGrid(const AbstractPlotView* plotView, KDContext* ctx,
                       KDRect rect) const {
    CGrid::DrawGrid(plotView, ctx, rect);
    drawAxes(plotView, ctx, rect);
  }
  void reloadAxes(AbstractPlotView* plotView) {
    m_xAxis.reloadAxis(plotView, OMG::Axis::Horizontal);
    m_yAxis.reloadAxis(plotView, OMG::Axis::Vertical);
  }

 protected:
  /* Compose instead of inheriting, as one cannot inherit twice from the same
   * class. */
  CAxisX m_xAxis;
  CAxisY m_yAxis;
};

class NoGrid {
 protected:
  static void DrawGrid(const AbstractPlotView*, KDContext*, KDRect) {}
};

class WithCartesianGrid {
 protected:
  static void DrawGrid(const AbstractPlotView* plotView, KDContext* ctx,
                       KDRect rect);

 private:
  constexpr static KDColor k_boldColor = Escher::Palette::GrayMiddle;
  constexpr static KDColor k_lightColor = Escher::Palette::GrayWhite;
  static void DrawGridLines(const AbstractPlotView* plotView, KDContext* ctx,
                            KDRect rect, OMG::Axis parallel, bool boldGrid);
};

class WithPolarGrid {
  friend class Graph::GraphView;

 protected:
  static void DrawGrid(const AbstractPlotView* plotView, KDContext* ctx,
                       KDRect rect);

 private:
  constexpr static KDColor k_boldColor = Escher::Palette::GrayMiddle;
  constexpr static KDColor k_lightColor = Escher::Palette::GrayBright;
  constexpr static float k_minimumGraduationDistanceToCenter = 60;
  constexpr static int k_angleStepInDegree = 15;

  static void DrawPolarCircles(const AbstractPlotView* plotView, KDContext* ctx,
                               KDRect rect);

  static void ComputeRadiusBounds(const AbstractPlotView* plotView, KDRect rect,
                                  float* min, float* max);
  static void ComputeAngleBounds(const AbstractPlotView* plotView, KDRect rect,
                                 float xMin, float xMax, float yMin, float yMax,
                                 float* angleMin, float* angleMax);
};

class NoAxis {
 public:
  void drawAxis(const AbstractPlotView* plotView, KDContext* ctx, KDRect rect,
                OMG::Axis) const {}
  void reloadAxis(AbstractPlotView*, OMG::Axis) {}
  bool isAxis() const { return false; }
  void setOtherAxis(bool other) {}
};

class PlainAxis {
 public:
  void drawAxis(const AbstractPlotView* plotView, KDContext* ctx, KDRect rect,
                OMG::Axis) const;
  void reloadAxis(AbstractPlotView*, OMG::Axis) {}
  bool isAxis() const { return true; }
  void setOtherAxis(bool other) {}

 protected:
  constexpr static KDColor k_color = KDColorBlack;
};

class SimpleAxis : public PlainAxis {
 public:
  void drawAxis(const AbstractPlotView* plotView, KDContext* ctx, KDRect rect,
                OMG::Axis axis) const;
  virtual void reloadAxis(AbstractPlotView*, OMG::Axis axis) {}
  virtual void setOtherAxis(bool other) {}

 protected:
  virtual size_t numberOfLabels() const = 0;

  virtual Poincare::ExpressionOrFloat tickPosition(
      size_t labelIndex, const AbstractPlotView* plotView,
      OMG::Axis axis) const;
  virtual Poincare::ExpressionOrFloat tickStep(const AbstractPlotView* plotView,
                                               OMG::Axis axis) const;
  virtual void drawLabel(size_t labelIndex, float t,
                         const AbstractPlotView* plotView, KDContext* ctx,
                         KDRect rect, OMG::Axis axis,
                         KDColor color = k_color) const {
    assert(labelIndex < numberOfLabels());
  }
};

/* Abstract class describing an axis with evenly spaced out labels. */
class AbstractLabeledAxis : public SimpleAxis {
 public:
  constexpr static int k_numberSignificantDigits =
      Poincare::Preferences::LargeNumberOfSignificantDigits;
  constexpr static size_t k_labelBufferMaxSize =
      Poincare::PrintFloat::charSizeForFloatsWithPrecision(
          k_numberSignificantDigits);
  constexpr static size_t k_labelBufferMaxGlyphLength =
      Poincare::PrintFloat::glyphLengthForFloatWithPrecision(
          k_numberSignificantDigits);
  constexpr static size_t k_maxNumberOfXLabels =
      CurveViewRange::k_maxNumberOfXGridUnits;
  constexpr static size_t k_maxNumberOfYLabels =
      CurveViewRange::k_maxNumberOfYGridUnits;

  AbstractLabeledAxis() : m_lastDrawnRect(KDRectZero), m_hidden(false) {}

  void reloadAxis(AbstractPlotView* plotView, OMG::Axis axis) override;
  void setOtherAxis(bool other) override { m_otherAxis = other; }
  void setHidden(bool hide) { m_hidden = hide; }

 protected:
  virtual char* mutableLabel(size_t labelIndex) = 0;
  const char* label(size_t labelIndex) const {
    assert(labelIndex < numberOfLabels());
    return const_cast<AbstractLabeledAxis*>(this)->mutableLabel(labelIndex);
  }
  virtual int computeLabel(size_t labelIndex, const AbstractPlotView* plotView,
                           OMG::Axis axis);
  virtual bool labelWillBeDisplayed(size_t labelIndex, KDRect rect) const;
  KDRect labelRect(size_t labelIndex, float t, const AbstractPlotView* plotView,
                   OMG::Axis axis) const;
  void drawLabel(size_t labelIndex, float t, const AbstractPlotView* plotView,
                 KDContext* ctx, KDRect rect, OMG::Axis axis,
                 KDColor color = k_color) const override;
  void computeLabelsRelativePosition(const AbstractPlotView* plotView,
                                     OMG::Axis axis) const;

  mutable float m_labelsPosition;
  mutable KDRect m_lastDrawnRect;
  mutable AbstractPlotView::RelativePosition m_relativePosition : 2;
  bool m_hidden : 1;
  bool m_otherAxis : 1;
};

template <size_t N>
class LabeledAxis : public AbstractLabeledAxis {
 public:
  LabeledAxis() {
    for (size_t labelIndex = 0; labelIndex < N; labelIndex++) {
      m_labels[labelIndex][0] = 0;
    }
  }

 protected:
  size_t numberOfLabels() const override { return N; }
  char* mutableLabel(size_t labelIndex) override {
    assert(labelIndex < N);
    return m_labels[labelIndex];
  }
  char m_labels[N][k_labelBufferMaxSize];
};

typedef LabeledAxis<AbstractLabeledAxis::k_maxNumberOfXLabels>
    HorizontalLabeledAxis;
typedef LabeledAxis<AbstractLabeledAxis::k_maxNumberOfYLabels>
    VerticalLabeledAxis;

/* The following classes are intended to be used as template arguments for
 * PlotView */

typedef Axes<NoGrid, NoAxis, NoAxis> NoAxes;
typedef Axes<NoGrid, HorizontalLabeledAxis, NoAxis> LabeledXAxis;
typedef Axes<WithCartesianGrid, HorizontalLabeledAxis, VerticalLabeledAxis>
    TwoLabeledAxes;

}  // namespace PlotPolicy
}  // namespace Shared

#endif
