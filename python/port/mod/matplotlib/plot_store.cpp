#include "plot_store.h"
#include <escher/palette.h>

namespace Matplotlib {

PlotStore::PlotStore() : Shared::InteractiveCurveViewRange(),
 m_grid(false)
{
  flush();
}

void PlotStore::flush() {
  m_dots = mp_obj_new_list(0, nullptr);
}

void PlotStore::addDots(mp_obj_t x, mp_obj_t y) {
  mp_obj_t items[2] = {x, y};
  mp_obj_t tuple = mp_obj_new_tuple(2, items);
  mp_obj_list_append(m_dots, tuple);
}

PlotStore::DotIterator PlotStore::DotIterator::Begin(mp_obj_t dots) {
  DotIterator it;
  mp_obj_list_get(dots, &(it.m_numberOfTuples), &(it.m_tuples));
  if (it.m_numberOfTuples > 0) {
    it.m_tupleIndex = 0;
    it.loadValues();
  }
  return it;
}

PlotStore::DotIterator PlotStore::DotIterator::End(mp_obj_t dots) {
  DotIterator it;
  mp_obj_list_get(dots, &(it.m_numberOfTuples), &(it.m_tuples));
  if (it.m_numberOfTuples > 0) {
    it.m_tupleIndex = it.m_numberOfTuples;
    it.m_valueIndex = 0;
  }
  return it;
}

PlotStore::Dot PlotStore::DotIterator::operator*() {
  return PlotStore::Dot(
    mp_obj_get_float(m_xValues[m_valueIndex]),
    mp_obj_get_float(m_yValues[m_valueIndex]),
    Palette::DataColor[m_tupleIndex] // FIXME: Share the "looping" routing
  );
};

bool PlotStore::DotIterator::operator!=(const DotIterator & it) const {
  return (m_tupleIndex != it.m_tupleIndex || m_valueIndex != it.m_valueIndex);
};

PlotStore::DotIterator & PlotStore::DotIterator::operator++() {
  if (m_valueIndex < m_numberOfValues - 1) {
    m_valueIndex++;
  } else if (m_tupleIndex < m_numberOfTuples - 1) {
    m_tupleIndex++;
    loadValues();
  } else {
    m_tupleIndex = m_numberOfTuples;
    m_valueIndex = 0;
  }

  return *this;
}


void PlotStore::DotIterator::loadValues() {
  mp_obj_t tuple = m_tuples[m_tupleIndex];

  mp_obj_t * coordinates;
  mp_obj_get_array_fixed_n(tuple, 2, &coordinates);

  mp_obj_get_array(coordinates[0], &m_numberOfValues, &m_xValues);
  mp_obj_get_array(coordinates[1], &m_numberOfValues, &m_yValues);

  m_valueIndex = 0;
}

void PlotStore::forEachDot(PlotStore::DotCallback callback) {
  size_t numberOfTuples;
  mp_obj_t * tuples;
  mp_obj_list_get(m_dots, &numberOfTuples, &tuples);

  for (size_t t=0; t<numberOfTuples; t++) {
    mp_obj_t tuple = tuples[t];
    mp_obj_t * coordinates;
    mp_obj_get_array_fixed_n(tuple, 2, &coordinates);

    size_t xLength, yLength;
    mp_obj_t * xItems, * yItems;
    mp_obj_get_array(coordinates[0], &xLength, &xItems);
    mp_obj_get_array(coordinates[1], &yLength, &yItems);
    assert(xLength == yLength); // Checked in modpyplot_plot

    for (size_t i=0; i<xLength; i++) {
      PlotStore::Dot dot(
        mp_obj_get_float(xItems[i]),
        mp_obj_get_float(yItems[i]),
        Palette::DataColor[t]
      );

      callback(&dot);
    }
  }
}

}
