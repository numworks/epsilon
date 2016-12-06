#ifndef GRAPH_GRAPH_VIEW_H
#define GRAPH_GRAPH_VIEW_H

#include <escher.h>
#include "cursor_view.h"
#include "axis_interval.h"
#include "../../curve_view.h"
#include "../../constant.h"
#include "../function_store.h"
#include "../evaluate_context.h"

namespace Graph {

class GraphView : public CurveView {
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
  constexpr static KDColor k_gridColor = KDColor::RGB24(0xEEEEEE);
  constexpr static KDCoordinate k_cursorSize = 10;
  constexpr static float k_cursorMarginToBorder = 5.0f;
  constexpr static KDCoordinate k_labelWidth =  32;
  constexpr static KDCoordinate k_labelHeight =  12;
  constexpr static KDCoordinate k_labelMargin =  4;
  constexpr static int k_maxNumberOfXLabels =  18;
  constexpr static int k_maxNumberOfYLabels =  13;

  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;

  float min(Axis axis) const override;
  float max(Axis axis) const override;
  float evaluateExpressionAtAbscissa(Expression * expression, float abscissa) const override;
  void drawGrid(KDContext * ctx, KDRect rect) const;
  void drawGridLines(KDContext * ctx, KDRect rect, Axis axis, float step, KDColor color) const;

  int numberOfXLabels() const;
  int numberOfYLabels() const;
  void computeLabels();
  void drawLabels(Axis axis, KDContext * ctx, KDRect rect) const;

  CursorView m_cursorView;
  float m_xCursorPosition;
  float m_yCursorPosition;
  int m_indexFunctionSelectedByCursor;
  bool m_visibleCursor;

  char m_xLabels[k_maxNumberOfXLabels][Constant::FloatBufferSizeInScientificMode];
  char m_yLabels[k_maxNumberOfYLabels][Constant::FloatBufferSizeInScientificMode];

  AxisInterval * m_axisInterval;
  FunctionStore * m_functionStore;
  EvaluateContext * m_evaluateContext;
};

}

#endif
