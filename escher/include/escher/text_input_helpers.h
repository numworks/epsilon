#ifndef ESCHER_TEXT_INPUT_HELPERS_H
#define ESCHER_TEXT_INPUT_HELPERS_H

#include <escher/i18n.h>

namespace TextInputHelpers {

int CursorIndexInCommand(const char * text);
/* Returns the index of the cursor position in a Command, which is the smallest
 * index between :
 *   - After the first open parenthesis
 *   - The end of the text */

}

#endif
