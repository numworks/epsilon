#include <assert.h>
#include <float.h>
#include <poincare/test/old/helper.h>
#include <quiz.h>
#include <string.h>

#include <cmath>

#include "distributions/models/distribution/binomial_distribution.h"
#include "distributions/models/distribution/chi_squared_distribution.h"
#include "distributions/models/distribution/exponential_distribution.h"
#include "distributions/models/distribution/fisher_distribution.h"
#include "distributions/models/distribution/geometric_distribution.h"
#include "distributions/models/distribution/hypergeometric_distribution.h"
#include "distributions/models/distribution/normal_distribution.h"
#include "distributions/models/distribution/poisson_distribution.h"
#include "distributions/models/distribution/student_distribution.h"
#include "distributions/models/distribution/uniform_distribution.h"

void assert_parameters_are(Distributions::Distribution* distribution,
                           std::initializer_list<double> parameters) {
  assert(static_cast<int>(parameters.size()) ==
         distribution->numberOfParameters());
  const double* parametersArray = distribution->constParametersArray();
  size_t i = 0;
  for (double param : parameters) {
    quiz_assert(parametersArray[i++] == param);
  }
}

QUIZ_CASE(distributions_parameters_binomial) {
  Distributions::BinomialDistribution distribution;
  assert_parameters_are(&distribution, {20, 0.5});
  constexpr int k_indexOfN = 0;
  constexpr int k_indexOfP = 1;
  // n can be any positive integer
  quiz_assert(!distribution.authorizedParameterAtIndex(-3.0, k_indexOfN));
  quiz_assert(distribution.authorizedParameterAtIndex(0.0, k_indexOfN));
  quiz_assert(distribution.authorizedParameterAtIndex(3.0, k_indexOfN));
  quiz_assert(!distribution.authorizedParameterAtIndex(4.2, k_indexOfN));
  // p can be any value in [0,1]
  quiz_assert(!distribution.authorizedParameterAtIndex(-1.0, k_indexOfP));
  quiz_assert(distribution.authorizedParameterAtIndex(0.0, k_indexOfP));
  quiz_assert(distribution.authorizedParameterAtIndex(0.5, k_indexOfP));
  quiz_assert(distribution.authorizedParameterAtIndex(1.0, k_indexOfP));
  quiz_assert(!distribution.authorizedParameterAtIndex(2.0, k_indexOfP));
}

QUIZ_CASE(distributions_parameters_uniform) {
  Distributions::UniformDistribution distribution;
  assert_parameters_are(&distribution, {-1, 1});
  constexpr int k_indexOfA = 0;
  constexpr int k_indexOfB = 1;
  // a can be any value (if superior to b, setParameterAtIndex will change b)
  quiz_assert(distribution.authorizedParameterAtIndex(0.0, k_indexOfA));
  quiz_assert(distribution.authorizedParameterAtIndex(2.5, k_indexOfA));
  // b must me superior to a
  quiz_assert(distribution.authorizedParameterAtIndex(5.5, k_indexOfB));
  quiz_assert(!distribution.authorizedParameterAtIndex(-2.0, k_indexOfB));

  // When setting a with a value lower than b, b is shifted
  distribution.setParameterAtIndex(2, k_indexOfA);
  assert_parameters_are(&distribution, {2, 3});
  distribution.setParameterAtIndex(400, k_indexOfA);
  assert_parameters_are(&distribution, {400, 404});
  distribution.setParameterAtIndex(22, k_indexOfA);
  assert_parameters_are(&distribution, {22, 404});
}

QUIZ_CASE(distributions_parameters_exponential) {
  Distributions::ExponentialDistribution distribution;
  assert_parameters_are(&distribution, {1});
  constexpr int k_indexOfLambda = 0;
  // λ can be any value in ]0,7500]
  quiz_assert(!distribution.authorizedParameterAtIndex(-1.0, k_indexOfLambda));
  quiz_assert(!distribution.authorizedParameterAtIndex(0.0, k_indexOfLambda));
  quiz_assert(distribution.authorizedParameterAtIndex(0.1, k_indexOfLambda));
  quiz_assert(distribution.authorizedParameterAtIndex(7500.0, k_indexOfLambda));
  quiz_assert(!distribution.authorizedParameterAtIndex(7600, k_indexOfLambda));
}

QUIZ_CASE(distributions_parameters_normal) {
  Distributions::NormalDistribution distribution;
  assert_parameters_are(&distribution, {0, 1});
  constexpr int k_indexOfMu = 0;
  constexpr int k_indexOfSigma = 1;
  // μ can be any value
  quiz_assert(distribution.authorizedParameterAtIndex(-10000.1, k_indexOfMu));
  quiz_assert(distribution.authorizedParameterAtIndex(1234567.89, k_indexOfMu));
  // σ can be any value > max(DBL_MIN,|μ|*1e-6)
  quiz_assert(!distribution.authorizedParameterAtIndex(1e-308, k_indexOfSigma));
  quiz_assert(distribution.authorizedParameterAtIndex(1e-307, k_indexOfSigma));
  distribution.setParameterAtIndex(1e8, k_indexOfMu);
  quiz_assert(!distribution.authorizedParameterAtIndex(20, k_indexOfSigma));
  quiz_assert(distribution.authorizedParameterAtIndex(200, k_indexOfSigma));

  // When setting μ with a value such that |μ| > σ*1e6, σ is shifted
  assert_parameters_are(&distribution, {1e8, 100});
  distribution.setParameterAtIndex(2, k_indexOfMu);
  assert_parameters_are(&distribution, {2, 100});
  distribution.setParameterAtIndex(-3e9, k_indexOfMu);
  assert_parameters_are(&distribution, {-3e9, 3000});

  // μ or σ can be empty but not at the same time
  quiz_assert(distribution.authorizedParameterAtIndex(NAN, k_indexOfMu));
  quiz_assert(distribution.authorizedParameterAtIndex(NAN, k_indexOfSigma));
  distribution.setParameterAtIndex(NAN, k_indexOfMu);
  quiz_assert(distribution.authorizedParameterAtIndex(NAN, k_indexOfMu));
  quiz_assert(!distribution.authorizedParameterAtIndex(NAN, k_indexOfSigma));
  distribution.setParameterAtIndex(1, k_indexOfMu);
  distribution.setParameterAtIndex(NAN, k_indexOfSigma);
  quiz_assert(!distribution.authorizedParameterAtIndex(NAN, k_indexOfMu));
  quiz_assert(distribution.authorizedParameterAtIndex(NAN, k_indexOfSigma));
}

QUIZ_CASE(distributions_parameters_chi_squared) {
  Distributions::ChiSquaredDistribution distribution;
  assert_parameters_are(&distribution, {1});
  constexpr int k_indexOfK = 0;
  // k can be any integer in ]0,31500]
  quiz_assert(!distribution.authorizedParameterAtIndex(-3.0, k_indexOfK));
  quiz_assert(!distribution.authorizedParameterAtIndex(0.0, k_indexOfK));
  quiz_assert(distribution.authorizedParameterAtIndex(3.0, k_indexOfK));
  quiz_assert(!distribution.authorizedParameterAtIndex(4.2, k_indexOfK));
  quiz_assert(distribution.authorizedParameterAtIndex(31500, k_indexOfK));
  quiz_assert(!distribution.authorizedParameterAtIndex(31600, k_indexOfK));
}

QUIZ_CASE(distributions_parameters_student) {
  Distributions::StudentDistribution distribution;
  assert_parameters_are(&distribution, {1});
  constexpr int k_indexOfK = 0;
  // k can be any value in ]DBL_EPSILON,200]
  quiz_assert(!distribution.authorizedParameterAtIndex(0.0, k_indexOfK));
  quiz_assert(distribution.authorizedParameterAtIndex(0.00001, k_indexOfK));
  quiz_assert(distribution.authorizedParameterAtIndex(4.2, k_indexOfK));
  quiz_assert(distribution.authorizedParameterAtIndex(200, k_indexOfK));
  quiz_assert(!distribution.authorizedParameterAtIndex(300, k_indexOfK));
}

QUIZ_CASE(distributions_parameters_geometric) {
  Distributions::GeometricDistribution distribution;
  assert_parameters_are(&distribution, {0.5});
  constexpr int k_indexOfP = 0;
  // p can be any value in ]0,1]
  quiz_assert(!distribution.authorizedParameterAtIndex(-1.0, k_indexOfP));
  quiz_assert(!distribution.authorizedParameterAtIndex(0.0, k_indexOfP));
  quiz_assert(distribution.authorizedParameterAtIndex(0.5, k_indexOfP));
  quiz_assert(distribution.authorizedParameterAtIndex(1.0, k_indexOfP));
  quiz_assert(!distribution.authorizedParameterAtIndex(2.0, k_indexOfP));
}

QUIZ_CASE(distributions_parameters_hypergeometric) {
  Distributions::HypergeometricDistribution distribution;
  assert_parameters_are(&distribution, {100, 60, 50});
  constexpr int k_indexOfN = 0;
  constexpr int k_indexOfK = 1;
  constexpr int k_indexOfn = 2;
  // N can be any integer in ]0,+inf[
  quiz_assert(!distribution.authorizedParameterAtIndex(-3.0, k_indexOfN));
  quiz_assert(!distribution.authorizedParameterAtIndex(0.0, k_indexOfN));
  quiz_assert(distribution.authorizedParameterAtIndex(3.0, k_indexOfN));
  quiz_assert(!distribution.authorizedParameterAtIndex(4.2, k_indexOfN));
  // K can be any integer in [0,N]
  quiz_assert(!distribution.authorizedParameterAtIndex(-3.0, k_indexOfK));
  quiz_assert(distribution.authorizedParameterAtIndex(0.0, k_indexOfK));
  quiz_assert(distribution.authorizedParameterAtIndex(3.0, k_indexOfK));
  quiz_assert(!distribution.authorizedParameterAtIndex(4.2, k_indexOfK));
  quiz_assert(!distribution.authorizedParameterAtIndex(101, k_indexOfK));
  // n can be any integer in [0,N]
  quiz_assert(!distribution.authorizedParameterAtIndex(-3.0, k_indexOfn));
  quiz_assert(distribution.authorizedParameterAtIndex(0.0, k_indexOfn));
  quiz_assert(distribution.authorizedParameterAtIndex(3.0, k_indexOfn));
  quiz_assert(!distribution.authorizedParameterAtIndex(101, k_indexOfn));

  // When setting N with a value lower than K or n, K and n are shifted
  distribution.setParameterAtIndex(55, k_indexOfN);
  assert_parameters_are(&distribution, {55, 55, 50});
  distribution.setParameterAtIndex(10, k_indexOfN);
  assert_parameters_are(&distribution, {10, 10, 10});
  distribution.setParameterAtIndex(20, k_indexOfN);
  assert_parameters_are(&distribution, {20, 10, 10});
}

QUIZ_CASE(distributions_parameters_poisson) {
  Distributions::PoissonDistribution distribution;
  assert_parameters_are(&distribution, {4});
  constexpr int k_indexOfLambda = 0;
  // λ can be any value in ]0,999]
  quiz_assert(!distribution.authorizedParameterAtIndex(-1.0, k_indexOfLambda));
  quiz_assert(!distribution.authorizedParameterAtIndex(0.0, k_indexOfLambda));
  quiz_assert(distribution.authorizedParameterAtIndex(0.5, k_indexOfLambda));
  quiz_assert(distribution.authorizedParameterAtIndex(999.0, k_indexOfLambda));
  quiz_assert(
      !distribution.authorizedParameterAtIndex(1000.0, k_indexOfLambda));
}

QUIZ_CASE(distributions_parameters_fisher) {
  Distributions::FisherDistribution distribution;
  assert_parameters_are(&distribution, {1, 1});
  constexpr int k_indexOfD1 = 0;
  constexpr int k_indexOfD2 = 1;
  // d1 can be any value in ]DBL_MIN, 144]
  quiz_assert(!distribution.authorizedParameterAtIndex(0, k_indexOfD1));
  quiz_assert(distribution.authorizedParameterAtIndex(1e-300, k_indexOfD1));
  quiz_assert(distribution.authorizedParameterAtIndex(100, k_indexOfD1));
  quiz_assert(!distribution.authorizedParameterAtIndex(200, k_indexOfD1));
  // d2 can be any value in ]DBL_MIN, 144]
  quiz_assert(!distribution.authorizedParameterAtIndex(0, k_indexOfD2));
  quiz_assert(distribution.authorizedParameterAtIndex(1e-300, k_indexOfD2));
  quiz_assert(distribution.authorizedParameterAtIndex(100, k_indexOfD2));
  quiz_assert(!distribution.authorizedParameterAtIndex(200, k_indexOfD2));
}
