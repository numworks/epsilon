#include "python_highlighter.h"

extern "C" {
#include "py/nlr.h"
#include "py/lexer.h"
}
#include <python/port/port.h>

#include <string.h>

#include <stdio.h>

namespace Code {

constexpr KDColor CommentColor = KDColor::RGB24(0x999988);
constexpr KDColor NumberColor =  KDColor::RGB24(0x009999);
constexpr KDColor KeywordColor = KDColor::RGB24(0xFF000C);
// constexpr KDColor BuiltinColor = KDColor::RGB24(0x0086B3);
constexpr KDColor OperatorColor = KDColor::RGB24(0xd73a49);

static KDColor TokenColor(mp_token_kind_t tokenKind) {
  if (tokenKind == MP_TOKEN_INTEGER || tokenKind == MP_TOKEN_FLOAT_OR_IMAG) {
    return NumberColor;
  }
  if (tokenKind >= MP_TOKEN_KW_FALSE && tokenKind <= MP_TOKEN_KW_YIELD) {
    return KeywordColor;
  }
  if (tokenKind >= MP_TOKEN_OP_PLUS && tokenKind <= MP_TOKEN_OP_NOT_EQUAL) {
    return OperatorColor;
  }
  return KDColorBlack;
}

static int TokenLength(mp_lexer_t * lex) {
  if (lex->vstr.len > 0) {
    return lex->vstr.len;
  }
  switch (lex->tok_kind) {
    case MP_TOKEN_OP_DBL_STAR:
    case MP_TOKEN_OP_DBL_SLASH:
    case MP_TOKEN_OP_DBL_LESS:
    case MP_TOKEN_OP_DBL_MORE:
    case MP_TOKEN_OP_DBL_EQUAL:
      return 2;
    case MP_TOKEN_DEL_DBL_MORE_EQUAL:
    case MP_TOKEN_DEL_DBL_LESS_EQUAL:
    case MP_TOKEN_DEL_DBL_STAR_EQUAL:
      return 3;
    default:
      return 1;
  }
}

#include <stdio.h>

void PythonHighlighter(const TextAreaRenderingContext * context, const char * text, size_t length, int fromColumn, int toColumn) {



  //printf("Drawing %.*s\n", length, text);

  char m_pythonHeap[4096];

  MicroPython::init(m_pythonHeap, m_pythonHeap + 4096);

  nlr_buf_t nlr;
  if (nlr_push(&nlr) == 0) {
    /* We're using the MicroPython lexer to do syntax highlighting on a per-line
     * basis. This can work, however the MicroPython lexer won't accept a line
     * starting with a whitespace. So we're discarding leading whitespaces
     * beforehand. */
    int whitespaceOffset = 0;
    while (text[whitespaceOffset] == ' ' && whitespaceOffset < length) {
      whitespaceOffset++;
    }

    mp_lexer_t * lex = mp_lexer_new_from_str_len(0, text + whitespaceOffset, length - whitespaceOffset, 0);

    int drawFrom = 0;
    int drawLength = 0;
    KDColor drawColor = KDColorBlack;

    while (lex->tok_kind != MP_TOKEN_END && drawFrom <= toColumn) {
      //printf("Drawing from %d to %d\n", drawFrom, drawFrom+drawLength);

      // Draw text from drawFrom to drawLength using drawColor
      if (drawFrom + drawLength >= fromColumn) {
        context->drawLineChunk(
          text + drawFrom, // text
          drawLength, // length
          drawFrom,
          drawColor,
          KDColorWhite
        );
      }

      drawFrom = drawFrom + drawLength;

      // Let's prepare the next draw call.
      // We have a token in front of us. Two possibilities:
      int tokenPosition = lex->tok_kind == MP_TOKEN_NEWLINE ? length : whitespaceOffset + lex->tok_column - 1;
      if (drawFrom == tokenPosition) {
        // Either the token is immediatly in front of us, so we'll draw it next
        drawLength = TokenLength(lex);
        drawColor = TokenColor(lex->tok_kind);
        //printf("Setting color from %d\n", lex->tok_kind);
        // So let's pop a new token
        mp_lexer_to_next(lex);
      } else {
        // Or we're not at the token yet, so let's draw what's before first
        assert(drawFrom < tokenPosition);
        drawLength = tokenPosition - drawFrom;
        drawColor = CommentColor;
        //printf("Setting color to comment\n");
      }
    }

    mp_lexer_free(lex);
    nlr_pop();
  }

  MicroPython::deinit();


}

}

