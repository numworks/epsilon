#ifndef SHARED_CURVE_VIEW_H
#define SHARED_CURVE_VIEW_H

#include "banner_view.h"
#include "curve_view_range.h"
#include "curve_view_cursor.h"
#include "cursor_view.h"
#include <poincare/preferences.h>
#include <poincare/coordinate_2D.h>
#include <cmath>

namespace Shared {

class CurveView : public View {
public:
  /* We want a 3 characters margin before the first label tick, so that most
   * labels appear completely. This gives 3*charWidth/320 = 3*7/320= 0.066 */
  static constexpr float k_labelsHorizontalMarginRatio = 0.066f;
  typedef Poincare::Coordinate2D<float> (*EvaluateXYForParameter)(float t, void * model, void * context);
  typedef float (*EvaluateYForX)(float x, void * model, void * context);
  enum class Axis {
    Horizontal = 0,
    Vertical = 1
  };
  CurveView(CurveViewRange * curveViewRange = nullptr,
      CurveViewCursor * curveViewCursor = nullptr,
      BannerView * bannerView = nullptr,
      CursorView * cursorView = nullptr,
      View * okView = nullptr,
      bool displayBanner = true);
  virtual void reload();
  // When the main view is selected, the banner view is visible
  bool isMainViewSelected() const;
  void selectMainView(bool mainViewSelected);
  CursorView * cursorView() { return m_cursorView; }
  void setCursorView(Shared::CursorView * cursorView);
  View * bannerView() { return m_bannerView; }
  void setBannerView(View * bannerView);
  void setOkView(View * okView);
  void setForceOkDisplay(bool force) { m_forceOkDisplay = force; }
  float pixelWidth() const;
  float pixelHeight() const;
protected:
  CurveViewRange * curveViewRange() const { return m_curveViewRange; }
  void setCurveViewRange(CurveViewRange * curveViewRange);
  // Drawing methods
  constexpr static KDCoordinate k_labelMargin = 4;
  constexpr static KDCoordinate k_okVerticalMargin = 23;
  constexpr static KDCoordinate k_okHorizontalMargin = 10;
  constexpr static KDCoordinate k_labelGraduationLength = 6;
  constexpr static int k_numberSignificantDigits = 6;
  constexpr static int k_bigNumberSignificantDigits = Poincare::Preferences::LargeNumberOfSignificantDigits;
  constexpr static int k_labelBufferMaxSize = 1 + k_bigNumberSignificantDigits + 1 + Poincare::PrintFloat::k_specialECodePointByteLength + 1 + 3 + 1; // '-' + significant digits + '.' + "E" + '-' + 3 digits + null-terminating char
  constexpr static int k_labelBufferMaxGlyphLength = 1 + k_bigNumberSignificantDigits + 3 + 3; // '-' + significant digits + ".E-" + 3 digits
  constexpr static int k_maxNumberOfXLabels = CurveViewRange::k_maxNumberOfXGridUnits;
  constexpr static int k_maxNumberOfYLabels = CurveViewRange::k_maxNumberOfYGridUnits;
  constexpr static int k_externRectMargin = 2;
  float pixelToFloat(Axis axis, KDCoordinate p) const;
  float floatToPixel(Axis axis, float f) const;
  void drawLine(KDContext * ctx, KDRect rect, Axis axis,
      float coordinate, KDColor color, KDCoordinate thickness = 1, KDCoordinate dashSize = -1) const {
    return drawHorizontalOrVerticalSegment(ctx, rect, axis, coordinate, -INFINITY, INFINITY, color,
        thickness, dashSize);
  }
  void drawHorizontalOrVerticalSegment(KDContext * ctx, KDRect rect, Axis axis,
      float coordinate, float lowerBound, float upperBound,
      KDColor color, KDCoordinate thickness = 1, KDCoordinate dashSize = -1) const;
  void drawSegment(KDContext * ctx, KDRect rect,
    float x, float y, float u, float v,
    KDColor color, bool thick = true
  ) const;
  enum class Size : uint8_t {
    Small,
    Medium,
    Large
  };
  void drawDot(KDContext * ctx, KDRect rect, float x, float y, KDColor color, Size size = Size::Small) const;
  /* 'drawArrow' draws the edge of an arrow pointing to (x,y) with the
   * orientation (dx,dy).
   * The parameters defining the shape of the arrow are the length in pixel of
   * the projection of the arrow on the segment -'pixelArrowLength'- and the
   * tangent of the angle between the segment and each wing of the arrow called
   * 'angle'.
   *
   *            /                  |
   *          /                    |
   *        /                      L
   *      /                        |
   *    /                          |
   *  <--------------------------------------------------
   *    \
   *      \
   *        \
   *          \
   *            \
   *
   *  <--- pl --->
   *
   *  pl = pixelArrowLength
   *  tan(angle) = L/pl
   */
  void drawArrow(KDContext * ctx, KDRect rect, float x, float y, float dx, float dy, KDColor color, KDCoordinate pixelArrowLength = 10, float angle = 0.4f) const;
  void drawGrid(KDContext * ctx, KDRect rect) const;
  void drawAxes(KDContext * ctx, KDRect rect) const;
  void drawAxis(KDContext * ctx, KDRect rect, Axis axis) const;
  void drawCurve(KDContext * ctx, KDRect rect, float tStart, float tEnd, float tStep, EvaluateXYForParameter xyEvaluation, void * model, void * context, bool drawStraightLinesEarly, KDColor color, bool thick = true, bool colorUnderCurve = false, float colorLowerBound = 0.0f, float colorUpperBound = 0.0f) const;
  void drawCartesianCurve(KDContext * ctx, KDRect rect, float xMin, float xMax, EvaluateXYForParameter xyEvaluation, void * model, void * context, KDColor color, bool thick = true, bool colorUnderCurve = false, float colorLowerBound = 0.0f, float colorUpperBound = 0.0f) const;
  void drawHistogram(KDContext * ctx, KDRect rect, EvaluateYForX yEvaluation, void * model, void * context, float firstBarAbscissa, float barWidth,
    bool fillBar, KDColor defaultColor, KDColor highlightColor,  float highlightLowerBound = INFINITY, float highlightUpperBound = -INFINITY) const;
  void computeLabels(Axis axis);
  void simpleDrawBothAxesLabels(KDContext * ctx, KDRect rect) const;
  enum class RelativePosition : uint8_t {
    None,
    Before,
    After
  };
  // Draw the label at the above/below and to the left/right of the given position
  void drawLabel(KDContext * ctx, KDRect rect, float xPosition, float yPosition, const char * label, KDColor color, RelativePosition horizontalPosition, RelativePosition verticalPosition) const;
  void drawLabelsAndGraduations(KDContext * ctx, KDRect rect, Axis axis, bool shiftOrigin, bool graduationOnly = false, bool fixCoordinate = false, KDCoordinate fixedCoordinate = 0, KDColor backgroundColor = KDColorWhite) const;
  View * m_bannerView;
  CurveViewCursor * m_curveViewCursor;
private:
  static constexpr const KDFont * k_font = KDFont::SmallFont;
  // returns the coordinates where should be drawn the label knowing the coordinates of its graduation and its relative position
   KDPoint positionLabel(KDCoordinate xPosition, KDCoordinate yPosition, KDSize labelSize, RelativePosition horizontalPosition, RelativePosition verticalPosition) const;
  void drawGridLines(KDContext * ctx, KDRect rect, Axis axis, float step, KDColor boldColor, KDColor lightColor) const;
  /* The window bounds are deduced from the model bounds but also take into
  account a margin (computed with k_marginFactor) */
  float min(Axis axis) const;
  float max(Axis axis) const;
  float gridUnit(Axis axis) const;
  virtual char * label(Axis axis, int index) const { return nullptr; }
  virtual size_t labelMaxGlyphLengthSize() const { return k_labelBufferMaxGlyphLength; }
  int numberOfLabels(Axis axis) const;
  /* Recursively join two dots (dichotomy). The method stops when the
   * maxNumberOfRecursion in reached. */
  void joinDots(KDContext * ctx, KDRect rect, EvaluateXYForParameter xyEvaluation, void * model, void * context, bool drawStraightLinesEarly, float t, float x, float y, float s, float u, float v, KDColor color, bool thick, int maxNumberOfRecursion) const;
  /* Join two dots with a straight line. */
  void straightJoinDots(KDContext * ctx, KDRect rect, float pxf, float pyf, float puf, float pvf, KDColor color, bool thick) const;
  /* Stamp centered around (pxf, pyf). If pxf and pyf are not round number, the
   * function shifts the stamp (by blending adjacent pixel colors) to draw with
   * anti alising. */
  void stampAtLocation(KDContext * ctx, KDRect rect, float pxf, float pyf, KDColor color, bool thick) const;
  void layoutSubviews(bool force = false) override;
  KDRect cursorFrame();
  KDRect bannerFrame();
  KDRect okFrame();
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  /* m_curveViewRange has to be non null but the cursor model, the banner and
   * cursor views may be nullptr if not needed. */
  void computeHorizontalExtremaLabels(bool increaseNumberOfSignificantDigits = false);
  float labelValueAtIndex(Axis axis, int i) const;
  bool bannerIsVisible() const;
  CurveViewRange * m_curveViewRange;
  CursorView * m_cursorView;
  View * m_okView;
  bool m_forceOkDisplay;
  bool m_mainViewSelected;
  uint32_t m_drawnRangeVersion;
};

}

#endif
