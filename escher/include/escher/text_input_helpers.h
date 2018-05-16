#ifndef ESCHER_TEXT_INPUT_HELPERS_H
#define ESCHER_TEXT_INPUT_HELPERS_H

#include <escher/i18n.h>
#include <stddef.h>

namespace TextInputHelpers {

size_t CursorIndexInCommand(const char * text);
/* Returns the index of the cursor position in a Command, which is the smallest
 * index between :
 *   - The first EmptyChar index (which is the position of the first argument)
 *   - The first empty quote
 *   - The end of the text */
}

#endif
