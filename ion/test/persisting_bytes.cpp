#include <quiz.h>
#include <ion/persisting_bytes.h>
#include <assert.h>

using namespace Ion;

QUIZ_CASE(ion_persisting_bytes) {
  uint8_t initialValue0 = PersistingBytes::read(0);
  uint8_t initialValue1 = PersistingBytes::read(1);
  PersistingBytes::write(255, 0);
  quiz_assert(PersistingBytes::read(0) == 255);
  PersistingBytes::write(0, 1);
  quiz_assert(PersistingBytes::read(0) == 255);
  quiz_assert(PersistingBytes::read(1) == 0);
  PersistingBytes::write(100, 0);
  quiz_assert(PersistingBytes::read(0) == 100);
  quiz_assert(PersistingBytes::read(1) == 0);
  PersistingBytes::write(111, 1);
  uint8_t value = 240;
  // TODO : Test a sector erase by writting a lot more (very slow on device)
  for (int i = 0; i < 5; i++) {
    // Writting the opposite (bitwise) byte to prevent any value overriding
    value = ~value;
    PersistingBytes::write(value, 0);
  }
  quiz_assert(PersistingBytes::read(0) == value);
  quiz_assert(PersistingBytes::read(1) == 111);
  // Restore persisting bytes
  PersistingBytes::write(initialValue0, 0);
  PersistingBytes::write(initialValue1, 1);
}
