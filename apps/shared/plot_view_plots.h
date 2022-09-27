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
  template<typename T> using Curve2D = Poincare::Coordinate2D<T> (*)(T, void *, void *);
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
    CurveDrawing(Curve2D<float> curve, void * model, void * context, float tStart, float tEnd, float tStep, KDColor color, AbstractPlotView::Axis axis = AbstractPlotView::Axis::Horizontal, bool thick = true, bool dashed = false);
    /* If one of the pattern bound is nullptr, the main curve is used instead. */
    void setPatternOptions(Pattern pattern, Curve2D<float> patternLowerBound, Curve2D<float> patternUpperBound, bool patternWithoutCurve);
    void setPrecisionOptions(bool drawStraightLinesEarly, Curve2D<double> curveDouble, DiscontinuityTest discontinuity);
    void draw(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const;

  private:
    /* When iterating between two abscissas, the steepest curve can go from bottom
     * to top of screen in one pixel of width. By computing a maximum of 2^8 dots,
     * we ensure to compute at least one value per pixel, since 2^8 > screen height
     * Note: this is not true if the curve goes outside of the screen though.
     */
    constexpr static int k_maxNumberOfIterations = 8;

    void joinDots(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect, float t1, Poincare::Coordinate2D<float> xy1, float t2, Poincare::Coordinate2D<float> xy2, int remainingIterations, DiscontinuityTest discontinuity) const;

    Pattern m_pattern;
    void * m_model;
    void * m_context;
    Curve2D<float> m_curve;
    Curve2D<float> m_patternLowerBound;
    Curve2D<float> m_patternUpperBound;
    Curve2D<double> m_curveDouble;
    DiscontinuityTest m_discontinuity;
    float m_tStart;
    float m_tEnd;
    float m_tStep;
    KDColor m_color;
    AbstractPlotView::Axis m_axis;
    bool m_thick;
    bool m_dashed;
    bool m_patternWithoutCurve;
    bool m_drawStraightLinesEarly;
  };
};

}
}

#endif
