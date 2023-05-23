#include <ion/assert.h>
#include <ion/console.h>
#include <ion/events.h>
#include <kandinsky/ion_context.h>

static bool s_preventAssertions = false;

/* Ad hoc functions to avoid requiring Poincare::PrintInt, which also needs a
 * sizeable part of liba. */
static size_t numberOfDigits(int x) {
  return x < 10 ? 1 : 1 + numberOfDigits(x / 10);
}

static int printInt(int x, char* buffer, size_t bufferSize) {
  size_t n = numberOfDigits(x);
  if (n > bufferSize) {
    return 0;
  }
  for (size_t i = 0; i < n; i++) {
    buffer[n - i - 1] = '0' + (x % 10);
    x /= 10;
  }
  return n;
}

void assertionAbort(const char* expression, const char* file, int line) {
  if (s_preventAssertions) {
    // An assertion was raised during an assertion.
    return;
  }
  /* Do not trigger assertions in writeLine. This can happen if the error
   * message contains an unknown char */
  s_preventAssertions = true;
  Ion::Events::setSpinner(false);
  KDIonContext::Clear();

  Ion::Console::writeLine("ASSERTION FAILED x_x");
  Ion::Console::writeLine(expression);
  Ion::Console::writeLine(file);

  constexpr size_t bufferSize = 20;
  char buffer[bufferSize] = {'l', 'i', 'n', 'e', ' '};
  int length = 5;
  length += printInt(line, buffer + length, bufferSize - length);
  buffer[length] = '\0';
  Ion::Console::writeLine(buffer);
  s_preventAssertions = false;

  while (true)
    ;
}
