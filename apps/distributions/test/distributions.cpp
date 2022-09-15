#include <assert.h>
#include <float.h>
#include <quiz.h>
#include <string.h>

#include <cmath>

#include <poincare/test/helper.h>
#include "distributions/models/distribution/binomial_distribution.h"
#include "distributions/models/distribution/chi_squared_distribution.h"
#include "distributions/models/distribution/exponential_distribution.h"
#include "distributions/models/distribution/fisher_distribution.h"
#include "distributions/models/distribution/geometric_distribution.h"
#include "distributions/models/distribution/normal_distribution.h"
#include "distributions/models/distribution/poisson_distribution.h"
#include "distributions/models/distribution/student_distribution.h"
#include "distributions/models/distribution/uniform_distribution.h"

void assert_cumulative_distributive_function_direct_and_inverse_is(Distributions::Distribution * distribution, double x, double result) {
  double r = distribution->cumulativeDistributiveFunctionAtAbscissa(x);
  quiz_assert(!std::isnan(r));
  quiz_assert(!std::isinf(r));
  quiz_assert(std::fabs(r-result) < FLT_EPSILON || std::fabs(r-result)/result < FLT_EPSILON);

  r = distribution->cumulativeDistributiveInverseForProbability(result);
  quiz_assert(!std::isnan(r));
  quiz_assert(!std::isinf(r));
  quiz_assert(std::fabs(r-x) < FLT_EPSILON || std::fabs(r-x)/x < FLT_EPSILON);
}

void assert_finite_integral_between_abscissas_is(Distributions::Distribution * distribution, double a, double b, double result) {
  double r = distribution->finiteIntegralBetweenAbscissas(a, b);
  quiz_assert(!std::isnan(r));
  quiz_assert(!std::isinf(r));
  quiz_assert(std::fabs(r-result) < FLT_EPSILON || std::fabs(r-result)/result < FLT_EPSILON);
}

QUIZ_CASE(probability_binomial_distribution) {
  // B(32, 0.6)
  Distributions::BinomialDistribution distribution;
  distribution.setParameterAtIndex(32.0, 0);
  distribution.setParameterAtIndex(0.6, 1);
  assert_roughly_equal<float>(distribution.evaluateAtAbscissa(4), std::pow(1 - 0.6, 32 - 4) * std::pow(0.6, 4) * 35960 /* 32 choose 4 */, 1e-4);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 19.0, 0.53819340022502382137048471122398041188716888427734375);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 10.0, 0.00094770581410635293569122428181117356871254742145538330078125);

  // B(17, 0.1)
  distribution.setParameterAtIndex(17.0, 0);
  distribution.setParameterAtIndex(0.1, 1);
  assert_roughly_equal<float>(distribution.evaluateAtAbscissa(4), std::pow(1 - 0.1, 17 - 4) * std::pow(0.1, 4) * 2380 /* 17 choose 4 */, 1e-5);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 0.0, 0.166771816996665822596668249389040283858776092529296875);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 1.0, 0.4817852491014791294077213024138472974300384521484375);

  // B(21, 0.2)
  distribution.setParameterAtIndex(21.0, 0);
  distribution.setParameterAtIndex(0.2, 1);
  assert_roughly_equal<float>(distribution.evaluateAtAbscissa(4), std::pow(1 - 0.2, 21 - 4) * std::pow(0.2, 4) * 5985 /* 21 choose 4 */, 1e-5);
  assert_finite_integral_between_abscissas_is(&distribution, 4.0, 4.0, 0.21563235015849934848);
  assert_finite_integral_between_abscissas_is(&distribution, 5.0, 4.0, 0.0);
  assert_finite_integral_between_abscissas_is(&distribution, 4.0, 5.0, 0.398919847793223794688);
}


QUIZ_CASE(probability_uniform_distribution) {
  // U(-1, 1)
  Distributions::UniformDistribution distribution;
  distribution.setParameterAtIndex(-1, 0);
  distribution.setParameterAtIndex(1, 1);
  assert_roughly_equal<float>(distribution.evaluateAtAbscissa(4), 0);
  assert_roughly_equal<float>(distribution.evaluateAtAbscissa(.2), 1. / 2.f);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 0.23f, (1.f + .23f) / 2.f);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, -0.9, (1.F - 0.9f) / 2.f);

  // U(0.1, 17)
  distribution.setParameterAtIndex(0.1, 0);
  distribution.setParameterAtIndex(17., 1);
  assert_roughly_equal<float>(distribution.evaluateAtAbscissa(4), 1.f / (17.f - .1f));
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 0.2 * (17. - 0.1) + 0.1, 0.2);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 17, 1);
}

QUIZ_CASE(probability_exponential_distribution) {
  // EXP(1)
  Distributions::ExponentialDistribution distribution;
  distribution.setParameterAtIndex(1, 0);
  assert_roughly_equal<float>(distribution.evaluateAtAbscissa(1), std::exp(-1.f));
  assert_roughly_equal<float>(distribution.evaluateAtAbscissa(14), std::exp(-14.f));
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 0.23f, 1 - std::exp(-.23f));
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 2.f, 1 - std::exp(-2.f));

  // EXP(13)
  distribution.setParameterAtIndex(.2f, 0);
  assert_roughly_equal<float>(distribution.evaluateAtAbscissa(2), .2f * std::exp(-.2f * 2.f));
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 2.3, 1.f - std::exp(-.2f * 2.3f));
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 5.f, 1.f - std::exp(-.2f * 5.f));
}


QUIZ_CASE(probability_poisson_distribution) {
  // POISSON(1)
  Distributions::PoissonDistribution distribution;
  distribution.setParameterAtIndex(1, 0);
  assert_roughly_equal<float>(distribution.evaluateAtAbscissa(1), std::exp(-1.f));
  assert_roughly_equal<float>(distribution.evaluateAtAbscissa(8), std::exp(-1.f) / 40320 /* 8! */);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 1, 0.7357588823428847);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 4, 0.9963401531726563);

  // POISSON(13)
  distribution.setParameterAtIndex(13.0, 0);
  assert_roughly_equal<float>(distribution.evaluateAtAbscissa(8), std::exp(-13.f) * std::pow(13.f, 8.f) / 40320 /* 8! */, 1e-6);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 4, 0.00374018590580994);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 16, 0.8354931476833607);
}


QUIZ_CASE(probability_chi_squared_distribution) {
  // Chi Squared distribution with 1 degree of freedom
  Distributions::ChiSquaredDistribution distribution;
  distribution.setParameterAtIndex(1.0, 0);
  assert_roughly_equal<float>(distribution.evaluateAtAbscissa(0.1), 1.2f, 1e-4);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 1.3, 0.7457867763960355222963016785797663033008575439453125);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 2.9874567, 0.91608813616541218127764523160294629633426666259765625);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 4.987, 0.97446155200631878745554104170878417789936065673828125);

  // Chi Squared distribution with 1.3 degrees of freedom
  distribution.setParameterAtIndex(1.3, 0);
  assert_roughly_equal<float>(distribution.evaluateAtAbscissa(0.1), 0.98f, 1e-5);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 1.3, 0.66209429400375563457004091105773113667964935302734375);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 2.9874567, 0.878804860316481750714956433512270450592041015625);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 4.987, 0.96090315938362369507785842870362102985382080078125);

  // Chi Squared distribution with 5.4 degrees of freedom
  distribution.setParameterAtIndex(5.4, 0);
  assert_roughly_equal<float>(distribution.evaluateAtAbscissa(0.1), 0.001890880662848345f, 1e-6);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 1.3, 0.047059684573231390369851823152202996425330638885498046875);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 2.9874567, 0.250530060451470470983537097708904184401035308837890625);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 4.987, 0.53051693435084168459781039928202517330646514892578125);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 20, 0.9982594816610862);

  // Chi Squared distribution with 6 degrees of freedom
  distribution.setParameterAtIndex(6.0, 0);
  assert_roughly_equal<float>(distribution.evaluateAtAbscissa(0.1), 0.0005945183903129464f, 1e-6);
  assert_finite_integral_between_abscissas_is(&distribution, 4.0, 4.0, 0.0);
  assert_finite_integral_between_abscissas_is(&distribution, 5.0, 4.0, 0.0);
  assert_finite_integral_between_abscissas_is(&distribution, 4.0, 5.0, 0.1328633002997339414718);
}

QUIZ_CASE(probability_student_distribution) {
  // Student distribution with 1 degree of freedom
  Distributions::StudentDistribution distribution;
  distribution.setParameterAtIndex(1.0, 0);
  assert_roughly_equal<float>(distribution.evaluateAtAbscissa(0.1), 0.31515830315226806f);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, -2.0, 0.1475836176504332741754010762247405259511345238869178945999223128627);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 0.0, 0.5);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 3.4567, 0.9103622230304611040452300585457123816013336181640625);

  // Student distribution with 1.3 degrees of freedom
  distribution.setParameterAtIndex(1.3, 0);
  assert_roughly_equal<float>(distribution.evaluateAtAbscissa(0.1), 0.33047714482602075f);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, -4.987, 0.041958035438260647687069848643659497611224651336669921875);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 1.3, 0.8111769751474751100062121622613631188869476318359375);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 2.9874567, 0.92143912473359812498330256858025677502155303955078125);

  // Student distribution with 5.4 degrees of freedom
  distribution.setParameterAtIndex(5.4, 0);
  assert_roughly_equal<float>(distribution.evaluateAtAbscissa(0.1), 0.3787417926621869f, 1e-6);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, -4.987, 0.00167496657737900025118837898929768925881944596767425537109375);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 1.3, 0.876837383157582639370275501278229057788848876953125);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 2.9874567, 0.98612148076325445433809591122553683817386627197265625);

  // Student distribution with 6 degrees of freedom
  distribution.setParameterAtIndex(6.0, 0);
  assert_roughly_equal<float>(distribution.evaluateAtAbscissa(0.1), 0.38050851123793844f);
  assert_finite_integral_between_abscissas_is(&distribution, 4.0, 4.0, 0.0);
  assert_finite_integral_between_abscissas_is(&distribution, 5.0, 4.0, 0.0);
  assert_finite_integral_between_abscissas_is(&distribution, 4.0, 5.0, 0.002333318101494775250);
}

QUIZ_CASE(probability_geometric_distribution) {
  // Geometric distribution with probability of success 0.5
  Distributions::GeometricDistribution distribution;
  distribution.setParameterAtIndex(0.5, 0);
  assert_roughly_equal<float>(distribution.evaluateAtAbscissa(3), 1.f / 8.f);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 1.0, 0.5);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 2.0, 0.75);

  // Geometric distribution with probability of success 0.2
  distribution.setParameterAtIndex(0.2, 0);
  assert_roughly_equal<float>(distribution.evaluateAtAbscissa(4), 0.1024);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 6.0, 0.737856);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 2.0, 0.36);

  // Geometric distribution with probability of success 0.4
  distribution.setParameterAtIndex(0.4, 0);
  assert_roughly_equal<float>(distribution.evaluateAtAbscissa(4), 0.0864, 1e-6);
  assert_finite_integral_between_abscissas_is(&distribution, 1.0, 1.0, 0.4);
  assert_finite_integral_between_abscissas_is(&distribution, 2.0, 1.0, 0.0);
  assert_finite_integral_between_abscissas_is(&distribution, 2.0, 3.0, 0.384);
}

QUIZ_CASE(probability_normal_distribution) {
  // N(0, 1)
  Distributions::NormalDistribution distribution;
  distribution.setParameterAtIndex(0, 0);
  distribution.setParameterAtIndex(1, 1);
  assert_roughly_equal<float>(distribution.evaluateAtAbscissa(1), 0.24197072451914337);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 0.2, 0.579259709439103);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 1.6448536269514722, 0.95);

  // N(123, 10.3)
  distribution.setParameterAtIndex(123, 0);
  distribution.setParameterAtIndex(10.3, 1);
  assert_roughly_equal<float>(distribution.evaluateAtAbscissa(103), 0.005879554726599732);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 123, 0.5);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 143, 0.9739161667561312);

  // N(-1.2, 0.3)
  distribution.setParameterAtIndex(-1.2, 0);
  distribution.setParameterAtIndex(0.3, 1);
  assert_roughly_equal<float>(distribution.evaluateAtAbscissa(-1), 1.0648266850745078, 1e-6);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, -1, 0.7475074624530771);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, -2, 0.0038303805675897287);
  assert_finite_integral_between_abscissas_is(&distribution, -1., 0, 0.2524608663050898);
}

QUIZ_CASE(probability_fisher_distribution) {
  // Fisher distribution with d1 = 1 and d2 = 1
  Distributions::FisherDistribution distribution;
  distribution.setParameterAtIndex(1.0, 0);
  distribution.setParameterAtIndex(1.0, 1);
  assert_roughly_equal<float>(distribution.evaluateAtAbscissa(1), 0.15915494309189535);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 1.7, 0.5834784097728860);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 3.3, 0.6796445888753);

  // Fisher distribution with d1 = 3 and d2 = 2
  distribution.setParameterAtIndex(3.0, 0);
  distribution.setParameterAtIndex(2.0, 1);
  assert_roughly_equal<float>(distribution.evaluateAtAbscissa(1), 0.27885480092693415);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 2.0, 0.6495190528383);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 3.0, 0.7400733003272);

  // Fisher distribution with d1 = 100 and d2 = 87
  distribution.setParameterAtIndex(100.0, 0);
  distribution.setParameterAtIndex(87.0, 1);
  /* Expected precision needs to be reduced from Poincare::Float<T>::Epsilon()
   * to 1e-6 for this test to pass on device. */
  assert_roughly_equal<float>(distribution.evaluateAtAbscissa(1.0), 1.9189567194868620, 1e-6);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 0.6, 0.00688477308162587);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 1.4, 0.94560850441205857);
  assert_cumulative_distributive_function_direct_and_inverse_is(&distribution, 1.425, 0.95425004959692871775);

  // Fisher distribution with d1 = 4 and d2 = 2
  distribution.setParameterAtIndex(4.0, 0);
  distribution.setParameterAtIndex(2.0, 1);
  assert_roughly_equal<float>(distribution.evaluateAtAbscissa(1), 0.2962962962962962);
  assert_finite_integral_between_abscissas_is(&distribution, 1.0, 1.0, 0.0);
  assert_finite_integral_between_abscissas_is(&distribution, 2.0, 1.0, 0.0);
  assert_finite_integral_between_abscissas_is(&distribution, 1.0, 2.0, 0.19555555555555555);
}

