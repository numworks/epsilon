#ifndef GRAPH_VALUES_INTERVAL_H
#define GRAPH_VALUES_INTERVAL_H

namespace Graph {

class Interval {
public:
  Interval();
  // Delete the implicit copy constructor: the object is heavy
  Interval(const Interval&) = delete;
  int numberOfElements();
  void deleteElementAtIndex(int index);
  float element(int i);
  float start();
  float end();
  float step();
  void setStart(float f);
  void setEnd(float f);
  void setStep(float f);
  void setElement(int i, float f);
  // TODO: decide the max number of elements after optimization
  constexpr static int k_maxNumberOfElements = 500;
private:
  void computeElements();
  int m_numberOfElements;
  float m_intervalBuffer[k_maxNumberOfElements];
  float m_start;
  float m_end;
  float m_step;
  bool m_needCompute;
};

typedef void (Interval::*SetterPointer)(float);
typedef float (Interval::*GetterPointer)();

}

#endif
