#ifndef GRAPH_GRAPH_AXIS_INTERVAL_H
#define GRAPH_GRAPH_AXIS_INTERVAL_H

#include "../function_store.h"
#include "../../curve_view_window_with_cursor.h"

namespace Graph {

class GraphWindow : public CurveViewWindowWithCursor {
public:
  GraphWindow(FunctionStore * functionStore);
  /* computeYAxis needs to be public to be called when a function is added to
   * the function store. */
  bool computeYaxis() override;
  Context * context();
  void setContext(Context * context);
  void setTrigonometric();
  void setDefault();
  float derivativeAtCursorPosition();
  Function * functionSelectedByCursor();
  void setCursorPositionAtAbscissaWithFunction(float abscissa, Function * function);
  void initCursorPosition() override;
  int moveCursorHorizontally(int direction) override;
  // the result of moveCursorVertically means:
  // -1 -> no next function   0-> the window has not changed  1->the window changed
  int moveCursorVertically(int direction) override;
private:
  int m_indexFunctionSelectedByCursor;
  FunctionStore * m_functionStore;
  Context * m_context;
};

}

#endif
