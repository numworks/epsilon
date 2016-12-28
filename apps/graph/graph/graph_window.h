#ifndef GRAPH_GRAPH_AXIS_INTERVAL_H
#define GRAPH_GRAPH_AXIS_INTERVAL_H

#include "../function_store.h"
#include "../../curve_view_window.h"

namespace Graph {

class GraphWindow : public CurveViewWindow {
public:
  enum class Direction {
    Up,
    Left,
    Down,
    Right
  };
  GraphWindow(FunctionStore * functionStore);
  float xMin() override;
  float xMax() override;
  float yMin() override;
  float yMax() override;
  bool yAuto();
  float xGridUnit() override;
  float yGridUnit() override;
  void setXMin(float f);
  void setXMax(float f);
  void setYMin(float f);
  void setYMax(float f);
  void setYAuto(bool yAuto);
  /* Need to be public to be called when a function is added to the function
   * store. */
  bool computeYaxes();
  Context * context();
  void setContext(Context * context);
  void zoom(float ratio);
  void translateWindow(Direction direction);
  void setTrigonometric();
  void roundAbscissa();
  void normalize();
  void setDefault();
  float xCursorPosition();
  float yCursorPosition();
  float derivativeAtCursorPosition();
  Function * functionSelectedByCursor();
  void setCursorPositionAtAbscissaWithFunction(float abscissa, Function * function);
  void initCursorPosition();
  bool moveCursorHorizontally(int direction);
  // the result of moveCursorVertically means:
  // -1 -> no next function   0-> the window has not changed  1->the window changed
  int moveCursorVertically(int direction);
private:
  constexpr static float k_cursorMarginFactorToBorder = 0.05f;
  constexpr static float k_numberOfCursorStepsInGradUnit = 5.0f;
  bool panToMakePointVisible(float x, float y, float xMargin, float yMargin);
  void centerAxisAround(Axis axis, float position);
  float m_xMin;
  float m_xMax;
  float m_yMin;
  float m_yMax;
  bool m_yAuto;
  float m_xGridUnit;
  float m_yGridUnit;
  float m_xCursorPosition;
  float m_yCursorPosition;
  int m_indexFunctionSelectedByCursor;
  FunctionStore * m_functionStore;
  Context * m_context;
};

}

#endif
