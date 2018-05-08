#ifndef ESCHER_TEXT_INPUT_HELPERS_H
#define ESCHER_TEXT_INPUT_HELPERS_H

#include <escher/i18n.h>

namespace TextInputHelpers {

int CursorIndexInCommand(const char * text);
/* Returns the index of the cursor position in a Command, which is the smallest
 * index between :
 *   - After the first open parenthesis/quote if the following element is
 *   either a quote, a coma or a parenthesi
 *   - The end of the text
 *   - Special case: when the text preceding the parenthesis is 'random', the
 *   cursor position is the end of the text. */
constexpr static const char * k_random = "random";
}

#endif
