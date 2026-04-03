#include <quiz.h>
#include <string.h>

#include <algorithm>
#include <array>

bool StringLengthComparator(const char* s1, const char* s2) {
  return strlen(s1) < strlen(s2);
}

QUIZ_CASE(libaxx_min_max) {
  quiz_assert(std::min(1, 2) == 1);
  quiz_assert(std::max(-1.2, -3.4) == -1.2);
  quiz_assert(std::min({'h', 'e', 'l', 'l', 'o'}) == 'e');
  quiz_assert(std::max({1, 2, 4, 5, 4, 5}) == 5);

  std::array<int, 4> intArray = {71, 22, 83, 14};
  quiz_assert(*std::min_element(intArray.begin(), intArray.end()) == 14);
  quiz_assert(*std::max_element(intArray.begin(), intArray.end()) == 83);

  std::array<const char*, 3> stringArray = {"hello", "hi", "good morning"};
  quiz_assert(strcmp(*std::min_element(stringArray.begin(), stringArray.end(),
                                       StringLengthComparator),
                     "hi") == 0);
  quiz_assert(strcmp(*std::max_element(stringArray.begin(), stringArray.end(),
                                       StringLengthComparator),
                     "good morning") == 0);
}

// TODO: test other libaxx algorithms
