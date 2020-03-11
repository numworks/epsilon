#ifndef PYTHON_MATPLOTLIB_PLOT_STORE_H
#define PYTHON_MATPLOTLIB_PLOT_STORE_H

//#include <apps/shared/curve_view_range.h>
#include <apps/shared/interactive_curve_view_range.h>
extern "C" {
#include <py/runtime.h>
}

namespace Matplotlib {

class PlotStore : public Shared::InteractiveCurveViewRange {
public:
  PlotStore();
  void flush();

  class Dot {
  public:
    Dot(float x, float y, KDColor color) : m_x(x), m_y(y), m_color(color) {}
    float x() const { return m_x; }
    float y() const { return m_y; }
    KDColor color() const { return m_color; }
  private:
    float m_x;
    float m_y;
    KDColor m_color;
  };

  // TODO: Use an iterator here. It will be a lot faster
  Dot dotAtIndex(int i);
  int numberOfDots();

  void addDots(mp_obj_t x, mp_obj_t y);

  void setGrid(bool grid) { m_grid = grid; }
  bool grid() { return m_grid; }
private:
  mp_obj_t m_dots; // A list of (x,y), where x and y are lists of numbers
  bool m_grid;

  /*
  mp_obj_array_t * m_plots;
  mp_obj_array_t * m_arrows;
  mp_obj_array_t * m_scatters;
  mp_obj_array_t * m_texts;
  mp_obj_array_t * m_rects;

  bool m_grid;
  */

};

}

#endif
