#include "probability/models/chi2_law.h"

#include <math.h>
#include <quiz.h>

#include <iostream>

#include "test_helper.h"

// TODO add tests for special case p=0 and 1 (one way)
TestCase chi2Tests[] = {TestCase(0.28, 0, 0, 0.0),
                        TestCase(0.28, 0.123, 0.7735483671408104, 0.7173556658305531),
                        TestCase(0.28, 0.25, 0.39445708636423615, 0.7863197131812323, 1e-6),
                        TestCase(0.28, 0.7, 0.1299354327320162, 0.8857276344687262, 1e-6),
                        TestCase(0.28, 2, 0.02750012716139315, 0.9650437489682855, 1e-6),
                        TestCase(0.28, 10, 0.000126195122829492, 0.999780321882247, 1e-6),
                        TestCase(1, 0, 0, 0.0),
                        TestCase(1, 0.123, 1.0696665117153819, 0.2741967471949104),
                        TestCase(1, 0.25, 0.704130653528599, 0.3829249225480261),
                        TestCase(1, 0.7, 0.3360144677267704, 0.5972163057535244),
                        TestCase(1, 2, 0.10377687435514868, 0.8427007929497151),
                        TestCase(1, 10, 0.0008500366602520334, 0.9984345977419975),
                        TestCase(2.57, 0, 0.0, 0.0),
                        TestCase(2.57, 0.123, 0.2360000701221212, 0.023209437129391372, 1e-6),
                        TestCase(2.57, 0.25, 0.27109679181024543, 0.055740896473783726, 1e-6),
                        TestCase(2.57, 0.7, 0.29030075052193394, 0.1851697163773158, 1e-6),
                        TestCase(2.57, 2, 0.20440774762468358, 0.5118671076373665),
                        TestCase(2.57, 10, 0.005922783752791853, 0.987553452753085),
                        TestCase(3, 0, 0.0, 0.0),
                        TestCase(3, 0.123, 0.13156898094099193, 0.011058785312926507),
                        TestCase(3, 0.25, 0.17603266338214976, 0.030859595783726757),
                        TestCase(3, 0.7, 0.23521012740873926, 0.12679605093604587),
                        TestCase(3, 2, 0.2075537487102974, 0.42759329552912023),
                        TestCase(3, 10, 0.008500366602520343, 0.9814338645369568, 1e-6),
                        TestCase(5, 0, 0.0, 0.0),
                        TestCase(5, 0.123, 0.005394328218580669, 0.0002701288757651666, 1e-6),
                        TestCase(5, 0.25, 0.014669388615179144, 0.001520818553368441, 1e-6),
                        TestCase(5, 0.7, 0.054882363062039155, 0.01703132481196757, 1e-6),
                        TestCase(5, 2, 0.1383691658068649, 0.15085496391539038, 1e-6),
                        TestCase(5, 10, 0.028334555341734475, 0.9247647538534878, 1e-6),
                        TestCase(5, 30.85618994044592, 4.5442869596536075e-06, .99999, 1e-6),
                        TestCase(10, 0, 0.0, 0.0),
                        TestCase(10, 0.25, 4.488611361615981e-06, 2.2919102136524132e-07),
                        TestCase(10, 0.7, 0.00022030678429878006, 3.273617475313252e-05, 1e-6),
                        TestCase(10, 2, 0.00766415502440505, 0.003659846827343713, 1e-6),
                        TestCase(20, 0, 0.0, 0.0),
                        TestCase(20, 5.3, 0.0006273926980786215, 0.0004347478563383611, 1e-6),
                        TestCase(20, 20, 0.06255501786056658, 0.5420702855281478, 1e-5),
                        TestCase(100, 0, 0.0, 0.0),
                        TestCase(100, 100, 0.028162503162596778, 0.5188083154720433, 1e-6)
                        };

QUIZ_CASE(chi2_law) {
  for (int i = 0; i < sizeof(chi2Tests) / sizeof(TestCase); i++) {
    TestCase t = chi2Tests[i];
    // double
    assertRoughlyEqual(Probability::Chi2Law::EvaluateAtAbscissa(t.x, t.k), t.density, t.precision);
    assertRoughlyEqual(Probability::Chi2Law::CumulativeDistributiveFunctionAtAbscissa(t.x, t.k),
                       t.probability,
                       t.precision);
    assertRoughlyEqual(
        Probability::Chi2Law::CumulativeDistributiveInverseForProbability(t.probability, t.k),
        t.x,
        t.precision / 10);

    // floats
    assertRoughlyEqual<float>(Probability::Chi2Law::EvaluateAtAbscissa<float>(t.x, t.k),
                              t.density,
                              sqrt(t.precision));
    assertRoughlyEqual<float>(
        Probability::Chi2Law::CumulativeDistributiveFunctionAtAbscissa<float>(t.x, t.k),
        t.probability,
        sqrt(t.precision));
    assertRoughlyEqual<float>(
        Probability::Chi2Law::CumulativeDistributiveInverseForProbability<float>(t.probability,
                                                                                 t.k),
        t.x,
        sqrt(t.precision));
  }
}