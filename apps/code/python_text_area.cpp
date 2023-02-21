#include "python_text_area.h"

#include <escher/palette.h>
#include <ion/unicode/utf8_helper.h>
#include <python/port/port.h>

#include "app.h"

/* py/parsenum.h is a C header which uses C keyword restrict.
 * It does not exist in C++ so we define it here in order to be able to include
 * py/parsenum.h header. */
#ifdef __cplusplus
#define restrict  // disable
#endif

extern "C" {
#include "py/lexer.h"
#include "py/nlr.h"
#include "py/parsenum.h"
}
#include <stdlib.h>

#include <algorithm>

using namespace Escher;

namespace Code {

constexpr KDColor CommentColor = KDColor::RGB24(0x999988);
constexpr KDColor NumberColor = KDColor::RGB24(0x009999);
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
  static_assert(MP_TOKEN_ELLIPSIS + 1 == MP_TOKEN_KW_FALSE &&
                    MP_TOKEN_KW_FALSE + 1 == MP_TOKEN_KW_NONE &&
                    MP_TOKEN_KW_NONE + 1 == MP_TOKEN_KW_TRUE &&
                    MP_TOKEN_KW_TRUE + 1 == MP_TOKEN_KW___DEBUG__ &&
                    MP_TOKEN_KW___DEBUG__ + 1 == MP_TOKEN_KW_AND &&
                    MP_TOKEN_KW_AND + 1 == MP_TOKEN_KW_AS &&
                    MP_TOKEN_KW_AS + 1 == MP_TOKEN_KW_ASSERT
                    /* Here there are keywords that depend on
                     * MICROPY_PY_ASYNC_AWAIT, we do not test them */
                    && MP_TOKEN_KW_BREAK + 1 == MP_TOKEN_KW_CLASS &&
                    MP_TOKEN_KW_CLASS + 1 == MP_TOKEN_KW_CONTINUE &&
                    MP_TOKEN_KW_CONTINUE + 1 == MP_TOKEN_KW_DEF &&
                    MP_TOKEN_KW_DEF + 1 == MP_TOKEN_KW_DEL &&
                    MP_TOKEN_KW_DEL + 1 == MP_TOKEN_KW_ELIF &&
                    MP_TOKEN_KW_ELIF + 1 == MP_TOKEN_KW_ELSE &&
                    MP_TOKEN_KW_ELSE + 1 == MP_TOKEN_KW_EXCEPT &&
                    MP_TOKEN_KW_EXCEPT + 1 == MP_TOKEN_KW_FINALLY &&
                    MP_TOKEN_KW_FINALLY + 1 == MP_TOKEN_KW_FOR &&
                    MP_TOKEN_KW_FOR + 1 == MP_TOKEN_KW_FROM &&
                    MP_TOKEN_KW_FROM + 1 == MP_TOKEN_KW_GLOBAL &&
                    MP_TOKEN_KW_GLOBAL + 1 == MP_TOKEN_KW_IF &&
                    MP_TOKEN_KW_IF + 1 == MP_TOKEN_KW_IMPORT &&
                    MP_TOKEN_KW_IMPORT + 1 == MP_TOKEN_KW_IN &&
                    MP_TOKEN_KW_IN + 1 == MP_TOKEN_KW_IS &&
                    MP_TOKEN_KW_IS + 1 == MP_TOKEN_KW_LAMBDA &&
                    MP_TOKEN_KW_LAMBDA + 1 == MP_TOKEN_KW_NONLOCAL &&
                    MP_TOKEN_KW_NONLOCAL + 1 == MP_TOKEN_KW_NOT &&
                    MP_TOKEN_KW_NOT + 1 == MP_TOKEN_KW_OR &&
                    MP_TOKEN_KW_OR + 1 == MP_TOKEN_KW_PASS &&
                    MP_TOKEN_KW_PASS + 1 == MP_TOKEN_KW_RAISE &&
                    MP_TOKEN_KW_RAISE + 1 == MP_TOKEN_KW_RETURN &&
                    MP_TOKEN_KW_RETURN + 1 == MP_TOKEN_KW_TRY &&
                    MP_TOKEN_KW_TRY + 1 == MP_TOKEN_KW_WHILE &&
                    MP_TOKEN_KW_WHILE + 1 == MP_TOKEN_KW_WITH &&
                    MP_TOKEN_KW_WITH + 1 == MP_TOKEN_KW_YIELD &&
                    MP_TOKEN_KW_YIELD + 1 == MP_TOKEN_OP_ASSIGN &&
                    MP_TOKEN_OP_ASSIGN + 1 == MP_TOKEN_OP_TILDE,
                "MP_TOKEN order changed, so Code::PythonTextArea::TokenColor "
                "might need to change too.");
  if (tokenKind >= MP_TOKEN_KW_FALSE && tokenKind <= MP_TOKEN_KW_YIELD) {
    return KeywordColor;
  }
  static_assert(
      MP_TOKEN_OP_TILDE + 1 == MP_TOKEN_OP_LESS &&
          MP_TOKEN_OP_LESS + 1 == MP_TOKEN_OP_MORE &&
          MP_TOKEN_OP_MORE + 1 == MP_TOKEN_OP_DBL_EQUAL &&
          MP_TOKEN_OP_DBL_EQUAL + 1 == MP_TOKEN_OP_LESS_EQUAL &&
          MP_TOKEN_OP_LESS_EQUAL + 1 == MP_TOKEN_OP_MORE_EQUAL &&
          MP_TOKEN_OP_MORE_EQUAL + 1 == MP_TOKEN_OP_NOT_EQUAL &&
          MP_TOKEN_OP_NOT_EQUAL + 1 == MP_TOKEN_OP_PIPE &&
          MP_TOKEN_OP_PIPE + 1 == MP_TOKEN_OP_CARET &&
          MP_TOKEN_OP_CARET + 1 == MP_TOKEN_OP_AMPERSAND &&
          MP_TOKEN_OP_AMPERSAND + 1 == MP_TOKEN_OP_DBL_LESS &&
          MP_TOKEN_OP_DBL_LESS + 1 == MP_TOKEN_OP_DBL_MORE &&
          MP_TOKEN_OP_DBL_MORE + 1 == MP_TOKEN_OP_PLUS &&
          MP_TOKEN_OP_PLUS + 1 == MP_TOKEN_OP_MINUS &&
          MP_TOKEN_OP_MINUS + 1 == MP_TOKEN_OP_STAR &&
          MP_TOKEN_OP_STAR + 1 == MP_TOKEN_OP_AT &&
          MP_TOKEN_OP_AT + 1 == MP_TOKEN_OP_DBL_SLASH &&
          MP_TOKEN_OP_DBL_SLASH + 1 == MP_TOKEN_OP_SLASH &&
          MP_TOKEN_OP_SLASH + 1 == MP_TOKEN_OP_PERCENT &&
          MP_TOKEN_OP_PERCENT + 1 == MP_TOKEN_OP_DBL_STAR &&
          MP_TOKEN_OP_DBL_STAR + 1 == MP_TOKEN_DEL_PIPE_EQUAL &&
          MP_TOKEN_DEL_PIPE_EQUAL + 1 == MP_TOKEN_DEL_CARET_EQUAL &&
          MP_TOKEN_DEL_CARET_EQUAL + 1 == MP_TOKEN_DEL_AMPERSAND_EQUAL &&
          MP_TOKEN_DEL_AMPERSAND_EQUAL + 1 == MP_TOKEN_DEL_DBL_LESS_EQUAL &&
          MP_TOKEN_DEL_DBL_LESS_EQUAL + 1 == MP_TOKEN_DEL_DBL_MORE_EQUAL &&
          MP_TOKEN_DEL_DBL_MORE_EQUAL + 1 == MP_TOKEN_DEL_PLUS_EQUAL &&
          MP_TOKEN_DEL_PLUS_EQUAL + 1 == MP_TOKEN_DEL_MINUS_EQUAL &&
          MP_TOKEN_DEL_MINUS_EQUAL + 1 == MP_TOKEN_DEL_STAR_EQUAL &&
          MP_TOKEN_DEL_STAR_EQUAL + 1 == MP_TOKEN_DEL_AT_EQUAL &&
          MP_TOKEN_DEL_AT_EQUAL + 1 == MP_TOKEN_DEL_DBL_SLASH_EQUAL &&
          MP_TOKEN_DEL_DBL_SLASH_EQUAL + 1 == MP_TOKEN_DEL_SLASH_EQUAL &&
          MP_TOKEN_DEL_SLASH_EQUAL + 1 == MP_TOKEN_DEL_PERCENT_EQUAL &&
          MP_TOKEN_DEL_PERCENT_EQUAL + 1 == MP_TOKEN_DEL_DBL_STAR_EQUAL &&
          MP_TOKEN_DEL_DBL_STAR_EQUAL + 1 == MP_TOKEN_DEL_PAREN_OPEN &&
          MP_TOKEN_DEL_PAREN_OPEN + 1 == MP_TOKEN_DEL_PAREN_CLOSE &&
          MP_TOKEN_DEL_PAREN_CLOSE + 1 == MP_TOKEN_DEL_BRACKET_OPEN &&
          MP_TOKEN_DEL_BRACKET_OPEN + 1 == MP_TOKEN_DEL_BRACKET_CLOSE &&
          MP_TOKEN_DEL_BRACKET_CLOSE + 1 == MP_TOKEN_DEL_BRACE_OPEN &&
          MP_TOKEN_DEL_BRACE_OPEN + 1 == MP_TOKEN_DEL_BRACE_CLOSE &&
          MP_TOKEN_DEL_BRACE_CLOSE + 1 == MP_TOKEN_DEL_COMMA &&
          MP_TOKEN_DEL_COMMA + 1 == MP_TOKEN_DEL_COLON &&
          MP_TOKEN_DEL_COLON + 1 == MP_TOKEN_DEL_PERIOD &&
          MP_TOKEN_DEL_PERIOD + 1 == MP_TOKEN_DEL_SEMICOLON &&
          MP_TOKEN_DEL_SEMICOLON + 1 == MP_TOKEN_DEL_EQUAL &&
          MP_TOKEN_DEL_EQUAL + 1 == MP_TOKEN_DEL_MINUS_MORE,
      "MP_TOKEN order changed, so Code::PythonTextArea::TokenColor might need "
      "to change too.");

  if ((tokenKind >= MP_TOKEN_OP_TILDE &&
       tokenKind <= MP_TOKEN_DEL_DBL_STAR_EQUAL) ||
      tokenKind == MP_TOKEN_DEL_EQUAL || tokenKind == MP_TOKEN_DEL_MINUS_MORE) {
    return OperatorColor;
  }
  return DefaultColor;
}

static inline size_t TokenLength(mp_lexer_t *lex, const char *tokenPosition) {
  /* The lexer stores the beginning of the current token and of the next token,
   * so we just use that. */
  if (lex->line > 1) {
    /* The next token is on the next line, so we cannot just make the difference
     * of the columns. */
    return UTF8Helper::CodePointSearch(tokenPosition, '\n') - tokenPosition;
  }
  return lex->column - lex->tok_column;
}

PythonTextArea::AutocompletionType PythonTextArea::autocompletionType(
    const char *autocompletionLocation,
    const char **autocompletionLocationBeginning,
    const char **autocompletionLocationEnd) const {
  const char *location = autocompletionLocation != nullptr
                             ? autocompletionLocation
                             : cursorLocation();
  const char *beginningOfToken = nullptr;

  /* If there is already autocompleting, the cursor must be at the end of an
   * identifier. Trying to compute autocompletionType will fail: because of the
   * autocompletion text, the cursor seems to be in the middle of an identifier.
   */
  AutocompletionType autocompleteType =
      isAutocompleting() ? AutocompletionType::EndOfIdentifier
                         : AutocompletionType::NoIdentifier;
  if (autocompletionLocationBeginning == nullptr &&
      autocompletionLocationEnd == nullptr) {
    return autocompleteType;
  }
  nlr_buf_t nlr;
  if (nlr_push(&nlr) == 0) {
    const char *firstNonSpace =
        UTF8Helper::BeginningOfWord(m_contentView.editedText(), location);
    mp_lexer_t *lex = mp_lexer_new_from_str_len(
        0, firstNonSpace, UTF8Helper::EndOfWord(location) - firstNonSpace, 0);

    const char *tokenStart;
    const char *tokenEnd;
    _mp_token_kind_t currentTokenKind = lex->tok_kind;

    while (currentTokenKind != MP_TOKEN_NEWLINE &&
           currentTokenKind != MP_TOKEN_END) {
      tokenStart = firstNonSpace + lex->tok_column - 1;
      tokenEnd = tokenStart + TokenLength(lex, tokenStart);

      if (location < tokenStart) {
        // The location for autocompletion is not in an identifier
        assert(autocompleteType == AutocompletionType::NoIdentifier);
        break;
      }
      if (location <= tokenEnd) {
        if (currentTokenKind == MP_TOKEN_NAME ||
            (currentTokenKind >= MP_TOKEN_KW_FALSE &&
             currentTokenKind <= MP_TOKEN_KW_YIELD)) {
          /* The location for autocompletion is in the middle or at the end of
           * an identifier. */
          beginningOfToken = tokenStart;
          /* If autocompleteType is already EndOfIdentifier, we are
           * autocompleting, so we do not need to update autocompleteType. If we
           * recomputed autocompleteType now, we might wrongly think that it is
           * MiddleOfIdentifier because of the autocompetion text.
           * Example : fin|ally -> the lexer is at the end of "fin", but because
           * we are autocompleting with "ally", the lexer thinks the cursor is
           * in the middle of an identifier. */
          if (autocompleteType != AutocompletionType::EndOfIdentifier) {
            autocompleteType = location < tokenEnd
                                   ? AutocompletionType::MiddleOfIdentifier
                                   : AutocompletionType::EndOfIdentifier;
          }
        }
        break;
      }
      mp_lexer_to_next(lex);
      currentTokenKind = lex->tok_kind;
    }
    mp_lexer_free(lex);
    nlr_pop();
  } else {  // Uncaught exception
    MicroPython::ExecutionEnvironment::HandleExceptionSilently();
    autocompleteType = AutocompletionType::NoIdentifier;
  }
  if (autocompletionLocationBeginning != nullptr) {
    *autocompletionLocationBeginning = beginningOfToken;
  }
  if (autocompletionLocationEnd != nullptr) {
    *autocompletionLocationEnd = location;
  }
  assert(!isAutocompleting() ||
         autocompleteType == AutocompletionType::EndOfIdentifier);
  return autocompleteType;
}

const char *PythonTextArea::ContentView::textToAutocomplete() const {
  return UTF8Helper::BeginningOfWord(editedText(), cursorLocation());
}

void PythonTextArea::ContentView::loadSyntaxHighlighter() {
  m_pythonDelegate->initPythonWithUser(this);
}

void PythonTextArea::ContentView::unloadSyntaxHighlighter() {
  m_pythonDelegate->deinitPython();
}

void PythonTextArea::ContentView::clearRect(KDContext *ctx, KDRect rect) const {
  ctx->fillRect(rect, BackgroundColor);
}

#define LOG_DRAWING 0
#if LOG_DRAWING
#include <stdio.h>
#define LOG_DRAW(...) printf(__VA_ARGS__)
#else
#define LOG_DRAW(...)
#endif

void PythonTextArea::ContentView::drawLine(KDContext *ctx, int line,
                                           const char *text, size_t byteLength,
                                           int fromColumn, int toColumn,
                                           const char *selectionStart,
                                           const char *selectionEnd) const {
  LOG_DRAW("Drawing \"%.*s\"\n", byteLength, text);

  assert(m_pythonDelegate->isPythonUser(this));

  /* We're using the MicroPython lexer to do syntax highlighting on a per-line
   * basis. This can work, however the MicroPython lexer won't accept a line
   * starting with a whitespace. So we're discarding leading whitespaces
   * beforehand. */
  const char *firstNonSpace = UTF8Helper::NotCodePointSearch(text, ' ');
  if (firstNonSpace != text) {
    // Color the discarded leading whitespaces
    const char *spacesStart =
        UTF8Helper::CodePointAtGlyphOffset(text, fromColumn);
    drawStringAt(ctx, line, fromColumn, spacesStart,
                 std::min(text + byteLength, firstNonSpace) - spacesStart,
                 StringColor, BackgroundColor, selectionStart, selectionEnd,
                 HighlightColor);
  }
  if (UTF8Helper::CodePointIs(firstNonSpace, UCodePointNull)) {
    return;
  }

  const char *autocompleteStart = m_autocomplete ? m_cursorLocation : nullptr;

  nlr_buf_t nlr;
  if (nlr_push(&nlr) == 0) {
    mp_lexer_t *lex = mp_lexer_new_from_str_len(
        0, firstNonSpace, byteLength - (firstNonSpace - text), 0);
    LOG_DRAW("Pop token %d\n", lex->tok_kind);

    const char *tokenFrom = firstNonSpace;
    size_t tokenLength = 0;
    const char *tokenEnd = firstNonSpace;
    while (lex->tok_kind != MP_TOKEN_NEWLINE && lex->tok_kind != MP_TOKEN_END) {
      tokenFrom = firstNonSpace + lex->tok_column - 1;
      if (tokenFrom != tokenEnd) {
        // We passed over white spaces, we need to color them
        drawStringAt(
            ctx, line, UTF8Helper::GlyphOffsetAtCodePoint(text, tokenEnd),
            tokenEnd, std::min(text + byteLength, tokenFrom) - tokenEnd,
            StringColor, BackgroundColor, selectionStart, selectionEnd,
            HighlightColor);
      }
      tokenLength = TokenLength(lex, tokenFrom);
      tokenEnd = tokenFrom + tokenLength;

      // If the token is being autocompleted, use DefaultColor
      KDColor color =
          (tokenFrom <= autocompleteStart && autocompleteStart < tokenEnd)
              ? DefaultColor
              : TokenColor(lex->tok_kind);

      if (color != DefaultColor && (lex->tok_kind == MP_TOKEN_INTEGER ||
                                    lex->tok_kind == MP_TOKEN_FLOAT_OR_IMAG)) {
        /* Check if the token can actually be parsed because lexer might label
         * tokens that cannot be parsed as integer or float */
        nlr_buf_t nlrNumberColorParse;
        if (nlr_push(&nlrNumberColorParse) == 0) {
          /* Use ex->vstr.len instead of tokenLength because it translates
           * escaped chars as the interpreter would do. */
          if (lex->tok_kind == MP_TOKEN_INTEGER) {
            mp_parse_num_integer(tokenFrom, lex->vstr.len, 0, NULL);
          } else {
            mp_parse_num_decimal(tokenFrom, lex->vstr.len, true, false, NULL);
          }
          nlr_pop();
        } else {
          // Parsing raised an exception, use DefaultColor.
          color = DefaultColor;
        }
      }

      LOG_DRAW("Draw \"%.*s\" for token %d\n", tokenLength, tokenFrom,
               lex->tok_kind);
      drawStringAt(ctx, line,
                   UTF8Helper::GlyphOffsetAtCodePoint(text, tokenFrom),
                   tokenFrom, tokenLength, color, BackgroundColor,
                   selectionStart, selectionEnd, HighlightColor);

      mp_lexer_to_next(lex);
      LOG_DRAW("Pop token %d\n", lex->tok_kind);
    }

    tokenFrom += tokenLength;

    // Even if the token is being autocompleted, use CommentColor
    if (tokenFrom < text + byteLength) {
      LOG_DRAW("Draw comment \"%.*s\" from %d\n",
               byteLength - (tokenFrom - text), firstNonSpace, tokenFrom);
      drawStringAt(
          ctx, line, UTF8Helper::GlyphOffsetAtCodePoint(text, tokenFrom),
          tokenFrom, text + byteLength - tokenFrom, CommentColor,
          BackgroundColor, selectionStart, selectionEnd, HighlightColor);
    }

    mp_lexer_free(lex);
    nlr_pop();
  } else {  // Uncaught exception
    MicroPython::ExecutionEnvironment::HandleExceptionSilently();
    drawStringAt(ctx, line, fromColumn, text, byteLength, DefaultColor,
                 BackgroundColor, selectionStart, selectionEnd, HighlightColor);
  }

  // Redraw the autocompleted word in the right color
  if (m_autocomplete && autocompleteStart >= text &&
      autocompleteStart < text + byteLength) {
    assert(m_autocompletionEnd != nullptr &&
           m_autocompletionEnd > autocompleteStart);
    drawStringAt(
        ctx, line, UTF8Helper::GlyphOffsetAtCodePoint(text, autocompleteStart),
        autocompleteStart,
        std::min(text + byteLength, m_autocompletionEnd) - autocompleteStart,
        AutocompleteColor, BackgroundColor, nullptr, nullptr, HighlightColor);
  }
}

KDRect PythonTextArea::ContentView::dirtyRectFromPosition(
    const char *position, bool includeFollowingLines) const {
  /* Mark the whole line as dirty.
   * TextArea has a very conservative approach and only dirties the surroundings
   * of the current character. That works for plain text, but when doing syntax
   * highlighting, you may want to redraw the surroundings as well. For example,
   * if editing "def foo" into "df foo", you'll want to redraw "df". */
  KDRect baseDirtyRect = TextArea::ContentView::dirtyRectFromPosition(
      position, includeFollowingLines);
  return KDRect(bounds().x(), baseDirtyRect.y(), bounds().width(),
                baseDirtyRect.height());
}

void PythonTextArea::didBecomeFirstResponder() {
  TextArea::didBecomeFirstResponder();
  /* If we are coming from a Varbox opened while autocompleting, the text was
   * removed to preserve the ScriptNodes name pointers. */
  if (!m_contentView.isAutocompleting() && m_wasAutocompleting) {
    addAutocompletion(m_autocompletionResultIndex);
    m_wasAutocompleting = false;
  }
}

bool PythonTextArea::handleEvent(Ion::Events::Event event) {
  if (m_contentView.isAutocompleting()) {
    // Handle event with autocompletion
    if (event == Ion::Events::Right || event == Ion::Events::ShiftRight ||
        event == Ion::Events::OK) {
      m_contentView.reloadRectFromPosition(m_contentView.cursorLocation(),
                                           false);
      acceptAutocompletion(event != Ion::Events::ShiftRight);
      if (event != Ion::Events::ShiftRight) {
        // Do not process the event more
        scrollToCursor();
        return true;
      }
    } else if (event == Ion::Events::Toolbox || event == Ion::Events::Shift ||
               event == Ion::Events::Alpha || event == Ion::Events::OnOff) {
    } else if (event == Ion::Events::Up || event == Ion::Events::Down) {
      cycleAutocompletion(event == Ion::Events::Down);
      return true;
    } else if (event == Ion::Events::Var) {
      /* Remove the autocompletion text so that opening the Varbox does not
       * invalidate the ScriptNodes name pointers. */
      m_wasAutocompleting = true;
      removeAutocompletion();
    } else if (event.isKeyPress() || event == Ion::Events::USBEnumeration) {
      /* USBEnumeration will pop the view (see EditorController::handleEvent).
       * Autocompletion is stopped to ensure that isAutocompleting() is false
       * when coming back */
      removeAutocompletion();
      m_contentView.reloadRectFromPosition(m_contentView.cursorLocation(),
                                           false);
      if (event == Ion::Events::Back) {
        // Do not process the event more
        return true;
      }
    }
  }
  bool result = TextArea::handleEvent(event);
  if (event == Ion::Events::Backspace && !m_contentView.isAutocompleting() &&
      selectionIsEmpty()) {
    /* We want to add autocompletion when we are editing a word (after adding or
     * deleting text). So if nothing is selected, we add the autocompletion if
     * the event is backspace, as autocompletion has already been added if the
     * event added text, in handleEventWithText. */
    addAutocompletion();
  } else if (event == Ion::Events::ShiftRight &&
             m_contentView.isAutocompleting()) {
    // Prevent further autocompletion after a ShiftRight event
    removeAutocompletion();
  }
  return result;
}

bool PythonTextArea::handleEventWithText(const char *text, bool indentation,
                                         bool forceCursorRightOfText) {
  if (*text == 0) {
    return false;
  }
  if (m_contentView.isAutocompleting()) {
    removeAutocompletion();
  }
  bool result =
      TextArea::handleEventWithText(text, indentation, forceCursorRightOfText);
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
  const char *autocompleteStart = m_contentView.cursorLocation();
  const char *autocompleteEnd = m_contentView.autocompletionEnd();
  assert(autocompleteEnd != nullptr && autocompleteEnd > autocompleteStart);
  m_contentView.removeText(autocompleteStart, autocompleteEnd);
}

void PythonTextArea::addAutocompletion(int index) {
  assert(!m_contentView.isAutocompleting());
  const char *autocompletionTokenBeginning = nullptr;
  const char *autocompletionLocation = const_cast<char *>(cursorLocation());
  m_autocompletionResultIndex = index;
  if (autocompletionType(autocompletionLocation,
                         &autocompletionTokenBeginning) !=
      AutocompletionType::EndOfIdentifier) {
    /* The cursor is not at the end of an identifier or Python encountered an
     * uncaught exception. */
    return;
  }

  // First load variables and functions that complete the textToAutocomplete
  const int scriptIndex =
      m_contentView.pythonDelegate()->menuController()->editedScriptIndex();
  m_contentView.pythonDelegate()->variableBox()->loadFunctionsAndVariables(
      scriptIndex, autocompletionTokenBeginning,
      autocompletionLocation - autocompletionTokenBeginning);

  addAutocompletionTextAtIndex(index);
}

bool PythonTextArea::addAutocompletionTextAtIndex(int nextIndex,
                                                  int *currentIndexToUpdate) {
  // The variable box should be loaded at this point
  const char *autocompletionTokenBeginning = nullptr;
  const char *autocompletionLocation = const_cast<char *>(cursorLocation());
  // Done to get autocompletionTokenBeginning
  AutocompletionType type =
      autocompletionType(autocompletionLocation, &autocompletionTokenBeginning);
  if (type == AutocompletionType::NoIdentifier) {
    // Python encountered an uncaught exception
    return false;
  }
  assert(type == AutocompletionType::EndOfIdentifier);
  (void)type;  // Silence warnings
  VariableBoxController *varBox = m_contentView.pythonDelegate()->variableBox();
  int textToInsertLength = 0;
  bool addParentheses = false;
  const char *textToInsert = varBox->autocompletionAlternativeAtIndex(
      autocompletionLocation - autocompletionTokenBeginning,
      &textToInsertLength, &addParentheses, nextIndex, currentIndexToUpdate);

  if (textToInsert == nullptr) {
    return false;
  }

  if (textToInsertLength > 0) {
    // Try to insert the text (this might fail if the buffer is full)
    if (!m_contentView.insertTextAtLocation(
            textToInsert, const_cast<char *>(autocompletionLocation),
            textToInsertLength)) {
      return false;
    }
    autocompletionLocation += textToInsertLength;
    m_contentView.setAutocompleting(true);
    m_contentView.setAutocompletionEnd(autocompletionLocation);
  }

  // Try to insert the parentheses if needed
  const char *parentheses = ScriptNodeCell::k_parentheses;
  constexpr int parenthesesLength = 2;
  assert(strlen(parentheses) == parenthesesLength);
  /* If couldInsertText is false, we should not try to add the parentheses as
   * there was already not enough space to add the autocompletion. */
  if (addParentheses &&
      m_contentView.insertTextAtLocation(
          parentheses, const_cast<char *>(autocompletionLocation),
          parenthesesLength)) {
    m_contentView.setAutocompleting(true);
    m_contentView.setAutocompletionEnd(autocompletionLocation +
                                       parenthesesLength);
    return true;
  }
  return (textToInsertLength > 0);
}

void PythonTextArea::cycleAutocompletion(bool downwards) {
  assert(m_contentView.isAutocompleting());
  removeAutocompletionText();
  addAutocompletionTextAtIndex(
      m_autocompletionResultIndex + (downwards ? 1 : -1),
      &m_autocompletionResultIndex);
}

void PythonTextArea::acceptAutocompletion(
    bool moveCursorToEndOfAutocompletion) {
  assert(m_contentView.isAutocompleting());

  // Save the cursor location
  const char *previousCursorLocation = cursorLocation();

  removeAutocompletion();

  m_contentView.pythonDelegate()->variableBox()->setSender(this);
  m_contentView.pythonDelegate()
      ->variableBox()
      ->insertAutocompletionResultAtIndex(m_autocompletionResultIndex);

  // insertAutocompletionResultAtIndex already added the autocompletion

  // If we did not want to move the cursor, restore its position.
  if (!moveCursorToEndOfAutocompletion) {
    setCursorLocation(previousCursorLocation);
  }
}

}  // namespace Code
