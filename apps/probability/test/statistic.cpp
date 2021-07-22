#include "probability/models/statistic/statistic.h"

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
#include "probability/models/statistic/two_means_t_statistic.h"
#include "probability/models/statistic/two_means_z_statistic.h"
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
                                                 {81, 100},
                                                 0.01,
                                                 0.99,
                                                 3,
                                                 false,
                                                 false,
                                                 2.3263483,
                                                 2.99999881,
                                                 0.13361454,
                                                 81.f / 100.f,
                                                 2.57582951,
                                                 0.0392300896,
                                                 0.101050019}};
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
                               StatisticTestCase{0.9,
                                                 HypothesisParams::ComparisonOperator::Different,
                                                 3,
                                                 {81, 100, 100},
                                                 0.01,
                                                 0.99,
                                                 4,
                                                 true,
                                                 true,
                                                 2.3646059036,
                                                 8.0100002289,
                                                 0.000120103359,
                                                 81.f,
                                                 2.6264057159,
                                                 10,
                                                 26.2640571594}};
  OneMeanTStatistic stat;
  for (int i = 0; i < sizeof(tests) / sizeof(StatisticTestCase); i++) {
    testStatistic(&stat, tests[i]);
  }
}


QUIZ_CASE(one_mean_z_statistic) {
  StatisticTestCase tests[] = {StatisticTestCase{128,
                                                 HypothesisParams::ComparisonOperator::Lower,
                                                 3,
                                                 {127.8, 3.2, 50},
                                                 0.05,
                                                 0.95,
                                                 4,
                                                 false,
                                                 false,
                                                 -1.6448534727,
                                                 -0.0071553085,
                                                 0.4971454740,
                                                 127.8,
                                                 1.9599643946,
                                                 28.8675136566,
                                                 56.5792999268},
                               StatisticTestCase{0.9,
                                                 HypothesisParams::ComparisonOperator::Different,
                                                 3,
                                                 {81, 100, 100},
                                                 0.01,
                                                 0.99,
                                                 4,
                                                 false,
                                                 true,
                                                 2.3263483047,
                                                 8.0100002289,
                                                 0.0000619888306,
                                                 81.f,
                                                 2.5758295059,
                                                 10,
                                                 25.7582950592}};
  OneMeanZStatistic stat;
  for (int i = 0; i < sizeof(tests) / sizeof(StatisticTestCase); i++) {
    testStatistic(&stat, tests[i]);
  }
}