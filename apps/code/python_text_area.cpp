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
constexpr KDColor AutocompleteColor = KDColor::RGB24(0xC6C6C6);
constexpr KDColor BackgroundColor = KDColorWhite;
constexpr KDColor HighlightColor = Palette::Select;
constexpr KDColor DefaultColor = KDColorBlack;

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
  return DefaultColor;
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

PythonTextArea::AutocompletionType PythonTextArea::autocompletionType(const char * autocompletionLocation, const char ** autocompletionLocationBeginning, const char ** autocompletionLocationEnd) const {
  const char * location = autocompletionLocation != nullptr ? autocompletionLocation : cursorLocation();
  const char * beginningOfToken = nullptr;

  // We want to autocomplete only at the end of an identifier or a keyword
  AutocompletionType autocompleteType = AutocompletionType::NoIdentifier;
  nlr_buf_t nlr;
  if (nlr_push(&nlr) == 0) {
    const char * firstNonSpace = UTF8Helper::BeginningOfWord(m_contentView.editedText(), location);
    mp_lexer_t * lex = mp_lexer_new_from_str_len(0, firstNonSpace, UTF8Helper::EndOfWord(location) - firstNonSpace, 0);

    const char * tokenStart;
    const char * tokenEnd;
    _mp_token_kind_t currentTokenKind = lex->tok_kind;

    while (currentTokenKind != MP_TOKEN_NEWLINE && currentTokenKind != MP_TOKEN_END) {
      tokenStart = firstNonSpace + lex->tok_column - 1;
      tokenEnd = tokenStart + TokenLength(lex, tokenStart);

      if (location < tokenStart) {
        // The location for autocompletion is not in an identifier
        break;
      }
      if (currentTokenKind == MP_TOKEN_NAME
          || (currentTokenKind >= MP_TOKEN_KW_FALSE
            && currentTokenKind <= MP_TOKEN_KW_YIELD))
      {
        if (location < tokenEnd) {
          // The location for autocompletion is in the middle of an identifier
          autocompleteType = AutocompletionType::MiddleOfIdentifier;
          break;
        }
        if (location == tokenEnd) {
          // The location for autocompletion is at the end of an identifier
          beginningOfToken = tokenStart;
          autocompleteType = AutocompletionType::EndOfIdentifier;
          break;
        }
      }
      if (location < tokenStart) {
        // The location for autocompletion is not in an identifier
        break;
      }
      mp_lexer_to_next(lex);
      currentTokenKind = lex->tok_kind;
    }
    mp_lexer_free(lex);
    nlr_pop();
  }
  if (autocompletionLocationBeginning != nullptr) {
    *autocompletionLocationBeginning = beginningOfToken;
  }
  if (autocompletionLocationEnd != nullptr) {
    *autocompletionLocationEnd = location;
  }
  assert(!isAutocompleting() || autocompleteType == AutocompletionType::EndOfIdentifier);
  return autocompleteType;
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

  const char * autocompleteStart = m_autocomplete ? m_cursorLocation : nullptr;

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

      // If the token is being autocompleted, use DefaultColor
      KDColor color = (tokenFrom <= autocompleteStart && autocompleteStart < tokenEnd) ? DefaultColor : TokenColor(lex->tok_kind);

      LOG_DRAW("Draw \"%.*s\" for token %d\n", tokenLength, tokenFrom, lex->tok_kind);
      drawStringAt(ctx, line,
        UTF8Helper::GlyphOffsetAtCodePoint(text, tokenFrom),
        tokenFrom,
        tokenLength,
        color,
        BackgroundColor,
        selectionStart,
        selectionEnd,
        HighlightColor);

      mp_lexer_to_next(lex);
      LOG_DRAW("Pop token %d\n", lex->tok_kind);
    }

    tokenFrom += tokenLength;

    // Even if the token is being autocompleted, use CommentColor
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
  if (m_autocomplete && autocompleteStart >= text && autocompleteStart < text + byteLength) {
    assert(m_autocompletionEnd != nullptr && m_autocompletionEnd > autocompleteStart);
    drawStringAt(
        ctx,
        line,
        UTF8Helper::GlyphOffsetAtCodePoint(text, autocompleteStart),
        autocompleteStart,
        std::min(text + byteLength, m_autocompletionEnd) - autocompleteStart,
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
    if (event == Ion::Events::Right
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
    } else if (event == Ion::Events::Toolbox
        || event == Ion::Events::Var
        || event == Ion::Events::Shift
        || event == Ion::Events::Alpha
        || event == Ion::Events::OnOff)
    {
    } else if(event == Ion::Events::Up
        || event == Ion::Events::Down)
    {
      //TODO LEA handle only one suggestion in var box.
      cycleAutocompletion(event == Ion::Events::Down);
      return true;
    } else {
      removeAutocompletion();
      m_contentView.reloadRectFromPosition(m_contentView.cursorLocation(), false);
      if (event == Ion::Events::Back) {
        // Do not process the event more
        return true;
      }
    }
  }
  bool result = TextArea::handleEvent(event);
  if (event == Ion::Events::Backspace && !m_contentView.isAutocompleting() && selectionIsEmpty()) {
    /* We want to add autocompletion when we are editing a word (after adding or
     * deleting text). So if nothing is selected, we add the autocompletion if
     * the event is backspace, as autocompletion has already been added if the
     * event added text, in handleEventWithText. */
    addAutocompletion();
  }
  return result;
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

void PythonTextArea::removeAutocompletion() {
  assert(m_contentView.isAutocompleting());
  removeAutocompletionText();
  m_contentView.setAutocompleting(false);
}

void PythonTextArea::removeAutocompletionText() {
  assert(m_contentView.isAutocompleting());
  assert(m_contentView.autocompletionEnd() != nullptr);
  const char * autocompleteStart = m_contentView.cursorLocation();
  const char * autocompleteEnd = m_contentView.autocompletionEnd();
  assert(autocompleteEnd != nullptr && autocompleteEnd > autocompleteStart);
  //TODO LEA if (autocompleteEnd > autocompleteStart) {
  m_contentView.removeText(autocompleteStart, autocompleteEnd);
  //TODO LEA }
}

void PythonTextArea::addAutocompletion() {
  assert(!m_contentView.isAutocompleting());
  const char * autocompletionTokenBeginning = nullptr;
  const char * autocompletionLocation = const_cast<char *>(cursorLocation());
  if (autocompletionType(autocompletionLocation, &autocompletionTokenBeginning) != AutocompletionType::EndOfIdentifier) {
    // The cursor is not at the end of an identifier.
    return;
  }

  // First load variables and functions that complete the textToAutocomplete
  const int scriptIndex = m_contentView.pythonDelegate()->menuController()->editedScriptIndex();
   m_contentView.pythonDelegate()->variableBoxController()->loadFunctionsAndVariables(scriptIndex, autocompletionTokenBeginning, autocompletionLocation - autocompletionTokenBeginning);

  if (addAutocompletionTextAtIndex(0)) {
    m_contentView.setAutocompleting(true);
  }
}

bool PythonTextArea::addAutocompletionTextAtIndex(int nextIndex, int * currentIndexToUpdate) {
  // The variable box should be loaded at this point
  const char * autocompletionTokenBeginning = nullptr;
  const char * autocompletionLocation = const_cast<char *>(cursorLocation());
  AutocompletionType type = autocompletionType(autocompletionLocation, &autocompletionTokenBeginning); // Done to get autocompletionTokenBeginning
  assert(type == AutocompletionType::EndOfIdentifier);
  (void)type; // Silence warnings
  VariableBoxController * varBox = m_contentView.pythonDelegate()->variableBoxController();
  int textToInsertLength = 0;
  bool addParentheses = false;
  const char * textToInsert = varBox->autocompletionAlternativeAtIndex(autocompletionLocation - autocompletionTokenBeginning, &textToInsertLength, &addParentheses, nextIndex, currentIndexToUpdate);

  // Try to insert the text (this might fail if the buffer is full)
  if (textToInsert != nullptr
      && textToInsertLength > 0
      && m_contentView.insertTextAtLocation(textToInsert, const_cast<char *>(autocompletionLocation), textToInsertLength))
  {
    autocompletionLocation += textToInsertLength;
    m_contentView.setAutocompletionEnd(autocompletionLocation);
    // Try to insert the parentheses if needed text
    const char * parentheses = ScriptNodeCell::k_parentheses;
    constexpr int parenthesesLength = 2;
    assert(strlen(parentheses) == parenthesesLength);
    if (addParentheses && m_contentView.insertTextAtLocation(parentheses, const_cast<char *>(autocompletionLocation), parenthesesLength)) {
      m_contentView.setAutocompleting(true);
      m_contentView.setAutocompletionEnd(autocompletionLocation + parenthesesLength);
    }
    return true;
  }
  return false;
}

void PythonTextArea::cycleAutocompletion(bool downwards) {
  assert(m_contentView.isAutocompleting());
  removeAutocompletionText();
  addAutocompletionTextAtIndex(m_autocompletionResultIndex + (downwards ? 1 : -1), &m_autocompletionResultIndex);
}

void PythonTextArea::acceptAutocompletion(bool moveCursorToEndOfAutocompletion) {
  assert(m_contentView.isAutocompleting());
  const char * autocompEnd = m_contentView.autocompletionEnd();
  m_contentView.setAutocompleting(false);
  m_contentView.setAutocompletionEnd(nullptr);
  if (moveCursorToEndOfAutocompletion) {
    setCursorLocation(autocompEnd);
    addAutocompletion();
  }
}

}
