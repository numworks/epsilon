#include <quiz.h>

#include "probability/helpers.h"
namespace Probability {

QUIZ_CASE(probability_distribution_array_max) {
  constexpr int array[] = {1, 12, 156, 432, 53, 1};
  int m = arrayMax(array);
  quiz_assert(m == 432);
}

}  // namespace Probability