#include <math.h>
#include <quiz.h>

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
  float m_inputs[100];
  float m_significanceLevel;
  float m_confidenceLevel;

  // Results
  int m_numberOfParameters;
  bool m_hasDegreeOfFreedom;
  float m_degreesOfFreedom;
  bool m_testPassed;
  float m_testCriticalValue;
  float m_pValue;
  float m_estimate;
  float m_intervalCriticalValue;
  float m_standardError;
  float m_marginOfError;
};

void inputValues(Statistic * stat, StatisticTestCase & test) {
  quiz_assert(stat->hasDegreeOfFreedom() == test.m_hasDegreeOfFreedom);

  stat->initThreshold(Data::SubApp::Tests);
  assertRoughlyEqual<float>(stat->threshold(), 0.05f);  // Significance level
  stat->setThreshold(test.m_significanceLevel);
  assertRoughlyEqual<float>(stat->threshold(), test.m_significanceLevel);

  stat->hypothesisParams()->setFirstParam(test.m_firstHypothesisParam);
  stat->hypothesisParams()->setComparisonOperator(test.m_op);

  for (int i = 0; i < test.m_numberOfInputs; i++) {
    stat->setParamAtIndex(i, test.m_inputs[i]);
    quiz_assert((stat->paramAtIndex(i) && test.m_inputs[i]) ||
                (stat->paramAtIndex(i) == test.m_inputs[i]));
  }
}

void runTest(Statistic * stat, StatisticTestCase & test) {
  stat->computeTest();

  quiz_assert(stat->numberOfParameters() == test.m_numberOfParameters);
  quiz_assert(stat->canRejectNull() == test.m_testPassed);
  assertRoughlyEqual<float>(stat->testCriticalValue(), test.m_testCriticalValue);
  assertRoughlyEqual<float>(stat->pValue(), test.m_pValue);
  if (stat->hasDegreeOfFreedom()) {
    assertRoughlyEqual(stat->degreeOfFreedom(), test.m_degreesOfFreedom);
  }
}

void testStatistic(Statistic * stat, StatisticTestCase & test) {
  inputValues(stat, test);

  // Test
  runTest(stat, test);

  // Confidence interval
  stat->initThreshold(Data::SubApp::Intervals);
  assertRoughlyEqual<float>(stat->threshold(), 0.95f);  // Confidence level
  stat->setThreshold(test.m_confidenceLevel);
  assertRoughlyEqual<float>(stat->threshold(), test.m_confidenceLevel);

  stat->computeInterval();

  assertRoughlyEqual<float>(stat->estimate(), test.m_estimate);
  assertRoughlyEqual<float>(stat->intervalCriticalValue(), test.m_intervalCriticalValue, 5 * FLT_EPSILON);
  assertRoughlyEqual<float>(stat->standardError(), test.m_standardError, 5 * FLT_EPSILON);
  assertRoughlyEqual<float>(stat->marginOfError(), test.m_marginOfError, 5 * FLT_EPSILON);
}

QUIZ_CASE(probability_one_proportion_statistic) {
  StatisticTestCase tests[] = {
      StatisticTestCase{.m_firstHypothesisParam = 0.4,
                        .m_op = HypothesisParams::ComparisonOperator::Lower,
                        .m_numberOfInputs = 2,
                        .m_inputs = {12, 50},
                        .m_significanceLevel = 0.05,
                        .m_confidenceLevel = 0.95,
                        .m_numberOfParameters = 3,
                        .m_hasDegreeOfFreedom = false,
                        .m_testPassed = true,
                        .m_testCriticalValue = -2.30940127,
                        .m_pValue = 0.0104606748,
                        .m_estimate = 12.f / 50.f,
                        .m_intervalCriticalValue = 1.95996439,
                        .m_standardError = 0.0603986755,
                        .m_marginOfError = 0.11837925},
      StatisticTestCase{.m_firstHypothesisParam = 0.9,
                        .m_op = HypothesisParams::ComparisonOperator::Different,
                        .m_numberOfInputs = 2,
                        .m_inputs = {84, 100},
                        .m_significanceLevel = 0.01,
                        .m_confidenceLevel = 0.99,
                        .m_numberOfParameters = 3,
                        .m_hasDegreeOfFreedom = false,
                        .m_testPassed = false,
                        .m_testCriticalValue = -1.9999998808,
                        .m_pValue = 0.0455002785,
                        .m_estimate = 84.f / 100.f,
                        .m_intervalCriticalValue = 2.57582951,
                        .m_standardError = 0.0366606079,
                        .m_marginOfError = 0.0944314748}};
  OneProportionStatistic stat;
  for (int i = 0; i < sizeof(tests) / sizeof(StatisticTestCase); i++) {
    testStatistic(&stat, tests[i]);
  }
}

QUIZ_CASE(probability_one_mean_t_statistic) {
  StatisticTestCase tests[] = {
      StatisticTestCase{.m_firstHypothesisParam = 128,
                        .m_op = HypothesisParams::ComparisonOperator::Lower,
                        .m_numberOfInputs = 3,
                        .m_inputs = {127.8, 3.2, 50},
                        .m_significanceLevel = 0.05,
                        .m_confidenceLevel = 0.95,
                        .m_numberOfParameters = 4,
                        .m_hasDegreeOfFreedom = true,
                        .m_degreesOfFreedom = 50 - 1,
                        .m_testPassed = false,
                        .m_testCriticalValue = -0.4419349730,
                        .m_pValue = 0.3302403092,
                        .m_estimate = 127.8,
                        .m_intervalCriticalValue = 2.0095756054,
                        .m_standardError = 0.4525483549,
                        .m_marginOfError = 0.9094301462},
      StatisticTestCase{.m_firstHypothesisParam = -4,
                        .m_op = HypothesisParams::ComparisonOperator::Different,
                        .m_numberOfInputs = 3,
                        .m_inputs = {1.4, 5, 10},
                        .m_significanceLevel = 0.01,
                        .m_confidenceLevel = 0.99,
                        .m_numberOfParameters = 4,
                        .m_hasDegreeOfFreedom = true,
                        .m_degreesOfFreedom = 10 - 1,
                        .m_testPassed = true,
                        .m_testCriticalValue = 3.4152598381,
                        .m_pValue = 0.0076853633,
                        .m_estimate = 1.4f,
                        .m_intervalCriticalValue = 3.2498362064,
                        .m_standardError = 1.5811388493,
                        .m_marginOfError = 5.1384425163}};
  OneMeanTStatistic stat;
  for (int i = 0; i < sizeof(tests) / sizeof(StatisticTestCase); i++) {
    testStatistic(&stat, tests[i]);
  }
}

QUIZ_CASE(probability_one_mean_z_statistic) {
  StatisticTestCase tests[] = {
      StatisticTestCase{.m_firstHypothesisParam = 128,
                        .m_op = HypothesisParams::ComparisonOperator::Lower,
                        .m_numberOfInputs = 3,
                        .m_inputs = {127.8, 50, 3.2},
                        .m_significanceLevel = 0.05,
                        .m_confidenceLevel = 0.95,
                        .m_numberOfParameters = 4,
                        .m_hasDegreeOfFreedom = false,
                        .m_testPassed = false,
                        .m_testCriticalValue = -0.4419349730,
                        .m_pValue = 0.3292680979,
                        .m_estimate = 127.8,
                        .m_intervalCriticalValue = 1.9599643946,
                        .m_standardError = 0.4525483549,
                        .m_marginOfError = 0.8869786859},
      StatisticTestCase{.m_firstHypothesisParam = 0.9,
                        .m_op = HypothesisParams::ComparisonOperator::Different,
                        .m_numberOfInputs = 3,
                        .m_inputs = {2.3, 1000, 14},
                        .m_significanceLevel = 0.01,
                        .m_confidenceLevel = 0.99,
                        .m_numberOfParameters = 4,
                        .m_hasDegreeOfFreedom = false,
                        .m_testPassed = true,
                        .m_testCriticalValue = 3.1622774601,
                        .m_pValue = 0.0015654564,
                        .m_estimate = 2.3f,
                        .m_intervalCriticalValue = 2.5758295059,
                        .m_standardError = 0.4427188933,
                        .m_marginOfError = 1.1403683424}};
  OneMeanZStatistic stat;
  for (int i = 0; i < sizeof(tests) / sizeof(StatisticTestCase); i++) {
    testStatistic(&stat, tests[i]);
  }
}

QUIZ_CASE(probability_two_proportions_statistic) {
  StatisticTestCase tests[] = {
      StatisticTestCase{.m_firstHypothesisParam = 0,
                        .m_op = HypothesisParams::ComparisonOperator::Higher,
                        .m_numberOfInputs = 4,
                        .m_inputs = {20, 50, 32, 103},
                        .m_significanceLevel = 0.05,
                        .m_confidenceLevel = 0.95,
                        .m_numberOfParameters = 5,
                        .m_hasDegreeOfFreedom = false,
                        .m_testPassed = false,
                        .m_testCriticalValue = 1.0940510035,
                        .m_pValue = 0.1369662881,
                        .m_estimate = 20. / 50. - 32. / 103.,
                        .m_intervalCriticalValue = 1.9599643946,
                        .m_standardError = 0.0829409584,
                        .m_marginOfError = 0.1625613272},
      StatisticTestCase{.m_firstHypothesisParam = 0.3,
                        .m_op = HypothesisParams::ComparisonOperator::Lower,
                        .m_numberOfInputs = 4,
                        .m_inputs = {60, 100, 44.4, 90},
                        .m_significanceLevel = 0.01,
                        .m_confidenceLevel = 0.99,
                        .m_numberOfParameters = 5,
                        .m_hasDegreeOfFreedom = false,
                        .m_testPassed = true,
                        .m_testCriticalValue = -2.6117918491,
                        .m_pValue = 0.0045034885,
                        .m_estimate = 60.f / 100.f - 44.f / 90.f,
                        .m_intervalCriticalValue = 2.5758295059,
                        .m_standardError = 0.0719472468,
                        .m_marginOfError = 0.1853238344}};
  TwoProportionsStatistic stat;
  for (int i = 0; i < sizeof(tests) / sizeof(StatisticTestCase); i++) {
    testStatistic(&stat, tests[i]);
  }
}

QUIZ_CASE(probability_two_means_t_statistic) {
  StatisticTestCase tests[] = {
      StatisticTestCase{.m_firstHypothesisParam = 0,
                        .m_op = HypothesisParams::ComparisonOperator::Higher,
                        .m_numberOfInputs = 6,
                        .m_inputs = {20, 2, 50, 24, 18, 60},
                        .m_significanceLevel = 0.05,
                        .m_confidenceLevel = 0.95,
                        .m_numberOfParameters = 7,
                        .m_hasDegreeOfFreedom = true,
                        .m_degreesOfFreedom = 60.7450408936,
                        .m_testPassed = false,
                        .m_testCriticalValue = -1.7087153196,
                        .m_pValue = 0.9536954761,
                        .m_estimate = 20. - 24.,
                        .m_intervalCriticalValue = 1.9997937679,
                        .m_standardError = 2.3409399986,
                        .m_marginOfError = 4.6813974380},
      StatisticTestCase{.m_firstHypothesisParam = -12.345,
                        .m_op = HypothesisParams::ComparisonOperator::Different,
                        .m_numberOfInputs = 6,
                        .m_inputs = {4.2, 46, 1000, 18.3, 18, 60},
                        .m_significanceLevel = 0.01,
                        .m_confidenceLevel = 0.99,
                        .m_numberOfParameters = 7,
                        .m_hasDegreeOfFreedom = true,
                        .m_degreesOfFreedom = 113.2706527710,
                        .m_testPassed = false,
                        .m_testCriticalValue = -0.6401526332,
                        .m_pValue = 0.5233662128,
                        .m_estimate = 4.2f - 18.3f,
                        .m_intervalCriticalValue = 2.6199319363,
                        .m_standardError = 2.7415323257,
                        .m_marginOfError = 7.1826281548}};
  TwoMeansTStatistic stat;
  for (int i = 0; i < sizeof(tests) / sizeof(StatisticTestCase); i++) {
    testStatistic(&stat, tests[i]);
  }
}

QUIZ_CASE(probability_pooled_t_test) {
  StatisticTestCase tests[] = {
      StatisticTestCase{.m_firstHypothesisParam = 0.3,
                        .m_op = HypothesisParams::ComparisonOperator::Higher,
                        .m_numberOfInputs = 6,
                        .m_inputs = {213.4, 14, 234, 213.5, 135, 64},
                        .m_significanceLevel = 0.02,
                        .m_confidenceLevel = 0.876,
                        .m_numberOfParameters = 7,
                        .m_hasDegreeOfFreedom = true,
                        .m_degreesOfFreedom = 296,
                        .m_testPassed = false,
                        .m_testCriticalValue = -0.0446507446,
                        .m_pValue = 0.5177921057,
                        .m_estimate = 213.4f - 213.5f,
                        .m_intervalCriticalValue = 1.5425841808,
                        .m_standardError = 8.9585542679,
                        .m_marginOfError = 13.8193244934},
      StatisticTestCase{.m_firstHypothesisParam = 0.,
                        .m_op = HypothesisParams::ComparisonOperator::Higher,
                        .m_numberOfInputs = 6,
                        .m_inputs = {1.23, 1.23, 12, 0.2, 0.12, 12},
                        .m_significanceLevel = 0.1,
                        .m_confidenceLevel = 0.9,
                        .m_numberOfParameters = 7,
                        .m_hasDegreeOfFreedom = true,
                        .m_degreesOfFreedom = 22,
                        .m_testPassed = true,
                        .m_testCriticalValue = 2.887125562,
                        .m_pValue = 0.0042771697,
                        .m_estimate = 1.23 - 0.2,
                        .m_intervalCriticalValue = 1.7171442509,
                        .m_standardError = 0.3567562103,
                        .m_marginOfError = 0.6126018763}};
  PooledTwoMeansStatistic stat;
  for (int i = 0; i < sizeof(tests) / sizeof(StatisticTestCase); i++) {
    testStatistic(&stat, tests[i]);
  }
}

QUIZ_CASE(probability_two_means_z_statistic) {
  TwoMeansZStatistic stat;
  StatisticTestCase tests[] = {
      StatisticTestCase{.m_firstHypothesisParam = 0.,
                        .m_op = HypothesisParams::ComparisonOperator::Higher,
                        .m_numberOfInputs = 6,
                        .m_inputs = {3.14, 119, 2, 2.07, 402, 5},
                        .m_significanceLevel = 0.1,
                        .m_confidenceLevel = 0.9,
                        .m_numberOfParameters = 7,
                        .m_hasDegreeOfFreedom = false,
                        .m_testPassed = true,
                        .m_testCriticalValue = 3.4569685459,
                        .m_pValue = 0.00027310848236,
                        .m_estimate = 3.14f - 2.07f,
                        .m_intervalCriticalValue = 1.6448534727,
                        .m_standardError = 0.3095197976,
                        .m_marginOfError = 0.5091147423},
      StatisticTestCase{.m_firstHypothesisParam = 0.,
                        .m_op = HypothesisParams::ComparisonOperator::Different,
                        .m_numberOfInputs = 6,
                        .m_inputs = {1542, 2, 140, 1345.8, 7, .1},
                        .m_significanceLevel = 0.01,
                        .m_confidenceLevel = 0.99,
                        .m_numberOfParameters = 7,
                        .m_hasDegreeOfFreedom = false,
                        .m_testPassed = false,
                        .m_testCriticalValue = 1.9819186926,
                        .m_pValue = 0.0474883318,
                        .m_estimate = 1542 - 1345.8f,
                        .m_intervalCriticalValue = 2.5758295059,
                        .m_standardError = 98.9949569702,
                        .m_marginOfError = 254.9941253662}};
  for (int i = 0; i < sizeof(tests) / sizeof(StatisticTestCase); i++) {
    testStatistic(&stat, tests[i]);
  }
}

QUIZ_CASE(probability_goodness_statistic) {
  StatisticTestCase tests[] = {
      StatisticTestCase{.m_op = HypothesisParams::ComparisonOperator::Higher,
                        .m_numberOfInputs = 8,
                        .m_inputs = {1, 2, 2, 1, 3, 4, 4, 3},
                        .m_significanceLevel = 0.03,
                        .m_confidenceLevel = 0.9,
                        .m_numberOfParameters = GoodnessStatistic::k_maxNumberOfRows * 2 + 1,
                        .m_hasDegreeOfFreedom = true,
                        .m_degreesOfFreedom = 3,
                        .m_testPassed = false,
                        .m_testCriticalValue = 2.0833332539,
                        .m_pValue = 0.5552918911}};
  GoodnessStatistic stat;
  for (int i = 0; i < sizeof(tests) / sizeof(StatisticTestCase); i++) {
    inputValues(&stat, tests[i]);
    stat.recomputeData();
    runTest(&stat, tests[i]);
  }
}

QUIZ_CASE(probability_homogeneity_statistic) {
  // clang-format off
  StatisticTestCase tests[] = {StatisticTestCase{
      .m_op = HypothesisParams::ComparisonOperator::Higher,
      .m_numberOfInputs = HomogeneityStatistic::k_maxNumberOfColumns *
                          HomogeneityStatistic::k_maxNumberOfRows,
      .m_inputs = {1,   2,   4,   NAN, NAN, NAN, NAN, NAN, NAN, NAN,
                   2,   5,   5,   NAN, NAN, NAN, NAN, NAN, NAN, NAN,
                   4,   3,   2,   NAN, NAN, NAN, NAN, NAN, NAN, NAN,
                   NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN,
                   NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN,
                   NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN,
                   NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN,
                   NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN,
                   NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN,
                   NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN},
      .m_significanceLevel = 0.03,
      .m_confidenceLevel = 0.9,
      .m_numberOfParameters =
          HomogeneityStatistic::k_maxNumberOfColumns * HomogeneityStatistic::k_maxNumberOfRows + 1,
      .m_hasDegreeOfFreedom = true,
      .m_degreesOfFreedom = 4,
      .m_testPassed = false,
      .m_testCriticalValue = 3.5017316341,
      .m_pValue = 0.4776151180}};
  float expectedValues[2][HomogeneityStatistic::k_maxNumberOfColumns *
                          HomogeneityStatistic::k_maxNumberOfRows] = {
      {1.75,  2.5,     2.75,   NAN,   NAN,   NAN,   NAN,   NAN,   NAN,   NAN,
       3,     4.2857,  4.714,  NAN,   NAN,   NAN,   NAN,   NAN,   NAN,   NAN,
       2.25,  3.214,   3.536,  NAN,   NAN,   NAN,   NAN,   NAN,   NAN,   NAN,
       NAN,   NAN,     NAN,    NAN,   NAN,   NAN,   NAN,   NAN,   NAN,   NAN,
       NAN,   NAN,     NAN,    NAN,   NAN,   NAN,   NAN,   NAN,   NAN,   NAN,
       NAN,   NAN,     NAN,    NAN,   NAN,   NAN,   NAN,   NAN,   NAN,   NAN,
       NAN,   NAN,     NAN,    NAN,   NAN,   NAN,   NAN,   NAN,   NAN,   NAN,
       NAN,   NAN,     NAN,    NAN,   NAN,   NAN,   NAN,   NAN,   NAN,   NAN}};
  // clang-format on
  HomogeneityStatistic stat;
  for (int i = 0; i < sizeof(tests) / sizeof(StatisticTestCase); i++) {
    inputValues(&stat, tests[i]);
    stat.recomputeData();
    runTest(&stat, tests[i]);
    // Check expected values
    for (int j = 0;
         j < HomogeneityStatistic::k_maxNumberOfColumns * HomogeneityStatistic::k_maxNumberOfRows;
         j++) {
      float expected = stat.expectedValueAtLocation(j / HomogeneityStatistic::k_maxNumberOfColumns,
                                                    j % HomogeneityStatistic::k_maxNumberOfColumns);
      float real = expectedValues[i][j];
      quiz_assert((std::isnan(real) && std::isnan(expected)) ||
                  roughlyEqual<float>(real, expected, 1e-4));
    }
  }
}