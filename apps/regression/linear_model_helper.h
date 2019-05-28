#ifndef REGRESSION_LINEAR_MODEL_HELPER
#define REGRESSION_LINEAR_MODEL_HELPER

namespace Regression {

namespace LinearModelHelper {

double Slope(double covariance, double variance);
double YIntercept(double meanOfY, double meanOfX, double slope);

}

}

#endif
