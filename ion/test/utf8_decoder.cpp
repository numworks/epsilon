#include <quiz.h>
#include <ion/unicode/utf8_decoder.h>

void assert_decodes_to(const char * string, CodePoint c) {
  UTF8Decoder d(string);
  quiz_assert(d.nextCodePoint() == c);
  quiz_assert(d.nextCodePoint() == 0);
}

QUIZ_CASE(ion_utf8_decoder) {
  assert_decodes_to("\x20", 0x20);
  assert_decodes_to("\xC2\xA2", 0xA2);
  assert_decodes_to("\xED\x9F\xBF", 0xD7FF);
  assert_decodes_to("\xCC\x81", 0x301);
}
