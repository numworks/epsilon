#include <math.h>
#include <poincare/test/helper.h>
#include <quiz.h>

#include <array>

#include "inference/models/statistic/goodness_test.h"
#include "inference/models/statistic/homogeneity_test.h"
#include "inference/models/statistic/one_mean_t_interval.h"
#include "inference/models/statistic/one_mean_t_test.h"
#include "inference/models/statistic/one_mean_z_interval.h"
#include "inference/models/statistic/one_mean_z_test.h"
#include "inference/models/statistic/one_proportion_z_interval.h"
#include "inference/models/statistic/one_proportion_z_test.h"
#include "inference/models/statistic/pooled_two_means_t_interval.h"
#include "inference/models/statistic/pooled_two_means_t_test.h"
#include "inference/models/statistic/slope_t_interval.h"
#include "inference/models/statistic/slope_t_statistic.h"
#include "inference/models/statistic/slope_t_test.h"
#include "inference/models/statistic/statistic.h"
#include "inference/models/statistic/two_means_t_interval.h"
#include "inference/models/statistic/two_means_t_test.h"
#include "inference/models/statistic/two_means_z_interval.h"
#include "inference/models/statistic/two_means_z_test.h"
#include "inference/models/statistic/two_proportions_z_interval.h"
#include "inference/models/statistic/two_proportions_z_test.h"
#include "inference/models/statistic_buffer.h"

using namespace Inference;

/* TODO: some values have been computed from an approximative source of
 * verity... It should be double-checked. */

struct StatisticTestCase {
  // Inputs
  double m_firstHypothesisParam;
  Poincare::ComparisonNode::OperatorType m_op;
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

void inputThreshold(Statistic* stat, double threshold) {
  stat->setThreshold(threshold);
  assert_roughly_equal<double>(stat->threshold(), threshold, tolerance());
}

void inputValues(Statistic* stat, StatisticTestCase& testCase,
                 double initialThreshold) {
  stat->initParameters();
  assert_roughly_equal<double>(stat->threshold(), initialThreshold,
                               tolerance());
  for (int i = 0; i < testCase.m_numberOfInputs; i++) {
    stat->setParameterAtIndex(testCase.m_inputs[i], i);
    quiz_assert((stat->parameterAtIndex(i) && testCase.m_inputs[i]) ||
                (stat->parameterAtIndex(i) == testCase.m_inputs[i]));
  }
}

void inputTableValues(Table* table, Statistic* stat,
                      StatisticTestCase& testCase) {
  stat->initParameters();
  for (int i = 0; i < testCase.m_numberOfInputs; i++) {
    Table::Index2D rowCol = table->indexToIndex2D(i);
    ;
    table->setParameterAtPosition(testCase.m_inputs[i], rowCol.row, rowCol.col);
    quiz_assert((table->parameterAtPosition(rowCol.row, rowCol.col) &&
                 testCase.m_inputs[i]) ||
                (table->parameterAtPosition(rowCol.row, rowCol.col) ==
                 testCase.m_inputs[i]));
  }
}

void testTest(Test* test, StatisticTestCase& testCase) {
  inputThreshold(test, testCase.m_significanceLevel);
  test->hypothesisParams()->setFirstParam(testCase.m_firstHypothesisParam);
  test->hypothesisParams()->setComparisonOperator(testCase.m_op);

  test->compute();

  quiz_assert(test->numberOfParameters() == testCase.m_numberOfParameters);
  quiz_assert(test->canRejectNull() == testCase.m_testPassed);
  assert_roughly_equal<double>(test->testCriticalValue(),
                               testCase.m_testCriticalValue, tolerance());
  assert_roughly_equal<double>(test->pValue(), testCase.m_pValue, tolerance());
  quiz_assert(test->hasDegreeOfFreedom() == testCase.m_hasDegreeOfFreedom);
  if (test->hasDegreeOfFreedom()) {
    assert_roughly_equal(test->degreeOfFreedom(), testCase.m_degreesOfFreedom,
                         tolerance());
  }
}

void testInterval(Interval* interval, StatisticTestCase& testCase) {
  inputThreshold(interval, testCase.m_confidenceLevel);

  interval->compute();

  assert_roughly_equal<double>(interval->estimate(), testCase.m_estimate,
                               tolerance());
  assert_roughly_equal<double>(interval->intervalCriticalValue(),
                               testCase.m_intervalCriticalValue, tolerance());
  assert_roughly_equal<double>(interval->standardError(),
                               testCase.m_standardError, tolerance());
  assert_roughly_equal<double>(interval->marginOfError(),
                               testCase.m_marginOfError, tolerance());
}

QUIZ_CASE(probability_one_proportion_statistic) {
  StatisticTestCase tests[2];
  tests[0].m_firstHypothesisParam = 0.4;
  tests[0].m_op = Poincare::ComparisonNode::OperatorType::Inferior;
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
  tests[0].m_standardError =
      0.0603986754821659975778934784555689364657775576369870411063890152;
  tests[0].m_marginOfError =
      0.118381403945045355252671217772915115472924012968494600568522469792;

  tests[1].m_firstHypothesisParam = 0.9;
  tests[1].m_op = Poincare::ComparisonNode::OperatorType::NotEqual;
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

  OneProportionZTest test;
  OneProportionZInterval interval;
  for (size_t i = 0; i < std::size(tests); i++) {
    inputValues(&test, tests[i], 0.05);
    testTest(&test, tests[i]);
    inputValues(&interval, tests[i], 0.95);
    testInterval(&interval, tests[i]);
  }
}

QUIZ_CASE(probability_one_mean_t_statistic) {
  StatisticTestCase tests[2];
  tests[0].m_firstHypothesisParam = 128;
  tests[0].m_op = Poincare::ComparisonNode::OperatorType::Inferior;
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
  tests[1].m_op = Poincare::ComparisonNode::OperatorType::NotEqual;
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

  OneMeanTTest test;
  OneMeanTInterval interval;
  for (size_t i = 0; i < std::size(tests); i++) {
    inputValues(&test, tests[i], 0.05);
    testTest(&test, tests[i]);
    inputValues(&interval, tests[i], 0.95);
    testInterval(&interval, tests[i]);
  }
}

QUIZ_CASE(probability_one_mean_z_statistic) {
  StatisticTestCase tests[2];
  tests[0].m_firstHypothesisParam = 128;
  tests[0].m_op = Poincare::ComparisonNode::OperatorType::Inferior;
  tests[0].m_numberOfInputs = 3;
  tests[0].m_inputs[0] = 127.8;
  tests[0].m_inputs[1] = 3.2;
  tests[0].m_inputs[2] = 50;
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
  tests[1].m_op = Poincare::ComparisonNode::OperatorType::NotEqual;
  tests[1].m_numberOfInputs = 3;
  tests[1].m_inputs[0] = 2.3;
  tests[1].m_inputs[1] = 14;
  tests[1].m_inputs[2] = 1000;
  tests[1].m_significanceLevel = 0.01;
  tests[1].m_confidenceLevel = 0.99;
  tests[1].m_numberOfParameters = 4;
  tests[1].m_hasDegreeOfFreedom = false;
  tests[1].m_testPassed = true;
  tests[1].m_testCriticalValue =
      3.1622776601683793319988935444327185337195551393252168268;
  tests[1].m_pValue = 0.0015654022580026;
  tests[1].m_estimate = 2.3;
  tests[1].m_intervalCriticalValue = 2.5758295059;
  tests[1].m_standardError = 0.4427188933;
  tests[1].m_marginOfError = 1.1403683424;

  OneMeanZTest test;
  OneMeanZInterval interval;
  for (size_t i = 0; i < std::size(tests); i++) {
    inputValues(&test, tests[i], 0.05);
    testTest(&test, tests[i]);
    inputValues(&interval, tests[i], 0.95);
    testInterval(&interval, tests[i]);
  }
}

QUIZ_CASE(probability_two_proportions_statistic) {
  StatisticTestCase tests[2];
  tests[0].m_firstHypothesisParam = 0;
  tests[0].m_op = Poincare::ComparisonNode::OperatorType::Superior;
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
  tests[1].m_op = Poincare::ComparisonNode::OperatorType::Inferior;
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

  TwoProportionsZTest test;
  TwoProportionsZInterval interval;
  for (size_t i = 0; i < std::size(tests); i++) {
    inputValues(&test, tests[i], 0.05);
    testTest(&test, tests[i]);
    inputValues(&interval, tests[i], 0.95);
    testInterval(&interval, tests[i]);
  }
}

QUIZ_CASE(probability_two_means_t_statistic) {
  StatisticTestCase tests[2];
  tests[0].m_firstHypothesisParam = 0;
  tests[0].m_op = Poincare::ComparisonNode::OperatorType::Superior;
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
  tests[1].m_op = Poincare::ComparisonNode::OperatorType::NotEqual;
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

  TwoMeansTTest test;
  TwoMeansTInterval interval;
  for (size_t i = 0; i < std::size(tests); i++) {
    inputValues(&test, tests[i], 0.05);
    testTest(&test, tests[i]);
    inputValues(&interval, tests[i], 0.95);
    testInterval(&interval, tests[i]);
  }
}

QUIZ_CASE(probability_pooled_t_test) {
  StatisticTestCase tests[2];
  tests[0].m_firstHypothesisParam = 0.3;
  tests[0].m_op = Poincare::ComparisonNode::OperatorType::Superior;
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
  tests[1].m_op = Poincare::ComparisonNode::OperatorType::Superior;
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

  PooledTwoMeansTTest test;
  PooledTwoMeansTInterval interval;
  for (size_t i = 0; i < std::size(tests); i++) {
    inputValues(&test, tests[i], 0.05);
    testTest(&test, tests[i]);
    inputValues(&interval, tests[i], 0.95);
    testInterval(&interval, tests[i]);
  }
}

QUIZ_CASE(probability_two_means_z_statistic) {
  StatisticTestCase tests[2];
  tests[0].m_firstHypothesisParam = 0.;
  tests[0].m_op = Poincare::ComparisonNode::OperatorType::Superior;
  tests[0].m_numberOfInputs = 6;
  tests[0].m_inputs[0] = 3.14;
  tests[0].m_inputs[1] = 2;
  tests[0].m_inputs[2] = 119;
  tests[0].m_inputs[3] = 2.07;
  tests[0].m_inputs[4] = 5;
  tests[0].m_inputs[5] = 402;
  tests[0].m_significanceLevel = 0.1;
  tests[0].m_confidenceLevel = 0.9;
  tests[0].m_numberOfParameters = 7;
  tests[0].m_hasDegreeOfFreedom = false;
  tests[0].m_testPassed = true;
  tests[0].m_testCriticalValue =
      3.4569679750017427679395672327548454397723101938825021184918;
  tests[0].m_pValue = 0.000273144883100289900;
  tests[0].m_estimate = 3.14 - 2.07;
  tests[0].m_intervalCriticalValue = 1.6448534727;
  tests[0].m_standardError = 0.3095197976;
  tests[0].m_marginOfError = 0.5091147423;

  tests[1].m_firstHypothesisParam = 0.;
  tests[1].m_op = Poincare::ComparisonNode::OperatorType::NotEqual;
  tests[1].m_numberOfInputs = 6;
  tests[1].m_inputs[0] = 1542;
  tests[1].m_inputs[1] = 140;
  tests[1].m_inputs[2] = 2;
  tests[1].m_inputs[3] = 1345.8;
  tests[1].m_inputs[4] = 0.1;
  tests[1].m_inputs[5] = 7;
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

  TwoMeansZTest test;
  TwoMeansZInterval interval;
  for (size_t i = 0; i < std::size(tests); i++) {
    inputValues(&test, tests[i], 0.05);
    testTest(&test, tests[i]);
    inputValues(&interval, tests[i], 0.95);
    testInterval(&interval, tests[i]);
  }
}

QUIZ_CASE(probability_goodness_test) {
  GoodnessTest stat;
  StatisticTestCase tests[1];
  tests[0].m_op = Poincare::ComparisonNode::OperatorType::Superior;
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
  tests[0].m_degreesOfFreedom = 3;
  tests[0].m_testPassed = false;
  tests[0].m_testCriticalValue = 2.0833332539;
  tests[0].m_pValue = 0.5552918911;

  for (size_t i = 0; i < std::size(tests); i++) {
    stat.recomputeData();
    // Initialize values before calling computeDegreesOfFreedom
    inputTableValues(&stat, &stat, tests[i]);
    /* Degree of freedom is either overridden or computed as the user inputs
     * values in the UI table. It must be set here to replicate this. */
    stat.setDegreeOfFreedom(stat.computeDegreesOfFreedom());
    testTest(&stat, tests[i]);
    // Simulate user-input degree of freedom
    tests[i].m_degreesOfFreedom = 5;
    stat.setDegreeOfFreedom(tests[i].m_degreesOfFreedom);
    tests[i].m_pValue = 0.837503;
    testTest(&stat, tests[i]);
  }
}

QUIZ_CASE(probability_homogeneity_test) {
  // clang-format off
  StatisticTestCase tests[1];
  tests[0].m_op = Poincare::ComparisonNode::OperatorType::Superior;
  tests[0].m_numberOfInputs = HomogeneityTest::k_maxNumberOfColumns *
                          HomogeneityTest::k_maxNumberOfRows;
  /*{1,   2,   4,   NAN, NAN, NAN, NAN, NAN, NAN,
     2,   5,   5,   NAN, NAN, NAN, NAN, NAN, NAN,
     4,   3,   2,   NAN, NAN, NAN, NAN, NAN, NAN,
     NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN,
     NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN,
     NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN,
     NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN,
     NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN,
     NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN} */
  for (size_t i = 0; i < std::size(tests[0].m_inputs); i++) {
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
          HomogeneityTest::k_maxNumberOfColumns * HomogeneityTest::k_maxNumberOfRows + 1;
  tests[0].m_hasDegreeOfFreedom = true;
  tests[0].m_degreesOfFreedom = 4;
  tests[0].m_testPassed = false;
  tests[0].m_testCriticalValue = 3.5017316341;
  tests[0].m_pValue = 0.4776151180;
  double expectedValues[2][HomogeneityTest::k_maxNumberOfColumns *
                          HomogeneityTest::k_maxNumberOfRows] = {
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
  HomogeneityTest test;
  for (size_t i = 0; i < std::size(tests); i++) {
    test.recomputeData();
    inputTableValues(&test, &test, tests[i]);

    testTest(&test, tests[i]);
    // Check expected values
    for (int j = 0; j < HomogeneityTest::k_maxNumberOfColumns *
                            HomogeneityTest::k_maxNumberOfRows;
         j++) {
      double expected = test.expectedValueAtLocation(
          j / HomogeneityTest::k_maxNumberOfColumns,
          j % HomogeneityTest::k_maxNumberOfColumns);
      double real = expectedValues[i][j];
      assert_roughly_equal(real, expected, 1E-4, true);
    }
  }
}

QUIZ_CASE(probability_slope_t_statistic) {
  Shared::GlobalContext context;
  StatisticTestCase testCase;
  testCase.m_firstHypothesisParam = 0.;
  testCase.m_op = Poincare::ComparisonNode::OperatorType::NotEqual;
  testCase.m_numberOfInputs = SlopeTStatistic::k_maxNumberOfColumns * 8;
  testCase.m_inputs[0] = 7;
  testCase.m_inputs[1] = 7.10;
  testCase.m_inputs[2] = 8;
  testCase.m_inputs[3] = 7.14;
  testCase.m_inputs[4] = 9;
  testCase.m_inputs[5] = 6.50;
  testCase.m_inputs[6] = 10;
  testCase.m_inputs[7] = 6.78;
  testCase.m_inputs[8] = 11;
  testCase.m_inputs[9] = 6.44;
  testCase.m_inputs[10] = 12;
  testCase.m_inputs[11] = 6.94;
  testCase.m_inputs[12] = 13;
  testCase.m_inputs[13] = 6.3;
  testCase.m_inputs[14] = 14;
  testCase.m_inputs[15] = 6.46;
  testCase.m_significanceLevel = 0.1;
  testCase.m_confidenceLevel = 0.9;
  testCase.m_numberOfParameters =
      SlopeTStatistic::k_maxNumberOfColumns *
          Shared::DoublePairStore::k_maxNumberOfPairs +
      1;
  testCase.m_hasDegreeOfFreedom = true;
  testCase.m_degreesOfFreedom = 6;
  testCase.m_testPassed = true;
  testCase.m_testCriticalValue = -0.0916667 / 0.03931119904518827;
  testCase.m_pValue = 0.05849481308399189;
  testCase.m_estimate = -0.0916667;
  testCase.m_intervalCriticalValue = 1.94318;
  testCase.m_standardError = 0.03931119904518827;
  testCase.m_marginOfError = 0.0763887357606289424986;

  SlopeTTest test(&context);
  inputTableValues(&test, &test, testCase);
  testTest(&test, testCase);
  SlopeTInterval interval(&context);
  inputTableValues(&interval, &interval, testCase);
  testInterval(&interval, testCase);
}
