/*
 * zlib License
 *
 * Regularized Incomplete Beta Function
 *
 * Copyright (c) 2016, 2017 Lewis Van Winkle
 * http://CodePlea.com
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

// WARNING: this code has been modified

#include <math.h>
#include <poincare/regularized_incomplete_beta_function.h>

#include <cmath>

namespace Poincare {

#define STOP 1.0e-8
#define TINY 1.0e-30

double RegularizedIncompleteBetaFunction(double a, double b, double x) {
  if (x < 0.0 || x > 1.0) return NAN;

  /*The continued fraction converges nicely for x < (a+1)/(a+b+2)*/
  if (x > (a + 1.0) / (a + b + 2.0)) {
    return (1.0 -
            RegularizedIncompleteBetaFunction(
                b, a, 1.0 - x)); /*Use the fact that beta is symmetrical.*/
  }

  /*Find the first part before the continued fraction.*/
  const double lbeta_ab = std::lgamma(a) + std::lgamma(b) - std::lgamma(a + b);
  const double front =
      std::exp(std::log(x) * a + std::log(1.0 - x) * b - lbeta_ab) / a;

  /*Use Lentz's algorithm to evaluate the continued fraction.*/
  double f = 1.0, c = 1.0, d = 0.0;

  // TODO Use Helper::ContinuedFractionEvaluation
  int i, m;
  for (i = 0; i <= 200; ++i) {
    m = i / 2;

    double numerator;
    if (i == 0) {
      numerator = 1.0; /*First numerator is 1.0.*/
    } else if (i % 2 == 0) {
      numerator = (m * (b - m) * x) /
                  ((a + 2.0 * m - 1.0) * (a + 2.0 * m)); /*Even term.*/
    } else {
      numerator = -((a + m) * (a + b + m) * x) /
                  ((a + 2.0 * m) * (a + 2.0 * m + 1)); /*Odd term.*/
    }

    /*Do an iteration of Lentz's algorithm.*/
    d = 1.0 + numerator * d;
    if (std::fabs(d) < TINY) d = TINY;
    d = 1.0 / d;

    c = 1.0 + numerator / c;
    if (std::fabs(c) < TINY) c = TINY;

    const double cd = c * d;
    f *= cd;

    /*Check for stop.*/
    if (std::fabs(1.0 - cd) < STOP) {
      return front * (f - 1.0);
    }
  }

  return NAN; /*Needed more loops, did not converge.*/
}

}  // namespace Poincare
