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
using namespace Shared;

namespace Statistics {

constexpr int k_defaultSeriesIndex = 0;

void assert_value_approximately_equal_to(double d1, double d2, double precision, double nullExpectedPrecision) {
  quiz_assert((std::isinf(d1) && std::isinf(d2) && d1 * d2 > 0.0 /*same sign*/)
      || roughly_equal(d1, d2, precision, true, nullExpectedPrecision));
}

// Set the data in the store
void setStoreData(Store * store, double v[], double n[], int numberOfData, int seriesIndex) {
  store->deleteAllPairsOfSeries(seriesIndex);
  for (int i = 0; i < numberOfData; i++) {
    store->set(v[i], seriesIndex, 0, i);
    store->set(n[i], seriesIndex, 1, i);
  }
}

void assert_data_statistics_equal_to(
    Store * store,
    int numberOfData,
    double trueSumOfOccurrences,
    double trueRange,
    double trueMean,
    double trueVariance,
    double trueStandardDeviation,
    double trueSampleStandardDeviation,
    double trueSum,
    double trueSquaredValueSum,
    int trueNumberOfModes,
    double trueModes[],
    double trueModeFrequency
  ) {

  double precision = 1e-3;

  double sumOfOccurrences = store->sumOfOccurrences(k_defaultSeriesIndex);
  double range = store->range(k_defaultSeriesIndex);
  double mean = store->mean(k_defaultSeriesIndex);
  double variance = store->variance(k_defaultSeriesIndex);
  double standardDeviation = store->standardDeviation(k_defaultSeriesIndex);
  double sampleStandardDeviation = store->sampleStandardDeviation(k_defaultSeriesIndex);
  double sum = store->sum(k_defaultSeriesIndex);
  double squaredValueSum = store->squaredValueSum(k_defaultSeriesIndex);

  // Check the positive statistics
  quiz_assert(range >= 0.0);
  quiz_assert(variance >= 0.0);
  quiz_assert(standardDeviation >= 0.0);
  quiz_assert(std::isnan(sampleStandardDeviation) || sampleStandardDeviation >= 0.0);
  quiz_assert(squaredValueSum >= 0.0);

  // Compare the statistics
  double nullExpectedPrecision = 1e-10;
  assert_value_approximately_equal_to(variance, trueVariance, precision, nullExpectedPrecision);
  assert_value_approximately_equal_to(squaredValueSum, trueSquaredValueSum, precision, nullExpectedPrecision);
  assert_value_approximately_equal_to(trueStandardDeviation * trueStandardDeviation, trueVariance, precision, nullExpectedPrecision);
  assert_value_approximately_equal_to(sumOfOccurrences, trueSumOfOccurrences, precision, nullExpectedPrecision);
  assert_value_approximately_equal_to(mean, trueMean, precision, nullExpectedPrecision);
  assert_value_approximately_equal_to(standardDeviation, trueStandardDeviation, precision, nullExpectedPrecision);
  assert_value_approximately_equal_to(sampleStandardDeviation, trueSampleStandardDeviation, precision, nullExpectedPrecision);
  assert_value_approximately_equal_to(sum, trueSum, precision, nullExpectedPrecision);

  // Perfect match
  assert_value_approximately_equal_to(range, trueRange, 0.0, 0.0);

  // Compare the modes
  int numberOfModes = store->numberOfModes(k_defaultSeriesIndex);
  quiz_assert(numberOfModes == trueNumberOfModes);
  assert_value_approximately_equal_to(store->modeFrequency(k_defaultSeriesIndex), trueModeFrequency, precision, nullExpectedPrecision);
  for (int i = 0; i < numberOfModes; i++) {
    assert_value_approximately_equal_to(store->modeAtIndex(k_defaultSeriesIndex, i), trueModes[i], 0.0, 0.0);
  }
}

// Compare the cumulated frequency data points
void assert_data_cumulated_frequency(
    Store * store,
    int numberOfData,
    double trueCumulatedFrequencyValues[],
    double trueCumulatedFrequencyResults[],
    int totalCumulatedFrequency
  ) {


  double precision = 1e-3;
  double nullExpectedPrecision = 1e-10;

  quiz_assert(store->totalCumulatedFrequencyValues(k_defaultSeriesIndex) == totalCumulatedFrequency);
  for (int i = 0; i < totalCumulatedFrequency; i++) {
    assert_value_approximately_equal_to(trueCumulatedFrequencyValues[i], store->cumulatedFrequencyValueAtIndex(k_defaultSeriesIndex, i), precision, nullExpectedPrecision);
    assert_value_approximately_equal_to(trueCumulatedFrequencyResults[i], store->cumulatedFrequencyResultAtIndex(k_defaultSeriesIndex, i), precision, nullExpectedPrecision);
  }
}

// Compare the normal probability data points
void assert_data_normal_probability(
    Store * store,
    int numberOfData,
    double trueNormalProbabilityValues[],
    double trueNormalProbabilityResults[],
    int totalNormalProbability
  ) {
  double precision = 1e-3;
  double nullExpectedPrecision = 1e-10;

  quiz_assert(store->totalNormalProbabilityValues(k_defaultSeriesIndex) == totalNormalProbability);
  for (int i = 0; i < totalNormalProbability; i++) {
    assert_value_approximately_equal_to(trueNormalProbabilityValues[i], store->normalProbabilityValueAtIndex(k_defaultSeriesIndex, i), precision, nullExpectedPrecision);
    assert_value_approximately_equal_to(trueNormalProbabilityResults[i], store->normalProbabilityResultAtIndex(k_defaultSeriesIndex, i), precision, nullExpectedPrecision);
  }
}

// Compare the box-and-whiskers data points
void assert_data_box_plot(
    Store * store,
    int numberOfData,
    double trueQuartileRange,
    double trueMinValue,
    double trueLowerOutliers[],
    int trueNumberOfLowerOutliers,
    double trueLowerFence,
    double trueLowerWhisker,
    double trueFirstQuartile,
    double trueMedian,
    double trueThirdQuartile,
    double trueUpperWhisker,
    double trueUpperFence,
    double trueUpperOutliers[],
    int trueNumberOfUpperOutliers,
    double trueMaxValue,
    bool shouldUseFrequencyMethod
  ) {

  double precision = 1e-3;
  double nullExpectedPrecision = 1e-10;

  // Metrics independent from country and outliers display setting
  assert_value_approximately_equal_to(store->median(k_defaultSeriesIndex), trueMedian, precision, nullExpectedPrecision);
  // Perfect match
  assert_value_approximately_equal_to(store->minValue(k_defaultSeriesIndex), trueMinValue, 0.0, 0.0);
  assert_value_approximately_equal_to(store->maxValue(k_defaultSeriesIndex), trueMaxValue, 0.0, 0.0);

  store->setDisplayOutliers(false);
  quiz_assert(store->numberOfLowerOutliers(k_defaultSeriesIndex) == 0);
  quiz_assert(store->numberOfUpperOutliers(k_defaultSeriesIndex) == 0);
  quiz_assert(store->lowerWhisker(k_defaultSeriesIndex) == trueMinValue);
  quiz_assert(store->upperWhisker(k_defaultSeriesIndex) == trueMaxValue);

  store->setDisplayOutliers(true);
  /* Compare the statistics for all countries using the same method for
   * quartiles:
   * SublistMethod is the method for computing quartiles used in most
   * countries, which defines quartiles as the medians of the left and right
   * subsets of data.
   * FrequencyMethod is the method used in France and Italy, which defines the
   * quartiles as the 25th and 75th percentile, in terms of cumulated
   * frequencies. */
  I18n::Country country;
  for (int c = 0; c < I18n::NumberOfCountries; c++) {
    country = static_cast<I18n::Country>(c);
    GlobalPreferences::sharedGlobalPreferences->setCountry(country);
    bool countryUseFrequencyMethod = (GlobalPreferences::sharedGlobalPreferences->methodForQuartiles() == CountryPreferences::MethodForQuartiles::CumulatedFrequency);
    if (shouldUseFrequencyMethod != countryUseFrequencyMethod) {
      continue;
    }
    double quartileRange = store->quartileRange(k_defaultSeriesIndex);
    quiz_assert(quartileRange >= 0.0);
    assert_value_approximately_equal_to(quartileRange, trueQuartileRange, 0.0, 0.0);

    assert_value_approximately_equal_to(store->lowerFence(k_defaultSeriesIndex), trueLowerFence, precision, nullExpectedPrecision);
    assert_value_approximately_equal_to(store->lowerWhisker(k_defaultSeriesIndex), trueLowerWhisker, precision, nullExpectedPrecision);
    assert_value_approximately_equal_to(store->firstQuartile(k_defaultSeriesIndex), trueFirstQuartile, precision, nullExpectedPrecision);
    assert_value_approximately_equal_to(store->thirdQuartile(k_defaultSeriesIndex), trueThirdQuartile, precision, nullExpectedPrecision);
    assert_value_approximately_equal_to(store->upperWhisker(k_defaultSeriesIndex), trueUpperWhisker, precision, nullExpectedPrecision);
    assert_value_approximately_equal_to(store->upperFence(k_defaultSeriesIndex), trueUpperFence, precision, nullExpectedPrecision);

    int numberOfLowerOutliers = store->numberOfLowerOutliers(k_defaultSeriesIndex);
    quiz_assert((trueNumberOfLowerOutliers) == numberOfLowerOutliers);
    for (int i = 0; i < trueNumberOfLowerOutliers; i++) {
      quiz_assert((trueLowerOutliers)[i] == store->lowerOutlierAtIndex(k_defaultSeriesIndex, i));
    }

    int numberOfUpperOutliers = store->numberOfUpperOutliers(k_defaultSeriesIndex);
    quiz_assert((trueNumberOfUpperOutliers) == numberOfUpperOutliers);
    for (int i = 0; i < trueNumberOfUpperOutliers; i++) {
      quiz_assert((trueUpperOutliers)[i] == store->upperOutlierAtIndex(k_defaultSeriesIndex, i));
    }
  }
  GlobalPreferences::sharedGlobalPreferences->setCountry(I18n::Country::WW);
}

QUIZ_CASE(data_statistics) {
  GlobalContext context;
  UserPreferences userPreferences;
  Store store(&context, &userPreferences);

  /* 1 2 3 4
   * 1 1 1 1 */

  constexpr int listLength1 = 4;
  double v1[listLength1] = {1.0, 2.0, 3.0, 4.0};
  double n1[listLength1] = {1.0, 1.0, 1.0, 1.0};
  setStoreData(&store, v1, n1, listLength1, k_defaultSeriesIndex);
  constexpr int numberOfModes1 = 4;
  double modes1[numberOfModes1] = {1.0, 2.0, 3.0, 4.0};
  assert_data_statistics_equal_to(
      &store,
      listLength1,
      /* sumOfOccurrences */ 4.0,
      /* range */ 3.0,
      /* mean */ 2.5,
      /* variance */ 1.25,
      /* standardDeviation */ 1.118,
      /* sampleStandardDeviation */ 1.291,
      /* sum */ 10.0,
      /* squaredValueSum */ 30.0,
      numberOfModes1,
      modes1,
      /* modesFrequency */ 1.0);

  constexpr int totalCumulatedFrequency1 = listLength1;
  double trueCumulatedFrequencyValues1[totalCumulatedFrequency1] = {1.0, 2.0, 3.0, 4.0};
  double trueCumulatedFrequencyResults1[totalCumulatedFrequency1] = {25.0, 50.0, 75.0, 100.0};
  assert_data_cumulated_frequency(&store, listLength1, trueCumulatedFrequencyValues1, trueCumulatedFrequencyResults1, totalCumulatedFrequency1);

  constexpr int totalNormalProbability1 = listLength1;
  double trueNormalProbabilityValues1[totalNormalProbability1] = {1.0, 2.0, 3.0, 4.0};
  double trueNormalProbabilityResults1[totalNormalProbability1] = {-1.150, -0.3186, 0.3186, 1.150};
  assert_data_normal_probability(&store, listLength1, trueNormalProbabilityValues1, trueNormalProbabilityResults1, totalNormalProbability1);

  constexpr int totalLowerOutliers1 = 0;
  double trueLowerOutliers1[totalLowerOutliers1] = {};
  constexpr int totalUpperOutliers1 = 0;
  double trueUpperOutliers1[totalUpperOutliers1] = {};
  // SublistMethod
  assert_data_box_plot(
      &store,
      listLength1,
      /* quartileRange */ 2.0,
      /* minValue */ 1.0,
      trueLowerOutliers1,
      totalLowerOutliers1,
      /* lowerFence */ -1.5,
      /* lowerWhisker */ 1.0,
      /* firstQuartile */ 1.5,
      /* median */ 2.5,
      /* thirdQuartile */ 3.5,
      /* upperWhisker */ 4.0,
      /* upperFence */ 6.5,
      trueUpperOutliers1,
      totalUpperOutliers1,
      /* maxValue */ 4.0,
      false);

  // FrequencyMethod
  assert_data_box_plot(
      &store,
      listLength1,
      /* quartileRange */ 2.0,
      /* minValue */ 1.0,
      trueLowerOutliers1,
      totalLowerOutliers1,
      /* lowerFence */ -2.0,
      /* lowerWhisker */ 1.0,
      /* firstQuartile */ 1.0,
      /* median */ 2.5,
      /* thirdQuartile */ 3.0,
      /* upperWhisker */ 4.0,
      /* upperFence */ 6.0,
      trueUpperOutliers1,
      totalUpperOutliers1,
      /* maxValue */ 4.0,
      true);


  /* 1 2 3 4 5 6 7 8 9 10 11
   * 1 1 1 1 1 1 1 1 1  1  1 */

  constexpr int listLength2 = 11;
  double v2[listLength2] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0};
  double n2[listLength2] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
  setStoreData(&store, v2, n2, listLength2, k_defaultSeriesIndex);
  constexpr int numberOfModes2 = 11;
  double modes2[numberOfModes2] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0};
  assert_data_statistics_equal_to(
      &store,
      listLength2,
      /* sumOfOccurrences */ 11.0,
      /* range */ 10.0,
      /* mean */ 6.0,
      /* variance */ 10.0,
      /* standardDeviation */ 3.1623,
      /* sampleStandardDeviation */ 3.3166,
      /* sum */ 66.0,
      /* squaredValueSum */ 506.0,
      numberOfModes2,
      modes2,
      /* modesFrequency */ 1.0);

  constexpr int totalCumulatedFrequency2 = listLength2;
  double trueCumulatedFrequencyValues2[totalCumulatedFrequency2] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0};
  double trueCumulatedFrequencyResults2[totalCumulatedFrequency2] = {9.090, 18.18, 27.27, 36.36, 45.45, 54.54, 63.63, 72.72, 81.81, 90.90, 100.0};
  assert_data_cumulated_frequency(&store, listLength2, trueCumulatedFrequencyValues2, trueCumulatedFrequencyResults2, totalCumulatedFrequency2);

  constexpr int totalNormalProbability2 = listLength2;
  double trueNormalProbabilityValues2[totalNormalProbability2] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0};
  double trueNormalProbabilityResults2[totalNormalProbability2] = {-1.691, -1.097, -0.7479, -0.4728, -0.2299, 0.0, 0.2299, 0.4728, 0.7479, 1.097, 1.691};
  assert_data_normal_probability(&store, listLength2, trueNormalProbabilityValues2, trueNormalProbabilityResults2, totalNormalProbability2);

  constexpr int totalLowerOutliers2 = 0;
  double trueLowerOutliers2[totalLowerOutliers2] = {};
  constexpr int totalUpperOutliers2 = 0;
  double trueUpperOutliers2[totalUpperOutliers2] = {};
  // SublistMethod
  assert_data_box_plot(
      &store,
      listLength2,
      /* quartileRange */ 6.0,
      /* minValue */ 1.0,
      trueLowerOutliers2,
      totalLowerOutliers2,
      /* lowerFence */ -6.0,
      /* lowerWhisker */ 1.0,
      /* firstQuartile */ 3.0,
      /* median */ 6.0,
      /* thirdQuartile */ 9.0,
      /* upperWhisker */ 11.0,
      /* upperFence */ 18.0,
      trueUpperOutliers2,
      totalUpperOutliers2,
      /* maxValue */ 11.0,
      false);

  // FrequencyMethod
  assert_data_box_plot(
      &store,
      listLength2,
      /* quartileRange */ 6.0,
      /* minValue */ 1.0,
      trueLowerOutliers2,
      totalLowerOutliers2,
      /* lowerFence */ -6.0,
      /* lowerWhisker */ 1.0,
      /* firstQuartile */ 3.0,
      /* median */ 6.0,
      /* thirdQuartile */ 9.0,
      /* upperWhisker */ 11.0,
      /* upperFence */ 18.0,
      trueUpperOutliers2,
      totalUpperOutliers2,
      /* maxValue */ 11.0,
      true);

  /* 1 2 3 4 5 6 7 8 9 10 11 12
   * 1 1 1 1 1 1 1 1 1  1  1  1 */

  constexpr int listLength3 = 12;
  double v3[listLength3] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0};
  double n3[listLength3] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
  setStoreData(&store, v3, n3, listLength3, k_defaultSeriesIndex);
  constexpr int numberOfModes3 = 12;
  double modes3[numberOfModes3] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0};
  assert_data_statistics_equal_to(
      &store,
      listLength3,
      /* sumOfOccurrences */ 12.0,
      /* range */ 11.0,
      /* mean */ 6.5,
      /* variance */ 11.917,
      /* standardDeviation */ 3.4521,
      /* sampleStandardDeviation */ 3.6056,
      /* sum */ 78.0,
      /* squaredValueSum */ 650.0,
      numberOfModes3,
      modes3,
      /* modesFrequency */ 1.0);

  constexpr int totalCumulatedFrequency3 = listLength3;
  double trueCumulatedFrequencyValues3[totalCumulatedFrequency3] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0};
  double trueCumulatedFrequencyResults3[totalCumulatedFrequency3] = {8.333, 16.67, 25.00, 33.33, 41.67, 50.00, 58.33, 66.67, 75.00, 83.33, 91.67, 100.0};
  assert_data_cumulated_frequency(&store, listLength3, trueCumulatedFrequencyValues3, trueCumulatedFrequencyResults3, totalCumulatedFrequency3);

  constexpr int totalNormalProbability3 = listLength3;
  double trueNormalProbabilityValues3[totalNormalProbability3] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0};
  double trueNormalProbabilityResults3[totalNormalProbability3] = {-1.732, -1.150, -0.8122, -0.5485, -0.3186, -0.1046, 0.1046, 0.3186, 0.5485, 0.8122, 1.150, 1.732};
  assert_data_normal_probability(&store, listLength3, trueNormalProbabilityValues3, trueNormalProbabilityResults3, totalNormalProbability3);

  constexpr int totalLowerOutliers3 = 0;
  double trueLowerOutliers3[totalLowerOutliers3] = {};
  constexpr int totalUpperOutliers3 = 0;
  double trueUpperOutliers3[totalUpperOutliers3] = {};
  // SublistMethod
  assert_data_box_plot(
      &store,
      listLength3,
      /* quartileRange */ 6.0,
      /* minValue */ 1.0,
      trueLowerOutliers3,
      totalLowerOutliers3,
      /* lowerFence */ -5.5,
      /* lowerWhisker */ 1.0,
      /* firstQuartile */ 3.5,
      /* median */ 6.5,
      /* thirdQuartile */ 9.5,
      /* upperWhisker */ 12.0,
      /* upperFence */ 18.5,
      trueUpperOutliers3,
      totalUpperOutliers3,
      /* maxValue */ 12.0,
      false);

  // FrequencyMethod
  assert_data_box_plot(
      &store,
      listLength3,
      /* quartileRange */ 6.0,
      /* minValue */ 1.0,
      trueLowerOutliers3,
      totalLowerOutliers3,
      /* lowerFence */ -6.0,
      /* lowerWhisker */ 1.0,
      /* firstQuartile */ 3.0,
      /* median */ 6.5,
      /* thirdQuartile */ 9.0,
      /* upperWhisker */ 12.0,
      /* upperFence */ 18.0,
      trueUpperOutliers3,
      totalUpperOutliers3,
      /* maxValue */ 12.0,
      true);

  /*   1    2   3      5     10
   * 0.2 0.05 0.3 0.0001 0.4499 */

  constexpr int listLength4 = 5;
  double v4[listLength4] = {1.0, 2.0, 3.0, 5.0, 10.0};
  double n4[listLength4] = {0.2, 0.05, 0.3, 0.0001, 0.4499};
  setStoreData(&store, v4, n4, listLength4, k_defaultSeriesIndex);
  constexpr int numberOfModes4 = 1;
  double modes4[numberOfModes4] = {10.0};
  assert_data_statistics_equal_to(
      &store,
      listLength4,
      /* sumOfOccurrences */ 1.0,
      /* range */ 9.0,
      /* mean */ 5.6995,
      /* variance */ 15.6082,
      /* standardDeviation */ 3.9507,
      /* sampleStandardDeviation */ INFINITY,
      /* sum */ 5.6995,
      /* squaredValueSum */ 48.0925,
      numberOfModes4,
      modes4,
      /* modesFrequency */ 0.4499);

  constexpr int totalCumulatedFrequency4 = listLength4;
  double trueCumulatedFrequencyValues4[totalCumulatedFrequency4] = {1.0, 2.0, 3.0, 5.0, 10.0};
  double trueCumulatedFrequencyResults4[totalCumulatedFrequency4] = {20.0, 25.0, 55.0, 55.01, 100.0};
  assert_data_cumulated_frequency(&store, listLength4, trueCumulatedFrequencyValues4, trueCumulatedFrequencyResults4, totalCumulatedFrequency4);

  constexpr int totalNormalProbability4 = 0;
  double trueNormalProbabilityValues4[totalNormalProbability4] = {};
  double trueNormalProbabilityResults4[totalNormalProbability4] = {};
  assert_data_normal_probability(&store, listLength4, trueNormalProbabilityValues4, trueNormalProbabilityResults4, totalNormalProbability4);

  constexpr int totalLowerOutliers4 = 0;
  double trueLowerOutliers4[totalLowerOutliers4] = {};
  constexpr int totalUpperOutliers4 = 0;
  double trueUpperOutliers4[totalUpperOutliers4] = {};
  // SublistMethod
  assert_data_box_plot(
      &store,
      listLength4,
      /* quartileRange */ 8.0,
      /* minValue */ 1.0,
      trueLowerOutliers4,
      totalLowerOutliers4,
      /* lowerFence */ -10.0,
      /* lowerWhisker */ 1.0,
      /* firstQuartile */ 2.0,
      /* median */ 3.0,
      /* thirdQuartile */ 10.0,
      /* upperWhisker */ 10.0,
      /* upperFence */ 22.0,
      trueUpperOutliers4,
      totalUpperOutliers4,
      /* maxValue */ 10.0,
      false);

  // FrequencyMethod
  assert_data_box_plot(
      &store,
      listLength4,
      /* quartileRange */ 8.0,
      /* minValue */ 1.0,
      trueLowerOutliers4,
      totalLowerOutliers4,
      /* lowerFence */ -10.0,
      /* lowerWhisker */ 1.0,
      /* firstQuartile */ 2.0,
      /* median */ 3.0,
      /* thirdQuartile */ 10.0,
      /* upperWhisker */ 10.0,
      /* upperFence */ 22.0,
      trueUpperOutliers4,
      totalUpperOutliers4,
      /* maxValue */ 10.0,
      true);

  /*   1      -2   3   5  10
   * 0.4 0.00005 0.9 0.4 0.5 */

  constexpr int listLength5 = 5;
  double v5[listLength5] = {1.0, -2.0, 3.0, 5.0, 10.0};
  double n5[listLength5] = {0.4, 0.00005, 0.9, 0.4, 0.5};
  setStoreData(&store, v5, n5, listLength5, k_defaultSeriesIndex);
  constexpr int numberOfModes5 = 1;
  double modes5[numberOfModes5] = {3.0};
  assert_data_statistics_equal_to(
      &store,
      listLength5,
      /* sumOfOccurrences */ 2.2,
      /* range */ 12.0,
      /* mean */ 4.5908,
      /* variance */ 10.06,
      /* standardDeviation */ 3.1719,
      /* sampleStandardDeviation */ 4.2947,
      /* sum */ 10.1,
      /* squaredValueSum */ 68.500,
      numberOfModes5,
      modes5,
      /* modesFrequency */ 0.9);

  constexpr int totalCumulatedFrequency5 = listLength5;
  double trueCumulatedFrequencyValues5[totalCumulatedFrequency5] = {-2.0, 1.0, 3.0, 5.0, 10.0};
  double trueCumulatedFrequencyResults5[totalCumulatedFrequency5] = {0.002273, 18.18, 59.09, 77.27, 100.0};
  assert_data_cumulated_frequency(&store, listLength5, trueCumulatedFrequencyValues5, trueCumulatedFrequencyResults5, totalCumulatedFrequency5);

  constexpr int totalNormalProbability5 = 0;
  double trueNormalProbabilityValues5[totalNormalProbability5] = {};
  double trueNormalProbabilityResults5[totalNormalProbability5] = {};
  assert_data_normal_probability(&store, listLength5, trueNormalProbabilityValues5, trueNormalProbabilityResults5, totalNormalProbability5);

  constexpr int totalLowerOutliers5 = 1;
  double trueLowerOutliers5[totalLowerOutliers5] = {-2.0};
  constexpr int totalUpperOutliers5 = 1;
  double trueUpperOutliers5[totalUpperOutliers5] = {10.0};
  // SublistMethod
  assert_data_box_plot(
      &store,
      listLength5,
      /* quartileRange */ 2.0,
      /* minValue */ -2.0,
      trueLowerOutliers5,
      totalLowerOutliers5,
      /* lowerFence */ 0.0,
      /* lowerWhisker */ 1.0,
      /* firstQuartile */ 3.0,
      /* median */ 3.0,
      /* thirdQuartile */ 5.0,
      /* upperWhisker */ 5.0,
      /* upperFence */ 8.0,
      trueUpperOutliers5,
      totalUpperOutliers5,
      /* maxValue */ 10.0,
      false);

  // FrequencyMethod
  assert_data_box_plot(
      &store,
      listLength5,
      /* quartileRange */ 2.0,
      /* minValue */ -2.0,
      trueLowerOutliers5,
      totalLowerOutliers5,
      /* lowerFence */ 0,
      /* lowerWhisker */ 1.0,
      /* firstQuartile */ 3.0,
      /* median */ 3.0,
      /* thirdQuartile */ 5.0,
      /* upperWhisker */ 5.0,
      /* upperFence */ 8.0,
      trueUpperOutliers5,
      totalUpperOutliers5,
      /* maxValue */ 10.0,
      true);

  /* -7 -10 12 5 -2
   *  4   5  3 1  9 */

  constexpr int listLength6 = 6;
  double v6[listLength6] = {-7.0, -10.0, 1.0, 2.0, 5.0, -2.0};
  double n6[listLength6] = {4.0, 5.0, 3.0, 0.5, 1.0, 9.0};
  setStoreData(&store, v6, n6, listLength6, k_defaultSeriesIndex);
  constexpr int numberOfModes6 = 1;
  double modes6[numberOfModes6] = {-2.0};
  assert_data_statistics_equal_to(
      &store,
      listLength6,
      /* sumOfOccurrences */ 22.5,
      /* range */ 15.0,
      /* mean */ -3.8667,
      /* variance */ 18.9155,
      /* standardDeviation */ 4.3492,
      /* sampleStandardDeviation */ 4.4492,
      /* sum */ -87.0,
      /* squaredValueSum */ 762.0,
      numberOfModes6,
      modes6,
      /* modesFrequency */ 9.0);

  constexpr int totalCumulatedFrequency6 = listLength6;
  double trueCumulatedFrequencyValues6[totalCumulatedFrequency6] = {-10.0, -7.0, -2.0, 1.0, 2.0, 5.0};
  double trueCumulatedFrequencyResults6[totalCumulatedFrequency6] = {22.22, 40.00, 80.00, 93.33, 95.56, 100.0};
  assert_data_cumulated_frequency(&store, listLength6, trueCumulatedFrequencyValues6, trueCumulatedFrequencyResults6, totalCumulatedFrequency6);

  constexpr int totalNormalProbability6 = 0;
  double trueNormalProbabilityValues6[totalNormalProbability6] = {};
  double trueNormalProbabilityResults6[totalNormalProbability6] = {};
  assert_data_normal_probability(&store, listLength6, trueNormalProbabilityValues6, trueNormalProbabilityResults6, totalNormalProbability6);

  constexpr int totalLowerOutliers6 = 0;
  double trueLowerOutliers6[totalLowerOutliers6] = {};
  constexpr int totalUpperOutliers6 = 0;
  double trueUpperOutliers6[totalUpperOutliers6] = {};
  // SublistMethod
  assert_data_box_plot(
      &store,
      listLength6,
      /* quartileRange */ 5.0,
      /* minValue */ -10.0,
      trueLowerOutliers6,
      totalLowerOutliers6,
      /* lowerFence */ -14.5,
      /* lowerWhisker */ -10.0,
      /* firstQuartile */ -7.0,
      /* median */ -2.0,
      /* thirdQuartile */ -2.0,
      /* upperWhisker */ 5.0,
      /* upperFence */ 5.5,
      trueUpperOutliers6,
      totalUpperOutliers6,
      /* maxValue */ 5.0,
      false);

  // FrequencyMethod
  assert_data_box_plot(
      &store,
      listLength6,
      /* quartileRange */ 5.0,
      /* minValue */ -10.0,
      trueLowerOutliers6,
      totalLowerOutliers6,
      /* lowerFence */ -14.5,
      /* lowerWhisker */ -10.0,
      /* firstQuartile */ -7.0,
      /* median */ -2.0,
      /* thirdQuartile */ -2.0,
      /* upperWhisker */ 5.0,
      /* upperFence */ 5.5,
      trueUpperOutliers6,
      totalUpperOutliers6,
      /* maxValue */ 5.0,
      true);

  /* 1 1 1 10 3 -1 3
   * 1 1 1  0 0  0 1 */

  constexpr int listLength7 = 7;
  double v7[listLength7] = {1.0, 1.0, 1.0, 10.0, 3.0, -1.0, 3.0};
  double n7[listLength7] = {1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0};
  setStoreData(&store, v7, n7, listLength7, k_defaultSeriesIndex);
  constexpr int numberOfModes7 = 1;
  double modes7[numberOfModes7] = {1.0};
  assert_data_statistics_equal_to(
      &store,
      listLength7,
      /* sumOfOccurrences */ 4.0,
      /* range */ 2.0,
      /* mean */ 1.5,
      /* variance */ 0.75,
      /* standardDeviation */ 0.866,
      /* sampleStandardDeviation */ 1.0,
      /* sum */ 6.0,
      /* squaredValueSum */ 12.0,
      numberOfModes7,
      modes7,
      /* modesFrequency */ 3.0);

  constexpr int totalCumulatedFrequency7 = 4;
  double trueCumulatedFrequencyValues7[totalCumulatedFrequency7] = {-1.0, 1.0, 3.0, 10.0};
  double trueCumulatedFrequencyResults7[totalCumulatedFrequency7] = {0.0, 75.0, 100.0, 100.0};
  assert_data_cumulated_frequency(&store, listLength7, trueCumulatedFrequencyValues7, trueCumulatedFrequencyResults7, totalCumulatedFrequency7);

  constexpr int totalNormalProbability7 = 4;
  double trueNormalProbabilityValues7[totalNormalProbability7] = {1.0, 1.0, 1.0, 3.0};
  double trueNormalProbabilityResults7[totalNormalProbability7] = {-1.150, -0.3186, 0.3186, 1.150};
  assert_data_normal_probability(&store, listLength7, trueNormalProbabilityValues7, trueNormalProbabilityResults7, totalNormalProbability7);

  constexpr int totalLowerOutliersSublistMethod7 = 0;
  double trueLowerOutliersSublistMethod7[totalLowerOutliersSublistMethod7] = {};
  constexpr int totalUpperOutliersSublistMethod7 = 0;
  double trueUpperOutliersSublistMethod7[totalUpperOutliersSublistMethod7] = {};
  // SublistMethod
  assert_data_box_plot(
      &store,
      listLength7,
      /* quartileRange */ 1.0,
      /* minValue */ 1.0,
      trueLowerOutliersSublistMethod7,
      totalLowerOutliersSublistMethod7,
      /* lowerFence */ -0.5,
      /* lowerWhisker */ 1.0,
      /* firstQuartile */ 1.0,
      /* median */ 1.0,
      /* thirdQuartile */ 2.0,
      /* upperWhisker */ 3.0,
      /* upperFence */ 3.5,
      trueUpperOutliersSublistMethod7,
      totalUpperOutliersSublistMethod7,
      /* maxValue */ 3.0,
      false);

  constexpr int totalLowerOutliersFrequencyMethod7 = 0;
  double trueLowerOutliersFrequencyMethod7[totalLowerOutliersFrequencyMethod7] = {};
  constexpr int totalUpperOutliersFrequencyMethod7 = 1;
  double trueUpperOutliersFrequencyMethod7[totalUpperOutliersFrequencyMethod7] = {3.0};
  // FrequencyMethod
  assert_data_box_plot(
      &store,
      listLength7,
      /* quartileRange */ 0.0,
      /* minValue */ 1.0,
      trueLowerOutliersFrequencyMethod7,
      totalLowerOutliersFrequencyMethod7,
      /* lowerFence */ 1.0,
      /* lowerWhisker */ 1.0,
      /* firstQuartile */ 1.0,
      /* median */ 1.0,
      /* thirdQuartile */ 1.0,
      /* upperWhisker */ 1.0,
      /* upperFence */ 1.0,
      trueUpperOutliersFrequencyMethod7,
      totalUpperOutliersFrequencyMethod7,
      /* maxValue */ 3.0,
      true);

  /* 1 2 3 4
   * 0 1 0 1 */

  constexpr int listLength8 = 4;
  double v8[listLength8] = {1.0, 2.0, 3.0, 4.0};
  double n8[listLength8] = {0.0, 1.0, 0.0, 1.0};
  setStoreData(&store, v8, n8, listLength8, k_defaultSeriesIndex);
  constexpr int numberOfModes8 = 2;
  double modes8[numberOfModes8] = {2.0, 4.0};
  assert_data_statistics_equal_to(
      &store,
      listLength8,
      /* sumOfOccurrences */ 2.0,
      /* range */ 2.0,
      /* mean */ 3.0,
      /* variance */ 1.0,
      /* standardDeviation */ 1.0,
      /* sampleStandardDeviation */ 1.414,
      /* sum */ 6.0,
      /* squaredValueSum */ 20.0,
      numberOfModes8,
      modes8,
      /* modesFrequency */ 1.0);

  constexpr int totalCumulatedFrequency8 = listLength8;
  double trueCumulatedFrequencyValues8[totalCumulatedFrequency8] = {1.0, 2.0, 3.0, 4.0};
  double trueCumulatedFrequencyResults8[totalCumulatedFrequency8] = {0.0, 50.0, 50.0, 100.0};
  assert_data_cumulated_frequency(&store, listLength8, trueCumulatedFrequencyValues8, trueCumulatedFrequencyResults8, totalCumulatedFrequency8);

  constexpr int totalNormalProbability8 = 2;
  double trueNormalProbabilityValues8[totalNormalProbability8] = {2.0, 4.0};
  double trueNormalProbabilityResults8[totalNormalProbability8] = {-0.6745, 0.6745};
  assert_data_normal_probability(&store, listLength8, trueNormalProbabilityValues8, trueNormalProbabilityResults8, totalNormalProbability8);

  constexpr int totalLowerOutliers8 = 0;
  double trueLowerOutliers8[totalLowerOutliers8] = {};
  constexpr int totalUpperOutliers8 = 0;
  double trueUpperOutliers8[totalUpperOutliers8] = {};
  // SublistMethod
  assert_data_box_plot(
      &store,
      listLength8,
      /* quartileRange */ 2.0,
      /* minValue */ 2.0,
      trueLowerOutliers8,
      totalLowerOutliers8,
      /* lowerFence */ -1.0,
      /* lowerWhisker */ 2.0,
      /* firstQuartile */ 2.0,
      /* median */ 3.0,
      /* thirdQuartile */ 4.0,
      /* upperWhisker */ 4.0,
      /* upperFence */ 7.0,
      trueUpperOutliers8,
      totalUpperOutliers8,
      /* maxValue */ 4.0,
      false);

  // FrequencyMethod
  assert_data_box_plot(
      &store,
      listLength8,
      /* quartileRange */ 2.0,
      /* minValue */ 2.0,
      trueLowerOutliers8,
      totalLowerOutliers8,
      /* lowerFence */ -1.0,
      /* lowerWhisker */ 2.0,
      /* firstQuartile */ 2.0,
      /* median */ 3.0,
      /* thirdQuartile */ 4.0,
      /* upperWhisker */ 4.0,
      /* upperFence */ 7.0,
      trueUpperOutliers8,
      totalUpperOutliers8,
      /* maxValue */ 4.0,
      true);

  /* -996.85840734641
   * 9 */

  constexpr int listLength9 = 1;
  double v9[listLength9] = {-996.85840734641};
  double n9[listLength9] = {9.0};
  setStoreData(&store, v9, n9, listLength9, k_defaultSeriesIndex);
  constexpr int numberOfModes9 = 1;
  double modes9[numberOfModes9] = {-996.85840734641};
  assert_data_statistics_equal_to(
      &store,
      listLength9,
      /* sumOfOccurrences */ 9.0,
      /* range */ 0.0,
      /* mean */ -996.85840734641,
      /* variance */ 0.0,
      /* standardDeviation */ 0.0,
      /* sampleStandardDeviation */ 0.0,
      /* sum */ -8971.72566611769,
      /* squaredValueSum */ 8943540.158675,
      numberOfModes9,
      modes9,
      /* modesFrequency */ 9.0);

  constexpr int totalCumulatedFrequency9 = listLength9;
  double trueCumulatedFrequencyValues9[totalCumulatedFrequency9] = {-996.85840734641};
  double trueCumulatedFrequencyResults9[totalCumulatedFrequency9] = {100.0};
  assert_data_cumulated_frequency(&store, listLength9, trueCumulatedFrequencyValues9, trueCumulatedFrequencyResults9, totalCumulatedFrequency9);

  constexpr int totalNormalProbability9 = 9;
  double trueNormalProbabilityValues9[totalNormalProbability9] = {-996.85840734641, -996.85840734641, -996.85840734641, -996.85840734641, -996.85840734641, -996.85840734641, -996.85840734641, -996.85840734641, -996.85840734641};
  double trueNormalProbabilityResults9[totalNormalProbability9] = {-1.593, -0.9674, -0.5895, -0.2822, 0.0, 0.2822, 0.5895, 0.9674, 1.593};
  assert_data_normal_probability(&store, listLength9, trueNormalProbabilityValues9, trueNormalProbabilityResults9, totalNormalProbability9);

  constexpr int totalLowerOutliers9 = 0;
  double trueLowerOutliers9[totalLowerOutliers9] = {};
  constexpr int totalUpperOutliers9 = 0;
  double trueUpperOutliers9[totalUpperOutliers9] = {};
  // SublistMethod
  assert_data_box_plot(
      &store,
      listLength9,
      /* quartileRange */ 0.0,
      /* minValue */ -996.85840734641,
      trueLowerOutliers9,
      totalLowerOutliers9,
      /* lowerFence */ -996.85840734641,
      /* lowerWhisker */ -996.85840734641,
      /* firstQuartile */ -996.85840734641,
      /* median */ -996.85840734641,
      /* thirdQuartile */ -996.85840734641,
      /* upperWhisker */ -996.85840734641,
      /* upperFence */ -996.85840734641,
      trueUpperOutliers9,
      totalUpperOutliers9,
      /* maxValue */ -996.85840734641,
      false);

  // FrequencyMethod
  assert_data_box_plot(
      &store,
      listLength9,
      /* quartileRange */ 0.0,
      /* minValue */ -996.85840734641,
      trueLowerOutliers9,
      totalLowerOutliers9,
      /* lowerFence */ -996.85840734641,
      /* lowerWhisker */ -996.85840734641,
      /* firstQuartile */ -996.85840734641,
      /* median */ -996.85840734641,
      /* thirdQuartile */ -996.85840734641,
      /* upperWhisker */ -996.85840734641,
      /* upperFence */ -996.85840734641,
      trueUpperOutliers9,
      totalUpperOutliers9,
      /* maxValue */ -996.85840734641,
      true);

   /* 1 4
    * 0 1 */

  constexpr int listLength10 = 2;
  double v10[listLength10] = {1.0, 4.0};
  double n10[listLength10] = {0.0, 1.0};
  setStoreData(&store, v10, n10, listLength10, k_defaultSeriesIndex);
  constexpr int numberOfModes10 = 1;
  double modes10[numberOfModes10] = {4.0};
  assert_data_statistics_equal_to(
      &store,
      listLength10,
      /* sumOfOccurrences */ 1.0,
      /* range */ 0.0,
      /* mean */ 4.0,
      /* variance */ 0.0,
      /* standardDeviation */ 0.0,
      /* sampleStandardDeviation */ NAN,
      /* sum */ 4.0,
      /* squaredValueSum */ 16.0,
      numberOfModes10,
      modes10,
      /* modesFrequency */ 1.0);

  constexpr int totalCumulatedFrequency10 = listLength10;
  double trueCumulatedFrequencyValues10[totalCumulatedFrequency10] = {1.0, 4.0};
  double trueCumulatedFrequencyResults10[totalCumulatedFrequency10] = {0.0, 100.0};
  assert_data_cumulated_frequency(&store, listLength10, trueCumulatedFrequencyValues10, trueCumulatedFrequencyResults10, totalCumulatedFrequency10);

  constexpr int totalNormalProbability10 = 1;
  double trueNormalProbabilityValues10[totalNormalProbability10] = {4.0};
  double trueNormalProbabilityResults10[totalNormalProbability10] = {0.0};
  assert_data_normal_probability(&store, listLength10, trueNormalProbabilityValues10, trueNormalProbabilityResults10, totalNormalProbability10);

  constexpr int totalLowerOutliers10 = 0;
  double trueLowerOutliers10[totalLowerOutliers10] = {};
  constexpr int totalUpperOutliers10 = 0;
  double trueUpperOutliers10[totalUpperOutliers10] = {};
  // SublistMethod
  assert_data_box_plot(
      &store,
      listLength10,
      /* quartileRange */ 0.0,
      /* minValue */ 4.0,
      trueLowerOutliers10,
      totalLowerOutliers10,
      /* lowerFence */ 4.0,
      /* lowerWhisker */ 4.0,
      /* firstQuartile */ 4.0,
      /* median */ 4.0,
      /* thirdQuartile */ 4.0,
      /* upperWhisker */ 4.0,
      /* upperFence */ 4.0,
      trueUpperOutliers10,
      totalUpperOutliers10,
      /* maxValue */ 4.0,
      false);

  // FrequencyMethod
  assert_data_box_plot(
      &store,
      listLength10,
      /* quartileRange */ 0.0,
      /* minValue */ 4.0,
      trueLowerOutliers10,
      totalLowerOutliers10,
      /* lowerFence */ 4.0,
      /* lowerWhisker */ 4.0,
      /* firstQuartile */ 4.0,
      /* median */ 4.0,
      /* thirdQuartile */ 4.0,
      /* upperWhisker */ 4.0,
      /* upperFence */ 4.0,
      trueUpperOutliers10,
      totalUpperOutliers10,
      /* maxValue */ 4.0,
      true);

  // Empty out the store
  setStoreData(&store, {}, {}, 0, k_defaultSeriesIndex);
}

void assert_data_statistics_multiple_series_equal_to(
    Store * store,
    int numberOfData1,
    int numberOfData2,
    int numberOfData3,
    double maxValue,
    double minValue,
    double maxValueIgnoringFrequency,
    double minValueIgnoringFrequency) {
  quiz_assert(maxValue == store->maxValueForAllSeries(false));
  quiz_assert(maxValueIgnoringFrequency == store->maxValueForAllSeries(true));
  quiz_assert(minValue == store->minValueForAllSeries(false));
  quiz_assert(minValueIgnoringFrequency == store->minValueForAllSeries(true));
}

QUIZ_CASE(data_statistics_multiple_series) {
  GlobalContext context;
  UserPreferences userPreferences;
  Store store(&context, &userPreferences);

  constexpr int listLength1 = 4;
  double v1[listLength1] = {-12.0, 3.0, -0.001, 4000.0};
  double n1[listLength1] = {0.0, 0.0, 1.0, 1.0};
  constexpr int listLength2 = 0;
  double v2[listLength2] = {};
  double n2[listLength2] = {};
  constexpr int listLength3 = 5;
  double v3[listLength3] = {1.0, 1.0, 5000.0, 5000.0, 7.0};
  double n3[listLength3] = {1.0, 2.0, 1.0, 0.0, 0.43};
  setStoreData(&store, v1, n1, listLength1, 0);
  setStoreData(&store, v2, n2, listLength2, 1);
  setStoreData(&store, v3, n3, listLength3, 2);

  assert_data_statistics_multiple_series_equal_to(&store, listLength1, listLength2, listLength3, 5000.0, -0.001, 5000.0, -12.0);

  // Empty out the store
  setStoreData(&store, {}, {}, 0, 0);
  setStoreData(&store, {}, {}, 0, 1);
  setStoreData(&store, {}, {}, 0, 2);
}

QUIZ_CASE(data_statistics_histograms) {
  GlobalContext context;
  UserPreferences userPreferences;
  Store store(&context, &userPreferences);

  constexpr int seriesIndex1 = 0;
  constexpr int listLength1 = 8;
  double v1[listLength1] = {11.97, 11.98, 12.01, 12.05, 12.08, 12.09, 12.11, 12.12};
  double n1[listLength1] = { 1.0 ,  1.0 ,  2.0 ,  3.0 ,  1.0 ,  2.0  , 1.0 , 1.0  };
  double barWidth1 = 0.01;
  double firstDrawnBarAbscissa1 = 11.97;
  constexpr int numberOfBars1 = 16;
  double barHeight1[numberOfBars1] = {1.0, 1.0, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 3.0, 0.0, 0.0, 1.0, 2.0, 0.0, 1.0, 1.0};

  setStoreData(&store, v1, n1, listLength1, seriesIndex1);
  userPreferences.setBarWidth(barWidth1);
  userPreferences.setFirstDrawnBarAbscissa(firstDrawnBarAbscissa1);

  int numberOfBars = store.numberOfBars(seriesIndex1);
  for (int i = 0; i < numberOfBars; i++) {
    quiz_assert(store.heightOfBarAtIndex(seriesIndex1, i) == barHeight1[i]);
  }
}

}
