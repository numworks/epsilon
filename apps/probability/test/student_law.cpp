#include "probability/models/student_law.h"

#include <assert.h>
#include <float.h>
#include <quiz.h>
#include <string.h>

#include <algorithm>
#include <cmath>
#include <iostream>

template <typename T>
bool roughlyEqual(T a, T b, T threshold, bool absolute) {
  if (absolute) {
    return std::fabs(a - b) < threshold;
  }
  T max = std::max(a, b);
  return max == 0 || std::fabs(a - b) / max < threshold;
}

template <typename T>
void assertRoughlyEqual(T a, T b, T threshold = FLT_EPSILON, bool absolute = false) {
  quiz_assert(roughlyEqual<T>(a, b, threshold, absolute));
}

struct TestCase {
  double k;
  double x;
  double density;
  double probability;
};

TestCase tests[]{
    {.k = 0.28, .x = -10, .density = 0.0051917760715735, .probability = 0.1857118516896359},
    {.k = 1, .x = -10, .density = 0.0031515830315226806, .probability = 0.03172551743055356},
    {.k = 2.57, .x = -10, .density = 0.0005030925333730742, .probability = 0.001996706792893251},
    {.k = 3, .x = -10, .density = 0.0003118082168470876, .probability = 0.0010641995292070756},
    {.k = 4, .x = -10, .density = 0.00010879223472902223, .probability = 0.00028100181135799556},
    {.k = 5, .x = -10, .density = 4.098981641534331e-05, .probability = 8.547378787148179e-05},
    {.k = 6, .x = -10, .density = 1.6514084372279296e-05, .probability = 2.8959913774768124e-05},
    {.k = 7, .x = -10, .density = 7.051932309805197e-06, .probability = 1.069710144538641e-05},
    {.k = 10, .x = -10, .density = 7.284685722912541e-07, .probability = 7.947765877982051e-07},
    {.k = 20, .x = -10, .density = 2.6600849800550903e-09, .probability = 1.5818908793571965e-09},
    {.k = 50, .x = -10, .density = 2.7048721527008514e-13, .probability = 8.038667344167695e-14},
    {.k = 500, .x = -10, .density = 5.831641042183955e-21, .probability = 6.930246799119944e-22},
    {.k = 2000, .x = -10, .density = 2.5174569902467e-22, .probability = 2.6176766864254786e-23},
    {.k = 0.28, .x = -1.0, .density = 0.08462111434249496, .probability = 0.34697350341129507},
    {.k = 1, .x = -1.0, .density = 0.15915494309189537, .probability = 0.24999999999999978},
    {.k = 2.57, .x = -1.0, .density = 0.20174670047005813, .probability = 0.20098201301585422},
    {.k = 3, .x = -1.0, .density = 0.20674833578317203, .probability = 0.19550110947788527},
    {.k = 4, .x = -1.0, .density = 0.21466252583997983, .probability = 0.1869504831500295},
    {.k = 5, .x = -1.0, .density = 0.2196797973509806, .probability = 0.18160873382456127},
    {.k = 6, .x = -1.0, .density = 0.2231422909165263, .probability = 0.17795884187479102},
    {.k = 7, .x = -1.0, .density = 0.2256749202754575, .probability = 0.17530833141010374},
    {.k = 10, .x = -1.0, .density = 0.2303619892291386, .probability = 0.17044656615103004},
    {.k = 20, .x = -1.0, .density = 0.23604564912670092, .probability = 0.1646282885858545},
    {.k = 50, .x = -1.0, .density = 0.239571062058688, .probability = 0.16106282255012236},
    {.k = 500, .x = -1.0, .density = 0.2417289553160677, .probability = 0.15889710363030426},
    {.k = 2000, .x = -1.0, .density = 0.24191024443895331, .probability = 0.15871573905037178},
    {.k = 0.28, .x = -0.2, .density = 0.20549304829293147, .probability = 0.45651086361470994},
    {.k = 1, .x = -0.2, .density = 0.3060671982536449, .probability = 0.43716704181099886},
    {.k = 2.57, .x = -0.2, .density = 0.35287094524284984, .probability = 0.4281163661715596},
    {.k = 3, .x = -0.2, .density = 0.35794379463845566, .probability = 0.4271351646231395},
    {.k = 4, .x = -0.2, .density = 0.3657866349659307, .probability = 0.4256185070684612},
    {.k = 5, .x = -0.2, .density = 0.3706399777139695, .probability = 0.42468025699791445},
    {.k = 6, .x = -0.2, .density = 0.3739346777416558, .probability = 0.42404349575697065},
    {.k = 7, .x = -0.2, .density = 0.37631593526996343, .probability = 0.42358337574489896},
    {.k = 10, .x = -0.2, .density = 0.3806581810544492, .probability = 0.42274459569017275},
    {.k = 20, .x = -0.2, .density = 0.38580918607411935, .probability = 0.42175008348394183},
    {.k = 50, .x = -0.2, .density = 0.38894005621150207, .probability = 0.4211459041217918},
    {.k = 500, .x = -0.2, .density = 0.39083190132875784, .probability = 0.42078094851278847},
    {.k = 2000, .x = -0.2, .density = 0.3909899850133841, .probability = 0.420750457016576},
    {.k = 0.28, .x = 0, .density = 0.22382674581166703, .probability = 0.5},
    {.k = 1, .x = 0, .density = 0.31830988618379075, .probability = 0.5},
    {.k = 2.57, .x = 0, .density = 0.36273426411692106, .probability = 0.5},
    {.k = 3, .x = 0, .density = 0.36755259694786135, .probability = 0.5},
    {.k = 4, .x = 0, .density = 0.37500000000000006, .probability = 0.5},
    {.k = 5, .x = 0, .density = 0.3796066898224944, .probability = 0.5},
    {.k = 6, .x = 0, .density = 0.3827327723098717, .probability = 0.5},
    {.k = 7, .x = 0, .density = 0.38499145083226727, .probability = 0.5},
    {.k = 10, .x = 0, .density = 0.3891083839660311, .probability = 0.5},
    {.k = 20, .x = 0, .density = 0.3939885857114327, .probability = 0.5},
    {.k = 50, .x = 0, .density = 0.39695267973111026, .probability = 0.5},
    {.k = 500, .x = 0, .density = 0.39874285925361797, .probability = 0.5},
    {.k = 2000, .x = 0, .density = 0.3988924157353542, .probability = 0.5},
    {.k = 0.28, .x = 0.3, .density = 0.18725956804685212, .probability = 0.5631515834389194},
    {.k = 1, .x = 0.3, .density = 0.2920274185172392, .probability = 0.5927735790777423},
    {.k = 2.57, .x = 0.3, .density = 0.3411186663028198, .probability = 0.6066171520733878},
    {.k = 3, .x = 0.3, .density = 0.34645357427454176, .probability = 0.6081183539800405},
    {.k = 4, .x = 0.3, .density = 0.35470962734618916, .probability = 0.6104392858612702},
    {.k = 5, .x = 0.3, .density = 0.3598243283490097, .probability = 0.6118754788683627},
    {.k = 6, .x = 0.3, .density = 0.3632992517027709, .probability = 0.6128503895410208},
    {.k = 7, .x = 0.3, .density = 0.3658123007801872, .probability = 0.6135549747989328},
    {.k = 10, .x = 0.3, .density = 0.37039846155274564, .probability = 0.6148396962171008},
    {.k = 20, .x = 0.3, .density = 0.375845416768084, .probability = 0.6163634983781199},
    {.k = 50, .x = 0.3, .density = 0.37916002269688465, .probability = 0.6172895598723485},
    {.k = 500, .x = 0.3, .density = 0.3811644097585056, .probability = 0.6178490820245964},
    {.k = 2000, .x = 0.3, .density = 0.381331951188423, .probability = 0.6178958340067642},
    {.k = 0.28, .x = 2.5, .density = 0.02982281844596167, .probability = 0.7270964204989578},
    {.k = 1, .x = 2.5, .density = 0.04390481188741942, .probability = 0.8788810584091566},
    {.k = 2.57, .x = 2.5, .density = 0.04014743299467839, .probability = 0.949091198755132},
    {.k = 3, .x = 2.5, .density = 0.0386614857271673, .probability = 0.9561466764959673},
    {.k = 4, .x = 2.5, .density = 0.03567562436955666, .probability = 0.966616727594006},
    {.k = 5, .x = 2.5, .density = 0.03332623888702283, .probability = 0.9727549503288119},
    {.k = 6, .x = 2.5, .density = 0.031473766397140154, .probability = 0.9767358838579163},
    {.k = 7, .x = 2.5, .density = 0.02999022558989219, .probability = 0.9795038907071236},
    {.k = 10, .x = 2.5, .density = 0.026938727628244466, .probability = 0.9842765778816956},
    {.k = 20, .x = 2.5, .density = 0.02266944371914488, .probability = 0.9893832272804338},
    {.k = 50, .x = 2.5, .density = 0.019694702081706403, .probability = 0.99212752086344},
    {.k = 500, .x = 2.5, .density = 0.01775159704911948, .probability = 0.9936307122495581},
    {.k = 2000, .x = 2.5, .density = 0.01758426285129498, .probability = 0.9937505737009433},
    {.k = 0.28, .x = 18, .density = 0.0024496510018582997, .probability = 0.8424460486448383},
    {.k = 1, .x = 18, .density = 0.0009794150344116638, .probability = 0.9823342771118654},
    {.k = 2.57, .x = 18, .density = 6.36607184320786e-05, .probability = 0.9995513677800985},
    {.k = 3, .x = 18, .density = 3.093616673241826e-05, .probability = 0.9998130098719767},
    {.k = 4, .x = 18, .density = 6.1588075963812405e-06, .probability = 0.999972000674229},
    {.k = 5, .x = 18, .density = 1.3324661240937334e-06, .probability = 0.9999951397795279},
    {.k = 6, .x = 18, .density = 3.1018888949841117e-07, .probability = 0.9999990543824999},
    {.k = 7, .x = 18, .density = 7.700718899661956e-08, .probability = 0.999999798186122},
    {.k = 10, .x = 18, .density = 1.6198135254805891e-09, .probability = 0.9999999970020259},
    {.k = 20, .x = 18, .density = 4.192131984744228e-14, .probability = 0.99999999999996},
    {.k = 50, .x = 18, .density = 2.0618069183792536e-23, .probability = 1.0},
    {.k = 500, .x = 18, .density = 1.790245021917116e-55, .probability = 1.0},
    {.k = 2000, .x = 18, .density = 2.3021025222541497e-66, .probability = 1.0},
};

QUIZ_CASE(student_law) {
  for (int i = 0; i < sizeof(tests) / sizeof(TestCase); i++) {
    TestCase t = tests[i];
    std::cout << "TestCase(" << t.k << ", " << t.x << ", " << t.density << ", " << t.probability
              << ")" << std::endl;
    assertRoughlyEqual(Probability::StudentLaw::EvaluateAtAbscissa(t.x, t.k), t.density, 1e-5);
    assertRoughlyEqual(Probability::StudentLaw::CumulativeDistributiveFunctionAtAbscissa(t.x, t.k),
                       t.probability, 1e-5);
    assertRoughlyEqual(
        Probability::StudentLaw::CumulativeDistributiveInverseForProbability(t.probability, t.k),
        t.x, 1e-2);
    // floats
    assertRoughlyEqual<float>(Probability::StudentLaw::EvaluateAtAbscissa<float>(t.x, t.k),
                              t.density, 1e-5);
    assertRoughlyEqual<float>(
        Probability::StudentLaw::CumulativeDistributiveFunctionAtAbscissa<float>(t.x, t.k),
        t.probability, 1e-5);
    assertRoughlyEqual<float>(
        Probability::StudentLaw::CumulativeDistributiveInverseForProbability<float>(t.probability,
                                                                                    t.k),
        t.x, 1e-2);
  }
}