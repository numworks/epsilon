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

  class DotIterator {
  public:
    static DotIterator Begin(mp_obj_t dots);
    static DotIterator End(mp_obj_t dots);
    Dot operator*();
    bool operator!=(const DotIterator & it) const;
    DotIterator & operator++();
  private:
    void loadValues();
    mp_obj_t * m_tuples;
    size_t m_numberOfTuples;
    size_t m_tupleIndex;
    mp_obj_t * m_xValues;
    mp_obj_t * m_yValues;
    size_t m_numberOfValues;
    size_t m_valueIndex;
  };

  class Dots {
  public:
    Dots(mp_obj_t dots) : m_dots(dots) {}
    DotIterator begin() const { return DotIterator::Begin(m_dots); }
    DotIterator end() const { return DotIterator::End(m_dots); }
  private:
    mp_obj_t m_dots;
  };

  void addDots(mp_obj_t x, mp_obj_t y);
  Dots dots() { return Dots(m_dots); }

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
