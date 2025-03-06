#include "converter.h"
#include "input_field.h"
#include <string.h>

InputField::InputField() :
  m_input("0")
{
}

const char * InputField::text() const {
  return m_input;
}

void InputField::setValue(int value) {
  remove(k_maxNumberOfCharacters);
  Converter::Serialize(value, m_input, k_maxNumberOfCharacters);
}

void InputField::append(const char * text) {
  int currentLength = strlen(m_input);
  int addedLength = strlen(text);
  if (currentLength + addedLength + 1 > k_maxNumberOfCharacters) {
    return;
  }
  strncpy(m_input + currentLength, text, addedLength + 1);
}

void InputField::remove(int numberOfChars) {
  int length = strlen(m_input);
  if (numberOfChars >= length) {
    m_input[0] = 0;
    return;
  }
  m_input[length - numberOfChars] = 0;
}
