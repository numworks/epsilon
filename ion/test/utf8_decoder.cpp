#include <quiz.h>
#include <ion/unicode/utf8_decoder.h>

void assert_decodes_to(const char * string, CodePoint c) {
  UTF8Decoder d(string);
  quiz_assert(d.nextCodePoint() == c);
  quiz_assert(d.nextCodePoint() == 0);
}

void assert_previous_code_point_is_to(const char * string, const char * stringPosition, CodePoint c) {
  UTF8Decoder d(string, stringPosition);
  quiz_assert(d.previousCodePoint() == c);
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
