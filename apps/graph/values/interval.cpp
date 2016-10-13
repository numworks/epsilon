#include "interval.h"
#include <assert.h>

namespace Graph {

Interval::Interval(float start, float end, float step) :
  m_start(start),
  m_end(end),
  m_step(step)
{
}

int Interval::numberOfElements() {
  if (m_start > m_end) {
    return 0;
  } else {
    return 1 + (m_end - m_start)/m_step;
  }
}

float Interval::element(int i) {
  assert(i >= 0 && i < numberOfElements());
  return m_start + i*m_step;
}

}
