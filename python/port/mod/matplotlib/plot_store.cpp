#include "plot_store.h"

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

PlotStore::Dot PlotStore::dotAtIndex(int i) {
  size_t numberOfTuples;
  mp_obj_t * tuples;
  mp_obj_list_get(m_dots, &numberOfTuples, &tuples);

  mp_obj_t firstTuple = tuples[0];
  size_t numberOfObjects;
  mp_obj_t * objects;
  mp_obj_tuple_get(firstTuple, &numberOfObjects, &objects);

  mp_obj_t x = objects[0];
  mp_obj_t y = objects[1];

  size_t numberOfX;
  mp_obj_t * xValues;
  mp_obj_list_get(x, &numberOfX, &xValues);
  size_t numberOfY;
  mp_obj_t * yValues;
  mp_obj_list_get(y, &numberOfY, &yValues);

  float goodX = mp_obj_get_float(xValues[i]);
  float goodY = mp_obj_get_float(yValues[i]);

  return PlotStore::Dot(goodX, goodY, KDColorRed);
}

int PlotStore::numberOfDots() {
  size_t numberOfTuples;
  mp_obj_t * tuples;
  mp_obj_list_get(m_dots, &numberOfTuples, &tuples);

  mp_obj_t firstTuple = tuples[0];
  size_t numberOfObjects;
  mp_obj_t * objects;
  mp_obj_tuple_get(firstTuple, &numberOfObjects, &objects);

  mp_obj_t x = objects[0];

  size_t numberOfX;
  mp_obj_t * xValues;
  mp_obj_list_get(x, &numberOfX, &xValues);

  return numberOfX;
}

}
