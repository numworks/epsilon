#ifndef CODE_HELPERS_H
#define CODE_HELPERS_H

#include <ion/events.h>

namespace Code {
namespace Helpers {

bool PythonTextForEvent(Ion::Events::Event event, char * buffer, bool * shouldRemoveLastCharacter);

}
}

#endif
