#ifndef SCORE_H
#define SCORE_H

#include "display.h"
#include "eadkpp.h"

class Score {
public:
  Score() : m_value(0) {}
  void draw() const;
  void increment();
private:
  constexpr static int k_maximalScore = 9999;
  constexpr static int k_maximalScoreNumberOfDigits = 4; // log(k_maximalScore, 10)
  int m_value;
};

#endif

