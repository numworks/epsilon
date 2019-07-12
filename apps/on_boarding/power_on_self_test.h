#ifndef APPS_POWER_ON_SELF_TEST_H
#define APPS_POWER_ON_SELF_TEST_H

#include <kandinsky/color.h>

namespace OnBoarding {

class PowerOnSelfTest {
public:
  /* Performs self tests, lights up the LED to indicate the tests results and
   * returns the LED color previous to the tests. */
  static KDColor Perform();
private:
  constexpr static int k_textErrorsLimit = 0;
};

}

#endif

