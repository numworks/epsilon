#include "input_field.h"

#include <string.h>

#include "converter.h"

InputField::InputField() {}

const char* InputField::text() const { return "123"; }

void InputField::setValue(int value) {}

void InputField::append(const char* text) {}

void InputField::remove(int numberOfChars) {}
