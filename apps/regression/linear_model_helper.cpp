#include "linear_model_helper.h"

namespace Regression {

namespace LinearModelHelper {

double Slope(double covariance, double variance) {
  return covariance / variance;
}

double YIntercept(double meanOfY, double meanOfX, double slope) {
  return meanOfY - slope * meanOfX;
}

}

}
