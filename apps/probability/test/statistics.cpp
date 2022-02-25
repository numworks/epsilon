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

/* TODO: some values have been computed from an approximative source of
 * verity... It should be double-checked. */

struct StatisticTestCase {
  // Inputs
  double m_firstHypothesisParam;
  HypothesisParams::ComparisonOperator m_op;
  int m_numberOfInputs;
  double m_inputs[100];
  double m_significanceLevel;
  double m_confidenceLevel;

  // Results
  int m_numberOfParameters;
  bool m_hasDegreeOfFreedom;
  double m_degreesOfFreedom;
  bool m_testPassed;
  double m_testCriticalValue;
  double m_pValue;
  double m_estimate;
  double m_intervalCriticalValue;
  double m_standardError;
  double m_marginOfError;
};

double tolerance() { return 1E11 * DBL_EPSILON; }

void inputValues(Statistic * stat, StatisticTestCase & test) {
  quiz_assert(stat->hasDegreeOfFreedom() == test.m_hasDegreeOfFreedom);

  stat->initThreshold(Data::SubApp::Tests);
  assertRoughlyEqual<double>(stat->threshold(), 0.05, tolerance());  // Significance level
  stat->setThreshold(test.m_significanceLevel);
  assertRoughlyEqual<double>(stat->threshold(), test.m_significanceLevel, tolerance());

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
  assertRoughlyEqual<double>(stat->testCriticalValue(), test.m_testCriticalValue, tolerance());
  assertRoughlyEqual<double>(stat->pValue(), test.m_pValue, tolerance());
  if (stat->hasDegreeOfFreedom()) {
    assertRoughlyEqual(stat->degreeOfFreedom(), test.m_degreesOfFreedom, tolerance());
  }
}

void testStatistic(Statistic * stat, StatisticTestCase & test) {
  inputValues(stat, test);

  // Test
  runTest(stat, test);

  // Confidence interval
  stat->initThreshold(Data::SubApp::Intervals);
  assertRoughlyEqual<double>(stat->threshold(), 0.95, tolerance());  // Confidence level
  stat->setThreshold(test.m_confidenceLevel);
  assertRoughlyEqual<double>(stat->threshold(), test.m_confidenceLevel, tolerance());

  stat->computeInterval();

  assertRoughlyEqual<double>(stat->estimate(), test.m_estimate, tolerance());
  assertRoughlyEqual<double>(stat->intervalCriticalValue(), test.m_intervalCriticalValue, tolerance());
  assertRoughlyEqual<double>(stat->standardError(), test.m_standardError, tolerance());
  assertRoughlyEqual<double>(stat->marginOfError(), test.m_marginOfError, tolerance());
}

QUIZ_CASE(probability_one_proportion_statistic) {
  StatisticTestCase tests[2];
  tests[0].m_firstHypothesisParam = 0.4;
  tests[0].m_op = HypothesisParams::ComparisonOperator::Lower;
  tests[0].m_numberOfInputs = 2;
  tests[0].m_inputs[0] = 12;
  tests[0].m_inputs[1] = 50;
  tests[0].m_significanceLevel = 0.05;
  tests[0].m_confidenceLevel = 0.95;
  tests[0].m_numberOfParameters = 3;
  tests[0].m_hasDegreeOfFreedom = false;
  tests[0].m_testPassed = true;
  tests[0].m_testCriticalValue = -2.309401076758503;
  tests[0].m_pValue = 0.0104606676688970144527;
  tests[0].m_estimate = 12. / 50.;
  tests[0].m_intervalCriticalValue = 1.96;
  tests[0].m_standardError = 0.0603986754821659975778934784555689364657775576369870411063890152;
  tests[0].m_marginOfError = 0.118381403945045355252671217772915115472924012968494600568522469792;

  tests[1].m_firstHypothesisParam = 0.9;
  tests[1].m_op = HypothesisParams::ComparisonOperator::Different;
  tests[1].m_numberOfInputs = 2;
  tests[1].m_inputs[0] = 84;
  tests[1].m_inputs[1] = 100;
  tests[1].m_significanceLevel = 0.01;
  tests[1].m_confidenceLevel = 0.99;
  tests[1].m_numberOfParameters = 3;
  tests[1].m_hasDegreeOfFreedom = false;
  tests[1].m_testPassed = false;
  tests[1].m_testCriticalValue = -2.0;
  tests[1].m_pValue = 0.04550026389635841440056;
  tests[1].m_estimate = 84. / 100.;
  tests[1].m_intervalCriticalValue = 2.57582951;
  tests[1].m_standardError = 0.0366606079;
  tests[1].m_marginOfError = 0.0944314748;

  OneProportionStatistic stat;
  for (int i = 0; i < sizeof(tests) / sizeof(StatisticTestCase); i++) {
    testStatistic(&stat, tests[i]);
  }
}

QUIZ_CASE(probability_one_mean_t_statistic) {
  StatisticTestCase tests[2];
  tests[0].m_firstHypothesisParam = 128;
  tests[0].m_op = HypothesisParams::ComparisonOperator::Lower;
  tests[0].m_numberOfInputs = 3;
  tests[0].m_inputs[0] = 127.8;
  tests[0].m_inputs[1] = 3.2;
  tests[0].m_inputs[2] = 50;
  tests[0].m_significanceLevel = 0.05;
  tests[0].m_confidenceLevel = 0.95;
  tests[0].m_numberOfParameters = 4;
  tests[0].m_hasDegreeOfFreedom = true;
  tests[0].m_degreesOfFreedom = 50.0 - 1.0;
  tests[0].m_testPassed = false;
  tests[0].m_testCriticalValue = -0.4419349730;
  tests[0].m_pValue = 0.3302403092;
  tests[0].m_estimate = 127.8;
  tests[0].m_intervalCriticalValue = 2.0095756054;
  tests[0].m_standardError = 0.4525483549;
  tests[0].m_marginOfError = 0.9094301462;

  tests[1].m_firstHypothesisParam = -4;
  tests[1].m_op = HypothesisParams::ComparisonOperator::Different;
  tests[1].m_numberOfInputs = 3;
  tests[1].m_inputs[0] = 1.4;
  tests[1].m_inputs[1] = 5;
  tests[1].m_inputs[2] = 10;
  tests[1].m_significanceLevel = 0.01;
  tests[1].m_confidenceLevel = 0.99;
  tests[1].m_numberOfParameters = 4;
  tests[1].m_hasDegreeOfFreedom = true;
  tests[1].m_degreesOfFreedom = 10 - 1;
  tests[1].m_testPassed = true;
  tests[1].m_testCriticalValue = 3.4152598381;
  tests[1].m_pValue = 0.0076853633;
  tests[1].m_estimate = 1.4;
  tests[1].m_intervalCriticalValue = 3.2498362064;
  tests[1].m_standardError = 1.5811388493;
  tests[1].m_marginOfError = 5.1384425163;

  OneMeanTStatistic stat;
  for (int i = 0; i < sizeof(tests) / sizeof(StatisticTestCase); i++) {
    testStatistic(&stat, tests[i]);
  }
}

QUIZ_CASE(probability_one_mean_z_statistic) {
  StatisticTestCase tests[2];
  tests[0].m_firstHypothesisParam = 128;
  tests[0].m_op = HypothesisParams::ComparisonOperator::Lower;
  tests[0].m_numberOfInputs = 3;
  tests[0].m_inputs[0] = 127.8;
  tests[0].m_inputs[1] = 50;
  tests[0].m_inputs[2] = 3.2;
  tests[0].m_significanceLevel = 0.05;
  tests[0].m_confidenceLevel = 0.95;
  tests[0].m_numberOfParameters = 4;
  tests[0].m_hasDegreeOfFreedom = false;
  tests[0].m_testPassed = false;
  tests[0].m_testCriticalValue = -0.4419349730;
  tests[0].m_pValue = 0.3292681310859755967358;
  tests[0].m_estimate = 127.8;
  tests[0].m_intervalCriticalValue = 1.96;
  tests[0].m_standardError = 0.4525483549;
  tests[0].m_marginOfError = 0.8869786859;

  tests[1].m_firstHypothesisParam = 0.9;
  tests[1].m_op = HypothesisParams::ComparisonOperator::Different;
  tests[1].m_numberOfInputs = 3;
  tests[1].m_inputs[0] = 2.3;
  tests[1].m_inputs[1] = 1000;
  tests[1].m_inputs[2] = 14;
  tests[1].m_significanceLevel = 0.01;
  tests[1].m_confidenceLevel = 0.99;
  tests[1].m_numberOfParameters = 4;
  tests[1].m_hasDegreeOfFreedom = false;
  tests[1].m_testPassed = true;
  tests[1].m_testCriticalValue = 3.1622776601683793319988935444327185337195551393252168268;
  tests[1].m_pValue = 0.0015654022580026;
  tests[1].m_estimate = 2.3;
  tests[1].m_intervalCriticalValue = 2.5758295059;
  tests[1].m_standardError = 0.4427188933;
  tests[1].m_marginOfError = 1.1403683424;

  OneMeanZStatistic stat;
  for (int i = 0; i < sizeof(tests) / sizeof(StatisticTestCase); i++) {
    testStatistic(&stat, tests[i]);
  }
}

QUIZ_CASE(probability_two_proportions_statistic) {
  StatisticTestCase tests[2];
  tests[0].m_firstHypothesisParam = 0;
  tests[0].m_op = HypothesisParams::ComparisonOperator::Higher;
  tests[0].m_numberOfInputs = 4;
  tests[0].m_inputs[0] = 20;
  tests[0].m_inputs[1] = 50;
  tests[0].m_inputs[2] = 32;
  tests[0].m_inputs[3] = 103;
  tests[0].m_significanceLevel = 0.05;
  tests[0].m_confidenceLevel = 0.95;
  tests[0].m_numberOfParameters = 5;
  tests[0].m_hasDegreeOfFreedom = false;
  tests[0].m_testPassed = false;
  tests[0].m_testCriticalValue = 1.0940510035;
  tests[0].m_pValue = 0.1369662881;
  tests[0].m_estimate = 20. / 50. - 32. / 103.;
  tests[0].m_intervalCriticalValue = 1.96;
  tests[0].m_standardError = 0.0829409584;
  tests[0].m_marginOfError = 0.1625613272;

  tests[1].m_firstHypothesisParam = 0.3;
  tests[1].m_op = HypothesisParams::ComparisonOperator::Lower;
  tests[1].m_numberOfInputs = 4;
  tests[1].m_inputs[0] = 60;
  tests[1].m_inputs[1] = 100;
  tests[1].m_inputs[2] = 44.4;
  tests[1].m_inputs[3] = 90;
  tests[1].m_significanceLevel = 0.01;
  tests[1].m_confidenceLevel = 0.99;
  tests[1].m_numberOfParameters = 5;
  tests[1].m_hasDegreeOfFreedom = false;
  tests[1].m_testPassed = true;
  tests[1].m_testCriticalValue = -2.6117918491;
  tests[1].m_pValue = 0.0045034885;
  tests[1].m_estimate = 60.f / 100.f - 44.f / 90.f;
  tests[1].m_intervalCriticalValue = 2.5758295059;
  tests[1].m_standardError = 0.0719472468;
  tests[1].m_marginOfError = 0.1853238344;

  TwoProportionsStatistic stat;
  for (int i = 0; i < sizeof(tests) / sizeof(StatisticTestCase); i++) {
    testStatistic(&stat, tests[i]);
  }
}

QUIZ_CASE(probability_two_means_t_statistic) {
  StatisticTestCase tests[2];
  tests[0].m_firstHypothesisParam = 0;
  tests[0].m_op = HypothesisParams::ComparisonOperator::Higher;
  tests[0].m_numberOfInputs = 6;
  tests[0].m_inputs[0] = 20;
  tests[0].m_inputs[1] = 2;
  tests[0].m_inputs[2] = 50;
  tests[0].m_inputs[3] = 24;
  tests[0].m_inputs[4] = 18;
  tests[0].m_inputs[5] = 60;
  tests[0].m_significanceLevel = 0.05;
  tests[0].m_confidenceLevel = 0.95;
  tests[0].m_numberOfParameters = 7;
  tests[0].m_hasDegreeOfFreedom = true;
  tests[0].m_degreesOfFreedom = 60.7450408936;
  tests[0].m_testPassed = false;
  tests[0].m_testCriticalValue = -1.7087153196;
  tests[0].m_pValue = 0.9536954761;
  tests[0].m_estimate = 20. - 24.;
  tests[0].m_intervalCriticalValue = 1.9997937679;
  tests[0].m_standardError = 2.3409399986;
  tests[0].m_marginOfError = 4.6813974380;

  tests[1].m_firstHypothesisParam = -12.345;
  tests[1].m_op = HypothesisParams::ComparisonOperator::Different;
  tests[1].m_numberOfInputs = 6;
  tests[1].m_inputs[0] = 4.2;
  tests[1].m_inputs[1] = 46;
  tests[1].m_inputs[2] = 1000;
  tests[1].m_inputs[3] = 18.3;
  tests[1].m_inputs[4] = 18;
  tests[1].m_inputs[5] = 60;
  tests[1].m_significanceLevel = 0.01;
  tests[1].m_confidenceLevel = 0.99;
  tests[1].m_numberOfParameters = 7;
  tests[1].m_hasDegreeOfFreedom = true;
  tests[1].m_degreesOfFreedom = 113.2706527710;
  tests[1].m_testPassed = false;
  tests[1].m_testCriticalValue = -0.6401526332;
  tests[1].m_pValue = 0.5233662128;
  tests[1].m_estimate = 4.2 - 18.3;
  tests[1].m_intervalCriticalValue = 2.6199319363;
  tests[1].m_standardError = 2.7415323257;
  tests[1].m_marginOfError = 7.1826281548;

  TwoMeansTStatistic stat;
  for (int i = 0; i < sizeof(tests) / sizeof(StatisticTestCase); i++) {
    testStatistic(&stat, tests[i]);
  }
}

QUIZ_CASE(probability_pooled_t_test) {
  StatisticTestCase tests[2];
  tests[0].m_firstHypothesisParam = 0.3;
  tests[0].m_op = HypothesisParams::ComparisonOperator::Higher;
  tests[0].m_numberOfInputs = 6;
  tests[0].m_inputs[0] = 213.4;
  tests[0].m_inputs[1] = 14;
  tests[0].m_inputs[2] = 234;
  tests[0].m_inputs[3] = 213.5;
  tests[0].m_inputs[4] = 135;
  tests[0].m_inputs[5] = 64;
  tests[0].m_significanceLevel = 0.02;
  tests[0].m_confidenceLevel = 0.876;
  tests[0].m_numberOfParameters = 7;
  tests[0].m_hasDegreeOfFreedom = true;
  tests[0].m_degreesOfFreedom = 296;
  tests[0].m_testPassed = false;
  tests[0].m_testCriticalValue = -0.0446507446;
  tests[0].m_pValue = 0.5177921057;
  tests[0].m_estimate = 213.4 - 213.5;
  tests[0].m_intervalCriticalValue = 1.5425841808;
  tests[0].m_standardError = 8.9585542679;
  tests[0].m_marginOfError = 13.8193244934;

  tests[1].m_firstHypothesisParam = 0.;
  tests[1].m_op = HypothesisParams::ComparisonOperator::Higher;
  tests[1].m_numberOfInputs = 6;
  tests[1].m_inputs[0] = 1.23;
  tests[1].m_inputs[1] = 1.23;
  tests[1].m_inputs[2] = 12;
  tests[1].m_inputs[3] = 0.2;
  tests[1].m_inputs[4] = 0.12;
  tests[1].m_inputs[5] = 12;
  tests[1].m_significanceLevel = 0.1;
  tests[1].m_confidenceLevel = 0.9;
  tests[1].m_numberOfParameters = 7;
  tests[1].m_hasDegreeOfFreedom = true;
  tests[1].m_degreesOfFreedom = 22;
  tests[1].m_testPassed = true;
  tests[1].m_testCriticalValue = 2.887125562;
  tests[1].m_pValue = 0.0042771697;
  tests[1].m_estimate = 1.23 - 0.2;
  tests[1].m_intervalCriticalValue = 1.7171442509;
  tests[1].m_standardError = 0.3567562103;
  tests[1].m_marginOfError = 0.6126018763;

  PooledTwoMeansStatistic stat;
  for (int i = 0; i < sizeof(tests) / sizeof(StatisticTestCase); i++) {
    testStatistic(&stat, tests[i]);
  }
}

QUIZ_CASE(probability_two_means_z_statistic) {
  TwoMeansZStatistic stat;
  StatisticTestCase tests[2];
  tests[0].m_firstHypothesisParam = 0.;
  tests[0].m_op = HypothesisParams::ComparisonOperator::Higher;
  tests[0].m_numberOfInputs = 6;
  tests[0].m_inputs[0] = 3.14;
  tests[0].m_inputs[1] = 119;
  tests[0].m_inputs[2] = 2;
  tests[0].m_inputs[3] = 2.07;
  tests[0].m_inputs[4] = 402;
  tests[0].m_inputs[5] = 5;
  tests[0].m_significanceLevel = 0.1;
  tests[0].m_confidenceLevel = 0.9;
  tests[0].m_numberOfParameters = 7;
  tests[0].m_hasDegreeOfFreedom = false;
  tests[0].m_testPassed = true;
  tests[0].m_testCriticalValue = 3.4569679750017427679395672327548454397723101938825021184918;
  tests[0].m_pValue = 0.000273144883100289900;
  tests[0].m_estimate = 3.14 - 2.07;
  tests[0].m_intervalCriticalValue = 1.6448534727;
  tests[0].m_standardError = 0.3095197976;
  tests[0].m_marginOfError = 0.5091147423;

  tests[1].m_firstHypothesisParam = 0.;
  tests[1].m_op = HypothesisParams::ComparisonOperator::Different;
  tests[1].m_numberOfInputs = 6;
  tests[1].m_inputs[0] = 1542;
  tests[1].m_inputs[1] = 2;
  tests[1].m_inputs[2] = 140;
  tests[1].m_inputs[3] = 1345.8;
  tests[1].m_inputs[4] = 7;
  tests[1].m_inputs[5] = 0.1;
  tests[1].m_significanceLevel = 0.01;
  tests[1].m_confidenceLevel = 0.99;
  tests[1].m_numberOfParameters = 7;
  tests[1].m_hasDegreeOfFreedom = false;
  tests[1].m_testPassed = false;
  tests[1].m_testCriticalValue = 1.9819186926;
  tests[1].m_pValue = 0.0474883318;
  tests[1].m_estimate = 1542 - 1345.8;
  tests[1].m_intervalCriticalValue = 2.5758295059;
  tests[1].m_standardError = 98.9949569702;
  tests[1].m_marginOfError = 254.9941253662;

  for (int i = 0; i < sizeof(tests) / sizeof(StatisticTestCase); i++) {
    testStatistic(&stat, tests[i]);
  }
}

QUIZ_CASE(probability_goodness_statistic) {
  GoodnessStatistic stat;
  StatisticTestCase tests[1];
  bool isDegreeOfFreedomOverridden = false;
  int overriddenDegreeOfFreedom = 3;
  tests[0].m_op = HypothesisParams::ComparisonOperator::Higher;
  tests[0].m_numberOfInputs = 8;
  tests[0].m_inputs[0] = 1;
  tests[0].m_inputs[1] = 2;
  tests[0].m_inputs[2] = 2;
  tests[0].m_inputs[3] = 1;
  tests[0].m_inputs[4] = 3;
  tests[0].m_inputs[5] = 4;
  tests[0].m_inputs[6] = 4;
  tests[0].m_inputs[7] = 3;
  tests[0].m_significanceLevel = 0.03;
  tests[0].m_confidenceLevel = 0.9;
  tests[0].m_numberOfParameters = stat.maxNumberOfRows() * 2 + 1;
  tests[0].m_hasDegreeOfFreedom = true;
  tests[0].m_degreesOfFreedom = overriddenDegreeOfFreedom;
  tests[0].m_testPassed = false;
  tests[0].m_testCriticalValue = 2.0833332539;
  tests[0].m_pValue = 0.5552918911;
  for (int i = 0; i < sizeof(tests) / sizeof(StatisticTestCase); i++) {
    inputValues(&stat, tests[i]);
    stat.recomputeData();
    /* Degree of freedom is either overridden or computed as the user inputs
     * values in the UI table. It must be set here to replicate this. */
    int degreeOfFreedom = isDegreeOfFreedomOverridden ? overriddenDegreeOfFreedom : stat.computeDegreesOfFreedom();
    stat.setDegreeOfFreedom(degreeOfFreedom);
    runTest(&stat, tests[i]);
  }
}

QUIZ_CASE(probability_goodness_statistic_overridden_degree_of_freedom) {
  GoodnessStatistic stat;
  StatisticTestCase tests[1];
  bool isDegreeOfFreedomOverridden = true;
  int overriddenDegreeOfFreedom = 5;
  tests[0].m_op = HypothesisParams::ComparisonOperator::Higher;
  tests[0].m_numberOfInputs = 8;
  tests[0].m_inputs[0] = 1;
  tests[0].m_inputs[1] = 2;
  tests[0].m_inputs[2] = 2;
  tests[0].m_inputs[3] = 1;
  tests[0].m_inputs[4] = 3;
  tests[0].m_inputs[5] = 4;
  tests[0].m_inputs[6] = 4;
  tests[0].m_inputs[7] = 3;
  tests[0].m_significanceLevel = 0.03;
  tests[0].m_confidenceLevel = 0.9;
  tests[0].m_numberOfParameters = stat.maxNumberOfRows() * 2 + 1;
  tests[0].m_hasDegreeOfFreedom = true;
  tests[0].m_degreesOfFreedom = overriddenDegreeOfFreedom;
  tests[0].m_testPassed = false;
  tests[0].m_testCriticalValue = 2.0833332539;
  tests[0].m_pValue = 0.837503;
  for (size_t i = 0; i < sizeof(tests) / sizeof(StatisticTestCase); i++) {
    inputValues(&stat, tests[i]);
    stat.recomputeData();
    /* Degree of freedom is either overridden or computed as the user inputs
     * values in the UI table. It must be set here to replicate this. */
    int degreeOfFreedom = isDegreeOfFreedomOverridden ? overriddenDegreeOfFreedom : stat.computeDegreesOfFreedom();
    stat.setDegreeOfFreedom(degreeOfFreedom);
    runTest(&stat, tests[i]);
  }
}

QUIZ_CASE(probability_homogeneity_statistic) {
  // clang-format off
  StatisticTestCase tests[1];
  tests[0].m_op = HypothesisParams::ComparisonOperator::Higher;
  tests[0].m_numberOfInputs = HomogeneityStatistic::k_maxNumberOfColumns *
                          HomogeneityStatistic::k_maxNumberOfRows;
  /*{1,   2,   4,   NAN, NAN, NAN, NAN, NAN, NAN,
     2,   5,   5,   NAN, NAN, NAN, NAN, NAN, NAN,
     4,   3,   2,   NAN, NAN, NAN, NAN, NAN, NAN,
     NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN,
     NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN,
     NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN,
     NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN,
     NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN,
     NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN} */
  for (int i = 0; i < sizeof(tests[0].m_inputs)/sizeof(tests[0].m_inputs[0]); i++) {
    tests[0].m_inputs[i] = NAN;
  }
  tests[0].m_inputs[0 * 9 + 0] = 1;
  tests[0].m_inputs[0 * 9 + 1] = 2;
  tests[0].m_inputs[0 * 9 + 2] = 4;
  tests[0].m_inputs[1 * 9 + 0] = 2;
  tests[0].m_inputs[1 * 9 + 1] = 5;
  tests[0].m_inputs[1 * 9 + 2] = 5;
  tests[0].m_inputs[2 * 9 + 0] = 4;
  tests[0].m_inputs[2 * 9 + 1] = 3;
  tests[0].m_inputs[2 * 9 + 2] = 2;
  tests[0].m_significanceLevel = 0.03;
  tests[0].m_confidenceLevel = 0.9;
  tests[0].m_numberOfParameters =
          HomogeneityStatistic::k_maxNumberOfColumns * HomogeneityStatistic::k_maxNumberOfRows + 1;
  tests[0].m_hasDegreeOfFreedom = true;
  tests[0].m_degreesOfFreedom = 4;
  tests[0].m_testPassed = false;
  tests[0].m_testCriticalValue = 3.5017316341;
  tests[0].m_pValue = 0.4776151180;
  double expectedValues[2][HomogeneityStatistic::k_maxNumberOfColumns *
                          HomogeneityStatistic::k_maxNumberOfRows] = {
      {1.75,  2.5,     2.75,   NAN,   NAN,   NAN,   NAN,   NAN,   NAN,
       3.0,   4.2857,  4.714,  NAN,   NAN,   NAN,   NAN,   NAN,   NAN,
       2.25,  3.214,   3.536,  NAN,   NAN,   NAN,   NAN,   NAN,   NAN,
       NAN,   NAN,     NAN,    NAN,   NAN,   NAN,   NAN,   NAN,   NAN,
       NAN,   NAN,     NAN,    NAN,   NAN,   NAN,   NAN,   NAN,   NAN,
       NAN,   NAN,     NAN,    NAN,   NAN,   NAN,   NAN,   NAN,   NAN,
       NAN,   NAN,     NAN,    NAN,   NAN,   NAN,   NAN,   NAN,   NAN,
       NAN,   NAN,     NAN,    NAN,   NAN,   NAN,   NAN,   NAN,   NAN,
       NAN,   NAN,     NAN,    NAN,   NAN,   NAN,   NAN,   NAN,   NAN}};
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
      double expected = stat.expectedValueAtLocation(j / HomogeneityStatistic::k_maxNumberOfColumns,
                                                    j % HomogeneityStatistic::k_maxNumberOfColumns);
      double real = expectedValues[i][j];
      quiz_assert((std::isnan(real) && std::isnan(expected)) ||
                  roughlyEqual<double>(real, expected, 1e-4));
    }
  }
}
