#ifndef POINCARE_JUNIOR_NUMERIC_REGULARIZED_GAMMA_FUNCTION_H
#define POINCARE_JUNIOR_NUMERIC_REGULARIZED_GAMMA_FUNCTION_H

#include <float.h>

namespace PoincareJ {

constexpr static int k_maxRegularizedGammaIterations = 1000;
constexpr static double k_regularizedGammaPrecision = DBL_EPSILON;
double RegularizedGammaFunction(double s, double x, double epsilon,
                                int maxNumberOfIterations, double* result);

}  // namespace PoincareJ

#endif
