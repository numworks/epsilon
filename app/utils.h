#ifndef APP_UTILS_H
#define APP_UTILS_H

extern "C" {
#include <ion.h>
}

typedef struct {
  char* text;
  ion_event_t event;
} text_event_t;

/* Returns a pointer to an input text allocated by the functions.
 * Also returns an event, which is its return reason.
 *
 * This function can get a text to work on instead of starting from an empty
 * string. */
text_event_t get_text(char* txt);

void clear_screen();

#endif // APP_UTILS_H
