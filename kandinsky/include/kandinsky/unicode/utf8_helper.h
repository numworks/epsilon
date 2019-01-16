#ifndef KANDINSKY_UNICODE_UTF8_HELPER_H
#define KANDINSKY_UNICODE_UTF8_HELPER_H

#include "code_point.h"
#include <stddef.h>

namespace UTF8Helper {

const char * CodePointSearch(const char * s, CodePoint c);
/* CopyAndRemoveCodePoint copies src into dst while removing all code points c.
 * It also updates an index that should be lower if code points where removed
 * before it. */
void CopyAndRemoveCodePoint(char * dst, size_t dstSize, const char * src, CodePoint c, size_t * indexToDUpdate = nullptr);

};

#endif
