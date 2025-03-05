#include "converter.h"
#include "eadkpp.h"
#include "input_field.h"
#include "store.h"
#include "view.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

const char eadk_app_name[]
#if PLATFORM_DEVICE
  __attribute__((section(".rodata.eadk_app_name")))
#endif
  = "RPN";

const uint32_t eadk_api_level
#if PLATFORM_DEVICE
  __attribute__((section(".rodata.eadk_api_level")))
#endif
  = 0;

int main(int argc, char * argv[]) {
  // Initialize instances
  Store store;
  InputField inputField;
  View view;

  // Draw initial view
  view.drawLayout();
  view.drawStore(&store);
  view.drawInputField(inputField.text());

  while (true) {
    int32_t timeout = 300;
    EADK::Keyboard::Event event = EADK::Keyboard::getEvent(&timeout);

    if (event == EADK::Keyboard::Event::Exe || event == EADK::Keyboard::Event::Ok || event == EADK::Keyboard::Event::Plus || event == EADK::Keyboard::Event::Minus) {
      int inputValue = Converter::Parse(inputField.text());
      if (event == EADK::Keyboard::Event::Exe || event == EADK::Keyboard::Event::Ok) {
        store.push(inputValue);
      } else if (event == EADK::Keyboard::Event::Plus) {
        int add = store.add(inputValue);
        inputField.setValue(add);
      } else if (event == EADK::Keyboard::Event::Minus) {
        int sub = store.subtract(inputValue);
        inputField.setValue(sub);
      }
      view.drawStore(&store);
    } else if (event == EADK::Keyboard::Event::Backspace) {
      inputField.remove(1);
    } else {
      inputField.append(EADK::Keyboard::getText(event));
    }
    view.drawInputField(inputField.text());
  }
}
