#include <escher/clipboard.h>
#include <poincare/expression.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>
#include <quiz.h>
#include <string.h>

using namespace Escher;
using namespace Poincare;

void assert_stored_text_is_parseable(Layout layout) {
  constexpr int bufferSize = 500;
  char buffer[bufferSize];
  layout.serializeForParsing(buffer, bufferSize);
  Clipboard* clipboard = Clipboard::SharedClipboard();
  clipboard->store(buffer);
  Expression e = Expression::Parse(clipboard->storedText(), nullptr, false);
  Layout result =
      e.createLayout(Preferences::SharedPreferences()->displayMode(),
                     PrintFloat::k_maxNumberOfSignificantDigits, nullptr);
  quiz_assert(layout.isIdenticalTo(result));
}

QUIZ_CASE(escher_clipboard_stored_text_is_parseable) {
  Layout l = KRackL(KIntegralL("x"_l, KRackL(), KRackL(), KRackL()));
  assert_stored_text_is_parseable(l);
  l = KRackL(KNthSqrtL(KRackL(), KRackL()));
  assert_stored_text_is_parseable(l);
  l = KRackL(KEmptyMatrixL);
  assert_stored_text_is_parseable(l);
  l = KRackL(KSumL("n"_l, KRackL(), KRackL(), KRackL()));
  assert_stored_text_is_parseable(l);
  l = KRackL(KProductL("n"_l, KRackL(), KRackL(), KRackL()));
  assert_stored_text_is_parseable(l);
}
