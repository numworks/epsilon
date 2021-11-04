#include "probability/models/student_law.h"

#include <assert.h>
#include <float.h>
#include <poincare/normal_distribution.h>
#include <quiz.h>
#include <string.h>

#include <algorithm>
#include <cmath>

#include "test_helper.h"

constexpr int k_numberOfTestCases = 79;

LawTestCase getStudentTestCase(int i) {
  static LawTestCase s_studentTests[k_numberOfTestCases] {
    LawTestCase(0.28, -10, 0.0051917760715735, 0.1857118516896359),
    LawTestCase(1, -10, 0.0031515830315226806, 0.03172551743055356),
    LawTestCase(2.57, -10, 0.0005030925333730742, 0.001996706792893251),
    LawTestCase(3, -10, 0.0003118082168470876, 0.0010641995292070756),
    LawTestCase(4, -10, 0.00010879223472902223, 0.00028100181135799556),
    LawTestCase(5, -10, 4.098981641534331e-05, 8.547378787148179e-05),
    LawTestCase(6, -10, 1.6514084372279296e-05, 2.8959913774768124e-05),
    LawTestCase(7, -10, 7.051932309805197e-06, 1.069710144538641e-05),
    LawTestCase(10, -10, 7.284685722912541e-07, 7.947765877982051e-07),
    LawTestCase(20, -10, 2.6600849800550903e-09, 1.5818908793571965e-09),
    LawTestCase(50, -10, 2.7048721527008514e-13, 8.038667344167695e-14, 1e-3),
    LawTestCase(0.28, -1.0, 0.08462111434249496, 0.34697350341129507),
    LawTestCase(1, -1.0, 0.15915494309189537, 0.24999999999999978),
    LawTestCase(2.57, -1.0, 0.20174670047005813, 0.20098201301585422),
    LawTestCase(3, -1.0, 0.20674833578317203, 0.19550110947788527),
    LawTestCase(4, -1.0, 0.21466252583997983, 0.1869504831500295),
    LawTestCase(5, -1.0, 0.2196797973509806, 0.18160873382456127),
    LawTestCase(6, -1.0, 0.2231422909165263, 0.17795884187479102),
    LawTestCase(7, -1.0, 0.2256749202754575, 0.17530833141010374),
    LawTestCase(10, -1.0, 0.2303619892291386, 0.17044656615103004),
    LawTestCase(20, -1.0, 0.23604564912670092, 0.1646282885858545),
    LawTestCase(50, -1.0, 0.239571062058688, 0.16106282255012236),
    LawTestCase(500, -1.0, 0.2417289553160677, 0.15889710363030426),
    LawTestCase(0.28, -0.2, 0.20549304829293147, 0.45651086361470994, 1e-5),
    LawTestCase(1, -0.2, 0.3060671982536449, 0.43716704181099886, 1e-5),
    LawTestCase(2.57, -0.2, 0.35287094524284984, 0.4281163661715596),
    LawTestCase(3, -0.2, 0.35794379463845566, 0.4271351646231395),
    LawTestCase(4, -0.2, 0.3657866349659307, 0.4256185070684612),
    LawTestCase(5, -0.2, 0.3706399777139695, 0.42468025699791445),
    LawTestCase(6, -0.2, 0.3739346777416558, 0.42404349575697065),
    LawTestCase(7, -0.2, 0.37631593526996343, 0.42358337574489896),
    LawTestCase(10, -0.2, 0.3806581810544492, 0.42274459569017275),
    LawTestCase(20, -0.2, 0.38580918607411935, 0.42175008348394183, 1e-5),
    LawTestCase(50, -0.2, 0.38894005621150207, 0.4211459041217918),
    LawTestCase(500, -0.2, 0.39083190132875784, 0.42078094851278847, 1e-5),
    LawTestCase(0.28, 0, 0.22382674581166703, 0.5),
    LawTestCase(1, 0, 0.31830988618379075, 0.5),
    LawTestCase(2.57, 0, 0.36273426411692106, 0.5),
    LawTestCase(3, 0, 0.36755259694786135, 0.5),
    LawTestCase(4, 0, 0.37500000000000006, 0.5),
    LawTestCase(5, 0, 0.3796066898224944, 0.5),
    LawTestCase(6, 0, 0.3827327723098717, 0.5),
    LawTestCase(7, 0, 0.38499145083226727, 0.5),
    LawTestCase(10, 0, 0.3891083839660311, 0.5),
    LawTestCase(20, 0, 0.3939885857114327, 0.5),
    LawTestCase(50, 0, 0.39695267973111026, 0.5),
    LawTestCase(500, 0, 0.39874285925361797, 0.5),
    LawTestCase(0.28, 0.3, 0.18725956804685212, 0.5631515834389194),
    LawTestCase(1, 0.3, 0.2920274185172392, 0.5927735790777423),
    LawTestCase(2.57, 0.3, 0.3411186663028198, 0.6066171520733878),
    LawTestCase(3, 0.3, 0.34645357427454176, 0.6081183539800405),
    LawTestCase(4, 0.3, 0.35470962734618916, 0.6104392858612702),
    LawTestCase(5, 0.3, 0.3598243283490097, 0.6118754788683627),
    LawTestCase(6, 0.3, 0.3632992517027709, 0.6128503895410208),
    LawTestCase(7, 0.3, 0.3658123007801872, 0.6135549747989328),
    LawTestCase(10, 0.3, 0.37039846155274564, 0.6148396962171008),
    LawTestCase(20, 0.3, 0.375845416768084, 0.6163634983781199),
    LawTestCase(50, 0.3, 0.37916002269688465, 0.6172895598723485),
    LawTestCase(500, 0.3, 0.3811644097585056, 0.6178490820245964, 1e-5),
    LawTestCase(0.28, 2.5, 0.02982281844596167, 0.7270964204989578),
    LawTestCase(1, 2.5, 0.04390481188741942, 0.8788810584091566),
    LawTestCase(2.57, 2.5, 0.04014743299467839, 0.949091198755132),
    LawTestCase(3, 2.5, 0.0386614857271673, 0.9561466764959673),
    LawTestCase(4, 2.5, 0.03567562436955666, 0.966616727594006),
    LawTestCase(5, 2.5, 0.03332623888702283, 0.9727549503288119),
    LawTestCase(6, 2.5, 0.031473766397140154, 0.9767358838579163),
    LawTestCase(7, 2.5, 0.02999022558989219, 0.9795038907071236),
    LawTestCase(10, 2.5, 0.026938727628244466, 0.9842765778816956),
    LawTestCase(20, 2.5, 0.02266944371914488, 0.9893832272804338),
    LawTestCase(50, 2.5, 0.019694702081706403, 0.99212752086344),
    LawTestCase(500, 2.5, 0.01775159704911948, 0.9936307122495581),
    LawTestCase(0.28, 18, 0.0024496510018582997, 0.8424460486448383),
    LawTestCase(1, 18, 0.0009794150344116638, 0.9823342771118654),
    LawTestCase(2.57, 18, 6.36607184320786e-05, 0.9995513677800985),
    LawTestCase(3, 18, 3.093616673241826e-05, 0.9998130098719767),
    LawTestCase(4, 18, 6.1588075963812405e-06, 0.999972000674229),
    LawTestCase(5, 18, 1.3324661240937334e-06, 0.9999951397795279, 1e-5),
    LawTestCase(6, 18, 3.1018888949841117e-07, 0.9999990543824999, 1e-5),
    LawTestCase(7, 18, 7.700718899661956e-08, 0.999999798186122, 1e-3),
  };
  return s_studentTests[i];
}

QUIZ_CASE(probability_student_law) {
  for (int i = 0; i < k_numberOfTestCases; i++) {
    LawTestCase t = getStudentTestCase(i);
    // double
    assertRoughlyEqual(Probability::StudentLaw::EvaluateAtAbscissa(t.x, t.k),
                       t.density,
                       t.precision);
    assertRoughlyEqual(Probability::StudentLaw::CumulativeDistributiveFunctionAtAbscissa(t.x, t.k),
                       t.probability,
                       t.precision);
    assertRoughlyEqual(
        Probability::StudentLaw::CumulativeDistributiveInverseForProbability(t.probability, t.k),
        t.x,
        t.precision / 10);

    // floats
    assertRoughlyEqual<float>(Probability::StudentLaw::EvaluateAtAbscissa<float>(t.x, t.k),
                              t.density,
                              std::sqrt(t.precision));
    assertRoughlyEqual<float>(
        Probability::StudentLaw::CumulativeDistributiveFunctionAtAbscissa<float>(t.x, t.k),
        t.probability,
        std::sqrt(t.precision));
    assertRoughlyEqual<float>(
        Probability::StudentLaw::CumulativeDistributiveInverseForProbability<float>(t.probability,
                                                                                    t.k),
        t.x,
        std::sqrt(t.precision));
  }

  // Special cases p=0
  // double
  assertRoughlyEqual<double>(Probability::StudentLaw::EvaluateAtAbscissa<double>(INFINITY, 5.), 0);
  assertRoughlyEqual<double>(Probability::StudentLaw::EvaluateAtAbscissa<double>(-INFINITY, 5.), 0);
  assertRoughlyEqual<double>(Probability::StudentLaw::CumulativeDistributiveFunctionAtAbscissa<double>(INFINITY, 3.),
                     1.);
  assertRoughlyEqual<double>(Probability::StudentLaw::CumulativeDistributiveFunctionAtAbscissa<double>(-INFINITY, 3.),
                     0.);
  assertRoughlyEqual<double>(Probability::StudentLaw::CumulativeDistributiveInverseForProbability<double>(0, 5.),
                     -INFINITY);
  assertRoughlyEqual<double>(Probability::StudentLaw::CumulativeDistributiveInverseForProbability<double>(1, 5.),
                     INFINITY);

  // floats
  assertRoughlyEqual<float>(Probability::StudentLaw::EvaluateAtAbscissa<float>(INFINITY, 5.), 0);
  assertRoughlyEqual<float>(Probability::StudentLaw::EvaluateAtAbscissa<float>(-INFINITY, 5.), 0);
  assertRoughlyEqual<float>(Probability::StudentLaw::CumulativeDistributiveFunctionAtAbscissa<float>(INFINITY, 3.),
                     1.);
  assertRoughlyEqual<float>(Probability::StudentLaw::CumulativeDistributiveFunctionAtAbscissa<float>(-INFINITY, 3.),
                     0.);
  assertRoughlyEqual<float>(Probability::StudentLaw::CumulativeDistributiveInverseForProbability<float>(0, 5.),
                     -INFINITY);
  assertRoughlyEqual<float>(Probability::StudentLaw::CumulativeDistributiveInverseForProbability<float>(1, 5.),
                     INFINITY);
}