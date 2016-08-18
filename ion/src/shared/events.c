#include <ion.h>

// More event type will be added later.
typedef enum {
  KEY_DOWN,
} ion_key_event_type_t;

typedef struct {
  ion_key_t key;
  ion_key_event_type_t event;
} ion_key_event_t;

// Debouncing, qnd change to get_key event.
static ion_key_event_t ion_get_key_event() {
  // Let's start by saving which keys we've seen up
  bool key_seen_up[ION_NUMBER_OF_KEYS];
  for (ion_key_t k=0; k<ION_NUMBER_OF_KEYS; k++) {
    key_seen_up[k] = !ion_key_down(k);
  }

  // Wait a little to debounce the button.
  ion_sleep(10);

  /* Let's discard the keys we previously saw up but which aren't anymore: those
   * were probably bouncing! */
  for (ion_key_t k=0; k<ION_NUMBER_OF_KEYS; k++) {
    key_seen_up[k] &= !ion_key_down(k);
  }

  while (1) {
    for (ion_key_t k=0; k<ION_NUMBER_OF_KEYS; k++) {
      if (ion_key_down(k)) {
        if (key_seen_up[k]) {
          return (ion_key_event_t) {.key = k, .event = KEY_DOWN};
        }
      } else {
        key_seen_up[k] = 1;
      }
    }
    ion_sleep(10);
  }
}

// For now this is a bit silly but needed for instreface purpose.
static const ion_event_t kEventForKeyDown[ION_NUMBER_OF_KEYS] = {
            F1,           F2,           F3,           F4,          F5,
        SECOND,        SHIFT,          ESC,   LEFT_ARROW,    UP_ARROW,
       DIAMOND,        ALPHA,         APPS,   DOWN_ARROW, RIGHT_ARROW,
          HOME,         MODE,      CATALOG,       DELETE,       CLEAR,
  LOWER_CASE_X, LOWER_CASE_Y, LOWER_CASE_Z, LOWER_CASE_T,       POWER,
         EQUAL, LEFT_PARENTHESIS, RIGHT_PARENTHESIS, COMMA, DIVISION,
         DOT,  SEVEN, EIGHT, NINE, PRODUCT,
         UPPER_CASE_E, FOUR, FIVE, SIX, MINUS,
         DOT, ONE, TWO, THREE, PLUS,
         DOT, ZERO, DOT, MINUS, ENTER
};

ion_event_t ion_get_event() {
  // First we get a key event.
  const ion_key_event_t key_event = ion_get_key_event();
  // We then generate the event associated to this key event.
  if (key_event.event == KEY_DOWN) {
    return kEventForKeyDown[key_event.key];
  } else {
    return ERROR;
  }
}
