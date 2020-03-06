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

constexpr int numberOfTests = 16;
const KDFont::GlyphIndex index_for_code_point[numberOfTests] = {
   KDFont::IndexForReplacementCharacterCodePoint,
   KDFont::IndexForReplacementCharacterCodePoint,
   KDFont::IndexForReplacementCharacterCodePoint,
   1,
   2,
   3,
   KDFont::IndexForReplacementCharacterCodePoint,
   KDFont::IndexForReplacementCharacterCodePoint,
   KDFont::IndexForReplacementCharacterCodePoint,
   4,
   KDFont::IndexForReplacementCharacterCodePoint,
   KDFont::IndexForReplacementCharacterCodePoint,
   5,
   6,
   7,
   KDFont::IndexForReplacementCharacterCodePoint,
};

QUIZ_CASE(kandinsky_font_index_for_code_point) {
  for (int i=0; i<numberOfTests; i++) {
    KDFont::GlyphIndex result = testFont.indexForCodePoint(i);
    quiz_assert(result == index_for_code_point[i]);
  }
}
