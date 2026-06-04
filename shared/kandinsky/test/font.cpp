#include <assert.h>
#include <kandinsky/font.h>
#include <kandinsky/fonts/code_points.h>
#include <quiz.h>

constexpr KDFont testFont(10, 10, nullptr, nullptr);

bool valueNotInArray(const uint32_t* array, int length, uint32_t value) {
  for (int i = 0; i < length; i++) {
    if (array[i] == value) {
      return false;
    }
  }
  return true;
}

QUIZ_CASE(kandinsky_font_can_be_written_with_glyphs) {
  // Normal strings are accepted
  quiz_assert(KDFont::CanBeWrittenWithGlyphs("hello"));
  quiz_assert(KDFont::CanBeWrittenWithGlyphs(""));
  // Newline is explicitly allowed
  quiz_assert(KDFont::CanBeWrittenWithGlyphs("foo\nbar"));
  // Tab has no glyph and is rejected
  quiz_assert(!KDFont::CanBeWrittenWithGlyphs("\t"));
  // Overlong-encoded NUL (invalid UTF-8) is rejected: \xe0\x80\x80 decodes to
  // U+0000, which would exit the validation loop early and let subsequent bytes
  // (such as a tab) bypass the glyph check.
  quiz_assert(!KDFont::CanBeWrittenWithGlyphs("\xe0\x80\x80"));
  quiz_assert(!KDFont::CanBeWrittenWithGlyphs("\xe0\x80\x80\t\xff"));
}

QUIZ_CASE(kandinsky_font_index_for_code_point) {
  quiz_assert(CodePoints[KDFont::k_indexForReplacementCharacterCodePoint] ==
              0xFFFD);

  // Test all CodePoints
  for (uint32_t codePoint = 0; codePoint < 65536; codePoint++) {
    KDFont::GlyphIndex index = testFont.indexForCodePoint(codePoint);
    quiz_assert(CodePoints[index] == codePoint ||
                (index == KDFont::k_indexForReplacementCharacterCodePoint &&
                 valueNotInArray(CodePoints, NumberOfCodePoints, codePoint)));
  }
}
