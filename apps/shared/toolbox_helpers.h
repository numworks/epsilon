#ifndef SHARED_TOOLBOX_HELPERS_H
#define SHARED_TOOLBOX_HELPERS_H

#include <escher/i18n.h>

namespace Shared {
namespace ToolboxHelpers {

void TextToInsertForCommandMessage(I18n::Message message, char * buffer, int bufferSize, bool replaceArgsWithEmptyChar = false);
void TextToInsertForCommandText(const char * command, int commandLength, char * buffer, int bufferSize, bool replaceArgsWithEmptyChar = false);
/* Removes the arguments from a command:
 *  - Removes text between parentheses or brackets, except commas */

}
}

#endif
