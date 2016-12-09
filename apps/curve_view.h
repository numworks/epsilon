#ifndef APPS_CURVE_VIEW_H
#define APPS_CURVE_VIEW_H

#include <escher.h>
#include <poincare.h>

class CurveView : public View {
public:
  enum class Axis {
    Horizontal = 0,
    Vertical = 1
  };
  CurveView();
  void reload();
protected:
  constexpr static KDColor k_axisColor = KDColor::RGB24(0x000000);
  constexpr static KDCoordinate k_labelMargin =  4;
  constexpr static int k_maxNumberOfXLabels =  18;
  constexpr static int k_maxNumberOfYLabels =  13;
  virtual float min(Axis axis) const = 0;
  virtual float max(Axis axis) const = 0;
  virtual float gridUnit(Axis axis) const = 0;
  virtual char * label(Axis axis, int index) const = 0;
  KDCoordinate pixelLength(Axis axis) const;
  float pixelToFloat(Axis axis, KDCoordinate p) const;
  float floatToPixel(Axis axis, float f) const;
  void drawLine(KDContext * ctx, KDRect rect, Axis axis,
      float coordinate, KDColor color, KDCoordinate thickness = 1) const;
  void drawAxes(Axis axis, KDContext * ctx, KDRect rect) const;
  void drawExpression(Expression * expression, KDColor color, KDContext * ctx, KDRect rect) const;
  void computeLabels(Axis axis);
  void drawLabels(Axis axis, KDContext * ctx, KDRect rect) const;
private:
  int numberOfLabels(Axis axis) const;
  virtual float evaluateExpressionAtAbscissa(Expression * expression, float abscissa) const = 0;
  /* Recursively join two dots (dichotomy). The method stops when the
   * maxNumberOfRecursion in reached. */
  void jointDots(Expression * expression, float x, float y, float u, float v, KDColor color, int maxNumberOfRecursion, KDContext * ctx) const;
  /* Join two dots with a straight line. */
  void straightJoinDots(float pxf, float pyf, float puf, float pvf, KDColor color, KDContext * ctx) const;
  /* Stamp centered around (pxf, pyf). If pxf and pyf are not round number, the
   * function shifts the stamp (by blending adjacent pixel colors) to draw with
   * anti alising. */
  void stampAtLocation(float pxf, float pyf, KDColor color, KDContext * ctx) const;
};

#endif
