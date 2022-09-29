#include "linear_model.h"
#include "../store.h"

using namespace Poincare;

namespace Regression {

void LinearModel::privateFit(Store * store, int series, double * modelCoefficients, Context * context) {
  modelCoefficients[slopeCoefficientIndex()] = store->slope(series);
  modelCoefficients[yInterceptCoefficientIndex()] = store->yIntercept(series);
}

}
