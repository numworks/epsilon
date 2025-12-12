#include <omg/algorithm.h>
#include <omg/vector.h>
#include <quiz.h>

#include <array>

using namespace OMG;

QUIZ_CASE(omg_algorithm_all_equal) {
  std::array<int, 3> case1 = {1, 1, 1};
  quiz_assert(OMG::AllEqual(case1.begin(), case1.end()));
  quiz_assert(OMG::AllEqual(case1.begin(), case1.begin() + 1));

  OMG::StaticVector<double, 10> case2 = {1.5, 1.5, 2.5};
  quiz_assert(!OMG::AllEqual(case2.begin(), case2.end()));
  quiz_assert(OMG::AllEqual(case2.begin(), case2.end() - 1));

  OMG::StaticVector<char, 10> case3 = {};
  quiz_assert(OMG::AllEqual(case3.begin(), case3.end()));
}
