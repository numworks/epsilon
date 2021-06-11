#include <quiz.h>
#include <ion/unicode/utf8_decoder.h>
#include <ion/unicode/utf8_helper.h>
#include <string.h>

void assert_decodes_to(const char * string, CodePoint c) {
  UTF8Decoder d(string);
  quiz_assert(d.nextCodePoint() == c);
  quiz_assert(d.nextCodePoint() == 0);
}

void assert_previous_code_point_is_to(const char * string, const char * stringPosition, CodePoint c) {
  UTF8Decoder d(string, stringPosition);
  quiz_assert(d.previousCodePoint() == c);
}

void assert_code_point_at_next_glyph_position_is(const char * string, CodePoint c) {
  UTF8Decoder d(string);
  d.nextGlyphPosition();
  quiz_assert(d.nextCodePoint() == c);
}

void assert_code_point_at_previous_glyph_position_is(const char * string, const char * stringPosition, CodePoint c) {
  UTF8Decoder d(string, stringPosition);
  d.previousGlyphPosition();
  quiz_assert(d.nextCodePoint() == c);
}

void assert_code_point_decode_to_chars(CodePoint c, const char * expected, char * buffer, size_t bufferSize) {
  UTF8Decoder::CodePointToChars(c, buffer, bufferSize);
  quiz_assert(strcmp(expected, buffer) == 0);
}

QUIZ_CASE(ion_utf8_code_point_system_parentheses) {
  quiz_assert(UCodePointLeftSystemParenthesis == '\u0012');
  quiz_assert(UCodePointRightSystemParenthesis == '\u0013');
}

QUIZ_CASE(ion_utf8_decode_forward) {
  assert_decodes_to("\x20", 0x20);
  assert_decodes_to("\xC2\xA2", 0xA2);
  assert_decodes_to("\xED\x9F\xBF", 0xD7FF);
  assert_decodes_to("\xCC\x81", 0x301);
}

QUIZ_CASE(ion_utf8_decode_backwards) {
  const char * a = "abcde";
  assert_previous_code_point_is_to(a, a+1, *a);
  assert_previous_code_point_is_to(a, a+4, *(a+3));
  assert_previous_code_point_is_to(a, a+6, *(a+5));
}

QUIZ_CASE(ion_utf8_decoder_next_glyph) {
  const char * string = u8"a\u0065\u0301i";
  assert_code_point_at_next_glyph_position_is(string, 'e');
  assert_code_point_at_next_glyph_position_is(string+1, 'i');
}

QUIZ_CASE(ion_utf8_decoder_previous_glyph) {
  const char * string = u8"a\u0065\u0301i";
  const char * iPosition = UTF8Helper::CodePointSearch(string, 'i');
  assert_code_point_at_previous_glyph_position_is(string, iPosition, 'e');
  assert_code_point_at_previous_glyph_position_is(string,string+1, 'a');
}

QUIZ_CASE(ion_utf8_decoder_code_point) {
  CodePoint c = UCodePointRightwardsArrow;
  quiz_assert(UTF8Decoder::CharSizeOfCodePoint(c) == 3);
  constexpr size_t maxBufferSize = 5;
  char buffer[maxBufferSize];
  // Buffer is big enough
  memset(buffer, 0, sizeof(buffer));
  assert_code_point_decode_to_chars(c, "→", buffer, 5);
  memset(buffer, 0, sizeof(buffer));
  assert_code_point_decode_to_chars(c, "→", buffer, 4);

  memset(buffer, 0, sizeof(buffer));
  /* Warning : Risk of a non-null terminated string.
   * Okay here because maxBufferSize > 3. */
  assert_code_point_decode_to_chars(c, "→", buffer, 3);

  // Buffer is too short
  memset(buffer, 0, sizeof(buffer));
  assert_code_point_decode_to_chars(c, "", buffer, 2);
  memset(buffer, 0, sizeof(buffer));
  assert_code_point_decode_to_chars(c, "", buffer, 1);
  memset(buffer, 0, sizeof(buffer));
  assert_code_point_decode_to_chars(c, "", buffer, 0);
}