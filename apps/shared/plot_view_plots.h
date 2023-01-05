#ifndef SHARED_PLOT_VIEW_PLOTS_H
#define SHARED_PLOT_VIEW_PLOTS_H

#include "plot_view.h"
#include <initializer_list>

namespace Shared {
namespace PlotPolicy {

class NoPlot {
protected:
  void drawPlot(const AbstractPlotView *, KDContext *, KDRect) const {};
};

/* A PlotPolicy trying to draw curves should derive WithCurves and
 * implement a drawPlot(const AbstractPlotView *, KDContext *, KDRect) method. */
class WithCurves {
protected:
  /* The 'model' argument is specific to one curve, while the 'context'
   * argument is shared between all curves in one drawing. */
  template<typename T> using Curve2DEvaluation = Poincare::Coordinate2D<T> (*)(T, void * model, void * context);

  class Curve2D {
  public:
    Curve2D(Curve2DEvaluation<float> f = nullptr, void * model = nullptr) : m_f(f), m_model(model) {}
    operator bool() const { return m_f != nullptr; }
    void * model() const { return m_model; }
    Poincare::Coordinate2D<float> evaluate(float t, void * context) const { assert(m_f); return m_f(t, m_model, context); }

  private:
    Curve2DEvaluation<float> m_f;
    void * m_model;
  };

  typedef bool (*DiscontinuityTest)(float, float, void *, void *);

  static bool NoDiscontinuity(float, float, void *, void *) { return false; }

  /* The screen is tiled with a 4×4 pattern. It takes the form of a
   * lattice with four colored sections and a transparent background.
   * e.g. With sections 1 and 3 colored by Xs:
   * 0 # 1 #        . . X .
   * # 2 # 3        . . . X
   * 1 # 0 #        X . . .
   * # 3 # 2        . X . .
   * FIXME For now, KDColorWhite is used as a shorthand for transparency, since
   * it is the background color in PlotView.
   */
  class Pattern {
  public:
    constexpr static int k_numberOfSections = 4;

    Pattern(KDColor c0, KDColor c1, KDColor c2, KDColor c3, KDColor cBackground = k_transparent);
    Pattern(KDColor c = k_transparent) : Pattern(c, c, c, c, c) {}
    Pattern(bool s0, bool s1, bool s2, bool s3, KDColor color, KDColor backgroundColor = k_transparent);
    Pattern(int s, KDColor color, KDColor backgroundColor = k_transparent);

    void drawInLine(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect, AbstractPlotView::Axis parallel, float position, float min, float max) const;

  private:
    constexpr static size_t k_size = 4;
    constexpr static KDColor k_transparent = AbstractPlotView::k_backgroundColor;

    KDColor m_c0, m_c1, m_c2, m_c3, m_cBackground;
  };

  class CurveDrawing {
  public:
    CurveDrawing(Curve2D curve, void * context, float tStart, float tEnd, float tStep, KDColor color, bool thick = true, bool dashed = false);
    /* If one of the pattern bound is nullptr, the main curve is used instead. */
    void setPatternOptions(Pattern pattern, float patternStart, float patternEnd, Curve2D patternLowerBound, Curve2D patternUpperBound, bool patternWithoutCurve, AbstractPlotView::Axis axis = AbstractPlotView::Axis::Horizontal);
    void setPrecisionOptions(bool drawStraightLinesEarly, Curve2DEvaluation<double> curveDouble, DiscontinuityTest discontinuity);
    void draw(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const;

  private:
    /* When iterating between two abscissas, the steepest curve can go from bottom
     * to top of screen in one pixel of width. By computing a maximum of 2^8 dots,
     * we ensure to compute at least one value per pixel, since 2^8 > screen height
     * Note: this is not true if the curve goes outside of the screen though.
     */
    constexpr static int k_maxNumberOfIterations = 8;

    void joinDots(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect, float t1, Poincare::Coordinate2D<float> xy1, float t2, Poincare::Coordinate2D<float> xy2, int remainingIterations, DiscontinuityTest discontinuity) const;

    Curve2D m_curve;
    Curve2D m_patternLowerBound;
    Curve2D m_patternUpperBound;
    Pattern m_pattern;
    void * m_context;
    Curve2DEvaluation<double> m_curveDouble;
    DiscontinuityTest m_discontinuity;
    float m_tStart;
    float m_tEnd;
    float m_tStep;
    float m_patternStart;
    float m_patternEnd;
    KDColor m_color;
    AbstractPlotView::Axis m_axis;
    bool m_thick;
    bool m_dashed;
    bool m_patternWithoutCurve;
    bool m_drawStraightLinesEarly;
  };

  // Methods for drawing special curves
  void drawArcOfEllipse(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect, Poincare::Coordinate2D<float> center, float width, float height, float angleStart, float angleEnd, KDColor color) const;
};

/* A PlotPolicy trying to draw histograms should derive WithHistogram and
 * implement a drawPlot(const AbstractPlotView *, KDContext *, KDRect) method. */
class WithHistogram {
protected:
  constexpr static KDCoordinate k_borderWidth = 1;
  typedef float (*Curve1D)(float, void *, void *);
  typedef bool (*HighlightTest)(float, void *, void *);

  class HistogramDrawing {
  public:
    HistogramDrawing(Curve1D curve, void * model, void * context, float start, float barsWidth, bool fillBars, KDColor color);
    void setBorderOptions(bool displayBorder, KDColor color);
    void setHighlightOptions(HighlightTest highlighted, KDColor color);
    void draw(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const;

  private:
    constexpr static KDCoordinate k_hollowBarWidth = 2;

    Curve1D m_curve;
    void * m_model;
    void * m_context;
    HighlightTest m_highlightTest;
    float m_start;
    float m_barsWidth;
    bool m_displayBorder;
    bool m_fillBars;
    KDColor m_color;
    KDColor m_highlightColor;
    KDColor m_borderColor;
  };
};

}
}

#endif
