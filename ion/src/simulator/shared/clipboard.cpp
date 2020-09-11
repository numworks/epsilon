#include <ion/clipboard.h>
#include <SDL.h>
#include <string.h>

namespace Ion {

void Clipboard::write(const char * text) {
  /* FIXME : Handle the error if need be. */
  SDL_SetClipboardText(text);
}

void Clipboard::read(char * buffer, size_t bufferSize) {
  if (!SDL_HasClipboardText()) {
    buffer[0] = '\0';
    return;
  }
  char * text = SDL_GetClipboardText();
  if (text) {
    strlcpy(buffer, text, bufferSize);
    SDL_free(text);
  }
}

}
