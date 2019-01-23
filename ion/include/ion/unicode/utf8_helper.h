#ifndef ION_UNICODE_UTF8_HELPER_H
#define ION_UNICODE_UTF8_HELPER_H

#include "code_point.h"
#include <stddef.h>

namespace UTF8Helper {

// Returns the number of occurences of a code point in a string
int CountOccurrences(const char * s, CodePoint c);

// Returns the first occurence of a code point in a string
const char * CodePointSearch(const char * s, CodePoint c);

/* Copy src into dst while removing all code points c. Also update an index
 * that should be lower if code points where removed before it. */
void CopyAndRemoveCodePoint(char * dst, size_t dstSize, const char * src, CodePoint c, const char * * indexToDUpdate = nullptr);

// Perform an action each time a code point is found
typedef void (*CodePointAction)(char * codePointLocation, void * contextPointer, int contextInt);
void PerformAtCodePoints(const char * s, CodePoint c, CodePointAction action, void * contextPointer, int contextInt);

};

#endif
