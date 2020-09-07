#include <quiz.h>
#include <string.h>
#include <poincare_layouts.h>
#include <poincare/expression.h>
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

using namespace Poincare;

void assert_stored_text_is_parseable(Poincare::Layout layout) {
  constexpr int bufferSize = 500;
  char buffer[bufferSize];
  layout.serializeForParsing(buffer, bufferSize);
  Clipboard * clipboard = Clipboard::sharedClipboard();
  clipboard->store(buffer);
  Expression e = Expression::Parse(clipboard->storedText(), nullptr, false);
  Layout result = e.createLayout(Preferences::sharedPreferences()->displayMode(), Poincare::PrintFloat::k_numberOfStoredSignificantDigits);
  quiz_assert(layout.isIdenticalTo(result));
}

QUIZ_CASE(escher_clipboard_stored_text_is_parseable) {
  Layout l = IntegralLayout::Builder(EmptyLayout::Builder(), CodePointLayout::Builder('x'), EmptyLayout::Builder(), EmptyLayout::Builder());
  assert_stored_text_is_parseable(l);
  l = NthRootLayout::Builder(EmptyLayout::Builder());
  assert_stored_text_is_parseable(l);
  l = MatrixLayout::Builder(CodePointLayout::Builder('1'), EmptyLayout::Builder(), EmptyLayout::Builder(), CodePointLayout::Builder('2'));
  assert_stored_text_is_parseable(l);
  l = SumLayout::Builder(EmptyLayout::Builder(), CodePointLayout::Builder('n'), EmptyLayout::Builder(), EmptyLayout::Builder());
  assert_stored_text_is_parseable(l);
  l = SumLayout::Builder(EmptyLayout::Builder(), CodePointLayout::Builder('n'), EmptyLayout::Builder(), EmptyLayout::Builder());
  assert_stored_text_is_parseable(l);;
}