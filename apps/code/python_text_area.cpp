#include "python_text_area.h"
#include "app.h"

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

static inline const char * minPointer(const char * x, const char * y) { return x < y ? x : y; }

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

static inline size_t TokenLength(mp_lexer_t * lex) {
  if (lex->tok_kind == MP_TOKEN_STRING) {
    return lex->vstr.len + 2;
  }
  if (lex->vstr.len > 0) {
    return lex->vstr.len;
  }
  switch (lex->tok_kind) {
    case MP_TOKEN_OP_DBL_STAR:
    case MP_TOKEN_OP_DBL_SLASH:
    case MP_TOKEN_OP_DBL_LESS:
    case MP_TOKEN_OP_DBL_MORE:
    case MP_TOKEN_OP_LESS_EQUAL:
    case MP_TOKEN_OP_MORE_EQUAL:
    case MP_TOKEN_OP_DBL_EQUAL:
    case MP_TOKEN_OP_NOT_EQUAL:
    case MP_TOKEN_DEL_PLUS_EQUAL:
    case MP_TOKEN_DEL_MINUS_EQUAL:
    case MP_TOKEN_DEL_STAR_EQUAL:
    case MP_TOKEN_DEL_SLASH_EQUAL:
    case MP_TOKEN_DEL_PERCENT_EQUAL:
    case MP_TOKEN_DEL_AMPERSAND_EQUAL:
    case MP_TOKEN_DEL_PIPE_EQUAL:
    case MP_TOKEN_DEL_CARET_EQUAL:
    case MP_TOKEN_DEL_MINUS_MORE:
      return 2;
    case MP_TOKEN_DEL_DBL_SLASH_EQUAL:
    case MP_TOKEN_DEL_DBL_MORE_EQUAL:
    case MP_TOKEN_DEL_DBL_LESS_EQUAL:
    case MP_TOKEN_DEL_DBL_STAR_EQUAL:
      return 3;
    default:
      return 1;
  }
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

void PythonTextArea::ContentView::drawLine(KDContext * ctx, int line, const char * text, size_t byteLength, int fromColumn, int toColumn) const {
  LOG_DRAW("Drawing \"%.*s\"\n", byteLength, text);

  if (!m_pythonDelegate->isPythonUser(this)) {
    const char * lineStart = UTF8Helper::CodePointAtGlyphOffset(text, fromColumn);
    const char * lineEnd = UTF8Helper::CodePointAtGlyphOffset(text, toColumn);
    drawStringAt(
      ctx,
      line,
      fromColumn,
      lineStart,
      minPointer(text + byteLength, lineEnd) - lineStart,
      StringColor,
      BackgroundColor
    );
    return;
  }

  nlr_buf_t nlr;
  if (nlr_push(&nlr) == 0) {
    /* We're using the MicroPython lexer to do syntax highlighting on a per-line
     * basis. This can work, however the MicroPython lexer won't accept a line
     * starting with a whitespace. So we're discarding leading whitespaces
     * beforehand. */
    const char * firstNonSpace = UTF8Helper::NotCodePointSearch(text, ' ');
    if (UTF8Helper::CodePointIs(firstNonSpace, UCodePointNull)) {
      nlr_pop();
      return;
    }

    mp_lexer_t * lex = mp_lexer_new_from_str_len(0, firstNonSpace, byteLength - (firstNonSpace - text), 0);
    LOG_DRAW("Pop token %d\n", lex->tok_kind);

    const char * tokenFrom = firstNonSpace;
    size_t tokenLength = 0;
    while (lex->tok_kind != MP_TOKEN_NEWLINE && lex->tok_kind != MP_TOKEN_END) {

      tokenFrom = firstNonSpace + lex->tok_column - 1;
      tokenLength = TokenLength(lex);
      LOG_DRAW("Draw \"%.*s\" for token %d\n", tokenLength, tokenFrom, lex->tok_kind);
      drawStringAt(ctx, line,
        UTF8Helper::GlyphOffsetAtCodePoint(text, tokenFrom),
        tokenFrom,
        tokenLength,
        TokenColor(lex->tok_kind),
        BackgroundColor
      );

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
          BackgroundColor);
    }

    mp_lexer_free(lex);
    nlr_pop();
  }
}

KDRect PythonTextArea::ContentView::dirtyRectFromPosition(const char * position, bool lineBreak) const {
  /* Mark the whole line as dirty.
   * TextArea has a very conservative approach and only dirties the surroundings
   * of the current character. That works for plain text, but when doing syntax
   * highlighting, you may want to redraw the surroundings as well. For example,
   * if editing "def foo" into "df foo", you'll want to redraw "df". */
  KDRect baseDirtyRect = TextArea::ContentView::dirtyRectFromPosition(position, lineBreak);
  return KDRect(
    bounds().x(),
    baseDirtyRect.y(),
    bounds().width(),
    baseDirtyRect.height()
  );
}

}
