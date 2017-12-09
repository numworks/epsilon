#ifndef SHARED_VALUES_INTERVAL_H
#define SHARED_VALUES_INTERVAL_H

namespace Shared {

class Interval final {
public:
  Interval() : m_numberOfElements(0), m_start(0), m_end(0), m_step(0), m_needCompute(false) {}
  // Delete the implicit copy constructor: the object is heavy
  Interval(const Interval&) = delete;
  int numberOfElements();
  void deleteElementAtIndex(int index);
  double element(int i);
  double start() { return m_start; }
  double end() { return m_end; }
  double step() { return m_step; }
  void setStart(double f);
  void setEnd(double f);
  void setStep(double f);
  void setElement(int i, double f);
  // TODO: decide the max number of elements after optimization
  constexpr static int k_maxNumberOfElements = 100;
private:
  void computeElements();
  int m_numberOfElements;
  double m_intervalBuffer[k_maxNumberOfElements];
  double m_start;
  double m_end;
  double m_step;
  bool m_needCompute;
};

typedef void (Interval::*SetterPointer)(double);
typedef double (Interval::*GetterPointer)();

}

#endif
