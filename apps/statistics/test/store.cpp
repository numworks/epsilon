#include <quiz.h>
#include <apps/i18n.h>
#include <apps/global_preferences.h>
#include <assert.h>
#include <math.h>
#include <cmath>
#include "../store.h"
#include <poincare/helpers.h>
#include <poincare/test/helper.h>

using namespace Poincare;

namespace Statistics {

void assert_value_approximately_equal_to(double d1, double d2, double precision, double reference) {
  quiz_assert((std::isnan(d1) && std::isnan(d2))
      || (std::isinf(d1) && std::isinf(d2) && d1 * d2 > 0.0 /*same sign*/)
      || IsApproximatelyEqual(d1, d2, precision, reference));
}

/* SublistMethod is the method for computing quartiles used in most
 * countries, which defines quartiles as the medians of the left and right
 * subsets of data.
 * FrequencyMethod is the method used in France and Italy, which defines the
 * quartiles as the 25th and 75th percentile, in terms of cumulated
 * frequencies. */
void assert_data_statictics_equal_to(
    double v[],
    double n[],
    int numberOfData,
    double trueSumOfOccurrences,
    double trueMaxValue,
    double trueMinValue,
    double trueRange,
    double trueMean,
    double trueVariance,
    double trueStandardDeviation,
    double trueSampleStandardDeviation,
    double trueFirstQuartileSublistMethod,
    double trueThirdQuartileSublistMethod,
    double trueQuartileRangeSublistMethod,
    double trueFirstQuartileFrequencyMethod,
    double trueThirdQuartileFrequencyMethod,
    double trueQuartileRangeFrequencyMethod,
    double trueMedian,
    double trueSum,
    double trueSquaredValueSum) {
  Store store;
  int seriesIndex = 0;

  // Set the data in the store
  for (int i = 0; i < numberOfData; i++) {
    store.set(v[i], seriesIndex, 0, i);
    store.set(n[i], seriesIndex, 1, i);
  }

  double precision = 1e-3;

  double sumOfOccurrences = store.sumOfOccurrences(seriesIndex);
  double maxValue = store.maxValue(seriesIndex);
  double minValue = store.minValue(seriesIndex);
  double range = store.range(seriesIndex);
  double mean = store.mean(seriesIndex);
  double variance = store.variance(seriesIndex);
  double standardDeviation = store.standardDeviation(seriesIndex);
  double sampleStandardDeviation = store.sampleStandardDeviation(seriesIndex);
  double median = store.median(seriesIndex);
  double sum = store.sum(seriesIndex);
  double squaredValueSum = store.squaredValueSum(seriesIndex);

  // Check the positive statistics
  quiz_assert(range >= 0.0);
  quiz_assert(variance >= 0.0);
  quiz_assert(standardDeviation >= 0.0);
  quiz_assert(sampleStandardDeviation >= 0.0);
  quiz_assert(squaredValueSum >= 0.0);

  // Compare the statistics
  double reference = trueSquaredValueSum;
  assert_value_approximately_equal_to(variance, trueVariance, precision, reference);
  assert_value_approximately_equal_to(squaredValueSum, trueSquaredValueSum, precision, reference);

  reference = std::sqrt(trueSquaredValueSum);
  assert_value_approximately_equal_to(trueStandardDeviation * trueStandardDeviation, trueVariance, precision, reference);
  assert_value_approximately_equal_to(sumOfOccurrences, trueSumOfOccurrences, precision, reference);
  assert_value_approximately_equal_to(mean, trueMean, precision, reference);
  assert_value_approximately_equal_to(standardDeviation, trueStandardDeviation, precision, reference);
  assert_value_approximately_equal_to(sampleStandardDeviation, trueSampleStandardDeviation, precision, reference);
  assert_value_approximately_equal_to(median, trueMedian, precision, reference);
  assert_value_approximately_equal_to(sum, trueSum, precision, reference);

  // Perfect match
  assert_value_approximately_equal_to(maxValue, trueMaxValue, 0.0, 0.0);
  assert_value_approximately_equal_to(minValue, trueMinValue, 0.0, 0.0);
  assert_value_approximately_equal_to(range, trueRange, 0.0, 0.0);

  // Compare the country specific statistics
  I18n::Country country;
  double quartileRange;
  bool shouldUseFrequencyMethod;
  for (int c = 0; c < I18n::NumberOfCountries; c++) {
    country = static_cast<I18n::Country>(c);
    GlobalPreferences::sharedGlobalPreferences()->setCountry(country);
    quartileRange = store.quartileRange(seriesIndex);
    quiz_assert(quartileRange >= 0.0);
    shouldUseFrequencyMethod = GlobalPreferences::sharedGlobalPreferences()->methodForQuartiles() == I18n::MethodForQuartiles::CumulatedFrequency;
    assert_value_approximately_equal_to(store.firstQuartile(seriesIndex), shouldUseFrequencyMethod ? trueFirstQuartileFrequencyMethod : trueFirstQuartileSublistMethod, precision, reference);
    assert_value_approximately_equal_to(store.thirdQuartile(seriesIndex), shouldUseFrequencyMethod ? trueThirdQuartileFrequencyMethod : trueThirdQuartileSublistMethod, precision, reference);
    assert_value_approximately_equal_to(quartileRange, shouldUseFrequencyMethod ? trueQuartileRangeFrequencyMethod : trueQuartileRangeSublistMethod, 0.0, 0.0);
  }
}

QUIZ_CASE(data_statistics) {

  /* 1 2 3 4
   * 1 1 1 1 */

  constexpr int listLength1 = 4;
  double v1[listLength1] = {1.0, 2.0, 3.0, 4.0};
  double n1[listLength1] = {1.0, 1.0, 1.0, 1.0};
  assert_data_statictics_equal_to(
      v1,
      n1,
      listLength1,
      /* sumOfOccurrences */ 4.0,
      /* maxValue */ 4.0,
      /* minValue */ 1.0,
      /* range */ 3.0,
      /* mean */ 2.5,
      /* variance */ 1.25,
      /* standardDeviation */ 1.118,
      /* sampleStandardDeviation */ 1.291,
      /* firstQuartileSublistMethod */ 1.5,
      /* thirdQuartileSublistMethod */ 3.5,
      /* quartileRangeSublistMethod */ 2.0,
      /* firstQuartileFrequencyMethod */ 1.0,
      /* thirdQuartileFrequencyMethod */ 3.0,
      /* quartileRangeFrequencyMethod */ 2.0,
      /* median */ 2.5,
      /* sum */ 10.0,
      /* squaredValueSum */ 30.0);


  /* 1 2 3 4 5 6 7 8 9 10 11
   * 1 1 1 1 1 1 1 1 1  1  1 */

  constexpr int listLength2 = 11;
  double v2[listLength2] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0};
  double n2[listLength2] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
  assert_data_statictics_equal_to(
      v2,
      n2,
      listLength2,
      /* sumOfOccurrences */ 11.0,
      /* maxValue */ 11.0,
      /* minValue */ 1.0,
      /* range */ 10.0,
      /* mean */ 6.0,
      /* variance */ 10.0,
      /* standardDeviation */ 3.1623,
      /* sampleStandardDeviation */ 3.3166,
      /* firstQuartileSublistMethod */ 3.0,
      /* thirdQuartileSublistMethod */ 9.0,
      /* quartileRangeSublistMethod */ 6.0,
      /* firstQuartileFrequencyMethod */ 3.0,
      /* thirdQuartileFrequencyMethod */ 9.0,
      /* quartileRangeFrequencyMethod */ 6.0,
      /* median */ 6.0,
      /* sum */ 66.0,
      /* squaredValueSum */ 506.0);

  /* 1 2 3 4 5 6 7 8 9 10 11 12
   * 1 1 1 1 1 1 1 1 1  1  1  1 */

  constexpr int listLength3 = 12;
  double v3[listLength3] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0};
  double n3[listLength3] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
  assert_data_statictics_equal_to(
      v3,
      n3,
      listLength3,
      /* sumOfOccurrences */ 12.0,
      /* maxValue */ 12.0,
      /* minValue */ 1.0,
      /* range */ 11.0,
      /* mean */ 6.5,
      /* variance */ 11.917,
      /* standardDeviation */ 3.4521,
      /* sampleStandardDeviation */ 3.6056,
      /* firstQuartileSublistMethod */ 3.5,
      /* thirdQuartileSublistMethod */ 9.5,
      /* quartileRangeSublistMethod */ 6.0,
      /* firstQuartileFrequencyMethod */ 3.0,
      /* thirdQuartileFrequencyMethod */ 9.0,
      /* quartileRangeFrequencyMethod */ 6.0,
      /* median */ 6.5,
      /* sum */ 78.0,
      /* squaredValueSum */ 650.0);

  /*   1    2   3      5     10
   * 0.2 0.05 0.3 0.0001 0.4499 */

  constexpr int listLength4 = 5;
  double v4[listLength4] = {1.0, 2.0, 3.0, 5.0, 10.0};
  double n4[listLength4] = {0.2, 0.05, 0.3, 0.0001, 0.4499};
  assert_data_statictics_equal_to(
      v4,
      n4,
      listLength4,
      /* sumOfOccurrences */ 1.0,
      /* maxValue */ 10.0,
      /* minValue */ 1.0,
      /* range */ 9.0,
      /* mean */ 5.6995,
      /* variance */ 15.6082,
      /* standardDeviation */ 3.9507,
      /* sampleStandardDeviation */ INFINITY,
      /* firstQuartileSublistMethod */ 2.0,
      /* thirdQuartileSublistMethod */ 10.0,
      /* quartileRangeSublistMethod */ 8.0,
      /* firstQuartileFrequencyMethod */ 2.0,
      /* thirdQuartileFrequencyMethod */ 10.0,
      /* quartileRangeFrequencyMethod */ 8.0,
      /* median */ 3.0,
      /* sum */ 5.6995,
      /* squaredValueSum */ 48.0925);

  /*   1      -2   3   5  10
   * 0.4 0.00005 0.9 0.4 0.5 */

  constexpr int listLength5 = 5;
  double v5[listLength5] = {1.0, -2.0, 3.0, 5.0, 10.0};
  double n5[listLength5] = {0.4, 0.00005, 0.9, 0.4, 0.5};
  assert_data_statictics_equal_to(
      v5,
      n5,
      listLength5,
      /* sumOfOccurrences */ 2.2,
      /* maxValue */ 10.0,
      /* minValue */ -2.0,
      /* range */ 12.0,
      /* mean */ 4.5908,
      /* variance */ 10.06,
      /* standardDeviation */ 3.1719,
      /* sampleStandardDeviation */ 4.2947,
      /* firstQuartileSublistMethod */ 3.0,
      /* thirdQuartileSublistMethod */ 5.0,
      /* quartileRangeSublistMethod */ 2.0,
      /* firstQuartileFrequencyMethod */ 3.0,
      /* thirdQuartileFrequencyMethod */ 5.0,
      /* quartileRangeFrequencyMethod */ 2.0,
      /* median */ 3.0,
      /* sum */ 10.1,
      /* squaredValueSum */ 68.500);

  /* -7 -10 12 5 -2
   *  4   5  3 1  9 */

  constexpr int listLength6 = 6;
  double v6[listLength6] = {-7.0, -10.0, 1.0, 2.0, 5.0, -2.0};
  double n6[listLength6] = {4.0, 5.0, 3.0, 0.5, 1.0, 9.0};
  assert_data_statictics_equal_to(
      v6,
      n6,
      listLength6,
      /* sumOfOccurrences */ 22.5,
      /* maxValue */ 5.0,
      /* minValue */ -10.0,
      /* range */ 15.0,
      /* mean */ -3.8667,
      /* variance */ 18.9155,
      /* standardDeviation */ 4.3492,
      /* sampleStandardDeviation */ 4.4492,
      /* firstQuartileSublistMethod */ -7.0,
      /* thirdQuartileSublistMethod */ -2.0,
      /* quartileRangeSublistMethod */ 5.0,
      /* firstQuartileFrequencyMethod */ -7.0,
      /* thirdQuartileFrequencyMethod */ -2.0,
      /* quartileRangeFrequencyMethod */ 5.0,
      /* median */ -2.0,
      /* sum */ -87.0,
      /* squaredValueSum */ 762.0);

  /* 1 1 1 10 3 -1 3
   * 1 1 1  0 0  0 1 */

  constexpr int listLength7 = 7;
  double v7[listLength7] = {1.0, 1.0, 1.0, 10.0, 3.0, -1.0, 3.0};
  double n7[listLength7] = {1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0};
  assert_data_statictics_equal_to(
      v7,
      n7,
      listLength7,
      /* sumOfOccurrences */ 4.0,
      /* maxValue */ 3.0,
      /* minValue */ 1.0,
      /* range */ 2.0,
      /* mean */ 1.5,
      /* variance */ 0.75,
      /* standardDeviation */ 0.866,
      /* sampleStandardDeviation */ 1.0,
      /* firstQuartileSublistMethod */ 1.0,
      /* thirdQuartileSublistMethod */ 2.0,
      /* quartileRangeSublistMethod */ 1.0,
      /* firstQuartileFrequencyMethod */ 1.0,
      /* thirdQuartileFrequencyMethod */ 1.0,
      /* quartileRangeFrequencyMethod */ 0.0,
      /* median */ 1.0,
      /* sum */ 6.0,
      /* squaredValueSum */ 12.0);

  /* 1 2 3 4
   * 0 1 0 1 */

  constexpr int listLength8 = 4;
  double v8[listLength8] = {1.0, 2.0, 3.0, 4.0};
  double n8[listLength8] = {0.0, 1.0, 0.0, 1.0};
  assert_data_statictics_equal_to(
      v8,
      n8,
      listLength8,
      /* sumOfOccurrences */ 2.0,
      /* maxValue */ 4.0,
      /* minValue */ 2.0,
      /* range */ 2.0,
      /* mean */ 3.0,
      /* variance */ 1.0,
      /* standardDeviation */ 1.0,
      /* sampleStandardDeviation */ 1.414,
      /* firstQuartileSublistMethod */ 2.0,
      /* thirdQuartileSublistMethod */ 4.0,
      /* quartileRangeSublistMethod */ 2.0,
      /* firstQuartileFrequencyMethod */ 2.0,
      /* thirdQuartileFrequencyMethod */ 4.0,
      /* quartileRangeFrequencyMethod */ 2.0,
      /* median */ 3.0,
      /* sum */ 6.0,
      /* squaredValueSum */ 20.0);

  /* -996.85840734641
   * 9 */

  constexpr int listLength9 = 1;
  double v9[listLength9] = {-996.85840734641};
  double n9[listLength9] = {9};
  assert_data_statictics_equal_to(
      v9,
      n9,
      listLength9,
      /* sumOfOccurrences */ 9.0,
      /* maxValue */ -996.85840734641,
      /* minValue */ -996.85840734641,
      /* range */ 0.0,
      /* mean */ -996.85840734641,
      /* variance */ 0.0,
      /* standardDeviation */ 0.0,
      /* sampleStandardDeviation */ 0.0,
      /* firstQuartileSublistMethod */ -996.85840734641,
      /* thirdQuartileSublistMethod */ -996.85840734641,
      /* quartileRangeSublistMethod */ 0.0,
      /* firstQuartileFrequencyMethod */ -996.85840734641,
      /* thirdQuartileFrequencyMethod */ -996.85840734641,
      /* quartileRangeFrequencyMethod */ 0.0,
      /* median */ -996.85840734641,
      /* sum */ -8971.72566611769,
      /* squaredValueSum */ 8943540.158675);
}

}
