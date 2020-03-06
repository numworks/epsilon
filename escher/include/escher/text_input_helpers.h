#ifndef ESCHER_TEXT_INPUT_HELPERS_H
#define ESCHER_TEXT_INPUT_HELPERS_H

#include <escher/i18n.h>
#include <stddef.h>

namespace TextInputHelpers {

const char * CursorPositionInCommand(const char * text, const char * stoppingPosition = nullptr);
/* Returns the pointer to the char that should be right of the cursor, which is
 * the first char between :
 *   - The first EmptyChar (which is the position of the first argument)
 *   - The char after the first empty quote
 *   - The end of the text */
}

#endif
