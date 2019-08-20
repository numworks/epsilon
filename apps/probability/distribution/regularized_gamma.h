#ifndef PROBABILITY_REGULARIZED_GAMMA_H
#define PROBABILITY_REGULARIZED_GAMMA_H

/* regularizedGamma(s,x) = int(e^-t * t^(s-1), t, 0, x) / gamma(s)
 * with :
 * regularizedGamma(s, 0) = 0
 * regularizedGamma(s, inf) = 1 */

bool regularizedGamma(double s, double x, double epsilon, int maxNumberOfIterations, double * result);

#endif

