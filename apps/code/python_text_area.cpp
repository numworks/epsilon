#include "python_text_area.h"
#include "app.h"
#include <escher/palette.h>
#include <ion/unicode/utf8_helper.h>
#include <python/port/port.h>

extern "C" {
#include "py/nlr.h"
#include "py/lexer.h"
}
#include <stdlib.h>
#include <algorithm>

namespace Code {

constexpr KDColor CommentColor = KDColor::RGB24(0x999988);
constexpr KDColor NumberColor =  KDColor::RGB24(0x009999);
constexpr KDColor KeywordColor = KDColor::RGB24(0xFF000C);
// constexpr KDColor BuiltinColor = KDColor::RGB24(0x0086B3);
constexpr KDColor OperatorColor = KDColor::RGB24(0xd73a49);
constexpr KDColor StringColor = KDColor::RGB24(0x032f62);
constexpr KDColor AutocompleteColor = KDColorRed;
constexpr KDColor BackgroundColor = KDColorWhite;
constexpr KDColor HighlightColor = Palette::Select;

static inline KDColor TokenColor(mp_token_kind_t tokenKind) {
  if (tokenKind == MP_TOKEN_STRING) {
    return StringColor;
  }
  if (tokenKind == MP_TOKEN_INTEGER || tokenKind == MP_TOKEN_FLOAT_OR_IMAG) {
    return NumberColor;
  }
  if (tokenKind >= MP_TOKEN_KW_FALSE && tokenKind <= MP_TOKEN_KW_YIELD) {
    return KeywordColor;
  }
  if (tokenKind >= MP_TOKEN_OP_PLUS && tokenKind <= MP_TOKEN_OP_NOT_EQUAL) {
    return OperatorColor;
  }
  if (tokenKind >= MP_TOKEN_DEL_EQUAL && tokenKind <= MP_TOKEN_DEL_MINUS_MORE) {
    return OperatorColor;
  }
  return KDColorBlack;
}

static inline size_t TokenLength(mp_lexer_t * lex, const char * tokenPosition) {
  /* The lexer stores the beginning of the current token and of the next token,
   * so we just use that. */
  if (lex->line > 1) {
    /* The next token is on the next line, so we cannot just make the difference
     * of the columns. */
    return UTF8Helper::CodePointSearch(tokenPosition, '\n') - tokenPosition;
  }
  return lex->column - lex->tok_column;
}

const char * PythonTextArea::ContentView::textToAutocomplete() const {
  return UTF8Helper::BeginningOfWord(editedText(), cursorLocation());
}

void PythonTextArea::ContentView::loadSyntaxHighlighter() {
  m_pythonDelegate->initPythonWithUser(this);
}

void PythonTextArea::ContentView::unloadSyntaxHighlighter() {
  m_pythonDelegate->deinitPython();
}

void PythonTextArea::ContentView::clearRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, BackgroundColor);
}

#define LOG_DRAWING 0
#if LOG_DRAWING
#include <stdio.h>
#define LOG_DRAW(...) printf(__VA_ARGS__)
#else
#define LOG_DRAW(...)
#endif

void PythonTextArea::ContentView::drawLine(KDContext * ctx, int line, const char * text, size_t byteLength, int fromColumn, int toColumn, const char * selectionStart, const char * selectionEnd) const {
  LOG_DRAW("Drawing \"%.*s\"\n", byteLength, text);

  assert(m_pythonDelegate->isPythonUser(this));

  /* We're using the MicroPython lexer to do syntax highlighting on a per-line
   * basis. This can work, however the MicroPython lexer won't accept a line
   * starting with a whitespace. So we're discarding leading whitespaces
   * beforehand. */
  const char * firstNonSpace = UTF8Helper::NotCodePointSearch(text, ' ');
  if (firstNonSpace != text) {
    // Color the discarded leading whitespaces
    const char * spacesStart = UTF8Helper::CodePointAtGlyphOffset(text, fromColumn);
    drawStringAt(
        ctx,
        line,
        fromColumn,
        spacesStart,
        std::min(text + byteLength, firstNonSpace) - spacesStart,
        StringColor,
        BackgroundColor,
        selectionStart,
        selectionEnd,
        HighlightColor);
  }
  if (UTF8Helper::CodePointIs(firstNonSpace, UCodePointNull)) {
    return;
  }

  nlr_buf_t nlr;
  if (nlr_push(&nlr) == 0) {
    mp_lexer_t * lex = mp_lexer_new_from_str_len(0, firstNonSpace, byteLength - (firstNonSpace - text), 0);
    LOG_DRAW("Pop token %d\n", lex->tok_kind);

    const char * tokenFrom = firstNonSpace;
    size_t tokenLength = 0;
    const char * tokenEnd = firstNonSpace;
    while (lex->tok_kind != MP_TOKEN_NEWLINE && lex->tok_kind != MP_TOKEN_END) {
      tokenFrom = firstNonSpace + lex->tok_column - 1;
      if (tokenFrom != tokenEnd) {
        // We passed over white spaces, we need to color them
        drawStringAt(
            ctx,
            line,
            UTF8Helper::GlyphOffsetAtCodePoint(text, tokenEnd),
            tokenEnd,
            std::min(text + byteLength, tokenFrom) - tokenEnd,
            StringColor,
            BackgroundColor,
            selectionStart,
            selectionEnd,
            HighlightColor);
      }
      tokenLength = TokenLength(lex, tokenFrom);
      tokenEnd = tokenFrom + tokenLength;
      LOG_DRAW("Draw \"%.*s\" for token %d\n", tokenLength, tokenFrom, lex->tok_kind);
      drawStringAt(ctx, line,
        UTF8Helper::GlyphOffsetAtCodePoint(text, tokenFrom),
        tokenFrom,
        tokenLength,
        TokenColor(lex->tok_kind),
        BackgroundColor,
        selectionStart,
        selectionEnd,
        HighlightColor);

      mp_lexer_to_next(lex);
      LOG_DRAW("Pop token %d\n", lex->tok_kind);
    }

    tokenFrom += tokenLength;

    if (tokenFrom < text + byteLength) {
      LOG_DRAW("Draw comment \"%.*s\" from %d\n", byteLength - (tokenFrom - text), firstNonSpace, tokenFrom);
      drawStringAt(ctx, line,
          UTF8Helper::GlyphOffsetAtCodePoint(text, tokenFrom),
          tokenFrom,
          text + byteLength - tokenFrom,
          CommentColor,
          BackgroundColor,
          selectionStart,
          selectionEnd,
          HighlightColor);
    }

    mp_lexer_free(lex);
    nlr_pop();
  }

  // Redraw the autocompleted word in the right color
  const char * autocompleteStart = m_cursorLocation;
  assert(!m_autocomplete || autocompleteStart != text);
  if (m_autocomplete && autocompleteStart > text && autocompleteStart < text + byteLength) {
    const char * autocompleteEnd = UTF8Helper::EndOfWord(autocompleteStart);
    drawStringAt(
        ctx,
        line,
        UTF8Helper::GlyphOffsetAtCodePoint(text, autocompleteStart),
        autocompleteStart,
        minPointer(text + byteLength, autocompleteEnd) - autocompleteStart,
        AutocompleteColor,
        BackgroundColor,
        nullptr,
        nullptr,
        HighlightColor);
  }
}

KDRect PythonTextArea::ContentView::dirtyRectFromPosition(const char * position, bool includeFollowingLines) const {
  /* Mark the whole line as dirty.
   * TextArea has a very conservative approach and only dirties the surroundings
   * of the current character. That works for plain text, but when doing syntax
   * highlighting, you may want to redraw the surroundings as well. For example,
   * if editing "def foo" into "df foo", you'll want to redraw "df". */
  KDRect baseDirtyRect = TextArea::ContentView::dirtyRectFromPosition(position, includeFollowingLines);
  return KDRect(
    bounds().x(),
    baseDirtyRect.y(),
    bounds().width(),
    baseDirtyRect.height()
  );
}

bool PythonTextArea::handleEvent(Ion::Events::Event event) {
  if (m_contentView.isAutocompleting()) {
    // Handle event with autocompletion
    if (event == Ion::Events::Toolbox || event == Ion::Events::Var) {
    } else if (event == Ion::Events::Right
        || event == Ion::Events::ShiftRight
        || event == Ion::Events::OK)
    {
      m_contentView.reloadRectFromPosition(m_contentView.cursorLocation(), false);
      acceptAutocompletion(event != Ion::Events::ShiftRight);
      if (event != Ion::Events::ShiftRight) {
        // Do not process the event more
        scrollToCursor();
        return true;
      }
    } else {
      removeAutocompletion();
      m_contentView.reloadRectFromPosition(m_contentView.cursorLocation(), false);
    }
  }
  return TextArea::handleEvent(event);
}

bool PythonTextArea::handleEventWithText(const char * text, bool indentation, bool forceCursorRightOfText) {
  if (*text == 0) {
    return false;
  }
  if (m_contentView.isAutocompleting()) {
    removeAutocompletion();
  }
  bool result = TextArea::handleEventWithText(text, indentation, forceCursorRightOfText);
  addAutocompletion();
  return result;
}

const char * PythonTextArea::textToAutocomplete() const {
  if (!m_contentView.isAutocompleting()) {
    return nullptr;
  }
  return m_contentView.textToAutocomplete();
}

void PythonTextArea::removeAutocompletion() {
  assert(m_contentView.isAutocompleting());
  const char * autocompleteStart = m_contentView.cursorLocation();
  const char * autocompleteEnd = UTF8Helper::EndOfWord(autocompleteStart);
  assert(autocompleteEnd > autocompleteStart);
  m_contentView.removeText(autocompleteStart, autocompleteEnd);
  m_contentView.setAutocompleting(false);
}

void PythonTextArea::addAutocompletion() {
  assert(!m_contentView.isAutocompleting());

  const char * autocompletionLocation = const_cast<char *>(cursorLocation());
  const char * textToInsert = nullptr;
  CodePoint prevCodePoint = UTF8Helper::PreviousCodePoint(m_contentView.editedText(), autocompletionLocation);
  if (!UTF8Helper::CodePointIsEndOfWord(prevCodePoint)
      && UTF8Helper::CodePointIsEndOfWord(UTF8Helper::CodePointAtLocation(autocompletionLocation)))
  {
    /* The previous code point is neither the beginning of the text, nor a
     * space, nor a \n, and the next code point is the end of the word.
     * Compute the text to insert:
     * Look first in the current script variables and functions, then in the
     * builtins, then in the imported modules/scripts. */
    VariableBoxController * varBox = m_contentView.pythonDelegate()->variableBoxController();
    const char * beginningOfWord = m_contentView.textToAutocomplete();
    textToInsert = varBox->autocompletionForText(m_contentView.pythonDelegate()->menuController()->editedScriptIndex(), beginningOfWord);
  }

  // Try to insert the text (this might fail if the buffer is full)
  if (textToInsert && m_contentView.insertTextAtLocation(textToInsert, const_cast<char *>(autocompletionLocation))) {
    m_contentView.setAutocompleting(true);
  }
}

void PythonTextArea::acceptAutocompletion(bool moveCursorToEndOfAutocompletion) {
  assert(m_contentView.isAutocompleting());
  m_contentView.setAutocompleting(false);
  if (moveCursorToEndOfAutocompletion) {
    const char * autocompleteEnd = UTF8Helper::EndOfWord(m_contentView.cursorLocation());
    setCursorLocation(autocompleteEnd);
    addAutocompletion();
  }
}

}
