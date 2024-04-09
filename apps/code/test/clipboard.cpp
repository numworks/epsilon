#include "../clipboard.h"

#include <poincare/expression.h>
#include <quiz.h>
#include <string.h>

using namespace Escher;
using namespace Poincare;

namespace Code {

void assert_clipboard_enters_and_exits_python(const char* string,
                                              const char* stringResult) {
  Clipboard* clipboard = Clipboard::sharedClipboard();
  clipboard->store(string);
  clipboard->enterPython();
  quiz_assert(strcmp(clipboard->storedText(), stringResult) == 0);
  clipboard->exitPython();
  quiz_assert(strcmp(clipboard->storedText(), string) == 0);
}

QUIZ_CASE(code_clipboard_enters_and_exits_python) {
  assert_clipboard_enters_and_exits_python("4×4", "4*4");
  assert_clipboard_enters_and_exits_python("e^\u00121+2\u0013",
                                           "exp\u00121+2\u0013");
  assert_clipboard_enters_and_exits_python("e^(ln(4))", "exp(log(4))");
  assert_clipboard_enters_and_exits_python("√(1ᴇ10)", "sqrt(1e10)");
  assert_clipboard_enters_and_exits_python("12^(1/4)×(π/6)×(12×π)^(1/4)",
                                           "12**(1/4)*(pi/6)*(12*pi)**(1/4)");
  /* This fails, as 1+e^x -> 1+expx, but should never occur : e^ should always
   * have its exponent in parentheses.
  assert_clipboard_enters_and_exits_python("1+e^x", "1+exp(x)");
   */
  Expression e = Expression::Parse("1+e^x", nullptr);
  char buffer[32];
  e.serialize(buffer, sizeof(buffer));
  assert_clipboard_enters_and_exits_python(buffer, "1+exp\u0012x\u0013");

  /* The character 'e' should only be changed to 'ᴇ' if it is part of a
   * floating-point number. */
  assert_clipboard_enters_and_exits_python("1ᴇ2", "1e2");
  assert_clipboard_enters_and_exits_python("123ᴇ4", "123e4");
  assert_clipboard_enters_and_exits_python("someVariableName",
                                           "someVariableName");
  assert_clipboard_enters_and_exits_python("leg+1.23ᴇ-4.56", "leg+1.23e-4.56");
  assert_clipboard_enters_and_exits_python("ceil(x)", "ceil(x)");
  assert_clipboard_enters_and_exits_python(
      "for i in range(1ᴇ2):\n  res[i] = 1ᴇ-1 × i",
      "for i in range(1e2):\n  res[i] = 1e-1 * i");
  /* Since ᴇ takes more space than e, we cannot convert as it would overflow
   * the buffer. */
  assert_clipboard_enters_and_exits_python(
      "e+1e2+e+1e2+e+1e2+e+1e2+e+1e2+e+1e2+e+1e2+e+1e2+e+1e2+e+1e2+e+1e2+e+1e2+"
      "e+1e2+\ne+1e2+e+1e2+e+1e2+e+1e2+e+1e2+e+1e2+e+1e2+e+1e2+e+1e2+\ne+1e2+e+"
      "1e2+e+1e2+e+1e2+e+1e2+e+1e2+e+1e2+e+1e2+\ne+1e2+e+1e2+e+1e2+e+1e2+e+1e2+"
      "exec()",
      "e+1e2+e+1e2+e+1e2+e+1e2+e+1e2+e+1e2+e+1e2+e+1e2+e+1e2+e+1e2+e+1e2+e+1e2+"
      "e+1e2+\ne+1e2+e+1e2+e+1e2+e+1e2+e+1e2+e+1e2+e+1e2+e+1e2+e+1e2+\ne+1e2+e+"
      "1e2+e+1e2+e+1e2+e+1e2+e+1e2+e+1e2+e+1e2+\ne+1e2+e+1e2+e+1e2+e+1e2+e+1e2+"
      "exec()");
}

}  // namespace Code
