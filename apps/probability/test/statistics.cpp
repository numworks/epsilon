#include <math.h>
#include <quiz.h>

#include <iostream>

#include "probability/models/data.h"
#include "probability/models/statistic/goodness_statistic.h"
#include "probability/models/statistic/homogeneity_statistic.h"
#include "probability/models/statistic/one_mean_t_statistic.h"
#include "probability/models/statistic/one_mean_z_statistic.h"
#include "probability/models/statistic/one_proportion_statistic.h"
#include "probability/models/statistic/pooled_two_means_statistic.h"
#include "probability/models/statistic/statistic.h"
#include "probability/models/statistic/two_means_t_statistic.h"
#include "probability/models/statistic/two_means_z_statistic.h"
#include "probability/models/statistic/two_proportions_statistic.h"
#include "test_helper.h"

using namespace Probability;

struct StatisticTestCase {
  // Inputs
  float m_firstHypothesisParam;
  HypothesisParams::ComparisonOperator m_op;
  int m_numberOfInputs;
  float m_inputs[10];
  float m_significanceLevel;
  float m_confidenceLevel;

  // Results
  int m_numberOfParameters;
  bool m_hasDegreeOfFreedom;
  bool m_testPassed;
  float m_zAlpha;
  float m_testCriticalValue;
  float m_pValue;
  float m_estimate;
  float m_intervalCriticalValue;
  float m_standardError;
  float m_marginOfError;
};

void testStatistic(Statistic * stat, StatisticTestCase & test) {
  quiz_assert(stat->numberOfParameters() == test.m_numberOfParameters);
  quiz_assert(stat->hasDegreeOfFreedom() == test.m_hasDegreeOfFreedom);

  // Test
  stat->initThreshold(Data::SubApp::Tests);
  quiz_assert(roughlyEqual<float>(stat->threshold(), 0.05f));  // Significance level
  stat->setThreshold(test.m_significanceLevel);
  quiz_assert(roughlyEqual<float>(stat->threshold(), test.m_significanceLevel));

  stat->hypothesisParams()->setFirstParam(test.m_firstHypothesisParam);
  stat->hypothesisParams()->setOp(test.m_op);

  for (int i = 0; i < test.m_numberOfInputs; i++) {
    stat->setParamAtIndex(i, test.m_inputs[i]);
  }

  stat->computeTest();

  quiz_assert(stat->testPassed() == test.m_testPassed);
  quiz_assert(roughlyEqual<float>(stat->zAlpha(), test.m_zAlpha));
  quiz_assert(roughlyEqual<float>(stat->testCriticalValue(), test.m_testCriticalValue));
  quiz_assert(roughlyEqual<float>(stat->pValue(), test.m_pValue));

  // Confidence interval
  stat->initThreshold(Data::SubApp::Intervals);
  quiz_assert(roughlyEqual<float>(stat->threshold(), 0.95f));  // Confidence level
  stat->setThreshold(test.m_confidenceLevel);
  quiz_assert(roughlyEqual<float>(stat->threshold(), test.m_confidenceLevel));

  stat->computeInterval();

  quiz_assert(roughlyEqual<float>(stat->estimate(), test.m_estimate));
  quiz_assert(roughlyEqual<float>(stat->intervalCriticalValue(), test.m_intervalCriticalValue));
  quiz_assert(roughlyEqual<float>(stat->standardError(), test.m_standardError));
  quiz_assert(roughlyEqual<float>(stat->marginOfError(), test.m_marginOfError));
}

QUIZ_CASE(one_proportion_statistic) {
  StatisticTestCase tests[] = {StatisticTestCase{0.4,
                                                 HypothesisParams::ComparisonOperator::Lower,
                                                 2,
                                                 {12, 50},
                                                 0.05,
                                                 0.95,
                                                 3,
                                                 false,
                                                 true,
                                                 -1.64485347,
                                                 -2.30940127,
                                                 0.0104606748,
                                                 12.f / 50.f,
                                                 1.95996439,
                                                 0.0603986755,
                                                 0.11837925},
                               StatisticTestCase{0.9,
                                                 HypothesisParams::ComparisonOperator::Different,
                                                 2,
                                                 {84, 100},
                                                 0.01,
                                                 0.99,
                                                 3,
                                                 false,
                                                 false,
                                                 2.3263483,
                                                 1.9999998808,
                                                 0.0455002785,
                                                 84.f / 100.f,
                                                 2.57582951,
                                                 0.0366606079,
                                                 0.0944314748}};
  OneProportionStatistic stat;
  for (int i = 0; i < sizeof(tests) / sizeof(StatisticTestCase); i++) {
    testStatistic(&stat, tests[i]);
  }
}

QUIZ_CASE(one_mean_t_statistic) {
  StatisticTestCase tests[] = {StatisticTestCase{128,
                                                 HypothesisParams::ComparisonOperator::Lower,
                                                 3,
                                                 {127.8, 3.2, 50},
                                                 0.05,
                                                 0.95,
                                                 4,
                                                 true,
                                                 false,
                                                 -1.6765508652,
                                                 -0.4419349730,
                                                 0.3302403092,
                                                 127.8,
                                                 2.0095756054,
                                                 0.4525483549,
                                                 0.9094301462},
                               StatisticTestCase{-4,
                                                 HypothesisParams::ComparisonOperator::Different,
                                                 3,
                                                 {1.4, 5, 10},
                                                 0.01,
                                                 0.99,
                                                 4,
                                                 true,
                                                 true,
                                                 2.8214378357,
                                                 3.4152598381,
                                                 0.0076853633,
                                                 1.4f,
                                                 3.2498362064,
                                                 1.5811388493,
                                                 5.1384425163}};
  OneMeanTStatistic stat;
  for (int i = 0; i < sizeof(tests) / sizeof(StatisticTestCase); i++) {
    testStatistic(&stat, tests[i]);
  }
}

QUIZ_CASE(one_mean_z_statistic) {
  StatisticTestCase tests[] = {StatisticTestCase{128,
                                                 HypothesisParams::ComparisonOperator::Lower,
                                                 3,
                                                 {127.8, 50, 3.2},
                                                 0.05,
                                                 0.95,
                                                 4,
                                                 false,
                                                 false,
                                                 -1.6448534727,
                                                 -0.4419349730,
                                                 0.3292680979,
                                                 127.8,
                                                 1.9599643946,
                                                 0.4525483549,
                                                 0.8869786859},
                               StatisticTestCase{0.9,
                                                 HypothesisParams::ComparisonOperator::Different,
                                                 3,
                                                 {2.3, 1000, 14},
                                                 0.01,
                                                 0.99,
                                                 4,
                                                 false,
                                                 true,
                                                 2.3263483047,
                                                 3.1622774601,
                                                 0.0015654564,
                                                 2.3f,
                                                 2.5758295059,
                                                 0.4427188635,
                                                 1.1403683424}};
  OneMeanZStatistic stat;
  for (int i = 0; i < sizeof(tests) / sizeof(StatisticTestCase); i++) {
    testStatistic(&stat, tests[i]);
  }
}

QUIZ_CASE(two_proportions_statistic) {
  StatisticTestCase tests[] = {StatisticTestCase{0,
                                                 HypothesisParams::ComparisonOperator::Higher,
                                                 4,
                                                 {20, 50, 32, 103},
                                                 0.05,
                                                 0.95,
                                                 5,
                                                 false,
                                                 false,
                                                 1.6448534727,
                                                 1.0940510035,
                                                 0.1369662881,
                                                 20. / 50. - 32. / 103.,
                                                 1.9599643946,
                                                 0.0829409584,
                                                 0.1625613272},
                               StatisticTestCase{0.3,
                                                 HypothesisParams::ComparisonOperator::Lower,
                                                 4,
                                                 {60, 100, 44.1, 90},
                                                 0.01,
                                                 0.99,
                                                 5,
                                                 false,
                                                 true,
                                                 -2.3263483047,
                                                 -2.6274206638,
                                                 0.0043017864,
                                                 60.f / 100.f - 44.1f / 90.f,
                                                 2.5758295059,
                                                 0.0719490498,
                                                 0.1853284836}};
  TwoProportionsStatistic stat;
  for (int i = 0; i < sizeof(tests) / sizeof(StatisticTestCase); i++) {
    testStatistic(&stat, tests[i]);
  }
}