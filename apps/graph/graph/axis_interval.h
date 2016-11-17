#ifndef GRAPH_GRAPH_AXIS_INTERVAL_H
#define GRAPH_GRAPH_AXIS_INTERVAL_H

#include "../function_store.h"
#include "../evaluate_context.h"

namespace Graph {

class AxisInterval {
public:
  AxisInterval(FunctionStore * functionStore);
  float xMin();
  float xMax();
  float yMin();
  float yMax();
  bool yAuto();
  void setXMin(float f);
  void setXMax(float f);
  void setYMin(float f);
  void setYMax(float f);
  void setYAuto(bool yAuto);
  /* Need to be public to be called when a function is added to the function
   * store. */
  void computeYaxes();
  Context * context();
  void setContext(Context * context);
private:
  float m_xMin;
  float m_xMax;
  float m_yMin;
  float m_yMax;
  bool m_yAuto;
  //float xScale;
  //float yScale;
  FunctionStore * m_functionStore;
  EvaluateContext * m_evaluateContext;
};

}

#endif
