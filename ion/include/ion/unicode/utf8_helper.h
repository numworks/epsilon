#ifndef ION_UNICODE_UTF8_HELPER_H
#define ION_UNICODE_UTF8_HELPER_H

#include "code_point.h"
#include <stddef.h>

namespace UTF8Helper {

class TextPair {
public:
  typedef bool (*ReplacementRule)(const char *, size_t, size_t);

  constexpr TextPair(const char * firstString, const char * secondString, bool removeParenthesesExtension = false, ReplacementRule rule = nullptr) :  m_firstString(firstString), m_secondString(secondString), m_removeParenthesesExtension(removeParenthesesExtension), m_rule(rule) {}
  const char * firstString() { return m_firstString; }
  const char * secondString() { return m_secondString; }
  bool removeParenthesesExtension() { return m_removeParenthesesExtension; }
  bool shouldReplace(const char * s, int length, int position) { return m_rule ? m_rule(s, length, position) : true; }
  constexpr static int k_maxLength = 20;
private:
  const char * m_firstString;
  const char * m_secondString;
  bool m_removeParenthesesExtension;
  ReplacementRule m_rule;
};

// Returns the number of occurences of a code point in a string
int CountOccurrences(const char * s, CodePoint c);

/* Returns the first occurence of a code point in a string, the position of the
 * null terminating char otherwise. */
const char * CodePointSearch(const char * s, CodePoint c, const char * stoppingPosition = nullptr);

// Returns true if the text had the code point
bool HasCodePoint(const char * s, CodePoint c, const char * stoppingPosition = nullptr);

/* Returns the first occurence of a code point that is not c in a string,
 * stopping at the null-terminating char or the start of string. */
const char * NotCodePointSearch(const char * s, CodePoint c, bool goingLeft = false, const char * initialPosition = nullptr);

// Compare similarly to strcmp
int CompareNonNullTerminatedStringWithNullTerminated(const char * nonNullTerminatedString, size_t nonNullTerminatedStringLength, const char * nullTerminatedString);

/* Copy src into dst while removing all code points in codePoints. Ensure null-
 * termination of dst. */
bool CopyAndRemoveCodePoints(char * dst, size_t dstSize, const char * src, CodePoint * codePoints, int numberOfCodePoints);

/* Remove all code points c. and update an index that should be lower if code
 * points where removed before it. Ensure null-termination of dst. */
void RemoveCodePoint(char * buffer, CodePoint c, const char * * indexToUpdate = nullptr, const char * stoppingPosition = nullptr);

/* Slides a string by a number of chars. If slidingSize < 0, the string is slided
 * to the left losing the first chars. Returns true if successful.
 * Exemples :
 * SlideStringByNumberOfChar("12345", 2, 7) gives "1212345"
 * SlideStringByNumberOfChar("12345", 2, 5) gives "12123"
 * SlideStringByNumberOfChar("12345", -2, 5) gives "34545"*/
bool SlideStringByNumberOfChar(char * text, int slidingSize, size_t textMaxLength);

/* Looks for patterns in a string. If a pattern is found, it is replaced by
 * the one associated in the TextPair struct.
 * - firstToSecond defines if replace the first string of a TextPair by the second
 *   or the other way around.
 * - indexToUpdate is a pointer to a char in the string. It will be updated to
 *   point to the same place after calling the function.
 * - stoppingPosition allows partial replacement in the string.
 *
 * Ensure null termination of the string or set the value of stoppingPosition*/
void TryAndReplacePatternsInStringByPatterns(char * text, int textMaxSize, const TextPair * textPairs, int numberOfPairs, bool firstToSecond, const char * * indexToUpdate = nullptr, const char * stoppingPosition = nullptr);

/* Copy src into dst until end of dst or code point c, with null termination. Return the length of the copy */
size_t CopyUntilCodePoint(char * dst, size_t dstSize, const char * src, CodePoint c);

/* Perform actionCodePoint each time a given code point is found, and
 * actionOtherCodePoint for other code points.
 * goingRight tells if we are decoding towards the right or the left. If
 * goingRight is false, initialPosition must be provided, and the action is not
 * done for *(initial position) even if it matches c.
 * The return value is the first address for which we did not perform an action.
 *
 *                                         r = return value
 *                                         x = actionCodePoint is performed
 *                                         o = actionOtherCodePoint is performed
 * Going right == true:                    s = stoppingCodePoint
 *     o  o  o  o  x  x  o  o  x  o
 *    |  |  |  |  |c |c |  |  |c |  |s |  |  |c |
 *    ^start of string              ^r
 *
 * Going right == false:
 *                          x  x  o  o
 *    |  |c |  |  |c |  |s |c |c |  |  |c |  |  |
 *    ^start of string  ^r             ^initialPosition
 *
 * */
typedef void (*CodePointAction)(int codePointOffset, void * contextPointer, int contextInt1, int contextInt2);
const char * PerformAtCodePoints(
    const char * string,
    CodePoint c,
    CodePointAction actionCodePoint,
    CodePointAction actionOtherCodePoint,
    void * contextPointer,
    int contextInt1,
    int contextInt2 = -1,
    CodePoint stoppingCodePoint = UCodePointNull,
    bool goingRight = true,
    const char * initialPosition = nullptr,
    const char * stoppingPosition = nullptr);

CodePoint PreviousCodePoint(const char * buffer, const char * location); // returns 0 if location == buffer
CodePoint CodePointAtLocation(const char * location);
bool PreviousCodePointIs(const char * buffer, const char * location, CodePoint c);
bool CodePointIs(const char * location, CodePoint c);
bool CodePointIsEndOfWord(CodePoint c);

// Shift the buffer and return the number of bytes removed.
int RemovePreviousGlyph(const char * text, char * location, CodePoint * c = nullptr);

/* Return the pointer to the (non combining) code point whose glyph is displayed
 * at the given position, and vice-versa */
const char * CodePointAtGlyphOffset(const char * buffer, int position);
size_t GlyphOffsetAtCodePoint(const char * buffer, const char * position);

/* Return the number of glyphs in a string.
 * For instance, strlen("∑") = 3 but StringGlyphLength("∑") = 1 */
size_t StringGlyphLength(const char * s, int maxSize = -1);

// Returns the position of the first previous char ' ', '\n' or text
const char * BeginningOfWord(const char * text, const char * word);
// Returns the position of the first following char ' ', '\n' or 0
const char * EndOfWord(const char * word);

// On a line, count number of glyphs before and after locations
void CountGlyphsInLine(const char * text, int * before, int * after, const char * beforeLocation, const char *afterLocation = nullptr);

/* Returns whether the first string is prefix to the second.
 * Considers e, E and é as the same character. */
bool IsPrefixCaseInsensitiveNoCombining(const char * prefix, const char * text);

}

#endif
