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
  class IntervalParameters {
  public:
    void setStart(double f) { m_start = f; }
    void setEnd(double f) { m_end = f; }
    void setStep(double f) { m_step = f; }
    double start() const { return m_start; }
    double end() const { return m_end; }
    double step() const { return m_step; }
  private:
    double m_start;
    double m_end;
    double m_step;
  };
  bool hasSameParameters(IntervalParameters parameters);
  double element(int i);
  IntervalParameters * parameters() { return &m_parameters; }
  void setParameters(IntervalParameters parameters) { m_parameters = parameters; }
  void setElement(int i, double f);
  void forceRecompute(){ m_needCompute = true;}
  void reset();
  void clear();
  void translateTo(double newStart);
  bool isEmpty() const { return m_parameters.start() > m_parameters.end(); }
  constexpr static int k_maxNumberOfElements = 101;
private:
  void computeElements();
  int m_numberOfElements;
  double m_intervalBuffer[k_maxNumberOfElements];
  bool m_needCompute;
  IntervalParameters m_parameters;
};

typedef void (Interval::IntervalParameters::*SetterPointer)(double);
typedef double (Interval::IntervalParameters::*GetterPointer)() const;

}

#endif
