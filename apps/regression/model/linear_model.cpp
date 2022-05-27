#include "linear_model.h"
#include "../store.h"

using namespace Poincare;

namespace Regression {

void LinearModel::privateFit(Store * store, int series, double * modelCoefficients, Poincare::Context * context) {
  modelCoefficients[0] = store->slope(series);
  modelCoefficients[1] = store->yIntercept(series);
}

}
