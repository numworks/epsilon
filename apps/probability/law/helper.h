#ifndef PROBABILITY_LAW_HELPER_H
#define PROBABILITY_LAW_HELPER_H

class Helper {
public:
  // Continued fractions
  typedef double (*ValueForIndex)(double index, double s, double x);
  static bool ContinuedFractionEvaluation(ValueForIndex a, ValueForIndex b, int maxNumberOfIterations, double * result, double context1, double context2);

  // Infinite series
  typedef double (*TermUpdater)(double previousTerm, double index, double s, double x);
  static bool InfiniteSeriesEvaluation(double firstTerm, TermUpdater termUpdater, double termLimit, int maxNumberOfIterations, double * result, double context1, double context2);

};

#endif

