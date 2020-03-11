#ifndef PYTHON_MATPLOTLIB_PLOT_STORE_H
#define PYTHON_MATPLOTLIB_PLOT_STORE_H

//#include <apps/shared/curve_view_range.h>
#include <apps/shared/interactive_curve_view_range.h>

namespace Matplotlib {

class PlotStore : public Shared::InteractiveCurveViewRange {
public:
  PlotStore() : Shared::InteractiveCurveViewRange() {}
  /*
  float xMin() const override { return 0.0f; }
  float xMax() const override { return 1.0f; }
  float yMin() const override { return 0.0f; }
  float yMax() const override { return 1.0f; }
  */
private:
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
