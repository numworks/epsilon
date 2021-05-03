#include "clipboard.h"

extern "C" {
#include "py/lexer.h"
}

using namespace Escher;

namespace Code {

int Clipboard::s_replacementRuleStartingPoint = 0;

Clipboard * Clipboard::sharedClipboard() {
  assert(sizeof(Clipboard) == sizeof(Escher::Clipboard));
  return static_cast<Clipboard *>(Escher::Clipboard::sharedClipboard());
}

bool Clipboard::ShouldReplaceLetterE(const char * text, int length, int position) {
  if (text[position] != 'e') {
    /* This method only exists to prevent the letter 'e' from being replaced
     * everywhere. */
    return true;
  }

  int start;
  int nextNewline = s_replacementRuleStartingPoint - 1;
  assert(nextNewline < position);
  while (nextNewline < position) {
    start = nextNewline + 1;
    nextNewline = UTF8Helper::CodePointSearch(text + start, '\n') - text;
  }
  while (text[start] == ' ') {
    start++;
  }

  mp_lexer_t * lex = mp_lexer_new_from_str_len(0, text + start, length - start, 0);
  int lastColumn = lex->tok_column;
  mp_token_kind_t lastKind = lex->tok_kind;
  while (lex->line == 1 && start + lex->tok_column - lastColumn <= position) {
    start += lex->tok_column - lastColumn;
    lastColumn = lex->tok_column;
    lastKind = lex->tok_kind;
    mp_lexer_to_next(lex);
  }
  s_replacementRuleStartingPoint = start;
  return lastKind == MP_TOKEN_FLOAT_OR_IMAG;
}

const UTF8Helper::TextPair * Clipboard::PythonTextPairs() {
  /* The order in which the text pairs are stored is important. Indeed when
   * leaving python, the text stored in the buffer is converted into an input
   * for other apps. Therefore if we want to convert "3**3" into "3^3", the
   * function must look for "**" paterns before "*". Otherwise, we will get
   * "3××3". */
  static constexpr UTF8Helper::TextPair k_pythonTextPairs[k_numberOfPythonTextPairs] = {
    UTF8Helper::TextPair("√(\x11)", "sqrt(\x11)", true),
    UTF8Helper::TextPair("ℯ^(\x11)", "exp(\x11)", true),
    UTF8Helper::TextPair("log(\x11)", "log10(\x11)", true),
    UTF8Helper::TextPair("ln(\x11)", "log(\x11)", true),
    UTF8Helper::TextPair("ᴇ", "e", false, ShouldReplaceLetterE),
    UTF8Helper::TextPair("𝐢", "1j"),
    /* Since TextPairs are also used to pair events, we need to keep both ^2
     * and ^ to get the desired behavior in python when using power or square
     * key. */
    UTF8Helper::TextPair("^2", "**2"),
    UTF8Helper::TextPair("^", "**"),
    UTF8Helper::TextPair("π", "pi"),
    UTF8Helper::TextPair("×", "*"),
    UTF8Helper::TextPair("·", "*"),
    UTF8Helper::TextPair("][", "], ["),
  };
  return k_pythonTextPairs;
}

void Clipboard::replaceCharForPython(bool entersPythonApp) {
  s_replacementRuleStartingPoint = 0;
  /* Size is maxBufferSize - 1 to ensure null termination. */
  UTF8Helper::TryAndReplacePatternsInStringByPatterns(m_textBuffer, TextField::maxBufferSize() - 1, PythonTextPairs(), k_numberOfPythonTextPairs, entersPythonApp);
}

}
