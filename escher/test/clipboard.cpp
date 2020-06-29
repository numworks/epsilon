#include <quiz.h>
#include <string.h>
#include <escher/clipboard.h>

void assert_clipboard_enters_and_exits_python(const char * string, const char * stringResult) {
  Clipboard * clipboard = Clipboard::sharedClipboard();
  clipboard->store(string);
  clipboard->enterPython();
  quiz_assert(strcmp(clipboard->storedText(), stringResult) == 0);
  clipboard->exitPython();
  quiz_assert(strcmp(clipboard->storedText(), string) == 0);
}

QUIZ_CASE(escher_clipboard_enters_and_exits_python) {
  assert_clipboard_enters_and_exits_python("4×4", "4*4");
  assert_clipboard_enters_and_exits_python("ℯ^(ln(4))", "exp(log(4))");
  assert_clipboard_enters_and_exits_python("ln(log(ln(π)))^𝐢", "log(log10(log(pi)))**1j");
  assert_clipboard_enters_and_exits_python("√(1ᴇ10)", "sqrt(1e10)");
  assert_clipboard_enters_and_exits_python("1×𝐢^2", "1*1j**2");
  assert_clipboard_enters_and_exits_python("12^(1/4)×(π/6)×(12×π)^(1/4)", "12**(1/4)*(pi/6)*(12*pi)**(1/4)");
}
