#include "python_text_area.h"

extern "C" {
#include "py/nlr.h"
#include "py/lexer.h"
}
#include <python/port/port.h>
#include <stdlib.h>

namespace Code {

constexpr KDColor CommentColor = KDColor::RGB24(0x999988);
constexpr KDColor NumberColor =  KDColor::RGB24(0x009999);
constexpr KDColor KeywordColor = KDColor::RGB24(0xFF000C);
// constexpr KDColor BuiltinColor = KDColor::RGB24(0x0086B3);
constexpr KDColor OperatorColor = KDColor::RGB24(0xd73a49);
constexpr KDColor StringColor = KDColor::RGB24(0x032f62);
constexpr KDColor BackgroundColor = KDColorWhite;

static inline KDColor TokenColor(mp_token_kind_t tokenKind) {
  if (tokenKind == MP_TOKEN_STRING || tokenKind == MP_TOKEN_LONELY_STRING_OPEN) {
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

static inline int TokenLength(mp_lexer_t * lex, const char * text) {
  if (lex->tok_kind == MP_TOKEN_LONELY_STRING_OPEN) {
    if(lex->vstr.len > 0 && text[0] == text[1]) {
      return lex->vstr.len + 2;
    } else {
      return lex->vstr.len + 1;
    }
  }
  if (lex->tok_kind == MP_TOKEN_STRING) {
    // If the string is empty, we have at least two chars + \0
    if(text[0] == text[2]) {
      return lex->vstr.len + 6;
    } else {
      return lex->vstr.len + 2;
    }
  }
  if (lex->vstr.len > 0) {
    return lex->vstr.len;
  }
  switch (lex->tok_kind) {
    case MP_TOKEN_OP_PLUS:
    case MP_TOKEN_OP_MINUS:
    case MP_TOKEN_OP_STAR:
    case MP_TOKEN_OP_SLASH:
    case MP_TOKEN_OP_PERCENT:
    case MP_TOKEN_OP_LESS:
    case MP_TOKEN_OP_MORE:
    case MP_TOKEN_OP_AMPERSAND:
    case MP_TOKEN_OP_PIPE:
    case MP_TOKEN_OP_CARET:
    case MP_TOKEN_OP_TILDE:
    case MP_TOKEN_OP_LESS_EQUAL:
    case MP_TOKEN_OP_MORE_EQUAL:
    case MP_TOKEN_OP_NOT_EQUAL:
    case MP_TOKEN_DEL_PAREN_OPEN:
    case MP_TOKEN_DEL_PAREN_CLOSE:
    case MP_TOKEN_DEL_BRACKET_OPEN:
    case MP_TOKEN_DEL_BRACKET_CLOSE:
    case MP_TOKEN_DEL_BRACE_OPEN:
    case MP_TOKEN_DEL_BRACE_CLOSE:
    case MP_TOKEN_DEL_COMMA:
    case MP_TOKEN_DEL_COLON:
    case MP_TOKEN_DEL_PERIOD:
    case MP_TOKEN_DEL_SEMICOLON:
    case MP_TOKEN_DEL_AT:
    case MP_TOKEN_DEL_EQUAL:
    case MP_TOKEN_DEL_PLUS_EQUAL:
    case MP_TOKEN_DEL_MINUS_EQUAL:
    case MP_TOKEN_DEL_STAR_EQUAL:
    case MP_TOKEN_DEL_SLASH_EQUAL:
    case MP_TOKEN_DEL_PERCENT_EQUAL:
    case MP_TOKEN_DEL_AMPERSAND_EQUAL:
    case MP_TOKEN_DEL_PIPE_EQUAL:
    case MP_TOKEN_DEL_CARET_EQUAL:
    case MP_TOKEN_DEL_MINUS_MORE:
      return 1;
    case MP_TOKEN_OP_DBL_STAR:
    case MP_TOKEN_OP_DBL_SLASH:
    case MP_TOKEN_OP_DBL_LESS:
    case MP_TOKEN_OP_DBL_MORE:
    case MP_TOKEN_OP_DBL_EQUAL:
    case MP_TOKEN_DEL_DBL_SLASH_EQUAL:
    case MP_TOKEN_DEL_DBL_MORE_EQUAL:
    case MP_TOKEN_DEL_DBL_LESS_EQUAL:
    case MP_TOKEN_DEL_DBL_STAR_EQUAL:
      return 2;
    default:
      return 0;
  }
}

void PythonTextArea::ContentView::loadSyntaxHighlighter() {
  assert(m_pythonHeap == nullptr);
  m_pythonHeap = static_cast<char *>(malloc(k_pythonHeapSize));
  if (m_pythonHeap != nullptr) {
    MicroPython::init(m_pythonHeap, m_pythonHeap + k_pythonHeapSize);
  }
}

void PythonTextArea::ContentView::unloadSyntaxHighlighter() {
  if (m_pythonHeap != nullptr) {
    MicroPython::deinit();
    free(m_pythonHeap);
    m_pythonHeap = nullptr;
  }
}

void PythonTextArea::ContentView::clearRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, BackgroundColor);
}

void PythonTextArea::ContentView::drawRect(KDContext * ctx, KDRect rect) const {
  if (m_pythonHeap == nullptr) {
    TextArea::ContentView::drawRect(ctx, rect);
    return;
  }

  // TODO: We're clearing areas we'll draw text over. It's not needed.
  clearRect(ctx, rect);

  KDSize charSize = KDText::charSize(m_fontSize);

  // We want to draw even partially visible characters. So we need to round
  // down for the top left corner and up for the bottom right one.
  Text::Position topLeft(
    rect.x()/charSize.width(),
    rect.y()/charSize.height()
  );
  Text::Position bottomRight(
    rect.right()/charSize.width() + 1,
    rect.bottom()/charSize.height() + 1
  );

  mp_lexer_t * lex = mp_lexer_new_from_str_len(0, text(), strlen(text()), 0);

  nlr_buf_t nlr;
  if (nlr_push(&nlr) == 0) {
    int cur_line = 1;
    int cur_col = 1;
    int cur_len = 0;
    mp_token_kind_t cur_tok_kind = MP_TOKEN_END;
    do {
      int beginIndex = m_text.indexAtPosition(TextArea::Text::Position(cur_col - 1, cur_line - 1));
      TextArea::Text::Position middlePosition = m_text.positionAtIndex(beginIndex + cur_len);
      TextArea::Text::Position endPosition = TextArea::Text::Position(lex->tok_column - 1, lex->tok_line - 1);
      int endIndex = m_text.indexAtPosition(endPosition);

      // Draw the current token.
      if(cur_line-1 <= bottomRight.line() && middlePosition.line() >= topLeft.line()) {
        ctx->drawString(
          m_text.text() + beginIndex,
          KDPoint(cur_col * charSize.width() - charSize.width(), cur_line * charSize.height() - charSize.height()),
          m_fontSize,
          TokenColor(cur_tok_kind),
          KDColorWhite,
          cur_len
        );
      }

      // Draw what is between the current token and the next token (comment or whitespace)
      if(middlePosition.line() <= bottomRight.line() &&  endPosition.line() >= topLeft.line()) {
        ctx->drawString(
           m_text.text() + beginIndex + cur_len,
           KDPoint(middlePosition.column() * charSize.width(), middlePosition.line() * charSize.height()),
           m_fontSize,
           CommentColor,
           KDColorWhite,
           endIndex - beginIndex - cur_len
         );
      }

      // Store the current token to be displayed
      cur_line = lex->tok_line;
      cur_col = lex->tok_column;
      cur_len = TokenLength(lex, text() + endIndex);
      cur_tok_kind = lex->tok_kind;

      mp_lexer_to_next(lex);
    } while(cur_tok_kind != MP_TOKEN_END);
    mp_lexer_free(lex);
    nlr_pop();
  }
}

void PythonTextArea::ContentView::drawLine(KDContext * ctx, int line, const char * text, size_t length, int fromColumn, int toColumn) const {
  drawStringAt(
    ctx,
    line,
    fromColumn,
    text + fromColumn,
    min(length - fromColumn, toColumn - fromColumn),
    StringColor,
    BackgroundColor
  );
}

KDRect PythonTextArea::ContentView::dirtyRectFromCursorPosition(size_t index, bool lineBreak) const {
  /* Mark the whole line and the subsequents as dirty.
   * TextArea has a very conservative approach and only dirties the surroundings
   * of the current character. That works for plain text, but when doing syntax
   * highlighting, you may want to redraw the surroundings as well. For example,
   * if editing "def foo" into "df foo", you'll want to redraw "df".
   * If starting a triple quoted string, all the subsequent text becomes a string */
  KDRect baseDirtyRect = TextArea::ContentView::dirtyRectFromCursorPosition(index, lineBreak);
  return KDRect(
    bounds().x(),
    baseDirtyRect.y(),
    bounds().width(),
    bounds().height() - baseDirtyRect.height()
  );}

}
