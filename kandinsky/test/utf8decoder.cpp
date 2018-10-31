#include <quiz.h>
#include <kandinsky/unicode/utf8decoder.h>

void assert_decodes_to(const char * string, Codepoint c) {
  UTF8Decoder d(string);
  quiz_assert(d.nextCodepoint() == c);
  quiz_assert(d.nextCodepoint() == 0);
}

QUIZ_CASE(kandinsky_utf8_decoder) {
  assert_decodes_to("\x20", 0x20);
  assert_decodes_to("\xC2\xA2", 0xA2);
  assert_decodes_to("\xED\x9F\xBF", 0xD7FF);
  assert_decodes_to("\xCC\x81", 0x301);
}
