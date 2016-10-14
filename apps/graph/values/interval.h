#ifndef GRAPH_VALUES_INTERVAL_H
#define GRAPH_VALUES_INTERVAL_H

namespace Graph {

class Interval {
public:
  Interval(float start, float end, float step);

  int numberOfElements();
  float element(int i);
  float start();
  float end();
  float step();
  void setStart(float f);
  void setEnd(float f);
  void setStep(float f);

private:
  float m_start;
  float m_end;
  float m_step;
};

}

#endif
