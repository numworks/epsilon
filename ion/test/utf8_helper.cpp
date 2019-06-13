#include <quiz.h>
#include <ion/unicode/utf8_helper.h>

void assert_string_glyph_length_is(const char * string, int maxSize, size_t result) {
  quiz_assert(UTF8Helper::StringGlyphLength(string, maxSize) == result);
}

QUIZ_CASE(ion_utf8_helper_glyph_length) {
  assert_string_glyph_length_is("123", -1, 3);
  assert_string_glyph_length_is("1ᴇ3", -1, 3);
  assert_string_glyph_length_is("∑∫𝐢", -1, 3);
  assert_string_glyph_length_is("123", 2, 2);
  assert_string_glyph_length_is("1ᴇ3", 2, 1);
}
