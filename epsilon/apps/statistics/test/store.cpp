#include "../store.h"

#include <apps/global_preferences.h>
#include <apps/i18n.h>
#include <assert.h>
#include <poincare/test/old/helper.h>
#include <quiz.h>

#include <cmath>

using namespace Poincare;
using namespace Shared;

namespace Statistics {

constexpr int k_defaultSeriesIndex = 0;

void assert_value_approximately_equal_to(double d1, double d2, double precision,
                                         double nullExpectedPrecision) {
  quiz_assert(
      (std::isinf(d1) && std::isinf(d2) && d1 * d2 > 0.0 /*same sign*/) ||
      roughly_equal(d1, d2, precision, true, nullExpectedPrecision));
}

// Set the data in the store
void setStoreData(Store* store, double v[], double n[], int numberOfData,
                  int seriesIndex) {
  store->deleteAllPairsOfSeries(seriesIndex);
  for (int i = 0; i < numberOfData; i++) {
    store->set(v[i], seriesIndex, 0, i);
    store->set(n[i], seriesIndex, 1, i);
  }
}

void assert_data_statistics_equal_to(
    Store* store, int numberOfData, double trueSumOfOccurrences,
    double trueRange, double trueMean, double trueVariance,
    double trueStandardDeviation, double trueSampleStandardDeviation,
    double trueSum, double trueSquaredValueSum, int trueNumberOfModes,
    double trueModes[], double trueModeFrequency) {
  double precision = 1e-3;

  double sumOfOccurrences = store->sumOfOccurrences(k_defaultSeriesIndex);
  double range = store->range(k_defaultSeriesIndex);
  double mean = store->mean(k_defaultSeriesIndex);
  double variance = store->variance(k_defaultSeriesIndex);
  double standardDeviation = store->standardDeviation(k_defaultSeriesIndex);
  double sampleStandardDeviation =
      store->sampleStandardDeviation(k_defaultSeriesIndex);
  double sum = store->sum(k_defaultSeriesIndex);
  double squaredValueSum = store->squaredValueSum(k_defaultSeriesIndex);

  // Check the positive statistics
  quiz_assert(range >= 0.0);
  quiz_assert(variance >= 0.0);
  quiz_assert(standardDeviation >= 0.0);
  quiz_assert(std::isnan(sampleStandardDeviation) ||
              sampleStandardDeviation >= 0.0);
  quiz_assert(squaredValueSum >= 0.0);

  // Compare the statistics
  double nullExpectedPrecision = 1e-10;
  assert_value_approximately_equal_to(variance, trueVariance, precision,
                                      nullExpectedPrecision);
  assert_value_approximately_equal_to(squaredValueSum, trueSquaredValueSum,
                                      precision, nullExpectedPrecision);
  assert_value_approximately_equal_to(
      trueStandardDeviation * trueStandardDeviation, trueVariance, precision,
      nullExpectedPrecision);
  assert_value_approximately_equal_to(sumOfOccurrences, trueSumOfOccurrences,
                                      precision, nullExpectedPrecision);
  assert_value_approximately_equal_to(mean, trueMean, precision,
                                      nullExpectedPrecision);
  assert_value_approximately_equal_to(standardDeviation, trueStandardDeviation,
                                      precision, nullExpectedPrecision);
  assert_value_approximately_equal_to(sampleStandardDeviation,
                                      trueSampleStandardDeviation, precision,
                                      nullExpectedPrecision);
  assert_value_approximately_equal_to(sum, trueSum, precision,
                                      nullExpectedPrecision);

  // Perfect match
  assert_value_approximately_equal_to(range, trueRange, 0.0, 0.0);

  // Compare the modes
  int numberOfModes = store->numberOfModes(k_defaultSeriesIndex);
  quiz_assert(numberOfModes == trueNumberOfModes);
  assert_value_approximately_equal_to(
      store->modeFrequency(k_defaultSeriesIndex), trueModeFrequency, precision,
      nullExpectedPrecision);
  for (int i = 0; i < numberOfModes; i++) {
    assert_value_approximately_equal_to(
        store->modeAtIndex(k_defaultSeriesIndex, i), trueModes[i], 0.0, 0.0);
  }
}

// Compare the cumulated frequency data points
void assert_data_cumulated_frequency(Store* store, int numberOfData,
                                     double trueCumulatedFrequencyValues[],
                                     double trueCumulatedFrequencyResults[],
                                     int totalCumulatedFrequency) {
  double precision = 1e-3;
  double nullExpectedPrecision = 1e-10;

  quiz_assert(store->totalCumulatedFrequencyValues(k_defaultSeriesIndex) ==
              totalCumulatedFrequency);
  for (int i = 0; i < totalCumulatedFrequency; i++) {
    assert_value_approximately_equal_to(
        trueCumulatedFrequencyValues[i],
        store->cumulatedFrequencyValueAtIndex(k_defaultSeriesIndex, i),
        precision, nullExpectedPrecision);
    assert_value_approximately_equal_to(
        trueCumulatedFrequencyResults[i],
        store->cumulatedFrequencyResultAtIndex(k_defaultSeriesIndex, i),
        precision, nullExpectedPrecision);
  }
}

// Compare the normal probability data points
void assert_data_normal_probability(Store* store, int numberOfData,
                                    double trueNormalProbabilityValues[],
                                    double trueNormalProbabilityResults[],
                                    int totalNormalProbability) {
  double precision = 1e-3;
  double nullExpectedPrecision = 1e-10;

  quiz_assert(store->totalNormalProbabilityValues(k_defaultSeriesIndex) ==
              totalNormalProbability);
  for (int i = 0; i < totalNormalProbability; i++) {
    assert_value_approximately_equal_to(
        trueNormalProbabilityValues[i],
        store->normalProbabilityValueAtIndex(k_defaultSeriesIndex, i),
        precision, nullExpectedPrecision);
    assert_value_approximately_equal_to(
        trueNormalProbabilityResults[i],
        store->normalProbabilityResultAtIndex(k_defaultSeriesIndex, i),
        precision, nullExpectedPrecision);
  }
}

// Compare the box-and-whiskers data points
void assert_data_box_plot(Store* store, int numberOfData,
                          double trueQuartileRange, double trueMinValue,
                          double trueLowerOutliers[],
                          int trueNumberOfLowerOutliers, double trueLowerFence,
                          double trueLowerWhisker, double trueFirstQuartile,
                          double trueMedian, double trueThirdQuartile,
                          double trueUpperWhisker, double trueUpperFence,
                          double trueUpperOutliers[],
                          int trueNumberOfUpperOutliers, double trueMaxValue,
                          bool shouldUseFrequencyMethod) {
  double precision = 1e-3;
  double nullExpectedPrecision = 1e-10;

  // Metrics independent from country and outliers display setting
  assert_value_approximately_equal_to(store->median(k_defaultSeriesIndex),
                                      trueMedian, precision,
                                      nullExpectedPrecision);
  // Perfect match
  assert_value_approximately_equal_to(store->minValue(k_defaultSeriesIndex),
                                      trueMinValue, 0.0, 0.0);
  assert_value_approximately_equal_to(store->maxValue(k_defaultSeriesIndex),
                                      trueMaxValue, 0.0, 0.0);

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
    GlobalPreferences::SharedGlobalPreferences()->setCountry(country);
    bool countryUseFrequencyMethod =
        (GlobalPreferences::SharedGlobalPreferences()->methodForQuartiles() ==
         Poincare::Preferences::MethodForQuartiles::CumulatedFrequency);
    if (shouldUseFrequencyMethod != countryUseFrequencyMethod) {
      continue;
    }
    double quartileRange = store->quartileRange(k_defaultSeriesIndex);
    quiz_assert(quartileRange >= 0.0);
    assert_value_approximately_equal_to(quartileRange, trueQuartileRange, 0.0,
                                        0.0);

    assert_value_approximately_equal_to(store->lowerFence(k_defaultSeriesIndex),
                                        trueLowerFence, precision,
                                        nullExpectedPrecision);
    assert_value_approximately_equal_to(
        store->lowerWhisker(k_defaultSeriesIndex), trueLowerWhisker, precision,
        nullExpectedPrecision);
    assert_value_approximately_equal_to(
        store->firstQuartile(k_defaultSeriesIndex), trueFirstQuartile,
        precision, nullExpectedPrecision);
    assert_value_approximately_equal_to(
        store->thirdQuartile(k_defaultSeriesIndex), trueThirdQuartile,
        precision, nullExpectedPrecision);
    assert_value_approximately_equal_to(
        store->upperWhisker(k_defaultSeriesIndex), trueUpperWhisker, precision,
        nullExpectedPrecision);
    assert_value_approximately_equal_to(store->upperFence(k_defaultSeriesIndex),
                                        trueUpperFence, precision,
                                        nullExpectedPrecision);

    int numberOfLowerOutliers =
        store->numberOfLowerOutliers(k_defaultSeriesIndex);
    quiz_assert((trueNumberOfLowerOutliers) == numberOfLowerOutliers);
    for (int i = 0; i < trueNumberOfLowerOutliers; i++) {
      quiz_assert((trueLowerOutliers)[i] ==
                  store->lowerOutlierAtIndex(k_defaultSeriesIndex, i));
    }

    int numberOfUpperOutliers =
        store->numberOfUpperOutliers(k_defaultSeriesIndex);
    quiz_assert((trueNumberOfUpperOutliers) == numberOfUpperOutliers);
    for (int i = 0; i < trueNumberOfUpperOutliers; i++) {
      quiz_assert((trueUpperOutliers)[i] ==
                  store->upperOutlierAtIndex(k_defaultSeriesIndex, i));
    }
  }
  GlobalPreferences::SharedGlobalPreferences()->setCountry(I18n::Country::WW);
}

QUIZ_CASE(statistics) {
  UserPreferences userPreferences;
  Store store(&userPreferences);

  {
    /* 1 2 3 4
     * 1 1 1 1 */
    constexpr int listLength = 4;
    double v[listLength] = {1.0, 2.0, 3.0, 4.0};
    double n[listLength] = {1.0, 1.0, 1.0, 1.0};
    setStoreData(&store, v, n, listLength, k_defaultSeriesIndex);
    constexpr int numberOfmodes = 4;
    double modes[numberOfmodes] = {1.0, 2.0, 3.0, 4.0};
    assert_data_statistics_equal_to(&store, listLength,
                                    /* sumOfOccurrences */ 4.0,
                                    /* range */ 3.0,
                                    /* mean */ 2.5,
                                    /* variance */ 1.25,
                                    /* standardDeviation */ 1.118,
                                    /* sampleStandardDeviation */ 1.291,
                                    /* sum */ 10.0,
                                    /* squaredValueSum */ 30.0, numberOfmodes,
                                    modes,
                                    /* modesFrequency */ 1.0);

    constexpr int totalCumulatedFrequency = listLength;
    double trueCumulatedFrequencyValues[totalCumulatedFrequency] = {1.0, 2.0,
                                                                    3.0, 4.0};
    double trueCumulatedFrequencyResults[totalCumulatedFrequency] = {
        25.0, 50.0, 75.0, 100.0};
    assert_data_cumulated_frequency(
        &store, listLength, trueCumulatedFrequencyValues,
        trueCumulatedFrequencyResults, totalCumulatedFrequency);

    constexpr int totalNormalProbability = listLength;
    double trueNormalProbabilityValues[totalNormalProbability] = {1.0, 2.0, 3.0,
                                                                  4.0};
    double trueNormalProbabilityResults[totalNormalProbability] = {
        -1.150, -0.3186, 0.3186, 1.150};
    assert_data_normal_probability(
        &store, listLength, trueNormalProbabilityValues,
        trueNormalProbabilityResults, totalNormalProbability);

    constexpr int totalLowerOutliers = 0;
    double trueLowerOutliers[totalLowerOutliers] = {};
    constexpr int totalUpperOutliers = 0;
    double trueUpperOutliers[totalUpperOutliers] = {};
    // SublistMethod
    assert_data_box_plot(
        &store, listLength,
        /* quartileRange */ 2.0,
        /* minValue */ 1.0, trueLowerOutliers, totalLowerOutliers,
        /* lowerFence */ -1.5,
        /* lowerWhisker */ 1.0,
        /* firstQuartile */ 1.5,
        /* median */ 2.5,
        /* thirdQuartile */ 3.5,
        /* upperWhisker */ 4.0,
        /* upperFence */ 6.5, trueUpperOutliers, totalUpperOutliers,
        /* maxValue */ 4.0, false);

    // FrequencyMethod
    assert_data_box_plot(
        &store, listLength,
        /* quartileRange */ 2.0,
        /* minValue */ 1.0, trueLowerOutliers, totalLowerOutliers,
        /* lowerFence */ -2.0,
        /* lowerWhisker */ 1.0,
        /* firstQuartile */ 1.0,
        /* median */ 2.5,
        /* thirdQuartile */ 3.0,
        /* upperWhisker */ 4.0,
        /* upperFence */ 6.0, trueUpperOutliers, totalUpperOutliers,
        /* maxValue */ 4.0, true);
  }

  {
    /* 1 2 3 4 5 6 7 8 9 10 11
     * 1 1 1 1 1 1 1 1 1  1  1 */
    constexpr int listLength = 11;
    double v[listLength] = {1.0, 2.0, 3.0, 4.0,  5.0, 6.0,
                            7.0, 8.0, 9.0, 10.0, 11.0};
    double n[listLength] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                            1.0, 1.0, 1.0, 1.0, 1.0};
    setStoreData(&store, v, n, listLength, k_defaultSeriesIndex);
    constexpr int numberOfmodes = 11;
    double modes[numberOfmodes] = {1.0, 2.0, 3.0, 4.0,  5.0, 6.0,
                                   7.0, 8.0, 9.0, 10.0, 11.0};
    assert_data_statistics_equal_to(&store, listLength,
                                    /* sumOfOccurrences */ 11.0,
                                    /* range */ 10.0,
                                    /* mean */ 6.0,
                                    /* variance */ 10.0,
                                    /* standardDeviation */ 3.1623,
                                    /* sampleStandardDeviation */ 3.3166,
                                    /* sum */ 66.0,
                                    /* squaredValueSum */ 506.0, numberOfmodes,
                                    modes,
                                    /* modesFrequency */ 1.0);

    constexpr int totalCumulatedFrequency = listLength;
    double trueCumulatedFrequencyValues[totalCumulatedFrequency] = {
        1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0};
    double trueCumulatedFrequencyResults[totalCumulatedFrequency] = {
        9.090, 18.18, 27.27, 36.36, 45.45, 54.54,
        63.63, 72.72, 81.81, 90.90, 100.0};
    assert_data_cumulated_frequency(
        &store, listLength, trueCumulatedFrequencyValues,
        trueCumulatedFrequencyResults, totalCumulatedFrequency);

    constexpr int totalNormalProbability = listLength;
    double trueNormalProbabilityValues[totalNormalProbability] = {
        1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0};
    double trueNormalProbabilityResults[totalNormalProbability] = {
        -1.691, -1.097, -0.7479, -0.4728, -0.2299, 0.0,
        0.2299, 0.4728, 0.7479,  1.097,   1.691};
    assert_data_normal_probability(
        &store, listLength, trueNormalProbabilityValues,
        trueNormalProbabilityResults, totalNormalProbability);

    constexpr int totalLowerOutliers = 0;
    double trueLowerOutliers[totalLowerOutliers] = {};
    constexpr int totalUpperOutliers = 0;
    double trueUpperOutliers[totalUpperOutliers] = {};
    // SublistMethod
    assert_data_box_plot(
        &store, listLength,
        /* quartileRange */ 6.0,
        /* minValue */ 1.0, trueLowerOutliers, totalLowerOutliers,
        /* lowerFence */ -6.0,
        /* lowerWhisker */ 1.0,
        /* firstQuartile */ 3.0,
        /* median */ 6.0,
        /* thirdQuartile */ 9.0,
        /* upperWhisker */ 11.0,
        /* upperFence */ 18.0, trueUpperOutliers, totalUpperOutliers,
        /* maxValue */ 11.0, false);

    // FrequencyMethod
    assert_data_box_plot(
        &store, listLength,
        /* quartileRange */ 6.0,
        /* minValue */ 1.0, trueLowerOutliers, totalLowerOutliers,
        /* lowerFence */ -6.0,
        /* lowerWhisker */ 1.0,
        /* firstQuartile */ 3.0,
        /* median */ 6.0,
        /* thirdQuartile */ 9.0,
        /* upperWhisker */ 11.0,
        /* upperFence */ 18.0, trueUpperOutliers, totalUpperOutliers,
        /* maxValue */ 11.0, true);
  }

  {
    /* 1 2 3 4 5 6 7 8 9 10 11 12
     * 1 1 1 1 1 1 1 1 1  1  1  1 */
    constexpr int listLength = 12;
    double v[listLength] = {1.0, 2.0, 3.0, 4.0,  5.0,  6.0,
                            7.0, 8.0, 9.0, 10.0, 11.0, 12.0};
    double n[listLength] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                            1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
    setStoreData(&store, v, n, listLength, k_defaultSeriesIndex);
    constexpr int numberOfmodes = 12;
    double modes[numberOfmodes] = {1.0, 2.0, 3.0, 4.0,  5.0,  6.0,
                                   7.0, 8.0, 9.0, 10.0, 11.0, 12.0};
    assert_data_statistics_equal_to(&store, listLength,
                                    /* sumOfOccurrences */ 12.0,
                                    /* range */ 11.0,
                                    /* mean */ 6.5,
                                    /* variance */ 11.917,
                                    /* standardDeviation */ 3.4521,
                                    /* sampleStandardDeviation */ 3.6056,
                                    /* sum */ 78.0,
                                    /* squaredValueSum */ 650.0, numberOfmodes,
                                    modes,
                                    /* modesFrequency */ 1.0);

    constexpr int totalCumulatedFrequency = listLength;
    double trueCumulatedFrequencyValues[totalCumulatedFrequency] = {
        1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0};
    double trueCumulatedFrequencyResults[totalCumulatedFrequency] = {
        8.333, 16.67, 25.00, 33.33, 41.67, 50.00,
        58.33, 66.67, 75.00, 83.33, 91.67, 100.0};
    assert_data_cumulated_frequency(
        &store, listLength, trueCumulatedFrequencyValues,
        trueCumulatedFrequencyResults, totalCumulatedFrequency);

    constexpr int totalNormalProbability = listLength;
    double trueNormalProbabilityValues[totalNormalProbability] = {
        1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0};
    double trueNormalProbabilityResults[totalNormalProbability] = {
        -1.732, -1.150, -0.8122, -0.5485, -0.3186, -0.1046,
        0.1046, 0.3186, 0.5485,  0.8122,  1.150,   1.732};
    assert_data_normal_probability(
        &store, listLength, trueNormalProbabilityValues,
        trueNormalProbabilityResults, totalNormalProbability);

    constexpr int totalLowerOutliers = 0;
    double trueLowerOutliers[totalLowerOutliers] = {};
    constexpr int totalUpperOutliers = 0;
    double trueUpperOutliers[totalUpperOutliers] = {};
    // SublistMethod
    assert_data_box_plot(
        &store, listLength,
        /* quartileRange */ 6.0,
        /* minValue */ 1.0, trueLowerOutliers, totalLowerOutliers,
        /* lowerFence */ -5.5,
        /* lowerWhisker */ 1.0,
        /* firstQuartile */ 3.5,
        /* median */ 6.5,
        /* thirdQuartile */ 9.5,
        /* upperWhisker */ 12.0,
        /* upperFence */ 18.5, trueUpperOutliers, totalUpperOutliers,
        /* maxValue */ 12.0, false);

    // FrequencyMethod
    assert_data_box_plot(
        &store, listLength,
        /* quartileRange */ 6.0,
        /* minValue */ 1.0, trueLowerOutliers, totalLowerOutliers,
        /* lowerFence */ -6.0,
        /* lowerWhisker */ 1.0,
        /* firstQuartile */ 3.0,
        /* median */ 6.5,
        /* thirdQuartile */ 9.0,
        /* upperWhisker */ 12.0,
        /* upperFence */ 18.0, trueUpperOutliers, totalUpperOutliers,
        /* maxValue */ 12.0, true);
  }

  {
    /*   1    2   3      5     10
     * 0.2 0.05 0.3 0.0001 0.4499 */
    constexpr int listLength = 5;
    double v[listLength] = {1.0, 2.0, 3.0, 5.0, 10.0};
    double n[listLength] = {0.2, 0.05, 0.3, 0.0001, 0.4499};
    setStoreData(&store, v, n, listLength, k_defaultSeriesIndex);
    constexpr int numberOfmodes = 1;
    double modes[numberOfmodes] = {10.0};
    assert_data_statistics_equal_to(&store, listLength,
                                    /* sumOfOccurrences */ 1.0,
                                    /* range */ 9.0,
                                    /* mean */ 5.6995,
                                    /* variance */ 15.6082,
                                    /* standardDeviation */ 3.9507,
                                    /* sampleStandardDeviation */ INFINITY,
                                    /* sum */ 5.6995,
                                    /* squaredValueSum */ 48.0925,
                                    numberOfmodes, modes,
                                    /* modesFrequency */ 0.4499);

    constexpr int totalCumulatedFrequency = listLength;
    double trueCumulatedFrequencyValues[totalCumulatedFrequency] = {
        1.0, 2.0, 3.0, 5.0, 10.0};
    double trueCumulatedFrequencyResults[totalCumulatedFrequency] = {
        20.0, 25.0, 55.0, 55.01, 100.0};
    assert_data_cumulated_frequency(
        &store, listLength, trueCumulatedFrequencyValues,
        trueCumulatedFrequencyResults, totalCumulatedFrequency);

    constexpr int totalNormalProbability = 0;
    double trueNormalProbabilityValues[totalNormalProbability] = {};
    double trueNormalProbabilityResults[totalNormalProbability] = {};
    assert_data_normal_probability(
        &store, listLength, trueNormalProbabilityValues,
        trueNormalProbabilityResults, totalNormalProbability);

    constexpr int totalLowerOutliers = 0;
    double trueLowerOutliers[totalLowerOutliers] = {};
    constexpr int totalUpperOutliers = 0;
    double trueUpperOutliers[totalUpperOutliers] = {};
    // SublistMethod
    assert_data_box_plot(
        &store, listLength,
        /* quartileRange */ 8.0,
        /* minValue */ 1.0, trueLowerOutliers, totalLowerOutliers,
        /* lowerFence */ -10.0,
        /* lowerWhisker */ 1.0,
        /* firstQuartile */ 2.0,
        /* median */ 3.0,
        /* thirdQuartile */ 10.0,
        /* upperWhisker */ 10.0,
        /* upperFence */ 22.0, trueUpperOutliers, totalUpperOutliers,
        /* maxValue */ 10.0, false);

    // FrequencyMethod
    assert_data_box_plot(
        &store, listLength,
        /* quartileRange */ 8.0,
        /* minValue */ 1.0, trueLowerOutliers, totalLowerOutliers,
        /* lowerFence */ -10.0,
        /* lowerWhisker */ 1.0,
        /* firstQuartile */ 2.0,
        /* median */ 3.0,
        /* thirdQuartile */ 10.0,
        /* upperWhisker */ 10.0,
        /* upperFence */ 22.0, trueUpperOutliers, totalUpperOutliers,
        /* maxValue */ 10.0, true);
  }

  {
    /*   1      -2   3   5  10
     * 0.4 0.00005 0.9 0.4 0.5 */
    constexpr int listLength = 5;
    double v[listLength] = {1.0, -2.0, 3.0, 5.0, 10.0};
    double n[listLength] = {0.4, 0.00005, 0.9, 0.4, 0.5};
    setStoreData(&store, v, n, listLength, k_defaultSeriesIndex);
    constexpr int numberOfmodes = 1;
    double modes[numberOfmodes] = {3.0};
    assert_data_statistics_equal_to(&store, listLength,
                                    /* sumOfOccurrences */ 2.2,
                                    /* range */ 12.0,
                                    /* mean */ 4.5908,
                                    /* variance */ 10.06,
                                    /* standardDeviation */ 3.1719,
                                    /* sampleStandardDeviation */ 4.2947,
                                    /* sum */ 10.1,
                                    /* squaredValueSum */ 68.500, numberOfmodes,
                                    modes,
                                    /* modesFrequency */ 0.9);

    constexpr int totalCumulatedFrequency = listLength;
    double trueCumulatedFrequencyValues[totalCumulatedFrequency] = {
        -2.0, 1.0, 3.0, 5.0, 10.0};
    double trueCumulatedFrequencyResults[totalCumulatedFrequency] = {
        0.002273, 18.18, 59.09, 77.27, 100.0};
    assert_data_cumulated_frequency(
        &store, listLength, trueCumulatedFrequencyValues,
        trueCumulatedFrequencyResults, totalCumulatedFrequency);

    constexpr int totalNormalProbability = 0;
    double trueNormalProbabilityValues[totalNormalProbability] = {};
    double trueNormalProbabilityResults[totalNormalProbability] = {};
    assert_data_normal_probability(
        &store, listLength, trueNormalProbabilityValues,
        trueNormalProbabilityResults, totalNormalProbability);

    constexpr int totalLowerOutliers = 1;
    double trueLowerOutliers[totalLowerOutliers] = {-2.0};
    constexpr int totalUpperOutliers = 1;
    double trueUpperOutliers[totalUpperOutliers] = {10.0};
    // SublistMethod
    assert_data_box_plot(
        &store, listLength,
        /* quartileRange */ 2.0,
        /* minValue */ -2.0, trueLowerOutliers, totalLowerOutliers,
        /* lowerFence */ 0.0,
        /* lowerWhisker */ 1.0,
        /* firstQuartile */ 3.0,
        /* median */ 3.0,
        /* thirdQuartile */ 5.0,
        /* upperWhisker */ 5.0,
        /* upperFence */ 8.0, trueUpperOutliers, totalUpperOutliers,
        /* maxValue */ 10.0, false);

    // FrequencyMethod
    assert_data_box_plot(
        &store, listLength,
        /* quartileRange */ 2.0,
        /* minValue */ -2.0, trueLowerOutliers, totalLowerOutliers,
        /* lowerFence */ 0,
        /* lowerWhisker */ 1.0,
        /* firstQuartile */ 3.0,
        /* median */ 3.0,
        /* thirdQuartile */ 5.0,
        /* upperWhisker */ 5.0,
        /* upperFence */ 8.0, trueUpperOutliers, totalUpperOutliers,
        /* maxValue */ 10.0, true);
  }

  {
    /* -7 -10 12 5 -2
     *  4   5  3 1  9 */
    constexpr int listLength = 6;
    double v[listLength] = {-7.0, -10.0, 1.0, 2.0, 5.0, -2.0};
    double n[listLength] = {4.0, 5.0, 3.0, 0.5, 1.0, 9.0};
    setStoreData(&store, v, n, listLength, k_defaultSeriesIndex);
    constexpr int numberOfmodes = 1;
    double modes[numberOfmodes] = {-2.0};
    assert_data_statistics_equal_to(&store, listLength,
                                    /* sumOfOccurrences */ 22.5,
                                    /* range */ 15.0,
                                    /* mean */ -3.8667,
                                    /* variance */ 18.9155,
                                    /* standardDeviation */ 4.3492,
                                    /* sampleStandardDeviation */ 4.4492,
                                    /* sum */ -87.0,
                                    /* squaredValueSum */ 762.0, numberOfmodes,
                                    modes,
                                    /* modesFrequency */ 9.0);

    constexpr int totalCumulatedFrequency = listLength;
    double trueCumulatedFrequencyValues[totalCumulatedFrequency] = {
        -10.0, -7.0, -2.0, 1.0, 2.0, 5.0};
    double trueCumulatedFrequencyResults[totalCumulatedFrequency] = {
        22.22, 40.00, 80.00, 93.33, 95.56, 100.0};
    assert_data_cumulated_frequency(
        &store, listLength, trueCumulatedFrequencyValues,
        trueCumulatedFrequencyResults, totalCumulatedFrequency);

    constexpr int totalNormalProbability = 0;
    double trueNormalProbabilityValues[totalNormalProbability] = {};
    double trueNormalProbabilityResults[totalNormalProbability] = {};
    assert_data_normal_probability(
        &store, listLength, trueNormalProbabilityValues,
        trueNormalProbabilityResults, totalNormalProbability);

    constexpr int totalLowerOutliers = 0;
    double trueLowerOutliers[totalLowerOutliers] = {};
    constexpr int totalUpperOutliers = 0;
    double trueUpperOutliers[totalUpperOutliers] = {};
    // SublistMethod
    assert_data_box_plot(
        &store, listLength,
        /* quartileRange */ 5.0,
        /* minValue */ -10.0, trueLowerOutliers, totalLowerOutliers,
        /* lowerFence */ -14.5,
        /* lowerWhisker */ -10.0,
        /* firstQuartile */ -7.0,
        /* median */ -2.0,
        /* thirdQuartile */ -2.0,
        /* upperWhisker */ 5.0,
        /* upperFence */ 5.5, trueUpperOutliers, totalUpperOutliers,
        /* maxValue */ 5.0, false);

    // FrequencyMethod
    assert_data_box_plot(
        &store, listLength,
        /* quartileRange */ 5.0,
        /* minValue */ -10.0, trueLowerOutliers, totalLowerOutliers,
        /* lowerFence */ -14.5,
        /* lowerWhisker */ -10.0,
        /* firstQuartile */ -7.0,
        /* median */ -2.0,
        /* thirdQuartile */ -2.0,
        /* upperWhisker */ 5.0,
        /* upperFence */ 5.5, trueUpperOutliers, totalUpperOutliers,
        /* maxValue */ 5.0, true);
  }

  {
    /* 1 1 1 10 3 -1 3
     * 1 1 1  0 0  0 1 */
    constexpr int listLength = 7;
    double v[listLength] = {1.0, 1.0, 1.0, 10.0, 3.0, -1.0, 3.0};
    double n[listLength] = {1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0};
    setStoreData(&store, v, n, listLength, k_defaultSeriesIndex);
    constexpr int numberOfmodes = 1;
    double modes[numberOfmodes] = {1.0};
    assert_data_statistics_equal_to(&store, listLength,
                                    /* sumOfOccurrences */ 4.0,
                                    /* range */ 2.0,
                                    /* mean */ 1.5,
                                    /* variance */ 0.75,
                                    /* standardDeviation */ 0.866,
                                    /* sampleStandardDeviation */ 1.0,
                                    /* sum */ 6.0,
                                    /* squaredValueSum */ 12.0, numberOfmodes,
                                    modes,
                                    /* modesFrequency */ 3.0);

    constexpr int totalCumulatedFrequency = 4;
    double trueCumulatedFrequencyValues[totalCumulatedFrequency] = {-1.0, 1.0,
                                                                    3.0, 10.0};
    double trueCumulatedFrequencyResults[totalCumulatedFrequency] = {
        0.0, 75.0, 100.0, 100.0};
    assert_data_cumulated_frequency(
        &store, listLength, trueCumulatedFrequencyValues,
        trueCumulatedFrequencyResults, totalCumulatedFrequency);

    constexpr int totalNormalProbability = 4;
    double trueNormalProbabilityValues[totalNormalProbability] = {1.0, 1.0, 1.0,
                                                                  3.0};
    double trueNormalProbabilityResults[totalNormalProbability] = {
        -1.150, -0.3186, 0.3186, 1.150};
    assert_data_normal_probability(
        &store, listLength, trueNormalProbabilityValues,
        trueNormalProbabilityResults, totalNormalProbability);

    constexpr int totalLowerOutliersSublistMethod = 0;
    double trueLowerOutliersSublistMethod[totalLowerOutliersSublistMethod] = {};
    constexpr int totalUpperOutliersSublistMethod = 0;
    double trueUpperOutliersSublistMethod[totalUpperOutliersSublistMethod] = {};
    // SublistMethod
    assert_data_box_plot(&store, listLength,
                         /* quartileRange */ 1.0,
                         /* minValue */ 1.0, trueLowerOutliersSublistMethod,
                         totalLowerOutliersSublistMethod,
                         /* lowerFence */ -0.5,
                         /* lowerWhisker */ 1.0,
                         /* firstQuartile */ 1.0,
                         /* median */ 1.0,
                         /* thirdQuartile */ 2.0,
                         /* upperWhisker */ 3.0,
                         /* upperFence */ 3.5, trueUpperOutliersSublistMethod,
                         totalUpperOutliersSublistMethod,
                         /* maxValue */ 3.0, false);

    constexpr int totalLowerOutliersFrequencyMethod = 0;
    double trueLowerOutliersFrequencyMethod[totalLowerOutliersFrequencyMethod] =
        {};
    constexpr int totalUpperOutliersFrequencyMethod = 1;
    double trueUpperOutliersFrequencyMethod[totalUpperOutliersFrequencyMethod] =
        {3.0};
    // FrequencyMethod
    assert_data_box_plot(&store, listLength,
                         /* quartileRange */ 0.0,
                         /* minValue */ 1.0, trueLowerOutliersFrequencyMethod,
                         totalLowerOutliersFrequencyMethod,
                         /* lowerFence */ 1.0,
                         /* lowerWhisker */ 1.0,
                         /* firstQuartile */ 1.0,
                         /* median */ 1.0,
                         /* thirdQuartile */ 1.0,
                         /* upperWhisker */ 1.0,
                         /* upperFence */ 1.0, trueUpperOutliersFrequencyMethod,
                         totalUpperOutliersFrequencyMethod,
                         /* maxValue */ 3.0, true);
  }

  {
    /* 1 2 3 4
     * 0 1 0 1 */
    constexpr int listLength = 4;
    double v[listLength] = {1.0, 2.0, 3.0, 4.0};
    double n[listLength] = {0.0, 1.0, 0.0, 1.0};
    setStoreData(&store, v, n, listLength, k_defaultSeriesIndex);
    constexpr int numberOfmodes = 2;
    double modes[numberOfmodes] = {2.0, 4.0};
    assert_data_statistics_equal_to(&store, listLength,
                                    /* sumOfOccurrences */ 2.0,
                                    /* range */ 2.0,
                                    /* mean */ 3.0,
                                    /* variance */ 1.0,
                                    /* standardDeviation */ 1.0,
                                    /* sampleStandardDeviation */ 1.414,
                                    /* sum */ 6.0,
                                    /* squaredValueSum */ 20.0, numberOfmodes,
                                    modes,
                                    /* modesFrequency */ 1.0);

    constexpr int totalCumulatedFrequency = listLength;
    double trueCumulatedFrequencyValues[totalCumulatedFrequency] = {1.0, 2.0,
                                                                    3.0, 4.0};
    double trueCumulatedFrequencyResults[totalCumulatedFrequency] = {
        0.0, 50.0, 50.0, 100.0};
    assert_data_cumulated_frequency(
        &store, listLength, trueCumulatedFrequencyValues,
        trueCumulatedFrequencyResults, totalCumulatedFrequency);

    constexpr int totalNormalProbability = 2;
    double trueNormalProbabilityValues[totalNormalProbability] = {2.0, 4.0};
    double trueNormalProbabilityResults[totalNormalProbability] = {-0.6745,
                                                                   0.6745};
    assert_data_normal_probability(
        &store, listLength, trueNormalProbabilityValues,
        trueNormalProbabilityResults, totalNormalProbability);

    constexpr int totalLowerOutliers = 0;
    double trueLowerOutliers[totalLowerOutliers] = {};
    constexpr int totalUpperOutliers = 0;
    double trueUpperOutliers[totalUpperOutliers] = {};
    // SublistMethod
    assert_data_box_plot(
        &store, listLength,
        /* quartileRange */ 2.0,
        /* minValue */ 2.0, trueLowerOutliers, totalLowerOutliers,
        /* lowerFence */ -1.0,
        /* lowerWhisker */ 2.0,
        /* firstQuartile */ 2.0,
        /* median */ 3.0,
        /* thirdQuartile */ 4.0,
        /* upperWhisker */ 4.0,
        /* upperFence */ 7.0, trueUpperOutliers, totalUpperOutliers,
        /* maxValue */ 4.0, false);

    // FrequencyMethod
    assert_data_box_plot(
        &store, listLength,
        /* quartileRange */ 2.0,
        /* minValue */ 2.0, trueLowerOutliers, totalLowerOutliers,
        /* lowerFence */ -1.0,
        /* lowerWhisker */ 2.0,
        /* firstQuartile */ 2.0,
        /* median */ 3.0,
        /* thirdQuartile */ 4.0,
        /* upperWhisker */ 4.0,
        /* upperFence */ 7.0, trueUpperOutliers, totalUpperOutliers,
        /* maxValue */ 4.0, true);
  }

  {
    /* -996.85840734641
     * 9 */
    constexpr int listLength = 1;
    double v[listLength] = {-996.85840734641};
    double n[listLength] = {9.0};
    setStoreData(&store, v, n, listLength, k_defaultSeriesIndex);
    constexpr int numberOfmodes = 1;
    double modes[numberOfmodes] = {-996.85840734641};
    assert_data_statistics_equal_to(&store, listLength,
                                    /* sumOfOccurrences */ 9.0,
                                    /* range */ 0.0,
                                    /* mean */ -996.85840734641,
                                    /* variance */ 0.0,
                                    /* standardDeviation */ 0.0,
                                    /* sampleStandardDeviation */ 0.0,
                                    /* sum */ -8971.72566611769,
                                    /* squaredValueSum */ 8943540.158675,
                                    numberOfmodes, modes,
                                    /* modesFrequency */ 9.0);

    constexpr int totalCumulatedFrequency = listLength;
    double trueCumulatedFrequencyValues[totalCumulatedFrequency] = {
        -996.85840734641};
    double trueCumulatedFrequencyResults[totalCumulatedFrequency] = {100.0};
    assert_data_cumulated_frequency(
        &store, listLength, trueCumulatedFrequencyValues,
        trueCumulatedFrequencyResults, totalCumulatedFrequency);

    constexpr int totalNormalProbability = 9;
    double trueNormalProbabilityValues[totalNormalProbability] = {
        -996.85840734641, -996.85840734641, -996.85840734641,
        -996.85840734641, -996.85840734641, -996.85840734641,
        -996.85840734641, -996.85840734641, -996.85840734641};
    double trueNormalProbabilityResults[totalNormalProbability] = {
        -1.593, -0.9674, -0.5895, -0.2822, 0.0, 0.2822, 0.5895, 0.9674, 1.593};
    assert_data_normal_probability(
        &store, listLength, trueNormalProbabilityValues,
        trueNormalProbabilityResults, totalNormalProbability);

    constexpr int totalLowerOutliers = 0;
    double trueLowerOutliers[totalLowerOutliers] = {};
    constexpr int totalUpperOutliers = 0;
    double trueUpperOutliers[totalUpperOutliers] = {};
    // SublistMethod
    assert_data_box_plot(&store, listLength,
                         /* quartileRange */ 0.0,
                         /* minValue */ -996.85840734641, trueLowerOutliers,
                         totalLowerOutliers,
                         /* lowerFence */ -996.85840734641,
                         /* lowerWhisker */ -996.85840734641,
                         /* firstQuartile */ -996.85840734641,
                         /* median */ -996.85840734641,
                         /* thirdQuartile */ -996.85840734641,
                         /* upperWhisker */ -996.85840734641,
                         /* upperFence */ -996.85840734641, trueUpperOutliers,
                         totalUpperOutliers,
                         /* maxValue */ -996.85840734641, false);

    // FrequencyMethod
    assert_data_box_plot(&store, listLength,
                         /* quartileRange */ 0.0,
                         /* minValue */ -996.85840734641, trueLowerOutliers,
                         totalLowerOutliers,
                         /* lowerFence */ -996.85840734641,
                         /* lowerWhisker */ -996.85840734641,
                         /* firstQuartile */ -996.85840734641,
                         /* median */ -996.85840734641,
                         /* thirdQuartile */ -996.85840734641,
                         /* upperWhisker */ -996.85840734641,
                         /* upperFence */ -996.85840734641, trueUpperOutliers,
                         totalUpperOutliers,
                         /* maxValue */ -996.85840734641, true);
  }

  {
    /* 1 4
     * 0 1 */
    constexpr int listLength = 2;
    double v[listLength] = {1.0, 4.0};
    double n[listLength] = {0.0, 1.0};
    setStoreData(&store, v, n, listLength, k_defaultSeriesIndex);
    constexpr int numberOfmodes = 1;
    double modes[numberOfmodes] = {4.0};
    assert_data_statistics_equal_to(&store, listLength,
                                    /* sumOfOccurrences */ 1.0,
                                    /* range */ 0.0,
                                    /* mean */ 4.0,
                                    /* variance */ 0.0,
                                    /* standardDeviation */ 0.0,
                                    /* sampleStandardDeviation */ NAN,
                                    /* sum */ 4.0,
                                    /* squaredValueSum */ 16.0, numberOfmodes,
                                    modes,
                                    /* modesFrequency */ 1.0);

    constexpr int totalCumulatedFrequency = listLength;
    double trueCumulatedFrequencyValues[totalCumulatedFrequency] = {1.0, 4.0};
    double trueCumulatedFrequencyResults[totalCumulatedFrequency] = {0.0,
                                                                     100.0};
    assert_data_cumulated_frequency(
        &store, listLength, trueCumulatedFrequencyValues,
        trueCumulatedFrequencyResults, totalCumulatedFrequency);

    constexpr int totalNormalProbability = 1;
    double trueNormalProbabilityValues[totalNormalProbability] = {4.0};
    double trueNormalProbabilityResults[totalNormalProbability] = {0.0};
    assert_data_normal_probability(
        &store, listLength, trueNormalProbabilityValues,
        trueNormalProbabilityResults, totalNormalProbability);

    constexpr int totalLowerOutliers = 0;
    double trueLowerOutliers[totalLowerOutliers] = {};
    constexpr int totalUpperOutliers = 0;
    double trueUpperOutliers[totalUpperOutliers] = {};
    // SublistMethod
    assert_data_box_plot(
        &store, listLength,
        /* quartileRange */ 0.0,
        /* minValue */ 4.0, trueLowerOutliers, totalLowerOutliers,
        /* lowerFence */ 4.0,
        /* lowerWhisker */ 4.0,
        /* firstQuartile */ 4.0,
        /* median */ 4.0,
        /* thirdQuartile */ 4.0,
        /* upperWhisker */ 4.0,
        /* upperFence */ 4.0, trueUpperOutliers, totalUpperOutliers,
        /* maxValue */ 4.0, false);

    // FrequencyMethod
    assert_data_box_plot(
        &store, listLength,
        /* quartileRange */ 0.0,
        /* minValue */ 4.0, trueLowerOutliers, totalLowerOutliers,
        /* lowerFence */ 4.0,
        /* lowerWhisker */ 4.0,
        /* firstQuartile */ 4.0,
        /* median */ 4.0,
        /* thirdQuartile */ 4.0,
        /* upperWhisker */ 4.0,
        /* upperFence */ 4.0, trueUpperOutliers, totalUpperOutliers,
        /* maxValue */ 4.0, true);
  }

  // Empty out the store
  setStoreData(&store, {}, {}, 0, k_defaultSeriesIndex);
}

void assert_data_statistics_multiple_series_equal_to(
    Store* store, int numberOfData1, int numberOfData2, int numberOfData3,
    double maxValue, double minValue, double maxValueIgnoringFrequency,
    double minValueIgnoringFrequency) {
  quiz_assert(maxValue == store->maxValueForAllSeries(false));
  quiz_assert(maxValueIgnoringFrequency == store->maxValueForAllSeries(true));
  quiz_assert(minValue == store->minValueForAllSeries(false));
  quiz_assert(minValueIgnoringFrequency == store->minValueForAllSeries(true));
}

QUIZ_CASE(statistics_multiple_series) {
  UserPreferences userPreferences;
  Store store(&userPreferences);

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

  assert_data_statistics_multiple_series_equal_to(
      &store, listLength1, listLength2, listLength3, 5000.0, -0.001, 5000.0,
      -12.0);

  // Empty out the store
  setStoreData(&store, {}, {}, 0, 0);
  setStoreData(&store, {}, {}, 0, 1);
  setStoreData(&store, {}, {}, 0, 2);
}

QUIZ_CASE(statistics_histograms) {
  UserPreferences userPreferences;
  Store store(&userPreferences);

  constexpr int seriesIndex1 = 0;
  constexpr int listLength1 = 8;
  double v1[listLength1] = {11.97, 11.98, 12.01, 12.05,
                            12.08, 12.09, 12.11, 12.12};
  double n1[listLength1] = {1.0, 1.0, 2.0, 3.0, 1.0, 2.0, 1.0, 1.0};
  double barWidth1 = 0.01;
  double firstDrawnBarAbscissa1 = 11.97;
  constexpr int numberOfBars1 = 16;
  double barHeight1[numberOfBars1] = {1.0, 1.0, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0,
                                      3.0, 0.0, 0.0, 1.0, 2.0, 0.0, 1.0, 1.0};

  setStoreData(&store, v1, n1, listLength1, seriesIndex1);
  userPreferences.setBarWidth(barWidth1);
  userPreferences.setFirstDrawnBarAbscissa(firstDrawnBarAbscissa1);
  quiz_assert(store.numberOfBars(seriesIndex1) == numberOfBars1);
  for (int i = 0; i < numberOfBars1; i++) {
    quiz_assert(store.heightOfBarAtIndex(seriesIndex1, i) == barHeight1[i]);
  }
}

}  // namespace Statistics
