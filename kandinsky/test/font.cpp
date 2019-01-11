#include <quiz.h>
#include <kandinsky.h>
#include <assert.h>

static constexpr KDFont::CodePointIndexPair table[] = {
  KDFont::CodePointIndexPair(3, 1), // CodePoint, identifier
  KDFont::CodePointIndexPair(9, 4),
  KDFont::CodePointIndexPair(12, 5),
  KDFont::CodePointIndexPair(14, 7)
};

constexpr KDFont testFont(4, table, 10, 10, nullptr, nullptr);

const KDFont::GlyphIndex index_for_code_point[] = {
/* 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15 */
   0,  0,  0,  1,  2,  3,  0,  0,  0,  4,  0,  0,  5,  6,  7,  0
};

QUIZ_CASE(kandinsky_font_index_for_code_point) {
  for (int i=0; i<16; i++) {
    KDFont::GlyphIndex result = testFont.indexForCodePoint(i);
    quiz_assert(result == index_for_code_point[i]);
  }
}
