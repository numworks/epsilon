#ifndef SHARED_CURVE_VIEW_H
#define SHARED_CURVE_VIEW_H

#include "banner_view.h"
#include "cursor_view.h"
#include "curve_view_cursor.h"
#include "curve_view_range.h"
#include <poincare/coordinate_2D.h>
#include <poincare/preferences.h>
#include <poincare/print_float.h>
#include <cmath>

namespace Shared {

class CurveView : public Escher::View {
public:
  /* We want a 3 characters margin before the first label tick, so that most
   * labels appear completely. This gives 3*charWidth/320 = 3*7/320= 0.066 */
  static constexpr float k_labelsHorizontalMarginRatio = 0.066f;
  static constexpr int k_numberOfPatternAreas = 4;

  typedef Poincare::Coordinate2D<float> (*EvaluateXYForFloatParameter)(float t, void * model, void * context);
  typedef Poincare::Coordinate2D<double> (*EvaluateXYForDoubleParameter)(double t, void * model, void * context);
  typedef float (*EvaluateYForX)(float x, void * model, void * context);

  enum class Axis {
    Horizontal = 0,
    Vertical = 1
  };

  CurveView(CurveViewRange * curveViewRange = nullptr, CurveViewCursor * curveViewCursor = nullptr, BannerView * bannerView = nullptr, CursorView * cursorView = nullptr);

  virtual void reload(bool resetInterrupted = false, bool force = false);
  virtual void resetCurvesInterrupted() {}
  // When the main view is selected, the banner view is visible
  bool isMainViewSelected() const { return m_mainViewSelected; }
  void selectMainView(bool mainViewSelected);
  CursorView * cursorView() { return m_cursorView; }
  void setCursorView(Shared::CursorView * cursorView);
  View * bannerView() { return m_bannerView; }
  void setBannerView(Escher::View * bannerView);
  float pixelWidth() const;
  float pixelHeight() const;
  float pixelLength(Axis axis) const { return axis == Axis::Horizontal ? pixelWidth() : pixelHeight(); }

protected:
  constexpr static KDCoordinate k_labelMargin = 4;
  constexpr static KDCoordinate k_labelGraduationLength = 6;
  constexpr static int k_numberSignificantDigits = Poincare::Preferences::LargeNumberOfSignificantDigits;
  constexpr static int k_bigNumberSignificantDigits = Poincare::Preferences::VeryLargeNumberOfSignificantDigits;
  constexpr static int k_labelBufferMaxSize = 1 + k_bigNumberSignificantDigits + 1 + Poincare::PrintFloat::k_specialECodePointByteLength + 1 + 3 + 1; // '-' + significant digits + '.' + "E" + '-' + 3 digits + null-terminating char
  constexpr static int k_labelBufferMaxGlyphLength = 1 + k_bigNumberSignificantDigits + 3 + 3; // '-' + significant digits + ".E-" + 3 digits
  constexpr static int k_maxNumberOfXLabels = CurveViewRange::k_maxNumberOfXGridUnits;
  constexpr static int k_maxNumberOfYLabels = CurveViewRange::k_maxNumberOfYGridUnits;
  constexpr static int k_externRectMargin = 2;

  enum class RelativePosition : uint8_t {
    None,
    Before,
    After
  };

  CurveViewRange * curveViewRange() const { return m_curveViewRange; }
  void setCurveViewRange(CurveViewRange * curveViewRange) { m_curveViewRange = curveViewRange; }
  void computeLabels(Axis axis);
  float pixelToFloat(Axis axis, KDCoordinate p) const;
  float floatToPixel(Axis axis, float f) const;
  float floatLengthToPixelLength(Axis axis, float f) const;
  float pixelLengthToFloatLength(Axis axis, float f) const { return f*pixelLength(axis); }
  float floatLengthToPixelLength(float dx, float dy) const;
  float labelStep(Axis axis) const { return labelStepFactor(axis) * gridUnit(axis); }
  virtual float labelStepFactor(Axis axis) const { return 2.0f; }
  KDPoint positionLabel(KDCoordinate xPosition, KDCoordinate yPosition, KDSize labelSize, RelativePosition horizontalPosition, RelativePosition verticalPosition) const;

  /* Drawing methods */
  /* Areas are tiled over a 4x4 pattern. The bits in the areaPattern argument
   * describe which pixels will be drawn. A negative areaPattern indicates all
   * pixels will be drawn.
   *              e.g. areaPattern = 0b1010
   * 0 # 1 #        . . X .
   * # 2 # 3        . . . X
   * 1 # 0 #        X . . .
   * # 3 # 2        . X . .
   */
  void drawHorizontalOrVerticalSegment(KDContext * ctx, KDRect rect, Axis axis, float coordinate, float lowerBound, float upperBound, KDColor color, KDCoordinate thickness = 1, KDCoordinate dashSize = -1, int areaPattern = -1) const;
  void drawLine(KDContext * ctx, KDRect rect, Axis axis, float coordinate, KDColor color, KDCoordinate thickness = 1, KDCoordinate dashSize = -1) const { return drawHorizontalOrVerticalSegment(ctx, rect, axis, coordinate, -INFINITY, INFINITY, color, thickness, dashSize); }
  void drawSegment(KDContext * ctx, KDRect rect, float x, float y, float u, float v, KDColor color, bool thick = true) const;

  enum class Size : uint8_t {
    Tiny,
    Small,
    Medium,
    Large
  };
  void drawDot(KDContext * ctx, KDRect rect, float x, float y, KDColor color, Size size = Size::Tiny) const;

  /* 'drawArrow' draws the edge of an arrow pointing to (x,y) with the
   * orientation (dx,dy).
   * The parameters defining the shape of the arrow are the length of
   * the base of the arrow triangle - 'pixelArrowWith' - and the tangent of the
   * angle between the segment and each wing of the arrow called 'tanAngle'.
   *
   *            /                  |
   *          /                    |
   *        /                      |
   *      / \                      |
   *    /    \ angle               |
   *  <----------------------------l---------------------
   *    \                          |
   *      \                        |
   *        \                      |
   *          \                    |
   *            \                  |
   *
   *  <--- L --->
   *
   *  l = pixelArrowWith
   *  tanAngle = tan(angle) = l/2L
   */
  void drawArrow(KDContext * ctx, KDRect rect, float x, float y, float dx, float dy, KDColor color, float arrowWidth, float tanAngle = 1.0f/3.0f) const;
  void drawGrid(KDContext * ctx, KDRect rect) const;
  void drawAxes(KDContext * ctx, KDRect rect) const;
  void drawAxis(KDContext * ctx, KDRect rect, Axis axis) const { drawLine(ctx, rect, axis, 0.0f, KDColorBlack, 1); }

  void drawCurve(KDContext * ctx, KDRect rect, const float tStart, float tEnd, const float tStep, EvaluateXYForFloatParameter xyFloatEvaluation, void * model, void * context, bool drawStraightLinesEarly, KDColor color, bool thick = true, bool colorUnderCurve = false, KDColor colorOfFill = KDColorBlack, float colorLowerBound = 0.0f, float colorUpperBound = 0.0f, EvaluateXYForDoubleParameter xyDoubleEvaluation = nullptr, bool dashedCurve = false, EvaluateXYForFloatParameter xyAreaBound = nullptr, bool shouldColorAreaWhenNan = false, int areaPattern = 1, Axis axis = Axis::Horizontal) const;
  void drawCartesianCurve(KDContext * ctx, KDRect rect, float tMin, float tMax, EvaluateXYForFloatParameter xyFloatEvaluation, void * model, void * context, KDColor color, bool thick = true, bool colorUnderCurve = false, KDColor colorOfFill = KDColorBlack, float colorLowerBound = 0.0f, float colorUpperBound = 0.0f, EvaluateXYForDoubleParameter xyDoubleEvaluation = nullptr, bool dashedCurve = false, EvaluateXYForFloatParameter xyAreaBound = nullptr, bool shouldColorAreaWhenNan = false, int areaPattern = 1, float cachedTStep = 0.f, Axis axis = Axis::Horizontal) const;
  void drawPolarCurve(KDContext * ctx, KDRect rect, float xMin, float xMax, float tStep, EvaluateXYForFloatParameter xyFloatEvaluation, void * model, void * context, bool drawStraightLinesEarly, KDColor color, bool thick = true, bool colorUnderCurve = false, float colorLowerBound = 0.0f, float colorUpperBound = 0.0f, EvaluateXYForDoubleParameter xyDoubleEvaluation = nullptr) const;

  void drawHistogram(KDContext * ctx, KDRect rect, EvaluateYForX yEvaluation, void * model, void * context, float firstBarAbscissa, float barWidth, bool fillBar, KDColor defaultColor, KDColor highlightColor,  float highlightLowerBound = INFINITY, float highlightUpperBound = -INFINITY) const;
  void simpleDrawBothAxesLabels(KDContext * ctx, KDRect rect) const;

  // Draw the label at the above/below and to the left/right of the given position
  void drawLabel(KDContext * ctx, KDRect rect, float xPosition, float yPosition, const char * label, KDColor color, RelativePosition horizontalPosition, RelativePosition verticalPosition) const;
  void drawLabelsAndGraduations(KDContext * ctx, KDRect rect, Axis axis, bool shiftOrigin, bool graduationOnly = false, bool fixCoordinate = false, KDCoordinate fixedCoordinate = 0, KDColor backgroundColor = KDColorWhite) const;

  Escher::View * m_bannerView;
  CurveViewCursor * m_curveViewCursor;

private:
  static constexpr const KDFont * k_font = KDFont::SmallFont;

  /* The window bounds are deduced from the model bounds but also take into
  account a margin (computed with k_marginFactor) */
  float min(Axis axis) const;
  float max(Axis axis) const;
  float gridUnit(Axis axis) const { return (axis == Axis::Horizontal ? m_curveViewRange->xGridUnit() : m_curveViewRange->yGridUnit()); }
  float roundFloatToPixelPerfect(Axis axis, float x) const;

  virtual char * label(Axis axis, int index) const { return nullptr; }
  virtual size_t labelMaxGlyphLengthSize() const { return k_labelBufferMaxGlyphLength; }
  virtual bool shouldDrawLabelAtPosition(float labelValue) const { return true; }
  virtual void appendLabelSuffix(Axis axis, char * labelBuffer, int maxSize, int glyphLength, int maxGlyphLength) {}
  int numberOfLabels(Axis axis) const;
  /* m_curveViewRange has to be non null but the cursor model, the banner and
   * cursor views may be nullptr if not needed. */
  void computeHorizontalExtremaLabels(bool increaseNumberOfSignificantDigits = false);
  float labelValueAtIndex(Axis axis, int i) const;

  void layoutSubviews(bool force = false) override;
  KDRect cursorFrame();
  KDRect bannerFrame();
  int numberOfSubviews() const override;
  Escher::View * subviewAtIndex(int index) override;
  bool bannerIsVisible() const { return m_bannerView && m_mainViewSelected; }

  void drawPatternAreaInLine(KDContext * ctx, Axis axis, KDCoordinate coordinate, KDCoordinate start, KDCoordinate end, int areaIndex, KDColor color) const;
  void drawGridLines(KDContext * ctx, KDRect rect, Axis axis, float step, KDColor boldColor, KDColor lightColor) const;
  /* Recursively join two dots (dichotomy). The method stops when the
   * maxNumberOfRecursion in reached. */
  int joinDots(KDContext * ctx, KDRect rect, EvaluateXYForFloatParameter xyFloatEvaluation, void * model, void * context, bool drawStraightLinesEarly, float t, float x, float y, float s, float u, float v, KDColor color, bool thick, int maxNumberOfRecursion, EvaluateXYForDoubleParameter xyDoubleEvaluation = nullptr, bool dashedCurve = false, int stampNumber = 0) const;
  /* Join two dots with a straight line. */
  int straightJoinDots(KDContext * ctx, KDRect rect, float pxf, float pyf, float puf, float pvf, KDColor color, bool thick, bool dashedCurve = false, int stampNumber = 0) const;
  /* Stamp centered around (pxf, pyf). If pxf and pyf are not round number, the
   * function shifts the stamp (by blending adjacent pixel colors) to draw with
   * anti alising. */
  int stampAtLocation(KDContext * ctx, KDRect rect, float pxf, float pyf, KDColor color, bool thick, bool dashedCurve = false, int stampNumber = 0) const;

  CurveViewRange * m_curveViewRange;
  CursorView * m_cursorView;
  uint32_t m_drawnRangeVersion;
  bool m_mainViewSelected;
};

}

#endif
