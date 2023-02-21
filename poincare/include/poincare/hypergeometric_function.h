#ifndef POINCARE_HYPERGEOMETRIC_FUNCTION_H
#define POINCARE_HYPERGEOMETRIC_FUNCTION_H

/* This code can be used to compute the Student distribution for |x| < root(k).
 * We do not use it because we want to cover more x, but we keep in case we need
 * it later.  */

/* hypergeometricFunction(a, b, c, z) = 2F1(a, b, c, z)
 *           an * bn    z^n
 *    = sum( -------- * --- )
 *             cn        n!
 *
 * with :
 * an = 1 if n = 0
 * an = (a*(a+1)*...*(a+n-1) otherwise */

namespace Poincare {

bool HypergeometricFunction(double a, double b, double c, double z,
                            double epsilon, int maxNumberOfIterations,
                            double* result);

}

#endif
