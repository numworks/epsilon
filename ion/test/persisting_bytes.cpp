#include <quiz.h>
#include <ion/persisting_bytes.h>
#include <assert.h>
#include <poincare/print.h>
#include <poincare/test/helper.h>

using namespace Ion;

void testPersistingByte(PersistingBytes::PersistingBytesInt expectedValue) {
  PersistingBytes::PersistingBytesInt observedValue = PersistingBytes::read();
  if (observedValue != expectedValue) {
    constexpr size_t bufferSize = sizeof("255 instead of 255.");
    char buffer[bufferSize];
    Poincare::Print::CustomPrintf(buffer, bufferSize, "%i instead of %i.", observedValue, expectedValue);
    quiz_assert_print_if_failure(false, buffer);
  }
}

/* This is one of the first executed tests. A shaky press on the RESET button
 * may reset the device in the middle of this test, and leave the persisting
 * bytes in a corrupted state. As a result, we systematically reset the
 * persisting bytes to 0 here. Also, one must be mindful of the persiting bytes
 * values that are tested here :
 * - 0xFF is a very invalid value to set as exam mode at any moment.
 * - Leaving an invalid value could make an unrelated test crash. Because we
 *   assert that active examMode is < ExamMode::Undefined, and that active
 *   pressToTest is null in most examModes.
 * - Leaving a non-zero value could make an unrelated test fail. Because most
 *   tests assume examMode is Off. */

QUIZ_CASE(ion_persisting_bytes) {
  // Reset the persisting bytes
  PersistingBytes::write(0);
  constexpr uint8_t k_numberOfTests = 10;
  uint8_t pressToTest[k_numberOfTests] = {0,0,0,0,255,1,2,0,0,0};
  uint8_t examModes[k_numberOfTests] = {0,1,2,3,4,4,4,4,5,0};
  // TODO : Test a sector erase by writting a lot more (very slow on device)
  for (uint8_t i = 1; i < k_numberOfTests; i++) {
    PersistingBytes::PersistingBytesInt value = pressToTest[i - 1] | (examModes[i - 1] << 8);
    testPersistingByte(value);
    value = pressToTest[i] | (examModes[i] << 8);
    PersistingBytes::write(value);
  }
  testPersistingByte(0);
}
