#ifndef ION_UNICODE_UTF8_HELPER_H
#define ION_UNICODE_UTF8_HELPER_H

#include "code_point.h"
#include <stddef.h>

namespace UTF8Helper {

// Returns the number of occurences of a code point in a string
int CountOccurrences(const char * s, CodePoint c);

/* Returns the first occurence of a code point in a string, the position of the
 * null terminating char otherwise. */
const char * CodePointSearch(const char * s, CodePoint c);

// Returns true if the text had the code point
bool HasCodePoint(const char * s, CodePoint c);

/* Returns the first occurence of a code point that is not c in a string,
 * stopping at the null-terminating char or the start of string. */
const char * NotCodePointSearch(const char * s, CodePoint c, bool goingLeft = false, const char * initialPosition = nullptr);

/* Copy src into dst while removing all code points in codePoints. Ensure null-
 * termination of dst. */
bool CopyAndRemoveCodePoints(char * dst, size_t dstSize, const char * src, CodePoint * codePoints, int numberOfCodePoints);

/* Remove all code points c. and update an index that should be lower if code
 * points where removed before it. Ensure null-termination of dst. */
void RemoveCodePoint(char * buffer, CodePoint c, const char * * indexToUpdate = nullptr, const char * stoppingPosition = nullptr);

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

bool PreviousCodePointIs(const char * buffer, const char * location, CodePoint c);
bool CodePointIs(const char * location, CodePoint c);

// Shift the buffer and return the number of bytes removed.
int RemovePreviousGlyph(const char * text, char * location, CodePoint * c = nullptr);

/* Return the pointer to the (non combining) code point whose glyph is displayed
 * at the given position, and vice-versa */
const char * CodePointAtGlyphOffset(const char * buffer, int position);
size_t GlyphOffsetAtCodePoint(const char * buffer, const char * position);

/* Return the number of glyphs in a string.
 * For instance, strlen("∑") = 3 but StringGlyphLength("∑") = 1 */
size_t StringGlyphLength(const char * s, int maxSize = -1);

};

#endif
