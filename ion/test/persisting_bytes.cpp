#include <quiz.h>
#include <ion/persisting_bytes.h>
#include <assert.h>
#include <poincare/print.h>
#include <poincare/test/helper.h>

using namespace Ion;

void testPersistingByte(uint8_t index, uint8_t expectedValue) {
  uint8_t observedValue = PersistingBytes::read(index);
  if (observedValue != expectedValue) {
    constexpr size_t bufferSize = sizeof("Persisting byte at 255 : Read 255 instead of 255.");
    char buffer[bufferSize];
    Poincare::Print::customPrintf(buffer, bufferSize, "Persisting byte at %i : Read %i instead of %i.", index, observedValue, expectedValue);
    quiz_assert_print_if_failure(false, buffer);
  }
}

/* This is one of the first executed tests. A shaky press on the RESET button
 * may reset the device in the middle of this test, and leave the persisting
 * bytes in a corrupted state. As a result, we systematically reset the
 * persisting bytes to 0 here. Also, one must be mindful of the persiting bytes
 * values that are tested here :
 * - 0xFF is an invalid value to set.
 * - Leaving an invalid value could make an unrelated test crash. Because we
 *   assert that active examMode is < ExamMode::Undefined, and that active
 *   pressToTest has it m_isUnknown member false and is null in most examModes.
 * - Leaving a non-zero value could make an unrelated test fail. Because most
 *   tests assume examMode is Off. */

QUIZ_CASE(ion_persisting_bytes) {
  // Reset the persisting bytes
  PersistingBytes::write(0, 0);
  PersistingBytes::write(0, 1);
  constexpr uint8_t k_numberOfTests = 10;
  uint8_t examModes[k_numberOfTests] = {0,1,2,3,4,4,4,4,5,0};
  uint8_t pressToTest[k_numberOfTests] = {0,0,0,0,127,1,2,0,0,0};
  // TODO : Test a sector erase by writting a lot more (very slow on device)
  for (uint8_t i = 1; i < k_numberOfTests; i++) {
    if (examModes[i] != examModes[i-1]) {
      testPersistingByte(0, examModes[i-1]);
      PersistingBytes::write(examModes[i], 0);
    }
    if (pressToTest[i] != pressToTest[i-1]) {
      testPersistingByte(1, pressToTest[i-1]);
      PersistingBytes::write(pressToTest[i], 1);
    }
  }
  testPersistingByte(0, 0);
  testPersistingByte(1, 0);
}
