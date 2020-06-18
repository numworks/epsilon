#ifndef SHARED_VALUES_INTERVAL_H
#define SHARED_VALUES_INTERVAL_H

namespace Shared {

class Interval {
public:
  Interval();
  // Delete the implicit copy constructor: the object is heavy
  Interval(const Interval&) = delete;
  int numberOfElements();
  void deleteElementAtIndex(int index);
  double element(int i);
  double start() const { return m_start; }
  double end() const { return m_end; }
  double step() const { return m_step; }
  void setStart(double f);
  void setEnd(double f);
  void setStep(double f);
  void setElement(int i, double f);
  void forceRecompute(){ m_needCompute = true;}
  void reset();
  void clear();
  // TODO: decide the max number of elements after optimization
  constexpr static int k_maxNumberOfElements = 50;
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
typedef double (Interval::*GetterPointer)() const;

}

#endif
