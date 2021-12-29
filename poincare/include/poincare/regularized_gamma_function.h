#ifndef POINCARE_REGULARIZED_GAMMA_FUNCTION_H
#define POINCARE_REGULARIZED_GAMMA_FUNCTION_H

#include <float.h>

namespace Poincare {

static constexpr int k_maxRegularizedGammaIterations = 1000;
static constexpr double k_regularizedGammaPrecision = DBL_EPSILON;
double RegularizedGammaFunction(double s, double x, double epsilon, int maxNumberOfIterations, double * result);

}

#endif
