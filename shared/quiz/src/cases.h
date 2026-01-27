#include <quiz.h>

namespace Quiz {

class Cases {
 public:
  static constexpr int k_maxNumber = 1024;

  static int s_number;
  static const char* s_names[k_maxNumber];
  static TestCase s_bodies[k_maxNumber];

  static void Sort();
};

}  // namespace Quiz
