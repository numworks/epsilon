#ifndef GRAPH_GRAPH_AXIS_INTERVAL_H
#define GRAPH_GRAPH_AXIS_INTERVAL_H

#include "../function_store.h"
#include "../evaluate_context.h"

namespace Graph {

class GraphWindow {
public:
  enum class Axis {
    X,
    Y
  };
  enum class Direction {
    Up,
    Left,
    Down,
    Right
  };
  GraphWindow(FunctionStore * functionStore);
  float xMin();
  float xMax();
  float yMin();
  float yMax();
  bool yAuto();
  float xGridUnit();
  float yGridUnit();
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
  void centerAxisAround(Axis axis, float position);
  void translateWindow(Direction direction);
  void setTrigonometric();
  void roundAbscissa();
  void normalize();
  void setDefault();
  void panToMakePointVisible(float x, float y, float xMargin, float yMargin);
private:
  constexpr static float k_minNumberOfXGridUnits = 7.0f;
  constexpr static float k_maxNumberOfXGridUnits = 18.0f;
  constexpr static float k_minNumberOfYGridUnits = 5.0f;
  constexpr static float k_maxNumberOfYGridUnits = 13.0f;
  constexpr static float k_oneUnit = 1.0f;
  constexpr static float k_twoUnit = 2.0f;
  constexpr static float k_fiveUnit = 5.0f;
  void computeGridUnit(Axis axis);
  float m_xMin;
  float m_xMax;
  float m_yMin;
  float m_yMax;
  bool m_yAuto;
  float m_xGridUnit;
  float m_yGridUnit;
  FunctionStore * m_functionStore;
  EvaluateContext * m_evaluateContext;
};

}

#endif
