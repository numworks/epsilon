#include <quiz.h>
#include <ion/persisting_bytes.h>
#include <assert.h>

using namespace Ion;

QUIZ_CASE(ion_persisting_bytes) {
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
  /* Persisting bytes might take up to 64k on N0110. With 2 persisting bytes,
   * writing more than 32k time should trigger an erase of the sector. */
  for (int i = 0; i < 33000; i++) {
    // Writting the opposite (bitwise) byte to prevent any value overriding
    value = ~value;
    PersistingBytes::write(value, 0);
  }
  quiz_assert(PersistingBytes::read(0) == value);
  quiz_assert(PersistingBytes::read(1) == 111);
}