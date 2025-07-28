#include <assert.h>
#include <float.h>
#include <poincare/statistics/distribution.h>
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

namespace Params = Poincare::Distribution::Params;

void assert_parameters_are(Distributions::Distribution* distribution,
                           std::initializer_list<double> parameters) {
  assert(static_cast<int>(parameters.size()) ==
         distribution->numberOfParameters());
  const Poincare::Distribution::ParametersArray<double> parametersArray =
      distribution->constParametersArray();
  size_t i = 0;
  for (double param : parameters) {
    quiz_assert(parametersArray[i++] == param);
  }
}

QUIZ_CASE(distributions_parameters_binomial) {
  Distributions::BinomialDistribution distribution;
  assert_parameters_are(&distribution, {20, 0.5});
  // n can be any positive integer
  quiz_assert(
      !distribution.authorizedParameterAtIndex(-3.0, Params::Binomial::N));
  quiz_assert(
      distribution.authorizedParameterAtIndex(0.0, Params::Binomial::N));
  quiz_assert(
      distribution.authorizedParameterAtIndex(3.0, Params::Binomial::N));
  quiz_assert(
      !distribution.authorizedParameterAtIndex(4.2, Params::Binomial::N));
  // p can be any value in [0,1]
  quiz_assert(
      !distribution.authorizedParameterAtIndex(-1.0, Params::Binomial::P));
  quiz_assert(
      distribution.authorizedParameterAtIndex(0.0, Params::Binomial::P));
  quiz_assert(
      distribution.authorizedParameterAtIndex(0.5, Params::Binomial::P));
  quiz_assert(
      distribution.authorizedParameterAtIndex(1.0, Params::Binomial::P));
  quiz_assert(
      !distribution.authorizedParameterAtIndex(2.0, Params::Binomial::P));
}

QUIZ_CASE(distributions_parameters_uniform) {
  Distributions::UniformDistribution distribution;
  assert_parameters_are(&distribution, {-1, 1});
  // a must be inferior to b
  quiz_assert(distribution.authorizedParameterAtIndex(0.0, Params::Uniform::A));
  quiz_assert(
      !distribution.authorizedParameterAtIndex(2.5, Params::Uniform::A));
  // b must me superior to a
  quiz_assert(distribution.authorizedParameterAtIndex(5.5, Params::Uniform::B));
  quiz_assert(
      !distribution.authorizedParameterAtIndex(-2.0, Params::Uniform::B));

  // When setting a with a value lower than b, b is shifted
  distribution.setParameterAtIndex(2, Params::Uniform::A);
  assert_parameters_are(&distribution, {2, 3});
  distribution.setParameterAtIndex(400, Params::Uniform::A);
  assert_parameters_are(&distribution, {400, 404});
  distribution.setParameterAtIndex(22, Params::Uniform::A);
  assert_parameters_are(&distribution, {22, 404});
}

QUIZ_CASE(distributions_parameters_exponential) {
  Distributions::ExponentialDistribution distribution;
  assert_parameters_are(&distribution, {1});
  // λ can be any value in ]0,7500]
  quiz_assert(!distribution.authorizedParameterAtIndex(
      -1.0, Params::Exponential::Lambda));
  quiz_assert(!distribution.authorizedParameterAtIndex(
      0.0, Params::Exponential::Lambda));
  quiz_assert(distribution.authorizedParameterAtIndex(
      0.1, Params::Exponential::Lambda));
  quiz_assert(distribution.authorizedParameterAtIndex(
      7500.0, Params::Exponential::Lambda));
  quiz_assert(!distribution.authorizedParameterAtIndex(
      7600, Params::Exponential::Lambda));
}

QUIZ_CASE(distributions_parameters_normal) {
  Distributions::NormalDistribution distribution;
  assert_parameters_are(&distribution, {0, 1});
  // μ can be any value
  quiz_assert(
      distribution.authorizedParameterAtIndex(-10000.1, Params::Normal::Mu));
  quiz_assert(
      distribution.authorizedParameterAtIndex(1234567.89, Params::Normal::Mu));
  // σ can be any value > max(DBL_MIN,|μ|*1e-6)
  quiz_assert(
      !distribution.authorizedParameterAtIndex(1e-308, Params::Normal::Sigma));
  quiz_assert(
      distribution.authorizedParameterAtIndex(1e-307, Params::Normal::Sigma));
  distribution.setParameterAtIndex(1e8, Params::Normal::Mu);
  quiz_assert(
      !distribution.authorizedParameterAtIndex(20, Params::Normal::Sigma));
  quiz_assert(
      distribution.authorizedParameterAtIndex(200, Params::Normal::Sigma));

  // When setting μ with a value such that |μ| > σ*1e6, σ is shifted
  assert_parameters_are(&distribution, {1e8, 100});
  distribution.setParameterAtIndex(2, Params::Normal::Mu);
  assert_parameters_are(&distribution, {2, 100});
  distribution.setParameterAtIndex(-3e9, Params::Normal::Mu);
  assert_parameters_are(&distribution, {-3e9, 3000});

  // μ or σ can be empty but not at the same time
  quiz_assert(distribution.authorizedParameterAtIndex(NAN, Params::Normal::Mu));
  quiz_assert(
      distribution.authorizedParameterAtIndex(NAN, Params::Normal::Sigma));
  distribution.setParameterAtIndex(NAN, Params::Normal::Mu);
  quiz_assert(distribution.authorizedParameterAtIndex(NAN, Params::Normal::Mu));
  quiz_assert(
      !distribution.authorizedParameterAtIndex(NAN, Params::Normal::Sigma));
  distribution.setParameterAtIndex(1, Params::Normal::Mu);
  distribution.setParameterAtIndex(NAN, Params::Normal::Sigma);
  quiz_assert(
      !distribution.authorizedParameterAtIndex(NAN, Params::Normal::Mu));
  quiz_assert(
      distribution.authorizedParameterAtIndex(NAN, Params::Normal::Sigma));
}

QUIZ_CASE(distributions_parameters_chi_squared) {
  Distributions::ChiSquaredDistribution distribution;
  assert_parameters_are(&distribution, {1});
  // k can be any integer in ]0,31500]
  quiz_assert(!distribution.authorizedParameterAtIndex(-3.0, Params::Chi2::K));
  quiz_assert(!distribution.authorizedParameterAtIndex(0.0, Params::Chi2::K));
  quiz_assert(distribution.authorizedParameterAtIndex(3.0, Params::Chi2::K));
  quiz_assert(!distribution.authorizedParameterAtIndex(4.2, Params::Chi2::K));
  quiz_assert(distribution.authorizedParameterAtIndex(31500, Params::Chi2::K));
  quiz_assert(!distribution.authorizedParameterAtIndex(31600, Params::Chi2::K));
}

QUIZ_CASE(distributions_parameters_student) {
  Distributions::StudentDistribution distribution;
  assert_parameters_are(&distribution, {1});
  // k can be any value in ]DBL_EPSILON,200]
  quiz_assert(
      !distribution.authorizedParameterAtIndex(0.0, Params::Student::K));
  quiz_assert(
      distribution.authorizedParameterAtIndex(0.00001, Params::Student::K));
  quiz_assert(distribution.authorizedParameterAtIndex(4.2, Params::Student::K));
  quiz_assert(distribution.authorizedParameterAtIndex(200, Params::Student::K));
  quiz_assert(
      !distribution.authorizedParameterAtIndex(300, Params::Student::K));
}

QUIZ_CASE(distributions_parameters_geometric) {
  Distributions::GeometricDistribution distribution;
  assert_parameters_are(&distribution, {0.5});
  // p can be any value in ]0,1]
  quiz_assert(
      !distribution.authorizedParameterAtIndex(-1.0, Params::Geometric::P));
  quiz_assert(
      !distribution.authorizedParameterAtIndex(0.0, Params::Geometric::P));
  quiz_assert(
      distribution.authorizedParameterAtIndex(0.5, Params::Geometric::P));
  quiz_assert(
      distribution.authorizedParameterAtIndex(1.0, Params::Geometric::P));
  quiz_assert(
      !distribution.authorizedParameterAtIndex(2.0, Params::Geometric::P));
}

QUIZ_CASE(distributions_parameters_hypergeometric) {
  Distributions::HypergeometricDistribution distribution;
  assert_parameters_are(&distribution, {100, 60, 50});
  // N can be any integer in [0,+inf[
  quiz_assert(!distribution.authorizedParameterAtIndex(
      -3.0, Params::Hypergeometric::NPop));
  quiz_assert(distribution.authorizedParameterAtIndex(
      0.0, Params::Hypergeometric::NPop));
  quiz_assert(distribution.authorizedParameterAtIndex(
      3.0, Params::Hypergeometric::NPop));
  quiz_assert(!distribution.authorizedParameterAtIndex(
      4.2, Params::Hypergeometric::NPop));
  // K can be any integer in [0,N]
  quiz_assert(!distribution.authorizedParameterAtIndex(
      -3.0, Params::Hypergeometric::K));
  quiz_assert(
      distribution.authorizedParameterAtIndex(0.0, Params::Hypergeometric::K));
  quiz_assert(
      distribution.authorizedParameterAtIndex(3.0, Params::Hypergeometric::K));
  quiz_assert(
      !distribution.authorizedParameterAtIndex(4.2, Params::Hypergeometric::K));
  quiz_assert(
      !distribution.authorizedParameterAtIndex(101, Params::Hypergeometric::K));
  // n can be any integer in [0,N]
  quiz_assert(!distribution.authorizedParameterAtIndex(
      -3.0, Params::Hypergeometric::NSample));
  quiz_assert(distribution.authorizedParameterAtIndex(
      0.0, Params::Hypergeometric::NSample));
  quiz_assert(distribution.authorizedParameterAtIndex(
      3.0, Params::Hypergeometric::NSample));
  quiz_assert(!distribution.authorizedParameterAtIndex(
      101, Params::Hypergeometric::NSample));

  // When setting N with a value lower than K or n, K and n are shifted
  distribution.setParameterAtIndex(55, Params::Hypergeometric::NPop);
  assert_parameters_are(&distribution, {55, 55, 50});
  distribution.setParameterAtIndex(10, Params::Hypergeometric::NPop);
  assert_parameters_are(&distribution, {10, 10, 10});
  distribution.setParameterAtIndex(20, Params::Hypergeometric::NPop);
  assert_parameters_are(&distribution, {20, 10, 10});
}

QUIZ_CASE(distributions_parameters_poisson) {
  Distributions::PoissonDistribution distribution;
  assert_parameters_are(&distribution, {4});
  // λ can be any value in ]0,999]
  quiz_assert(!distribution.authorizedParameterAtIndex(
      -1.0, Params::Exponential::Lambda));
  quiz_assert(!distribution.authorizedParameterAtIndex(
      0.0, Params::Exponential::Lambda));
  quiz_assert(distribution.authorizedParameterAtIndex(
      0.5, Params::Exponential::Lambda));
  quiz_assert(distribution.authorizedParameterAtIndex(
      999.0, Params::Exponential::Lambda));
  quiz_assert(!distribution.authorizedParameterAtIndex(
      1000.0, Params::Exponential::Lambda));
}

QUIZ_CASE(distributions_parameters_fisher) {
  Distributions::FisherDistribution distribution;
  assert_parameters_are(&distribution, {1, 1});
  // d1 can be any value in ]DBL_MIN, 144]
  quiz_assert(!distribution.authorizedParameterAtIndex(0, Params::Fisher::D1));
  quiz_assert(
      distribution.authorizedParameterAtIndex(1e-300, Params::Fisher::D1));
  quiz_assert(distribution.authorizedParameterAtIndex(100, Params::Fisher::D1));
  quiz_assert(
      !distribution.authorizedParameterAtIndex(200, Params::Fisher::D1));
  // d2 can be any value in ]DBL_MIN, 144]
  quiz_assert(!distribution.authorizedParameterAtIndex(0, Params::Fisher::D2));
  quiz_assert(
      distribution.authorizedParameterAtIndex(1e-300, Params::Fisher::D2));
  quiz_assert(distribution.authorizedParameterAtIndex(100, Params::Fisher::D2));
  quiz_assert(
      !distribution.authorizedParameterAtIndex(200, Params::Fisher::D2));
}
