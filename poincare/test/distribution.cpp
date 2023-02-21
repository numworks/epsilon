#include <poincare/chi2_distribution.h>
#include <poincare/normal_distribution.h>
#include <poincare/student_distribution.h>

#include <algorithm>
#include <cmath>

#include "helper.h"

using namespace Poincare;

struct DistributionTestCase {
  DistributionTestCase(double K, double X, double d, double p,
                       float precision = 1e-7)
      : k(K), x(X), density(d), probability(p), precision(precision) {}
  double k;
  double x;
  double density;
  double probability;
  double precision;  // Lower precision for extreme case (x or k "big")
};

constexpr int k_numberOfStudentTestCases = 79;

DistributionTestCase getStudentTestCase(int i) {
  static DistributionTestCase s_studentTests[k_numberOfStudentTestCases]{
      DistributionTestCase(0.28, -10, 0.0051917760715735, 0.1857118516896359),
      DistributionTestCase(1, -10, 0.0031515830315226806, 0.03172551743055356),
      DistributionTestCase(2.57, -10, 0.0005030925333730742,
                           0.001996706792893251),
      DistributionTestCase(3, -10, 0.0003118082168470876,
                           0.0010641995292070756),
      DistributionTestCase(4, -10, 0.00010879223472902223,
                           0.00028100181135799556),
      DistributionTestCase(5, -10, 4.098981641534331e-05,
                           8.547378787148179e-05),
      DistributionTestCase(6, -10, 1.6514084372279296e-05,
                           2.8959913774768124e-05),
      DistributionTestCase(7, -10, 7.051932309805197e-06,
                           1.069710144538641e-05),
      DistributionTestCase(10, -10, 7.284685722912541e-07,
                           7.947765877982051e-07),
      DistributionTestCase(20, -10, 2.6600849800550903e-09,
                           1.5818908793571965e-09),
      DistributionTestCase(50, -10, 2.7048721527008514e-13,
                           8.038667344167695e-14, 1e-3),
      DistributionTestCase(0.28, -1.0, 0.08462111434249496,
                           0.34697350341129507),
      DistributionTestCase(1, -1.0, 0.15915494309189537, 0.24999999999999978),
      DistributionTestCase(2.57, -1.0, 0.20174670047005813,
                           0.20098201301585422),
      DistributionTestCase(3, -1.0, 0.20674833578317203, 0.19550110947788527),
      DistributionTestCase(4, -1.0, 0.21466252583997983, 0.1869504831500295),
      DistributionTestCase(5, -1.0, 0.2196797973509806, 0.18160873382456127),
      DistributionTestCase(6, -1.0, 0.2231422909165263, 0.17795884187479102),
      DistributionTestCase(7, -1.0, 0.2256749202754575, 0.17530833141010374),
      DistributionTestCase(10, -1.0, 0.2303619892291386, 0.17044656615103004),
      DistributionTestCase(20, -1.0, 0.23604564912670092, 0.1646282885858545),
      DistributionTestCase(50, -1.0, 0.239571062058688, 0.16106282255012236),
      DistributionTestCase(500, -1.0, 0.2417289553160677, 0.15889710363030426),
      DistributionTestCase(0.28, -0.2, 0.20549304829293147, 0.45651086361470994,
                           1e-5),
      DistributionTestCase(1, -0.2, 0.3060671982536449, 0.43716704181099886,
                           1e-5),
      DistributionTestCase(2.57, -0.2, 0.35287094524284984, 0.4281163661715596),
      DistributionTestCase(3, -0.2, 0.35794379463845566, 0.4271351646231395),
      DistributionTestCase(4, -0.2, 0.3657866349659307, 0.4256185070684612),
      DistributionTestCase(5, -0.2, 0.3706399777139695, 0.42468025699791445),
      DistributionTestCase(6, -0.2, 0.3739346777416558, 0.42404349575697065),
      DistributionTestCase(7, -0.2, 0.37631593526996343, 0.42358337574489896),
      DistributionTestCase(10, -0.2, 0.3806581810544492, 0.42274459569017275),
      DistributionTestCase(20, -0.2, 0.38580918607411935, 0.42175008348394183,
                           1e-5),
      DistributionTestCase(50, -0.2, 0.38894005621150207, 0.4211459041217918),
      DistributionTestCase(500, -0.2, 0.39083190132875784, 0.42078094851278847,
                           1e-5),
      DistributionTestCase(0.28, 0, 0.22382674581166703, 0.5),
      DistributionTestCase(1, 0, 0.31830988618379075, 0.5),
      DistributionTestCase(2.57, 0, 0.36273426411692106, 0.5),
      DistributionTestCase(3, 0, 0.36755259694786135, 0.5),
      DistributionTestCase(4, 0, 0.37500000000000006, 0.5),
      DistributionTestCase(5, 0, 0.3796066898224944, 0.5),
      DistributionTestCase(6, 0, 0.3827327723098717, 0.5),
      DistributionTestCase(7, 0, 0.38499145083226727, 0.5),
      DistributionTestCase(10, 0, 0.3891083839660311, 0.5),
      DistributionTestCase(20, 0, 0.3939885857114327, 0.5),
      DistributionTestCase(50, 0, 0.39695267973111026, 0.5),
      DistributionTestCase(500, 0, 0.39874285925361797, 0.5),
      DistributionTestCase(0.28, 0.3, 0.18725956804685212, 0.5631515834389194),
      DistributionTestCase(1, 0.3, 0.2920274185172392, 0.5927735790777423),
      DistributionTestCase(2.57, 0.3, 0.3411186663028198, 0.6066171520733878),
      DistributionTestCase(3, 0.3, 0.34645357427454176, 0.6081183539800405),
      DistributionTestCase(4, 0.3, 0.35470962734618916, 0.6104392858612702),
      DistributionTestCase(5, 0.3, 0.3598243283490097, 0.6118754788683627),
      DistributionTestCase(6, 0.3, 0.3632992517027709, 0.6128503895410208),
      DistributionTestCase(7, 0.3, 0.3658123007801872, 0.6135549747989328),
      DistributionTestCase(10, 0.3, 0.37039846155274564, 0.6148396962171008),
      DistributionTestCase(20, 0.3, 0.375845416768084, 0.6163634983781199),
      DistributionTestCase(50, 0.3, 0.37916002269688465, 0.6172895598723485),
      DistributionTestCase(500, 0.3, 0.3811644097585056, 0.6178490820245964,
                           1e-5),
      DistributionTestCase(0.28, 2.5, 0.02982281844596167, 0.7270964204989578),
      DistributionTestCase(1, 2.5, 0.04390481188741942, 0.8788810584091566),
      DistributionTestCase(2.57, 2.5, 0.04014743299467839, 0.949091198755132),
      DistributionTestCase(3, 2.5, 0.0386614857271673, 0.9561466764959673),
      DistributionTestCase(4, 2.5, 0.03567562436955666, 0.966616727594006),
      DistributionTestCase(5, 2.5, 0.03332623888702283, 0.9727549503288119),
      DistributionTestCase(6, 2.5, 0.031473766397140154, 0.9767358838579163),
      DistributionTestCase(7, 2.5, 0.02999022558989219, 0.9795038907071236),
      DistributionTestCase(10, 2.5, 0.026938727628244466, 0.9842765778816956),
      DistributionTestCase(20, 2.5, 0.02266944371914488, 0.9893832272804338),
      DistributionTestCase(50, 2.5, 0.019694702081706403, 0.99212752086344),
      DistributionTestCase(500, 2.5, 0.01775159704911948, 0.9936307122495581),
      DistributionTestCase(0.28, 18, 0.0024496510018582997, 0.8424460486448383),
      DistributionTestCase(1, 18, 0.0009794150344116638, 0.9823342771118654),
      DistributionTestCase(2.57, 18, 6.36607184320786e-05, 0.9995513677800985),
      DistributionTestCase(3, 18, 3.093616673241826e-05, 0.9998130098719767),
      DistributionTestCase(4, 18, 6.1588075963812405e-06, 0.999972000674229),
      DistributionTestCase(5, 18, 1.3324661240937334e-06, 0.9999951397795279,
                           1e-5),
      DistributionTestCase(6, 18, 3.1018888949841117e-07, 0.9999990543824999,
                           1e-5),
      DistributionTestCase(7, 18, 7.700718899661956e-08, 0.999999798186122,
                           1e-3),
  };
  return s_studentTests[i];
}

QUIZ_CASE(poincare_student_distribution) {
  for (int i = 0; i < k_numberOfStudentTestCases; i++) {
    DistributionTestCase t = getStudentTestCase(i);
    // double
    assert_roughly_equal(StudentDistribution::EvaluateAtAbscissa(t.x, t.k),
                         t.density, t.precision);
    assert_roughly_equal(
        StudentDistribution::CumulativeDistributiveFunctionAtAbscissa(t.x, t.k),
        t.probability, t.precision);
    assert_roughly_equal(
        StudentDistribution::CumulativeDistributiveInverseForProbability(
            t.probability, t.k),
        t.x, t.precision / 10);

    // floats
    assert_roughly_equal<float>(
        StudentDistribution::EvaluateAtAbscissa<float>(t.x, t.k), t.density,
        std::sqrt(t.precision));
    assert_roughly_equal<float>(
        StudentDistribution::CumulativeDistributiveFunctionAtAbscissa<float>(
            t.x, t.k),
        t.probability, std::sqrt(t.precision));
    assert_roughly_equal<float>(
        StudentDistribution::CumulativeDistributiveInverseForProbability<float>(
            t.probability, t.k),
        t.x, std::sqrt(t.precision));
  }

  // Special cases p=0
  // double
  assert_roughly_equal<double>(
      StudentDistribution::EvaluateAtAbscissa<double>(INFINITY, 5.), 0);
  assert_roughly_equal<double>(
      StudentDistribution::EvaluateAtAbscissa<double>(-INFINITY, 5.), 0);
  assert_roughly_equal<double>(
      StudentDistribution::CumulativeDistributiveFunctionAtAbscissa<double>(
          INFINITY, 3.),
      1.);
  assert_roughly_equal<double>(
      StudentDistribution::CumulativeDistributiveFunctionAtAbscissa<double>(
          -INFINITY, 3.),
      0.);
  assert_roughly_equal<double>(
      StudentDistribution::CumulativeDistributiveInverseForProbability<double>(
          0, 5.),
      -INFINITY);
  assert_roughly_equal<double>(
      StudentDistribution::CumulativeDistributiveInverseForProbability<double>(
          1, 5.),
      INFINITY);

  // floats
  assert_roughly_equal<float>(
      StudentDistribution::EvaluateAtAbscissa<float>(INFINITY, 5.), 0);
  assert_roughly_equal<float>(
      StudentDistribution::EvaluateAtAbscissa<float>(-INFINITY, 5.), 0);
  assert_roughly_equal<float>(
      StudentDistribution::CumulativeDistributiveFunctionAtAbscissa<float>(
          INFINITY, 3.),
      1.);
  assert_roughly_equal<float>(
      StudentDistribution::CumulativeDistributiveFunctionAtAbscissa<float>(
          -INFINITY, 3.),
      0.);
  assert_roughly_equal<float>(
      StudentDistribution::CumulativeDistributiveInverseForProbability<float>(
          0, 5.),
      -INFINITY);
  assert_roughly_equal<float>(
      StudentDistribution::CumulativeDistributiveInverseForProbability<float>(
          1, 5.),
      INFINITY);
}

constexpr int k_numberOfChi2TestCases = 40;

DistributionTestCase getChi2TestCase(int i) {
  static DistributionTestCase s_chi2Tests[k_numberOfChi2TestCases]{
      DistributionTestCase(0.28, 0, 0, 0.0),
      DistributionTestCase(0.28, 0.123, 0.7735483671408104, 0.7173556658305531),
      DistributionTestCase(0.28, 0.25, 0.39445708636423615, 0.7863197131812323,
                           1e-6),
      DistributionTestCase(0.28, 0.7, 0.1299354327320162, 0.8857276344687262,
                           1e-6),
      DistributionTestCase(0.28, 2, 0.02750012716139315, 0.9650437489682855,
                           1e-6),
      DistributionTestCase(0.28, 10, 0.000126195122829492, 0.999780321882247,
                           1e-6),
      DistributionTestCase(1, 0, 0, 0.0),
      DistributionTestCase(1, 0.123, 1.0696665117153819, 0.2741967471949104),
      DistributionTestCase(1, 0.25, 0.704130653528599, 0.3829249225480261),
      DistributionTestCase(1, 0.7, 0.3360144677267704, 0.5972163057535244),
      DistributionTestCase(1, 2, 0.10377687435514868, 0.8427007929497151),
      DistributionTestCase(1, 10, 0.0008500366602520334, 0.9984345977419975),
      DistributionTestCase(2.57, 0, 0.0, 0.0),
      DistributionTestCase(2.57, 0.123, 0.2360000701221212,
                           0.023209437129391372, 1e-6),
      DistributionTestCase(2.57, 0.25, 0.27109679181024543,
                           0.055740896473783726, 1e-6),
      DistributionTestCase(2.57, 0.7, 0.29030075052193394, 0.1851697163773158,
                           1e-6),
      DistributionTestCase(2.57, 2, 0.20440774762468358, 0.5118671076373665),
      DistributionTestCase(2.57, 10, 0.005922783752791853, 0.987553452753085),
      DistributionTestCase(3, 0, 0.0, 0.0),
      DistributionTestCase(3, 0.123, 0.13156898094099193, 0.011058785312926507),
      DistributionTestCase(3, 0.25, 0.17603266338214976, 0.030859595783726757),
      DistributionTestCase(3, 0.7, 0.23521012740873926, 0.12679605093604587),
      DistributionTestCase(3, 2, 0.2075537487102974, 0.42759329552912023),
      DistributionTestCase(3, 10, 0.008500366602520343, 0.9814338645369568,
                           1e-6),
      DistributionTestCase(5, 0, 0.0, 0.0),
      DistributionTestCase(5, 0.123, 0.005394328218580669,
                           0.0002701288757651666, 1e-6),
      DistributionTestCase(5, 0.25, 0.014669388615179144, 0.001520818553368441,
                           1e-6),
      DistributionTestCase(5, 0.7, 0.054882363062039155, 0.01703132481196757,
                           1e-6),
      DistributionTestCase(5, 2, 0.1383691658068649, 0.15085496391539038, 1e-6),
      DistributionTestCase(5, 10, 0.028334555341734475, 0.9247647538534878,
                           1e-6),
      DistributionTestCase(5, 30.85618994044592, 4.5442869596536075e-06, .99999,
                           1e-6),
      DistributionTestCase(10, 0, 0.0, 0.0),
      DistributionTestCase(10, 0.25, 4.488611361615981e-06,
                           2.2919102136524132e-07),
      DistributionTestCase(10, 0.7, 0.00022030678429878006,
                           3.273617475313252e-05, 1e-6),
      DistributionTestCase(10, 2, 0.00766415502440505, 0.003659846827343713,
                           1e-6),
      DistributionTestCase(20, 0, 0.0, 0.0),
      DistributionTestCase(20, 5.3, 0.0006273926980786215,
                           0.0004347478563383611, 1e-6),
      DistributionTestCase(20, 20, 0.06255501786056658, 0.5420702855281478,
                           1e-5),
      DistributionTestCase(100, 0, 0.0, 0.0),
      DistributionTestCase(100, 100, 0.028162503162596778, 0.5188083154720433,
                           1e-6),
  };
  return s_chi2Tests[i];
}

QUIZ_CASE(probability_chi2_law) {
  for (unsigned int i = 0; i < k_numberOfChi2TestCases; i++) {
    DistributionTestCase t = getChi2TestCase(i);
    // double
    assert_roughly_equal(Chi2Distribution::EvaluateAtAbscissa(t.x, t.k),
                         t.density, t.precision);
    assert_roughly_equal(
        Chi2Distribution::CumulativeDistributiveFunctionAtAbscissa(t.x, t.k),
        t.probability, t.precision);
    assert_roughly_equal(
        Chi2Distribution::CumulativeDistributiveInverseForProbability(
            t.probability, t.k),
        t.x, t.precision / 10);

    // floats
    assert_roughly_equal<float>(
        Chi2Distribution::EvaluateAtAbscissa<float>(t.x, t.k), t.density,
        sqrt(t.precision));
    assert_roughly_equal<float>(
        Chi2Distribution::CumulativeDistributiveFunctionAtAbscissa<float>(t.x,
                                                                          t.k),
        t.probability, sqrt(t.precision));
    assert_roughly_equal<float>(
        Chi2Distribution::CumulativeDistributiveInverseForProbability<float>(
            t.probability, t.k),
        t.x, sqrt(t.precision));
  }
  // Special cases
  // double
  quiz_assert(std::isnan(Chi2Distribution::EvaluateAtAbscissa<double>(-1, 5.)));
  assert_roughly_equal<double>(
      Chi2Distribution::EvaluateAtAbscissa<double>(INFINITY, 5.), 0);
  assert_roughly_equal<double>(
      Chi2Distribution::EvaluateAtAbscissa<double>(0, 2.), 0);
  assert_roughly_equal<double>(
      Chi2Distribution::CumulativeDistributiveFunctionAtAbscissa<double>(
          INFINITY, 3.),
      1.);
  assert_roughly_equal<double>(
      Chi2Distribution::CumulativeDistributiveFunctionAtAbscissa<double>(0, 3.),
      0.);
  assert_roughly_equal<double>(
      Chi2Distribution::CumulativeDistributiveFunctionAtAbscissa<double>(0, .3),
      0.);
  assert_roughly_equal<double>(
      Chi2Distribution::CumulativeDistributiveInverseForProbability<double>(0,
                                                                            5.),
      0.);
  assert_roughly_equal<double>(
      Chi2Distribution::CumulativeDistributiveInverseForProbability<double>(1,
                                                                            5.),
      INFINITY);

  // floats
  quiz_assert(std::isnan(Chi2Distribution::EvaluateAtAbscissa<float>(-1, 5.)));
  assert_roughly_equal<float>(
      Chi2Distribution::EvaluateAtAbscissa<float>(INFINITY, 5.f), 0);
  assert_roughly_equal<float>(
      Chi2Distribution::EvaluateAtAbscissa<float>(0, 2.f), 0);
  assert_roughly_equal<float>(
      Chi2Distribution::CumulativeDistributiveFunctionAtAbscissa<float>(
          INFINITY, 3.f),
      1.f);
  assert_roughly_equal<float>(
      Chi2Distribution::CumulativeDistributiveFunctionAtAbscissa<float>(0, 3.f),
      0.f);
  assert_roughly_equal<float>(
      Chi2Distribution::CumulativeDistributiveFunctionAtAbscissa<float>(0, .3f),
      0.f);
  assert_roughly_equal<float>(
      Chi2Distribution::CumulativeDistributiveInverseForProbability<float>(0,
                                                                           5.f),
      0.f);
  assert_roughly_equal<float>(
      Chi2Distribution::CumulativeDistributiveInverseForProbability<float>(1,
                                                                           5.f),
      INFINITY);
}

QUIZ_CASE(poincare_normal_distribution_find_parameters) {
  NormalDistribution normalDistribution;
  // Compute mu
  double parameters1[2] = {NAN, 3.};
  assert_roughly_equal<double>(
      normalDistribution.evaluateParameterForProbabilityAndBound(
          0, parameters1, 0.75, 2., true),
      -0.02347, 1.e-3);
  assert_roughly_equal<double>(
      normalDistribution.evaluateParameterForProbabilityAndBound(
          0, parameters1, 0.75, 2., false),
      4.023, 1.e-3);

  // Compute sigma
  double parameters2[2] = {1., NAN};
  assert_roughly_equal<double>(
      normalDistribution.evaluateParameterForProbabilityAndBound(
          1, parameters2, 0.8, -3., false),
      4.753, 1.e-3);
  // Impossible value for mu
  assert_roughly_equal<double>(
      normalDistribution.evaluateParameterForProbabilityAndBound(
          1, parameters2, 0.8, -3., true),
      NAN, 1.e-3, true);
  // Infinite values for mu
  assert_roughly_equal<double>(
      normalDistribution.evaluateParameterForProbabilityAndBound(1, parameters2,
                                                                 0.5, 1., true),
      NAN, 1.e-3, true);
}
