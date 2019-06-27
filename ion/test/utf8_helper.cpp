#include <quiz.h>
#include <ion/unicode/utf8_decoder.h>
#include <ion/unicode/utf8_helper.h>
#include <string.h>

void assert_occurences_count(const char * string, CodePoint c, int result) {
  quiz_assert(UTF8Helper::CountOccurrences(string, c) == result);
}

QUIZ_CASE(ion_utf8_helper_count_occurrences) {
  assert_occurences_count("1234", '1', 1);
  assert_occurences_count("2232", '2', 3);
  assert_occurences_count("2π2∑32∑", UCodePointGreekSmallLetterPi, 1);
  assert_occurences_count("2π2∑32∑", UCodePointNArySummation, 2);
}

void assert_code_point_searched_is(const char * string, CodePoint c, const char * result) {
  quiz_assert(UTF8Helper::CodePointSearch(string, c) == result);
}

QUIZ_CASE(ion_utf8_helper_code_point_search) {
  const char * s = "2π2∑32∑";
  assert_code_point_searched_is(s, '2', s);
  assert_code_point_searched_is(s, UCodePointGreekSmallLetterPi, s+1);
  assert_code_point_searched_is(s, 'a', s+strlen(s));
}

void assert_not_code_point_searched_is(const char * string, CodePoint c, bool goingLeft, const char * initialPosition, const char * result) {
  quiz_assert(UTF8Helper::NotCodePointSearch(string, c, goingLeft, initialPosition) == result);
}

QUIZ_CASE(ion_utf8_helper_not_code_point_search) {
  // Going right
  const char * s = "2π2∑32∑";
  assert_not_code_point_searched_is(s, '2', false, nullptr, s+1);
  s = "aaaaa";
  assert_not_code_point_searched_is(s, 'a', false, nullptr, s+5);
  s = "π234";
  assert_not_code_point_searched_is(s, UCodePointGreekSmallLetterPi, false, nullptr, s+UTF8Decoder::CharSizeOfCodePoint(UCodePointGreekSmallLetterPi));
  // Going left
  s = "aaaaa";
  assert_not_code_point_searched_is(s, 'a', true, s+5, s);
  assert_not_code_point_searched_is(s, 'b', true, s+5, s+4);
  s = "234π";
  assert_not_code_point_searched_is(s, UCodePointGreekSmallLetterPi, true, s+5, s+2);
}

void assert_copy_and_remove_code_point_gives(char * dst, size_t dstSize, const char * src, CodePoint c, const char * result) {
  size_t resultLen = strlen(result);
  quiz_assert(dstSize >= resultLen + 1);
  UTF8Helper::CopyAndRemoveCodePoint(dst, dstSize, src, c);
  for (int i = 0; i <= resultLen; i++) {
    quiz_assert(dst[i] == result[i]);
  }
}

QUIZ_CASE(ion_utf8_copy_and_remove_code_point) {
  constexpr int bufferSize = 100;
  char buffer[bufferSize];

  const char * s = "12345";
  CodePoint c = '1';
  const char * result = "2345";
  assert_copy_and_remove_code_point_gives(buffer, bufferSize, s, c, result);

  s = "12345";
  c = '2';
  result = "1345";
  assert_copy_and_remove_code_point_gives(buffer, bufferSize, s, c, result);

  s = "2123224252";
  c = '2';
  result = "1345";
  assert_copy_and_remove_code_point_gives(buffer, bufferSize, s, c, result);

  s = "12345";
  c = '6';
  result = "12345";
  assert_copy_and_remove_code_point_gives(buffer, bufferSize, s, c, result);

  s = "12ᴇ4";
  c = UCodePointLatinLetterSmallCapitalE;
  result = "124";
  assert_copy_and_remove_code_point_gives(buffer, bufferSize, s, c, result);

  s = "12ᴇᴇᴇ4";
  c = UCodePointLatinLetterSmallCapitalE;
  result = "124";
  assert_copy_and_remove_code_point_gives(buffer, bufferSize, s, c, result);
}

void assert_remove_code_point_gives(char * buffer, CodePoint c, const char * * indexToUpdate, const char * stoppingPosition, const char * indexToUpdateResult, const char * result) {
  UTF8Helper::RemoveCodePoint(buffer, c, indexToUpdate, stoppingPosition);
  quiz_assert(*indexToUpdate == indexToUpdateResult);
  quiz_assert(strcmp(buffer, result) == 0);
}

QUIZ_CASE(ion_utf8_remove_code_point) {
  constexpr int bufferSize = 100;
  char buffer[bufferSize];

  const char * s = "2345";
  strlcpy(buffer, s, strlen(s) + 1);
  CodePoint c = '1';
  const char * indexToUpdate = buffer + 3;
  const char * indexToUpdateResult = indexToUpdate;
  const char * result = "2345";
  const char * stoppingPosition = nullptr;
  assert_remove_code_point_gives(buffer, c, &indexToUpdate, stoppingPosition, indexToUpdateResult, result);

  s = "12345";
  strlcpy(buffer, s, strlen(s) + 1);
  c = '2';
  indexToUpdate = buffer + 3;
  indexToUpdateResult = indexToUpdate - UTF8Decoder::CharSizeOfCodePoint(c);
  result = "1345";
  stoppingPosition = nullptr;
  assert_remove_code_point_gives(buffer, c, &indexToUpdate, stoppingPosition, indexToUpdateResult, result);

  s = "2123224252";
  strlcpy(buffer, s, strlen(s) + 1);
  c = '2';
  indexToUpdate = buffer + 5;
  indexToUpdateResult = indexToUpdate - 3*UTF8Decoder::CharSizeOfCodePoint(c);
  result = "13452";
  stoppingPosition = buffer+8;
  assert_remove_code_point_gives(buffer, c, &indexToUpdate, stoppingPosition, indexToUpdateResult, result);

  s = "12ᴇ4";
  strlcpy(buffer, s, strlen(s) + 1);
  c = UCodePointLatinLetterSmallCapitalE;
  indexToUpdate = buffer + strlen(s);
  indexToUpdateResult = indexToUpdate - UTF8Decoder::CharSizeOfCodePoint(c);
  result = "124";
  stoppingPosition = nullptr;
  assert_remove_code_point_gives(buffer, c, &indexToUpdate, stoppingPosition, indexToUpdateResult, result);

  s = "12ᴇᴇᴇ4";
  strlcpy(buffer, s, strlen(s) + 1);
  c = UCodePointLatinLetterSmallCapitalE;
  indexToUpdate = buffer + 2 + UTF8Decoder::CharSizeOfCodePoint(c);
  indexToUpdateResult = buffer + 2;
  result = "124";
  stoppingPosition = nullptr;
  assert_remove_code_point_gives(buffer, c, &indexToUpdate, stoppingPosition, indexToUpdateResult, result);
}

void assert_string_glyph_length_is(const char * string, int maxSize, size_t result) {
  quiz_assert(UTF8Helper::StringGlyphLength(string, maxSize) == result);
}

QUIZ_CASE(ion_utf8_helper_string_glyph_length) {
  assert_string_glyph_length_is("123", -1, 3);
  assert_string_glyph_length_is("1ᴇ3", -1, 3);
  assert_string_glyph_length_is("∑∫𝐢", -1, 3);
  assert_string_glyph_length_is("123", 2, 2);
  uint8_t testString[] = {'a', 'b', 'c', 0b11111111, 0b11111111, 0}; // Malformed utf-8 string
  assert_string_glyph_length_is((const char *)testString, 3, 3);
}
