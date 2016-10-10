#ifndef GRAPH_VALUES_INTERVAL_H
#define GRAPH_VALUES_INTERVAL_H

namespace Graph {

class Interval {
public:
  Interval(float start, float end, float step);

  int numberOfElements();
  float element(int i);
private:
  float m_start;
  float m_end;
  float m_step;
};

}

#endif
