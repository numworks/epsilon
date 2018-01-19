#ifndef SHARED_TOOLBOX_HELPERS_H
#define SHARED_TOOLBOX_HELPERS_H

#include <escher/i18n.h>

namespace Shared {
namespace ToolboxHelpers {

int CursorIndexInCommandText(const char * text);
/* Returns the index of the cursor position in a command, which is the smallest
 * index between :
 *   - After the first open parenthesis
 *   - The end of the text */


void TextToInsertForCommandMessage(I18n::Message message, char * buffer, int bufferSize);
void TextToInsertForCommandText(const char * command, char * buffer, int bufferSize);
/* Removes the arguments from a command:
 *  - Removes text between parentheses or brackets, except commas */
void TextToParseIntoLayoutForCommandMessage(I18n::Message message, char * buffer, int bufferSize);
void TextToParseIntoLayoutForCommandText(const char * command, char * buffer, int bufferSize);
/* Removes the arguments from a command and replaces them with empty chars. */

}
}

#endif
