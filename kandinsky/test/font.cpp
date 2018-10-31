#include <quiz.h>
#include <kandinsky.h>
#include <assert.h>

static constexpr KDFont::CodepointIndexPair table[] = {
  KDFont::CodepointIndexPair(3, 1), // Codepoint, identifier
  KDFont::CodepointIndexPair(9, 4),
  KDFont::CodepointIndexPair(12, 5),
  KDFont::CodepointIndexPair(14, 7)
};

constexpr KDFont testFont(4, table, 10, 10, nullptr, nullptr);

const KDFont::GlyphIndex index_for_codepoint[] = {
/* 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15 */
   0,  0,  0,  1,  2,  3,  0,  0,  0,  4,  0,  0,  5,  6,  7,  0
};

QUIZ_CASE(kandinsky_font_index_for_codepoint) {
  for (int i=0; i<16; i++) {
    KDFont::GlyphIndex result = testFont.indexForCodepoint(i);
    quiz_assert(result == index_for_codepoint[i]);
  }
}
