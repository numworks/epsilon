#ifndef APPS_CURVE_VIEW_H
#define APPS_CURVE_VIEW_H

#include <escher.h>
#include <poincare.h>
#include "curve_view_window.h"
#include <math.h>

class CurveView : public View {
public:
  typedef void Model;
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
  void drawSegment(KDContext * ctx, KDRect rect, Axis axis,
      float coordinate, float lowerBound, float upperBound,
      KDColor color, KDCoordinate thickness = 1) const;
  void drawAxes(KDContext * ctx, KDRect rect, Axis axis) const;
  void drawCurve(KDContext * ctx, KDRect rect, Model * curve, KDColor color, bool colorUnderCurve = false, float colorLowerBound = 0.0f, float colorUpperBound = 0.0f, bool continuously = false) const;
  void drawHistogram(KDContext * ctx, KDRect rect, Model * model,  float firstBarAbscissa, float barWidth,
    bool fillBar, KDColor defaultColor, KDColor highlightColor,  float highlightLowerBound = INFINITY, float highlightUpperBound = -INFINITY) const;
  void computeLabels(Axis axis);
  void drawLabels(KDContext * ctx, KDRect rect, Axis axis, bool shiftOrigin) const;
private:
  constexpr static float k_marginFactor = 0.2f;
  int numberOfLabels(Axis axis) const;
  virtual float evaluateModelWithParameter(Model * curve, float t) const;
  /* Recursively join two dots (dichotomy). The method stops when the
   * maxNumberOfRecursion in reached. */
  void jointDots(KDContext * ctx, KDRect rect, Model * curve, float x, float y, float u, float v, KDColor color, int maxNumberOfRecursion) const;
  /* Join two dots with a straight line. */
  void straightJoinDots(KDContext * ctx, KDRect rect, float pxf, float pyf, float puf, float pvf, KDColor color) const;
  /* Stamp centered around (pxf, pyf). If pxf and pyf are not round number, the
   * function shifts the stamp (by blending adjacent pixel colors) to draw with
   * anti alising. */
  void stampAtLocation(KDContext * ctx, KDRect rect, float pxf, float pyf, KDColor color) const;
  CurveViewWindow * m_curveViewWindow;
};

#endif
