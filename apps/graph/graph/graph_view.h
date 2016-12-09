#ifndef GRAPH_GRAPH_VIEW_H
#define GRAPH_GRAPH_VIEW_H

#include <escher.h>
#include "cursor_view.h"
#include "graph_window.h"
#include "../../curve_view.h"
#include "../../constant.h"
#include "../function_store.h"
#include "../evaluate_context.h"

namespace Graph {

class GraphView : public CurveView {
public:
  GraphView(FunctionStore * functionStore, GraphWindow * graphWindow);
  void drawRect(KDContext * ctx, KDRect rect) const override;

  float xPixelCursorPosition();
  float xCursorPosition();
  void goToAbscissaOnFunction(float abscissa, Function * function);
  void setCursorVisible(bool visibleCursor);
  void initCursorPosition();
  void moveCursorHorizontally(KDCoordinate xOffset);
  Function * moveCursorVertically(int direction);
  void setContext(Context * evaluateContext);
  Context * context() const;
  int indexFunctionSelectedByCursor();
  void reload();
private:
  constexpr static KDColor k_gridColor = KDColor::RGB24(0xEEEEEE);
  constexpr static KDCoordinate k_cursorSize = 10;
  constexpr static float k_cursorMarginToBorder = 6.0f;

  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;

  float min(Axis axis) const override;
  float max(Axis axis) const override;
  float gridUnit(Axis axis) const override;
  char * label(Axis axis, int index) const override;
  float evaluateExpressionAtAbscissa(Expression * expression, float abscissa) const override;
  void drawGrid(KDContext * ctx, KDRect rect) const;
  void drawGridLines(KDContext * ctx, KDRect rect, Axis axis, float step, KDColor color) const;

  CursorView m_cursorView;
  float m_xCursorPosition;
  float m_yCursorPosition;
  int m_indexFunctionSelectedByCursor;
  bool m_visibleCursor;

  char m_xLabels[k_maxNumberOfXLabels][Constant::FloatBufferSizeInScientificMode];
  char m_yLabels[k_maxNumberOfYLabels][Constant::FloatBufferSizeInScientificMode];

  GraphWindow * m_graphWindow;
  FunctionStore * m_functionStore;
  EvaluateContext * m_evaluateContext;
};

}

#endif
