#include "clipboard_helper.h"

#include <SDL.h>
#include <string.h>

/* This implementation is used for all targets but the web simulator. */

namespace Ion {
namespace Clipboard {

void sendToSystemClipboard(const char* text) { SDL_SetClipboardText(text); }

void fetchFromSystemClipboard(char* buffer, size_t bufferSize) {
  if (!SDL_HasClipboardText()) {
    buffer[0] = '\0';
    return;
  }
  char* text = SDL_GetClipboardText();
  if (text) {
    strlcpy(buffer, text, bufferSize);
  } else {
    buffer[0] = '\0';
  }
  SDL_free(text);
}

}  // namespace Clipboard
}  // namespace Ion
