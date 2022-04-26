#include "plot_cursor.h"
#include <assert.h>
#include <cmath>

namespace Statistics {

void PlotCursor::moveTo(double t, double x, double y) {
  assert(!std::isnan(t));
  m_t = t;
  m_x = x;
  m_y = y;
}

}
