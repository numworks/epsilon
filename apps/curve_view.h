#ifndef APPS_CURVE_VIEW_H
#define APPS_CURVE_VIEW_H

#include <escher.h>
#include <poincare.h>
#include "curve_view_window.h"

class CurveView : public View {
public:
  enum class Axis {
    Horizontal = 0,
    Vertical = 1
  };
  CurveView(CurveViewWindow * curveViewWindow = nullptr);
  void reload();
protected:
  constexpr static KDColor k_axisColor = KDColor::RGB24(0x000000);
  constexpr static KDCoordinate k_labelMargin =  4;
  constexpr static int k_maxNumberOfXLabels =  18;
  constexpr static int k_maxNumberOfYLabels =  13;
  void setCurveViewWindow(CurveViewWindow * curveViewWindow);
  /* The window bounds are deduced from the model bounds but also take into
  account a margin (computed with k_marginFactor) */
  float min(Axis axis) const;
  float max(Axis axis) const;
  float gridUnit(Axis axis) const;
  virtual char * label(Axis axis, int index) const = 0;
  KDCoordinate pixelLength(Axis axis) const;
  float pixelToFloat(Axis axis, KDCoordinate p) const;
  float floatToPixel(Axis axis, float f) const;
  void drawLine(KDContext * ctx, KDRect rect, Axis axis,
      float coordinate, KDColor color, KDCoordinate thickness = 1) const;
  void drawAxes(Axis axis, KDContext * ctx, KDRect rect) const;
  void drawCurve(void * curve, KDColor color, KDContext * ctx, KDRect rect, bool colorUnderCurve = false, float colorLowerBound = 0.0f, float colorUpperBound = 0.0f, bool continuously = false) const;
  void drawHistogram(void * curve, KDColor color, KDContext * ctx, KDRect rect, bool colorHighlightBin = false, KDColor highlightColor = KDColorBlack, float colorLowerBound = 0.0f, float colorUpperBound = 0.0f) const;
  void computeLabels(Axis axis);
  void drawLabels(Axis axis, bool shiftOrigin, KDContext * ctx, KDRect rect) const;
private:
  constexpr static float k_marginFactor = 0.2f;
  int numberOfLabels(Axis axis) const;
  virtual float evaluateCurveAtAbscissa(void * curve, float t) const = 0;
  /* Recursively join two dots (dichotomy). The method stops when the
   * maxNumberOfRecursion in reached. */
  void jointDots(void * curve, float x, float y, float u, float v, KDColor color, int maxNumberOfRecursion, KDContext * ctx, KDRect rect) const;
  /* Join two dots with a straight line. */
  void straightJoinDots(float pxf, float pyf, float puf, float pvf, KDColor color, KDContext * ctx, KDRect rect) const;
  /* Stamp centered around (pxf, pyf). If pxf and pyf are not round number, the
   * function shifts the stamp (by blending adjacent pixel colors) to draw with
   * anti alising. */
  void stampAtLocation(float pxf, float pyf, KDColor color, KDContext * ctx, KDRect rect) const;
  CurveViewWindow * m_curveViewWindow;
};

#endif
