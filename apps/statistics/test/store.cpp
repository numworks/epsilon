#include <quiz.h>
#include <assert.h>
#include <math.h>
#include <cmath>
#include "../store.h"

namespace Statistics {

void assert_value_approximately_equal_to(double d1, double d2) {
  assert((std::isnan(d1) && std::isnan(d2))
      || (std::isinf(d1) && std::isinf(d2) && d1*d2 > 0 /*same sign*/)
      || fabs(d1-d2) < 0.001);
}

void assert_data_statictics_equal_to(double n[], double v[], int numberOfData, double sumOfOccurrences, double maxValue, double minValue, double range, double mean, double variance, double standardDeviation, double sampleStandardDeviation, double firstQuartile, double thirdQuartile, double quartileRange, double median, double sum, double squaredValueSum) {
  Store store;
  int seriesIndex = 0;

  // Set the data in the store
  for (int i = 0; i < numberOfData; i++) {
    store.set(n[i], seriesIndex, 0, i);
    store.set(v[i], seriesIndex, 1, i);
  }

  // Compare the statistics
  assert_value_approximately_equal_to(standardDeviation * standardDeviation, variance);
  assert_value_approximately_equal_to(store.sumOfOccurrences(seriesIndex), sumOfOccurrences);
  assert_value_approximately_equal_to(store.maxValue(seriesIndex), maxValue);
  assert_value_approximately_equal_to(store.minValue(seriesIndex), minValue);
  assert_value_approximately_equal_to(store.range(seriesIndex), range);
  assert_value_approximately_equal_to(store.mean(seriesIndex), mean);
  assert_value_approximately_equal_to(store.variance(seriesIndex), variance);
  assert_value_approximately_equal_to(store.standardDeviation(seriesIndex), standardDeviation);
  assert_value_approximately_equal_to(store.sampleStandardDeviation(seriesIndex), sampleStandardDeviation);
  assert_value_approximately_equal_to(store.firstQuartile(seriesIndex), firstQuartile);
  assert_value_approximately_equal_to(store.thirdQuartile(seriesIndex), thirdQuartile);
  assert_value_approximately_equal_to(store.quartileRange(seriesIndex), quartileRange);
  assert_value_approximately_equal_to(store.median(seriesIndex), median);
  assert_value_approximately_equal_to(store.sum(seriesIndex), sum);
  assert_value_approximately_equal_to(store.squaredValueSum(seriesIndex), squaredValueSum);
}

QUIZ_CASE(data_statistics) {

  /* 1 2 3 4
   * 1 1 1 1 */

  constexpr int listLength1 = 4;
  double n1[listLength1] = {1.0, 2.0, 3.0, 4.0};
  double v1[listLength1] = {1.0, 1.0, 1.0, 1.0};
  assert_data_statictics_equal_to(
      n1,
      v1,
      listLength1,
      /* sumOfOccurrences */ 4.0,
      /* maxValue */ 4.0,
      /* minValue */ 1.0,
      /* range */ 3.0,
      /* mean */ 2.5,
      /* variance */ 1.25,
      /* standardDeviation */ 1.118,
      /* sampleStandardDeviation */ 1.291,
      /* firstQuartile */ 1.0,
      /* thirdQuartile */ 3.0,
      /* quartileRange */ 2.0,
      /* median */ 2.5,
      /* sum */ 10.0,
      /* squaredValueSum */ 30.0);


  /* 1 2 3 4 5 6 7 8 9 10 11
   * 1 1 1 1 1 1 1 1 1  1  1 */

  constexpr int listLength2 = 11;
  double n2[listLength2] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0};
  double v2[listLength2] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
  assert_data_statictics_equal_to(
      n2,
      v2,
      listLength2,
      /* sumOfOccurrences */ 11.0,
      /* maxValue */ 11.0,
      /* minValue */ 1.0,
      /* range */ 10.0,
      /* mean */ 6.0,
      /* variance */ 10.0,
      /* standardDeviation */ 3.1623,
      /* sampleStandardDeviation */ 3.3166,
      /* firstQuartile */ 3.0,
      /* thirdQuartile */ 9.0,
      /* quartileRange */ 6.0,
      /* median */ 6.0,
      /* sum */ 66.0,
      /* squaredValueSum */ 506.0);

  /* 1 2 3 4 5 6 7 8 9 10 11 12
   * 1 1 1 1 1 1 1 1 1  1  1  1 */

  constexpr int listLength3 = 13;
  double n3[listLength3] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0};
  double v3[listLength3] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
  assert_data_statictics_equal_to(
      n3,
      v3,
      listLength3,
      /* sumOfOccurrences */ 12.0,
      /* maxValue */ 12.0,
      /* minValue */ 1.0,
      /* range */ 11.0,
      /* mean */ 6.5,
      /* variance */ 11.917,
      /* standardDeviation */ 3.4521,
      /* sampleStandardDeviation */ 3.6056,
      /* firstQuartile */ 3.0,
      /* thirdQuartile */ 9.0,
      /* quartileRange */ 6.0,
      /* median */ 6.5,
      /* sum */ 78.0,
      /* squaredValueSum */ 650.0);

  /*   1    2   3      5     10
   * 0.2 0.05 0.3 0.0001 0.4499 */

  constexpr int listLength4 = 5;
  double n4[listLength4] = {1.0, 2.0, 3.0, 5.0, 10.0};
  double v4[listLength4] = {0.2, 0.05, 0.3, 0.0001, 0.4499};
  assert_data_statictics_equal_to(
      n4,
      v4,
      listLength4,
      /* sumOfOccurrences */ 1.0,
      /* maxValue */ 10.0,
      /* minValue */ 1.0,
      /* range */ 9.0,
      /* mean */ 5.6995,
      /* variance */ 15.6082,
      /* standardDeviation */ 3.9507,
      /* sampleStandardDeviation */ INFINITY,
      /* firstQuartile */ 2.0,
      /* thirdQuartile */ 10.0,
      /* quartileRange */ 8.0,
      /* median */ 3.0,
      /* sum */ 5.6995,
      /* squaredValueSum */ 48.0925);

  /*   1      -2   3   5  10
   * 0.4 0.00005 0.9 0.4 0.5 */

  constexpr int listLength5 = 5;
  double n5[listLength5] = {1.0, -2.0, 3.0, 5.0, 10.0};
  double v5[listLength5] = {0.4, 0.00005, 0.9, 0.4, 0.5};
  assert_data_statictics_equal_to(
      n5,
      v5,
      listLength5,
      /* sumOfOccurrences */ 2.2,
      /* maxValue */ 10.0,
      /* minValue */ -2.0,
      /* range */ 12.0,
      /* mean */ 4.5908,
      /* variance */ 10.06,
      /* standardDeviation */ 3.1719,
      /* sampleStandardDeviation */ 4.2947,
      /* firstQuartile */ 3.0,
      /* thirdQuartile */ 5.0,
      /* quartileRange */ 2.0,
      /* median */ 3.0,
      /* sum */ 10.1,
      /* squaredValueSum */ 68.500);

  /* -7 -10 12 5 -2
   *  4   5  3 1  9 */

  constexpr int listLength6 = 6;
  double n6[listLength6] = {-7.0, -10.0, 1.0, 2.0, 5.0, -2.0};
  double v6[listLength6] = {4.0, 5.0, 3.0, 0.5, 1.0, 9.0};
  assert_data_statictics_equal_to(
      n6,
      v6,
      listLength6,
      /* sumOfOccurrences */ 22.5,
      /* maxValue */ 5.0,
      /* minValue */ -10.0,
      /* range */ 15.0,
      /* mean */ -3.8667,
      /* variance */ 18.9155,
      /* standardDeviation */ 4.3492,
      /* sampleStandardDeviation */ 4.4492,
      /* firstQuartile */ -7.0,
      /* thirdQuartile */ -2.0,
      /* quartileRange */ 5.0,
      /* median */ -2.0,
      /* sum */ -87.0,
      /* squaredValueSum */ 762.0);

  /* 1 1 1 10 3 -1 3
   * 1 1 1  0 0  0 1 */

  constexpr int listLength7 = 7;
  double n7[listLength7] = {1.0, 1.0, 1.0, 10.0, 3.0, -1.0, 3.0};
  double v7[listLength7] = {1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0};
  assert_data_statictics_equal_to(
      n7,
      v7,
      listLength7,
      /* sumOfOccurrences */ 4.0,
      /* maxValue */ 3.0,
      /* minValue */ 1.0,
      /* range */ 2.0,
      /* mean */ 1.5,
      /* variance */ 0.75,
      /* standardDeviation */ 0.866,
      /* sampleStandardDeviation */ 1.0,
      /* firstQuartile */ 1.0,
      /* thirdQuartile */ 1.0,
      /* quartileRange */ 0.0,
      /* median */ 1.0,
      /* sum */ 6.0,
      /* squaredValueSum */ 12.0);

  /* 1 2 3 4
   * 0 1 0 1 */

  constexpr int listLength8 = 4;
  double n8[listLength8] = {1.0, 2.0, 3.0, 4.0};
  double v8[listLength8] = {0.0, 1.0, 0.0, 1.0};
  assert_data_statictics_equal_to(
      n8,
      v8,
      listLength8,
      /* sumOfOccurrences */ 2.0,
      /* maxValue */ 4.0,
      /* minValue */ 2.0,
      /* range */ 2.0,
      /* mean */ 3.0,
      /* variance */ 1.0,
      /* standardDeviation */ 1.0,
      /* sampleStandardDeviation */ 1.414,
      /* firstQuartile */ 2.0,
      /* thirdQuartile */ 4.0,
      /* quartileRange */ 2.0,
      /* median */ 3.0,
      /* sum */ 6.0,
      /* squaredValueSum */ 20.0);
}

}
