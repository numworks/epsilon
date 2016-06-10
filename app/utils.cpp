extern "C" {
#include <assert.h>
#include <kandinsky.h>
#include <stdlib.h>
#include <string.h>
}

#include "utils.h"

static constexpr int16_t kPromptHeight = 25;
static constexpr uint8_t kBufferSize = 255;

void clear_screen() {
  KDRect r;
  r.x = 0;
  r.y = 0;
  r.width = ION_SCREEN_WIDTH;
  r.height = ION_SCREEN_HEIGHT;
  KDFillRect(r, 0x00);
}

static void clear_prompt() {
  KDRect r;
  r.x = 0;
  r.y = ION_SCREEN_HEIGHT - kPromptHeight;
  r.width = ION_SCREEN_WIDTH;
  r.height = kPromptHeight;
  KDFillRect(r, 0x00);
}

static void print_prompt(char* text, int index) {
  char* tmp = (char*) " ";
  KDSize font_size = KDStringSize(tmp);
  KDDrawLine(KDPointMake(0, ION_SCREEN_HEIGHT - kPromptHeight),
             KDPointMake(ION_SCREEN_WIDTH, ION_SCREEN_HEIGHT - kPromptHeight), 0xff);
  KDDrawString(text, KDPointMake(0, ION_SCREEN_HEIGHT - (kPromptHeight / 2)), 0);
  KDDrawChar(text[index], KDPointMake(index * font_size.width, ION_SCREEN_HEIGHT - (kPromptHeight / 2)), true);
}

static void clear_trig_menu() {
  {
    KDRect r;
    r.x = ION_SCREEN_WIDTH / 4 - 1;
    r.y = ION_SCREEN_HEIGHT / 4 - 1;
    r.width = ION_SCREEN_WIDTH / 2 + 2;
    r.height = ION_SCREEN_HEIGHT / 2 + 2;
    KDFillRect(r, 0x00);
  }
}

static void print_trig_menu() {
  {
    KDRect r;
    r.x = ION_SCREEN_WIDTH / 4;
    r.y = ION_SCREEN_HEIGHT / 4;
    r.width = ION_SCREEN_WIDTH / 2;
    r.height = ION_SCREEN_HEIGHT / 2;
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
  const KDPoint orig = KDPointMake(ION_SCREEN_WIDTH / 4, ION_SCREEN_HEIGHT / 4);
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
  char input[kBufferSize];
  int index = 0;
  int max = 0;
  text_event_t text_event = {nullptr, ERROR};

  for (int i = 0; i < kBufferSize; i++) {
    input[i] = 0;
  }

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
