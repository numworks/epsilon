extern "C" {
#include <assert.h>
#include <kandinsky.h>
#include <stdlib.h>
#include <string.h>
}

#include "utils.h"

#define PROMPT_HEIGHT 30

void clear_screen() {
  KDRect r;
  r.x = 0;
  r.y = 0;
  r.width = SCREEN_WIDTH;
  r.height = SCREEN_HEIGHT;
  KDFillRect(r, 0x00);
}

static void clear_prompt() {
  KDRect r;
  r.x = 0;
  r.y = SCREEN_HEIGHT - PROMPT_HEIGHT;
  r.width = SCREEN_WIDTH;
  r.height = PROMPT_HEIGHT;
  KDFillRect(r, 0x00);
}

static void print_prompt(char* text, int index) {
  char* tmp = (char*) " ";
  KDSize font_size = KDStringSize(tmp);
  KDDrawLine(KDPointMake(0, SCREEN_HEIGHT - PROMPT_HEIGHT),
             KDPointMake(SCREEN_WIDTH, SCREEN_HEIGHT - PROMPT_HEIGHT), 0xff);
  KDDrawString(text, KDPointMake(0, SCREEN_HEIGHT - (PROMPT_HEIGHT / 2)), 0);
  KDDrawChar(text[index], KDPointMake(index * font_size.width, SCREEN_HEIGHT - (PROMPT_HEIGHT / 2)), true);
}

static void clear_trig_menu() {
  {
    KDRect r;
    r.x = SCREEN_WIDTH / 4 - 1;
    r.y = SCREEN_HEIGHT / 4 - 1;
    r.width = SCREEN_WIDTH / 2 + 2;
    r.height = SCREEN_HEIGHT / 2 + 2;
    KDFillRect(r, 0x00);
  }
}

static void print_trig_menu() {
  {
    KDRect r;
    r.x = SCREEN_WIDTH / 4;
    r.y = SCREEN_HEIGHT / 4;
    r.width = SCREEN_WIDTH / 2;
    r.height = SCREEN_HEIGHT / 2;
    KDDrawRect(r, 0xff);
  }
}

static int get_trig_input(char* input) {
  int pos = 0;
  const char* kTexts[] = {
    "sine",
    "cosine",
    "tangent",
    "cancel"
  };
  const char* kOutputs[] = {
    "sin( )",
    "cos( )",
    "tan( )",
  };
  const KDPoint orig = KDPointMake(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4);
  while (true) {
    print_trig_menu();

    uint16_t vert_acc = 0;
    for (int i(0); i<4; i++) {
      KDSize text_size = KDStringSize(kTexts[i]);
      KDDrawString(kTexts[i],
          KDPointMake(orig.x, orig.y + vert_acc + text_size.height / 2),
          i==pos);
      vert_acc += text_size.height;
    }

    ion_event_t event = ion_get_event();
    if (event == UP_ARROW) {
      pos--;
      if (pos < 0) {
        pos = 0;
      }
    } else if (event == DOWN_ARROW) {
      pos++;
      if (pos >= 4) {
        pos = 3;
      }
    } else if (event == '=') {
      clear_trig_menu();
      if (pos == 3) {
        return 0;
      } else {
        memcpy(input, kOutputs[pos], (size_t) strlen(kOutputs[pos]));
        return strlen(kOutputs[pos]);
      }
    }
  }
}

text_event_t get_text(char* txt) {
  char input[255] = {0};
  int index = 0;
  int max = 0;
  text_event_t text_event = {nullptr, ERROR};

  if (txt != nullptr) {
    index = strlen(txt);
    max = index;
    memcpy(input, txt, (size_t) index);
  }

  input[max] = ' ';
  input[max+1] = '\0';

  while (1) {
    clear_prompt();
    print_prompt(input, index);
    ion_event_t event = ion_get_event();
    if (event == EQUAL) {
      input[max] = '\0';
      text_event.event = EQUAL;
      text_event.text = (char*) malloc(sizeof(char) * (index + 1));
      memcpy(text_event.text, input, (size_t) (index + 1));
      break;
    } else if (event == LEFT_ARROW) {
      index--;
      if (index < 0) {
        index = 0;
      }
    } else if (event == RIGHT_ARROW) {
        if (index < max) {
          index++;
        }
    } else if (event <= 0x7f) {
      input[index++] = (char) event;
      // We are at the end of the line.
      if (index > max) {
        max=index;
        input[max] = ' ';
        input[max+1] = '\0';
      }
    } else if (event == TRIG_MENU) {
      int tmp = get_trig_input(&input[index]);
      index+=tmp;
      if (index > max) {
        max=index;
        input[max] = ' ';
        input[max+1] = '\0';
      }
      // we want to be inside the parenthese if there are some.
      index -= (tmp > 2) ? 2 : 0;
    } else if (event == DELETE) {
      // Nothing to delete.
      if (index == max) {
        continue;
      }
      for (int i=index; i<max+1; i++) {
        input[i] = input[i+1];
      }
      max--;
    } else if (event == UP_ARROW) {
      text_event.event = UP_ARROW;
      break;
    } else if (event == DOWN_ARROW) {
      text_event.event = DOWN_ARROW;
      break;
    } else if (event == PLOT) {
      text_event.event = PLOT;
      input[max] = '\0';
      text_event.text = (char*) malloc(sizeof(char) * (index + 1));
      memcpy(text_event.text, input, (size_t) (index + 1));
      break;
    } else {
      assert(false); // unreachable.
    }
  }

  clear_prompt();
  return text_event;
}
