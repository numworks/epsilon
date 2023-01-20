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

void assert_copy_and_remove_code_points_gives(char * dst, size_t dstSize, const char * src, CodePoint * c, int numberOfCodePoints, const char * result) {
  size_t resultLen = strlen(result);
  quiz_assert(dstSize >= resultLen + 1);
  UTF8Helper::CopyAndRemoveCodePoints(dst, dstSize, src, c, numberOfCodePoints);
  for (size_t i = 0; i <= resultLen; i++) {
    quiz_assert(dst[i] == result[i]);
  }
}
static int bufferSize = 100;
QUIZ_CASE(ion_utf8_copy_and_remove_code_point) {
  char buffer[bufferSize];

  const char * s = "12345";
  CodePoint c1[] = {'1'};
  const char * result = "2345";
  assert_copy_and_remove_code_points_gives(buffer, bufferSize, s, c1, 1, result);

  s = "12345";
  CodePoint c2[] = {'2'};
  result = "1345";
  assert_copy_and_remove_code_points_gives(buffer, bufferSize, s, c2, 1, result);

  s = "2123224252";
  CodePoint c3[] = {'2'};
  result = "1345";
  assert_copy_and_remove_code_points_gives(buffer, bufferSize, s, c3, 1, result);

  s = "12345";
  CodePoint c4[] = {'6'};
  result = "12345";
  assert_copy_and_remove_code_points_gives(buffer, bufferSize, s, c4, 1, result);

  s = "12ᴇ4";
  CodePoint c5[] = {UCodePointLatinLetterSmallCapitalE};
  result = "124";
  assert_copy_and_remove_code_points_gives(buffer, bufferSize, s, c5, 1, result);

  s = "12ᴇᴇᴇ4";
  CodePoint c6[] = {UCodePointLatinLetterSmallCapitalE};
  result = "124";
  assert_copy_and_remove_code_points_gives(buffer, bufferSize, s, c6, 1, result);

  // The buffer size is to small to hold s
  s = "1234ᴇ";
  CodePoint c7[] = {'5'};
  result = "1234"; // "1234ᴇ" size is 7
  assert_copy_and_remove_code_points_gives(buffer, 6, s, c7, 1, result);
  assert_copy_and_remove_code_points_gives(buffer, 7, s, c7, 1, result);
  result = "1234ᴇ";
  assert_copy_and_remove_code_points_gives(buffer, 8, s, c7, 1, result);

  s = "1234ᴇ";
  CodePoint c8[] = {'4'};
  result = "123ᴇ";
  assert_copy_and_remove_code_points_gives(buffer, 7, s, c8, 1, result);

  // Remove several code points
  s = "1234ᴇ3";
  CodePoint c9[] = {'4', UCodePointLatinLetterSmallCapitalE};
  result = "1233";
  assert_copy_and_remove_code_points_gives(buffer, bufferSize, s, c9, 2, result);
}

void assert_remove_code_point_gives(char * buffer, CodePoint c, const char * * indexToUpdate, const char * stoppingPosition, const char * indexToUpdateResult, const char * result) {
  UTF8Helper::RemoveCodePoint(buffer, c, indexToUpdate, stoppingPosition);
  quiz_assert(*indexToUpdate == indexToUpdateResult);
  quiz_assert(strcmp(buffer, result) == 0);
}

QUIZ_CASE(ion_utf8_remove_code_point) {
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

void assert_slide_string_by_number_of_char_gives(const char * string, int slidingSize, bool successResult, const char * stringResult = nullptr) {
  char buffer[bufferSize];
  strlcpy(buffer, string, bufferSize);
  bool success = UTF8Helper::SlideStringByNumberOfChar((char *)buffer, slidingSize, bufferSize);
  quiz_assert(success == successResult);
  if (successResult) {
    quiz_assert(strncmp(buffer, stringResult, bufferSize) == 0);
  }
}


QUIZ_CASE(ion_utf8_move_string_from_index_by_number_of_char) {
  const char * string1 = "12345";
  assert_slide_string_by_number_of_char_gives(string1, 1, true, "112345");
  const char * string2 = "(1+3)";
  assert_slide_string_by_number_of_char_gives(string2, 3, true, "(1+(1+3)");
  assert_slide_string_by_number_of_char_gives(string2, bufferSize - strlen(string2)/2, false);
  const char * string3 = "exp(3+4)";
  assert_slide_string_by_number_of_char_gives(string3, -3, true, "(3+4)");
  assert_slide_string_by_number_of_char_gives(string3, -(strlen(string3)+3), false);
  assert_slide_string_by_number_of_char_gives(string3, -8, true, "");
}

void assert_try_and_replace_pattern_in_string_by_pattern_gives(char * buffer, int bufferSize, UTF8Helper::TextPair * textPairs, int numberOfPairs, bool firstToSecond, const char * stringResult, const char ** indexToUpdate = nullptr, const char * indexToUpdateResult = nullptr, const char * stoppingPosition = nullptr) {
  UTF8Helper::TryAndReplacePatternsInStringByPatterns(buffer, bufferSize, textPairs, numberOfPairs, firstToSecond, indexToUpdate, stoppingPosition);
  quiz_assert(strncmp(buffer, stringResult, bufferSize) == 0);
  if (indexToUpdateResult != nullptr) {
    quiz_assert(*indexToUpdate == indexToUpdateResult);
  }
}

QUIZ_CASE(ion_utf8_try_and_replace_pattern_in_string_by_pattern) {
  constexpr int numberOfPairs = 2;
  constexpr UTF8Helper::TextPair textPairs[numberOfPairs] = {
    UTF8Helper::TextPair("12", "2.3"),
    UTF8Helper::TextPair("exp", "ln"),
  };

  char buffer[bufferSize];
  const char * string = "1234512";
  strlcpy(buffer, string, bufferSize);
  const char * indexToUpdate = buffer + 3;
  const char * indexToUpdateResult = indexToUpdate + 1;
  const char * result = "2.33452.3";
  const char * stoppingPosition = nullptr;
  assert_try_and_replace_pattern_in_string_by_pattern_gives(buffer, bufferSize, (UTF8Helper::TextPair *)&textPairs, numberOfPairs, true, result, &indexToUpdate, indexToUpdateResult);

  string = "exp(2.3)12";
  strlcpy(buffer, string, bufferSize);
  indexToUpdate = buffer + 3;
  indexToUpdateResult = indexToUpdate - 1;
  result = "ln(2.3)12";
  stoppingPosition = buffer + 5;
  assert_try_and_replace_pattern_in_string_by_pattern_gives(buffer, bufferSize, (UTF8Helper::TextPair *)&textPairs, numberOfPairs, true, result, &indexToUpdate, indexToUpdateResult, stoppingPosition);

  string = "12*ln(7)+ln";
  strlcpy(buffer, string, bufferSize);
  indexToUpdate = buffer + 7;
  indexToUpdateResult = indexToUpdate + 1;
  result = "12*exp(7)+ln";
  stoppingPosition = buffer + 7;
  assert_try_and_replace_pattern_in_string_by_pattern_gives(buffer, bufferSize, (UTF8Helper::TextPair *)&textPairs, numberOfPairs, false, result, &indexToUpdate, indexToUpdateResult, stoppingPosition);

}

void assert_string_copy_until_code_point_gives(char * dst, size_t dstSize, const char * src, CodePoint c, const char * result, size_t returnedResult) {
  quiz_assert(UTF8Helper::CopyUntilCodePoint(dst, dstSize, src, c) == returnedResult);
  quiz_assert(strcmp(dst, result) == 0);
}

QUIZ_CASE(ion_utf8_helper_copy_until_code_point) {
  char buffer[bufferSize];

  const char * s = "1234";
  CodePoint c = '1';
  const char * result = "";
  size_t returnedResult = 0;
  assert_string_copy_until_code_point_gives(buffer, bufferSize, s, c, result, returnedResult);

  s = "1234";
  c = '3';
  result = "12";
  returnedResult = 2;
  assert_string_copy_until_code_point_gives(buffer, bufferSize, s, c, result, returnedResult);

  s = "1234";
  c = '6';
  result = s;
  returnedResult = 4;
  assert_string_copy_until_code_point_gives(buffer, bufferSize, s, c, result, returnedResult);
}

QUIZ_CASE(ion_utf8_helper_previous_code_point_is) {
  const char * s = "1234";
  quiz_assert(UTF8Helper::PreviousCodePointIs(s, s+2, '2'));
  quiz_assert(!UTF8Helper::PreviousCodePointIs(s, s+2, '4'));
  s = "1234∑";
  quiz_assert(UTF8Helper::PreviousCodePointIs(s, s+strlen(s), UCodePointNArySummation));
}

QUIZ_CASE(ion_utf8_helper_code_point_is) {
  const char * s = "34";
  quiz_assert(UTF8Helper::CodePointIs(s, '3'));
  quiz_assert(!UTF8Helper::CodePointIs(s, '4'));
  s = "∑1234";
  quiz_assert(UTF8Helper::CodePointIs(s, UCodePointNArySummation));
}

void assert_string_remove_previous_glyph_gives(const char * text, char * location, const char * result, int returnedResult, CodePoint returnedCodePoint) {
  CodePoint c = 0;
  quiz_assert(UTF8Helper::RemovePreviousGlyph(text, location, &c) == returnedResult);
  quiz_assert(c == returnedCodePoint);
  quiz_assert(strcmp(text, result) == 0);
}

QUIZ_CASE(ion_utf8_helper_remove_previous_glyph) {
  char buffer[bufferSize];
  // 3é4
  buffer[0] = '3';
  buffer[1] = 0x65;
  buffer[2] = 0xCC;
  buffer[3] = 0x81;
  buffer[4] = '4';
  buffer[5] = 0;
  size_t sLen = 5;
  char * location = const_cast<char *>(buffer+sLen - 1);
  const char * result = "34";
  int returnedResult = sLen - 2;
  CodePoint returnedCodePoint = 'e';
  assert_string_remove_previous_glyph_gives(buffer, location, result, returnedResult, returnedCodePoint);

  const char * s = "345";
  sLen = strlen(s);
  strlcpy(buffer, s, sLen+1);
  location = const_cast<char *>(buffer+sLen);
  result = "34";
  returnedResult = UTF8Decoder::CharSizeOfCodePoint('5');
  returnedCodePoint = '5';
  assert_string_remove_previous_glyph_gives(buffer, location, result, returnedResult, returnedCodePoint);
}

QUIZ_CASE(ion_utf8_code_point_at_glyph_offset) {
  const char * s = "abc";
  quiz_assert(UTF8Helper::CodePointAtGlyphOffset(s, 0) == s);
  quiz_assert(UTF8Helper::CodePointAtGlyphOffset(s, 1) == s+1);
  quiz_assert(UTF8Helper::CodePointAtGlyphOffset(s, 2) == s+2);
  quiz_assert(UTF8Helper::CodePointAtGlyphOffset(s, 5) == s+3);
  s = "a∑∫ity";
  quiz_assert(UTF8Helper::CodePointAtGlyphOffset(s, 4) == s + strlen(s) - 2);
  s = "fée";
  quiz_assert(UTF8Helper::CodePointAtGlyphOffset(s, 2) == s + strlen(s) - 1);
}

QUIZ_CASE(ion_utf8_glyph_offset_at_code_point) {
  const char * s = "abc";
  quiz_assert(UTF8Helper::GlyphOffsetAtCodePoint(s, s) == 0);
  quiz_assert(UTF8Helper::GlyphOffsetAtCodePoint(s, s+1) == 1);
  quiz_assert(UTF8Helper::GlyphOffsetAtCodePoint(s, s+2) == 2);
  s = "a∑∫ity";
  quiz_assert(UTF8Helper::GlyphOffsetAtCodePoint(s, s + strlen(s) - 2) == 4);
}

void assert_string_glyph_length_is(const char * string, int maxSize, size_t result) {
  quiz_assert(UTF8Helper::StringGlyphLength(string, maxSize) == result);
}

QUIZ_CASE(ion_utf8_helper_string_glyph_length) {
  assert_string_glyph_length_is("123", -1, 3);
  assert_string_glyph_length_is("1ᴇ3", -1, 3);
  assert_string_glyph_length_is("∑∫i", -1, 3);
  assert_string_glyph_length_is("123", 2, 2);
  uint8_t testString[] = {'a', 'b', 'c', 0b11111111, 0b11111111, 0}; // Malformed utf-8 string
  assert_string_glyph_length_is((const char *)testString, 3, 3);
}


void assert_beginning_of_word_is(const char * text, const char * word, const char * beginningOfWord) {
  quiz_assert(UTF8Helper::BeginningOfWord(text, word) == beginningOfWord);
}

QUIZ_CASE(ion_utf8_helper_beginning_of_word) {
  const char * test_sentence = "01 34+ \n89";
  assert_beginning_of_word_is(test_sentence, test_sentence, test_sentence);
  assert_beginning_of_word_is(test_sentence, test_sentence + 1, test_sentence);
  assert_beginning_of_word_is(test_sentence, test_sentence + 2, test_sentence);
  assert_beginning_of_word_is(test_sentence, test_sentence + 5, test_sentence + 3);
  assert_beginning_of_word_is(test_sentence, test_sentence + 8, test_sentence + 8);
}

void assert_end_of_word_is(const char * word, const char * endOfWord) {
  quiz_assert(UTF8Helper::EndOfWord(word) == endOfWord);
}

QUIZ_CASE(ion_utf8_helper_end_of_word) {
  const char * test_sentence = "01 34+ 789";
  assert_end_of_word_is(test_sentence, test_sentence + 2);
  assert_end_of_word_is(test_sentence + 2, test_sentence + 2);
  assert_end_of_word_is(test_sentence + 3, test_sentence + 6);
  assert_end_of_word_is(test_sentence + 8, test_sentence + 10);
}

void assert_is_prefix(const char * a, const char * b, const char * c) {
  quiz_assert(strcmp(UTF8Helper::SuffixCaseInsensitiveNoCombining(a, b), c) == 0);
}

void assert_is_not_prefix(const char * a, const char * b) {
  quiz_assert(!UTF8Helper::IsPrefixCaseInsensitiveNoCombining(a, b));
}

QUIZ_CASE(ion_utf8_helper_is_prefix) {
  /* Beware the "é" character below must be a combined character not the usual,
   * single codepoint "é". */
  assert_is_prefix("pre", "prefix", "fix");
  assert_is_prefix("calc", "CALCULATOR", "ULATOR");
  assert_is_prefix("he", "Hélium", "lium");
  assert_is_prefix("hél", "Hélium", "ium");
  assert_is_not_prefix("prefix", "pre");
  assert_is_not_prefix("cla", "CALCULATOR");
  assert_is_not_prefix("ha", "hélium");
}
