#include "clipboard.h"

extern "C" {
#include "py/lexer.h"
#include "py/nlr.h"
}

using namespace Escher;

namespace Code {

int Clipboard::s_replacementRuleStartingPoint = 0;

Clipboard * Clipboard::sharedClipboard() {
  assert(sizeof(Clipboard) == sizeof(Escher::Clipboard));
  return static_cast<Clipboard *>(Escher::Clipboard::SharedClipboard());
}

bool Clipboard::ShouldReplaceLetterE(const char * text, size_t length, size_t position) {
  if (text[position] != 'e') {
    /* This method only exists to prevent the letter 'e' from being replaced
     * everywhere. */
    return true;
  }

  size_t start;
  size_t nextLineStart = s_replacementRuleStartingPoint;
  assert(nextLineStart <= position);
  do {
    start = nextLineStart;
    nextLineStart = UTF8Helper::CodePointSearch(text + start, '\n') - text + 1;
  } while (nextLineStart <= position);
  while (text[start] == ' ') {
    start++;
  }

  nlr_buf_t nlr;
  if (nlr_push(&nlr) == 0) {
    mp_lexer_t * lex = mp_lexer_new_from_str_len(0, text + start, length - start, 0);
    size_t lastColumn = lex->tok_column;
    mp_token_kind_t lastKind = lex->tok_kind;
    while (lex->line == 1 && start + lex->tok_column - lastColumn <= position) {
      start += lex->tok_column - lastColumn;
      lastColumn = lex->tok_column;
      lastKind = lex->tok_kind;
      mp_lexer_to_next(lex);
    }
    s_replacementRuleStartingPoint = start;
    mp_lexer_free(lex);
    nlr_pop();
    return lastKind == MP_TOKEN_FLOAT_OR_IMAG;
  } else {
    /* Lexer encountered an exception. */
    return false;
  }
}

const UTF8Helper::TextPair * Clipboard::PythonTextPairs() {
  /* The order in which the text pairs are stored is important. Indeed when
   * leaving python, the text stored in the buffer is converted into an input
   * for other apps. Therefore if we want to convert "3**3" into "3^3", the
   * function must look for "**" paterns before "*". Otherwise, we will get
   * "3××3". */
  constexpr static UTF8Helper::TextPair k_pythonTextPairs[k_numberOfPythonTextPairs] = {
    UTF8Helper::TextPair("√(\x11)", "sqrt(\x11)", true),
    UTF8Helper::TextPair("e^(\x11)", "exp(\x11)", true),
    UTF8Helper::TextPair("log(\x11)", "log10(\x11)", true),
    UTF8Helper::TextPair("ln(\x11)", "log(\x11)", true),
    UTF8Helper::TextPair("ᴇ", "e", false, ShouldReplaceLetterE),
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
