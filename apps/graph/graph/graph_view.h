#ifndef GRAPH_GRAPH_VIEW_H
#define GRAPH_GRAPH_VIEW_H

#include <escher.h>
#include "cursor_view.h"
#include "axis_interval.h"
#include "../function_store.h"
#include "../evaluate_context.h"

namespace Graph {

class GraphView : public View {
public:
  GraphView(FunctionStore * functionStore, AxisInterval * axisInterval);
  void drawRect(KDContext * ctx, KDRect rect) const override;

  float xPixelCursorPosition();
  float xCursorPosition();
  void setXCursorPosition(float xPosition, Function * function);
  void setVisibleCursor(bool visibleCursor);
  void initCursorPosition();
  void moveCursorHorizontally(KDCoordinate xOffset);
  Function * moveCursorUp();
  Function * moveCursorDown();
  void setContext(Context * evaluateContext);
  Context * context() const;
  int indexFunctionSelectedByCursor();
  void reload();
private:
  constexpr static KDColor k_axisColor = KDColor::RGB24(0x000000);
  constexpr static KDColor k_gridColor = KDColor::RGB24(0xEEEEEE);
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;

  enum class Axis {
    Horizontal = 0,
    Vertical = 1
  };

  float min(Axis axis) const;
  float max(Axis axis) const;
  KDCoordinate pixelLength(Axis axis) const;

  float pixelToFloat(Axis axis, KDCoordinate p) const;
  float floatToPixel(Axis axis, float f) const;

  void drawLine(KDContext * ctx, KDRect rect, Axis axis,
      float coordinate, KDColor color, KDCoordinate thickness = 1) const;

  void drawAxes(KDContext * ctx, KDRect rect) const;
  void drawGrid(KDContext * ctx, KDRect rect) const;
  void drawGridLines(KDContext * ctx, KDRect rect, Axis axis, float step, KDColor color) const;
  void drawFunction(KDContext * ctx, KDRect rect) const;
  /* Recursively join two dots (dichotomy). The method stops when the
   * maxNumberOfRecursion in reached. */
  void jointDots(float x, float y, float u, float v, Function * function, int maxNumberOfRecursion, KDContext * ctx) const;
  /* Join two dots with a straight line. */
  void straightJoinDots(float pxf, float pyf, float puf, float pvf, KDColor color, KDContext * ctx) const;
  /* Stamp centered around (pxf, pyf). If pxf and pyf are not round number, the
   * function shifts the stamp (by blending adjacent pixel colors) to draw with
   * anti alising. */
  void stampAtLocation(float pxf, float pyf, KDColor color, KDContext * ctx) const;
  int numberOfXLabels() const;
  int numberOfYLabels() const;
  CursorView m_cursorView;
  float m_xCursorPosition;
  float m_yCursorPosition;
  int m_indexFunctionSelectedByCursor;
  bool m_visibleCursor;
  constexpr static KDCoordinate k_cursorSize = 10;
  constexpr static float k_cursorMarginToBorder = 5.0f;
  constexpr static KDCoordinate k_labelWidth =  32;
  constexpr static KDCoordinate k_labelHeight =  12;
  constexpr static KDCoordinate k_labelMargin =  4;
  constexpr static int k_maxNumberOfXLabels =  18;
  constexpr static int k_maxNumberOfYLabels =  13;
  BufferTextView m_xLabels[k_maxNumberOfXLabels];
  BufferTextView m_yLabels[k_maxNumberOfYLabels];

  AxisInterval * m_axisInterval;

  FunctionStore * m_functionStore;
  EvaluateContext * m_evaluateContext;
};

}

#endif
