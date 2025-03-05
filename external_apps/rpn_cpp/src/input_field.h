#ifndef INPUT_FIELD_H
#define INPUT_FIELD_H

class InputField {
public:
  InputField();
  const char * text() const;
  void setValue(int value);
  void append(const char * text);
  void remove(int numberOfChars);
};

#endif

