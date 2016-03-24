extern "C" {
#include <kandinsky.h>
#include <stdlib.h>
#include <string.h>
#include <ion.h>
}

#define PROMPT_HEIGHT 30

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
  KDDrawString(text, KDPointMake(0, SCREEN_HEIGHT - (PROMPT_HEIGHT / 2)));
  KDDrawInverseChar(text[index], KDPointMake(index * font_size.width, SCREEN_HEIGHT - (PROMPT_HEIGHT / 2)));
}

char* get_text() {
  char input[255] = {0};
  int index = 0;
  int max = 0;
  input[max] = ' ';
  input[max+1] = '\0';

  while (1) {
    clear_prompt();
    print_prompt(input, index);
    ion_event_t event = ion_get_event();
    if (event == EQUAL) {
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
        max++;
        input[max] = ' ';
        input[max+1] = '\0';
      }
    }
  }
  
  clear_prompt();
  input[max] = '\0';
  char* output = (char*) malloc(sizeof(char) * (index + 1));
  memcpy(output, input, (size_t) (index + 1));
  return output;
}
