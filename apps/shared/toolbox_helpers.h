#ifndef SHARED_TOOLBOX_HELPERS_H
#define SHARED_TOOLBOX_HELPERS_H

#include <escher/i18n.h>

namespace Shared {
namespace ToolboxHelpers {

int CursorIndexInCommand(const char * text);
/* Returns the index of the cursor position in a Command, which is the smallest
 * index between :
 *   - After the first open parenthesis
 *   - The end of the text */


void TextToInsertForCommandMessage(I18n::Message message, char * buffer);
/* Removes the arguments from a command message:
 *  - Removes text between parentheses, except commas */

}
}

#endif
